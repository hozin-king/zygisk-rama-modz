#include <jni.h>
#include <unistd.h>
#include "memory.h"
static pid_t g_pid = -1;
static uintptr_t g_il2cpp = 0;
extern "C" JNIEXPORT jboolean JNICALL
Java_com_bench_ramamodz_RootHelper_nativeAttach(JNIEnv*, jclass, jint pid) {
    g_pid = (pid_t)pid; g_il2cpp = 0;
    for (int i = 0; i < 50 && !g_il2cpp; ++i) {
        g_il2cpp = get_module_base(g_pid, "libil2cpp.so");
        if (!g_il2cpp) usleep(200000);
    }
    return g_il2cpp ? JNI_TRUE : JNI_FALSE;
}
extern "C" JNIEXPORT jlong JNICALL
Java_com_bench_ramamodz_RootHelper_nativeReadLong(JNIEnv*, jclass, jlong addr) {
    long v = 0; if (g_pid > 0) mem_read(g_pid, (uintptr_t)addr, &v, sizeof(v)); return (jlong)v;
}
extern "C" JNIEXPORT jboolean JNICALL
Java_com_bench_ramamodz_RootHelper_nativeWriteInt(JNIEnv*, jclass, jlong addr, jint val) {
    if (g_pid <= 0) return JNI_FALSE;
    int v = (int)val;
    return mem_write(g_pid, (uintptr_t)addr, &v, sizeof(v)) ? JNI_TRUE : JNI_FALSE;
}
