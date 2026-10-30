#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <string>
#include "memory.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "RamaRoot", __VA_ARGS__)

static pid_t g_pid = -1;
static uintptr_t g_il2cpp = 0;

extern "C" JNIEXPORT jstring JNICALL
Java_com_bench_ramamodz_RootHelper_nativeAttachStatus(JNIEnv *env, jclass, jstring jpkg) {
    const char *pkg = env->GetStringUTFChars(jpkg, nullptr);
    std::string msg;

    pid_t pid = find_pid(pkg);
    if (pid <= 0) {
        msg = std::string("pid not found: ") + pkg;
        env->ReleaseStringUTFChars(jpkg, pkg);
        return env->NewStringUTF(msg.c_str());
    }

    g_pid = pid;
    g_il2cpp = 0;
    // wait up to ~15s for libil2cpp (lobby / after load)
    for (int i = 0; i < 75; ++i) {
        g_il2cpp = get_module_base(g_pid, "libil2cpp.so");
        if (g_il2cpp) break;
        usleep(200000);
    }

    env->ReleaseStringUTFChars(jpkg, pkg);

    if (!g_il2cpp) {
        msg = "pid=" + std::to_string((int)g_pid) + " but libil2cpp.so not mapped yet — enter lobby/match first";
        g_pid = -1;
        return env->NewStringUTF(msg.c_str());
    }

    // probe read 8 bytes at base
    long probe = 0;
    bool ok = mem_read(g_pid, g_il2cpp, &probe, sizeof(probe));
    if (!ok) {
        msg = "pid=" + std::to_string((int)g_pid) + " base=0x" + 
              [&]{ char b[32]; snprintf(b,sizeof(b),"%llx",(unsigned long long)g_il2cpp); return std::string(b);}() +
              " read FAIL: " + last_error();
        return env->NewStringUTF(msg.c_str());
    }

    char buf[160];
    snprintf(buf, sizeof(buf), "OK pid=%d il2cpp=0x%llx", (int)g_pid, (unsigned long long)g_il2cpp);
    LOGI("%s", buf);
    return env->NewStringUTF(buf);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_bench_ramamodz_RootHelper_nativeAttach(JNIEnv *env, jclass clazz, jint /*pid unused*/) {
    // kept for ABI; prefer nativeAttachStatus
    return g_il2cpp != 0 ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_bench_ramamodz_RootHelper_nativeGetIl2CppBase(JNIEnv *, jclass) {
    return (jlong)g_il2cpp;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_bench_ramamodz_RootHelper_nativeReadLong(JNIEnv *, jclass, jlong addr) {
    long v = 0;
    if (g_pid > 0) mem_read(g_pid, (uintptr_t)addr, &v, sizeof(v));
    return (jlong)v;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_bench_ramamodz_RootHelper_nativeWriteInt(JNIEnv *, jclass, jlong addr, jint val) {
    if (g_pid <= 0) return JNI_FALSE;
    int v = (int)val;
    return mem_write(g_pid, (uintptr_t)addr, &v, sizeof(v)) ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_bench_ramamodz_RootHelper_nativeFindPid(JNIEnv *env, jclass, jstring jpkg) {
    const char *pkg = env->GetStringUTFChars(jpkg, nullptr);
    pid_t p = find_pid(pkg);
    env->ReleaseStringUTFChars(jpkg, pkg);
    return (jint)p;
}
