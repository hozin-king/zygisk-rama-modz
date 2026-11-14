LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ramaroot
LOCAL_SRC_FILES := injector.cpp memory.cpp
LOCAL_LDLIBS := -llog
LOCAL_CPPFLAGS := -std=c++17 -O2 -fvisibility=hidden -Wall
LOCAL_CFLAGS := -O2 -fvisibility=hidden
include $(BUILD_SHARED_LIBRARY)

# Injector ptrace root — executable PIE, dinamai lib*.so agar ikut ter-package ke APK.
# Dijalankan via su dari nativeLibraryDir (lihat RootHelper.injectSo).
include $(CLEAR_VARS)
LOCAL_MODULE := librama_inject.so
LOCAL_SRC_FILES := inject_main.cpp memory.cpp
LOCAL_LDLIBS := -llog
LOCAL_CPPFLAGS := -std=c++17 -O2 -Wall
LOCAL_CFLAGS := -O2
include $(BUILD_EXECUTABLE)

# Payload in-process: sumber Zygisk yang sama (ImGui + Dobby + resolver + stealth).
# Entry constructor aktif via APP_CFLAGS=-DRAMA_ROOTINJECT dari build.gradle.
# 4 level naik: jni -> main -> src -> root-apk -> root repo.
include $(LOCAL_PATH)/../../../../app/src/main/jni/Android.mk
