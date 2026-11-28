package com.nbteditor;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.OpenableColumns;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Locale;

/**
 * Thin SAF bridge.  The application itself is pure native C++; this Activity
 * exists only to run the OS file manager (ACTION_GET_CONTENT / chooser) and to
 * hand the picked bytes back to libnbt through a static native callback.
 *
 * It holds no UI of its own (translucent theme) and finishes immediately after
 * the result is forwarded, so the user sees the system file manager and then
 * returns straight to the native screen.
 */
public class FilePickerActivity extends Activity {

    public static final String EXTRA_MODE = "nbt_picker_mode";

    // Keep in sync with PickerMode in main.native.cpp
    public static final int MODE_IMPORT_WORLD = 0;
    public static final int MODE_OPEN_NBT = 1;

    private static final int REQUEST_PICK = 1001;

    static {
        System.loadLibrary("nbt");
    }

    /** Implemented in libnbt.so. Runs on the Android main thread. */
    public static native void onFilePicked(int mode, int resultCode,
                                           String path, String displayName, String uri);

    /** Write back to the document the user picked (ACTION_OPEN_DOCUMENT grant). */
    public static boolean writeBytes(String uriStr, byte[] data) {
        if (uriStr == null || uriStr.isEmpty()) return false;
        try {
            if (sContext == null) return false;
            android.content.ContentResolver cr = sContext.getContentResolver();
            java.io.OutputStream os = cr.openOutputStream(android.net.Uri.parse(uriStr));
            if (os == null) return false;
            try { os.write(data); os.flush(); } finally { os.close(); }
            return true;
        } catch (Throwable t) {
            return false;
        }
    }

    private int mode = MODE_IMPORT_WORLD;
    private static String sUri = "";
    static Context sContext = null;

    /**
     * Serahkan file .mcworld sementara (di cache privat) ke Minecraft lewat
     * ACTION_VIEW + content Uri. Dipanggil dari native setelah zip selesai.
     */
    public static boolean openInMinecraft(String fileName) {
        try {
            Context ctx = sContext;
            if (ctx == null || fileName == null || fileName.isEmpty()) return false;
            File dir = new File(ctx.getCacheDir(), WorldFileProvider.PATH);
            File file = new File(dir, fileName);
            if (!file.isFile() || file.length() == 0) return false;

            Uri uri = Uri.parse("content://" + WorldFileProvider.AUTHORITY + "/" +
                    WorldFileProvider.PATH + "/" + Uri.encode(file.getName()));
            Intent view = new Intent(Intent.ACTION_VIEW);
            view.setDataAndType(uri, WorldFileProvider.MIME);
            view.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            Intent chooser = Intent.createChooser(view, "Open in Minecraft");
            chooser.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            ctx.startActivity(chooser);
            return true;
        } catch (Throwable t) {
            return false;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sContext = getApplicationContext();
        mode = getIntent().getIntExtra(EXTRA_MODE, MODE_IMPORT_WORLD);

        // Device file manager. OPEN_DOCUMENT (instead of GET_CONTENT) also grants
        // write access, so "Open NBT file" can save back to the original file.
        Intent pick = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        pick.addFlags(Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
        pick.addCategory(Intent.CATEGORY_OPENABLE);
        pick.setType("*/*");
        pick.putExtra(Intent.EXTRA_MIME_TYPES, new String[] {
                "application/octet-stream",
                "application/x-mcworld",
                "application/x-mctemplate",
                "application/x-mcpack",
                "application/x-mcaddon",
                "application/zip",
                "application/x-zip-compressed"
        });
        try {
            startActivityForResult(Intent.createChooser(pick, "Select a .mcworld file"),
                                   REQUEST_PICK);
        } catch (Throwable t) {
            // No file manager installed / chooser failed: report cancellation
            // so the native side never stays stuck in a progress state.
            deliver(RESULT_CANCELED, "", "", "");
            finish();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        String path = "";
        String name = "";
        sUri = "";
        if (resultCode == RESULT_OK && data != null && data.getData() != null) {
            Uri uri = data.getData();
            sUri = uri.toString();
            try {
                getContentResolver().takePersistableUriPermission(
                        uri, Intent.FLAG_GRANT_READ_URI_PERMISSION |
                             Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
            } catch (Throwable ignored) { }
            name = displayName(uri);
            try {
                File dir = new File(getExternalFilesDir(null), "picked");
                if (!dir.exists() && !dir.mkdirs()) {
                    dir = getCacheDir();
                }
                File out = new File(dir, "pick_" + System.currentTimeMillis()
                                    + safeExtension(name));
                copyTo(uri, out);
                path = out.getAbsolutePath();
            } catch (Throwable t) {
                path = "";
            }
        }
        deliver(resultCode, path, name, sUri);
        finish();
    }

    private void deliver(int resultCode, String path, String name, String uri) {
        try {
            onFilePicked(mode, resultCode, path == null ? "" : path,
                         name == null ? "" : name, uri == null ? "" : uri);
        } catch (Throwable ignored) {
            // Never let a Java-side failure crash the native process.
        }
    }

    private String displayName(Uri uri) {
        String name = "";
        if (ContentResolver.SCHEME_CONTENT.equals(uri.getScheme())) {
            Cursor c = null;
            try {
                c = getContentResolver().query(uri, null, null, null, null);
                if (c != null && c.moveToFirst()) {
                    int idx = c.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                    if (idx >= 0) name = c.getString(idx);
                }
            } catch (Throwable ignored) {
            } finally {
                if (c != null) c.close();
            }
        }
        if (name == null || name.isEmpty()) {
            String last = uri.getLastPathSegment();
            name = (last == null) ? "world.mcworld" : last;
        }
        return name;
    }

    private void copyTo(Uri uri, File out) throws IOException {
        try (InputStream in = getContentResolver().openInputStream(uri);
             OutputStream os = new FileOutputStream(out)) {
            if (in == null) throw new IOException("Cannot read " + uri);
            byte[] buf = new byte[16384];
            int n;
            while ((n = in.read(buf)) > 0) os.write(buf, 0, n);
        }
    }

    /** Keep the real extension (mcworld, mctemplate, dat …) but strip anything unsafe. */
    private static String safeExtension(String name) {
        int dot = name.lastIndexOf('.');
        if (dot < 0 || dot == name.length() - 1) return "";
        StringBuilder sb = new StringBuilder();
        String ext = name.substring(dot + 1).toLowerCase(Locale.ROOT);
        for (int i = 0; i < ext.length(); i++) {
            char ch = ext.charAt(i);
            if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) sb.append(ch);
            else break;
        }
        return sb.length() == 0 ? "" : "." + sb;
    }
}
