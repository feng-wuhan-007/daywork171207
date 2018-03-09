package com.routon.scanner;

import android.util.Log;

public class RoutonScanner {
    static private String TAG = "SN";

    static {
        try {
            System.loadLibrary("routonscanner");
        } catch(Exception e) {
            Log.i(TAG, "load routonscanner failed. ");
        }
    }

    private long mNative;
    private ScannerCallback mCB;

    public native int start();
    public native void stop();
    public native void poweron_trigger();
    public native void poweroff();
    public native void uninit();
    public native String check_type();//fengmc add 180211
    public RoutonScanner(ScannerCallback cb) {
        mCB = cb;
    }

    public void messageMe(String msg) {
        mCB.onMessage(msg);
    }

    protected void finalize() {
        uninit();
    }

    public interface ScannerCallback {
        public void onMessage(String msg);
    }
};


