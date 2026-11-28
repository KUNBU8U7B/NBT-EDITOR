package com.nbteditor;

import android.app.NativeActivity;
import android.os.Bundle;

/**
 * NativeActivity with one Java override: BACK must be routed to our native
 * dispatcher instead of finishing the task.  The reference app uses an
 * OnBackPressedCallback for exactly this (w.a).
 *
 * All UI/logic stays in libnbt.so; this class only forwards the system BACK
 * key and reports whether native handled it.
 */
public class MainActivity extends NativeActivity {

    static {
        System.loadLibrary("nbt");
    }

    /** Returns true when native code consumed BACK (drawer/dialog/editor/home). */
    public static native boolean onBackPressedNative();

    /** Called from the native worker thread through ANativeActivity.clazz. */
    public static boolean openInMinecraft(String fileName) {
        return FilePickerActivity.openInMinecraft(fileName);
    }

    @Override
    public void onBackPressed() {
        boolean handled = false;
        try {
            handled = onBackPressedNative();
        } catch (Throwable ignored) {
            // Never let a bridge failure crash the native process.
        }
        if (!handled) super.onBackPressed();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Dibutuhkan FilePickerActivity.openInMinecraft() walau SAF picker
        // belum pernah dibuka (misal langsung memilih "Open in Minecraft").
        FilePickerActivity.sContext = getApplicationContext();
    }
}
