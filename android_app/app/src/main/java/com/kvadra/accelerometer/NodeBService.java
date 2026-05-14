package com.kvadra.accelerometer;

import android.util.Log;

public class NodeBService {
    private static final String TAG = "NodeBService";

    static {
        try {
            System.loadLibrary("client_node_b");
            Log.i(TAG, "libclient_node_b.so loaded");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Native library load failed!", e);
        }
    }

    public native boolean startNativeProcessor(
            String targetIp,
            String apiKey,
            String caCert,
            String clientCert,
            String clientKey
    );

    public native void stopNativeProcessor();
}