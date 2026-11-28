package com.nbteditor;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.MatrixCursor;
import android.net.Uri;
import android.os.ParcelFileDescriptor;

import java.io.File;
import java.io.FileNotFoundException;

/**
 * FileProvider minimal untuk menyerahkan file .mcworld sementara ke Minecraft.
 *
 * File disimpan di cache privat aplikasi (getCacheDir()/mcworld), TIDAK di
 * folder Download dan TIDAK terlihat sebagai file ekspor permanen. Provider
 * ini hanya memberi akses baca lewat content:// Uri dengan izin sementara
 * (FLAG_GRANT_READ_URI_PERMISSION) saat "Open in Minecraft" dipilih.
 *
 * Menggunakan API Android standar saja (tanpa androidx) agar tetap bisa
 * dikompilasi dengan android.jar + javac/d8 dari skrip build yang ada.
 */
public class WorldFileProvider extends ContentProvider {

    public static final String AUTHORITY = "com.nbteditor.fileprovider";
    public static final String PATH = "mcworld";
    public static final String MIME = "application/x-mcworld";

    @Override
    public boolean onCreate() {
        return true;
    }

    private File fileFor(Uri uri) throws FileNotFoundException {
        String name = uri.getLastPathSegment();
        if (name == null || name.isEmpty() || name.contains("/") || name.contains(".."))
            throw new FileNotFoundException("Invalid mcworld name");
        Context ctx = getContext();
        if (ctx == null) throw new FileNotFoundException("No context");
        File dir = new File(ctx.getCacheDir(), PATH);
        return new File(dir, name);
    }

    @Override
    public ParcelFileDescriptor openFile(Uri uri, String mode) throws FileNotFoundException {
        File f = fileFor(uri);
        if (!f.isFile()) throw new FileNotFoundException(f.getAbsolutePath());
        return ParcelFileDescriptor.open(f, ParcelFileDescriptor.MODE_READ_ONLY);
    }

    @Override
    public String getType(Uri uri) {
        return MIME;
    }

    /** Minecraft / file manager biasanya menanyakan nama dan ukuran file. */
    @Override
    public Cursor query(Uri uri, String[] projection, String selection,
                        String[] selectionArgs, String sortOrder) {
        try {
            File f = fileFor(uri);
            MatrixCursor c = new MatrixCursor(new String[]{"_display_name", "_size"});
            c.addRow(new Object[]{f.getName(), f.length()});
            return c;
        } catch (Throwable t) {
            return null;
        }
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) { return null; }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) { return 0; }

    @Override
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) {
        return 0;
    }
}
