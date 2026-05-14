package com.kvadra.accelerometer;

import android.util.Log;

public class NodeAService {
    private static final String TAG = "NodeAService";

    static {
        try {
            System.loadLibrary("client_node_a");
            Log.i(TAG, "libclient_node_a.so loaded");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Native library load failed!", e);
        }
    }

    public native boolean startNativeClient(
            String targetIp,
            String apiKey,
            String caCert,
            String clientCert,
            String clientKey,
            String logPath
    );

    public native void stopNativeClient();
}