LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ramaroot
LOCAL_SRC_FILES := injector.cpp memory.cpp
LOCAL_LDLIBS := -llog
LOCAL_CPPFLAGS := -std=c++17 -O2 -fvisibility=hidden -Wall
LOCAL_CFLAGS := -O2 -fvisibility=hidden
include $(BUILD_SHARED_LIBRARY)

# Injector ptrace root TIDAK di-build di sini: Gradle ndkBuild tidak mendukung
# BUILD_EXECUTABLE (configure abort). Di-build terpisah via root-apk/injector.mk
# (CI step / build-injector.sh) ke src/main/jniLibs/<abi>/librama_inject.so.

# Payload in-process: sumber Zygisk yang sama (ImGui + Dobby + resolver + stealth).
# Entry constructor aktif via APP_CFLAGS=-DRAMA_ROOTINJECT dari build.gradle.
# 4 level naik: jni -> main -> src -> root-apk -> root repo.
include $(LOCAL_PATH)/../../../../app/src/main/jni/Android.mk
