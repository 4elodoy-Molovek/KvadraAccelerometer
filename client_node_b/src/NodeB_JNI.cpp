#include <jni.h>
#include <string>
#include "ClientB_Core.h"
#include <cstdint>

static std::unique_ptr<kvadra::client_b::ClientB_Core> g_client = nullptr;

extern "C" JNIEXPORT jboolean JNICALL
Java_com_kvadra_accelerometer_NodeBService_startNativeProcessor(
        JNIEnv* env,
        jobject /* this */,
        jstring targetIp,
        jstring apiKey,
        jstring caCert,
        jstring clientCert,
        jstring clientKey) {

    if (g_client != nullptr) {
        return JNI_FALSE;
    }

    const char* c_targetIp = env->GetStringUTFChars(targetIp, nullptr);
    const char* c_apiKey = env->GetStringUTFChars(apiKey, nullptr);
    const char* c_caCert = env->GetStringUTFChars(caCert, nullptr);
    const char* c_clientCert = env->GetStringUTFChars(clientCert, nullptr);
    const char* c_clientKey = env->GetStringUTFChars(clientKey, nullptr);

    g_client = std::make_unique<kvadra::client_b::ClientB_Core>(
        c_targetIp, c_apiKey, c_caCert, c_clientCert, c_clientKey
    );
    
    g_client->start();

    env->ReleaseStringUTFChars(targetIp, c_targetIp);
    env->ReleaseStringUTFChars(apiKey, c_apiKey);
    env->ReleaseStringUTFChars(caCert, c_caCert);
    env->ReleaseStringUTFChars(clientCert, c_clientCert);
    env->ReleaseStringUTFChars(clientKey, c_clientKey);

    return JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_kvadra_accelerometer_NodeBService_stopNativeProcessor(
        JNIEnv*,
        jobject) {
    
    if (g_client != nullptr) {
        g_client->stop();
        g_client.reset();
    }
}


namespace absl { // чтобы линковщик не вонял
    inline namespace lts_20260107 {
        namespace cord_internal {
            thread_local SamplingState cordz_next_sample = {0, 0};
        }
    }
}