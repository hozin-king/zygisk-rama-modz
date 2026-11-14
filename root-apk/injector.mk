# Standalone ndk-build script: injector ptrace sebagai executable PIE.
# Dipanggil TERPISAH dari Gradle (lihat build-injector.sh / CI step), output ke jniLibs.
LOCAL_PATH := $(call my-dir)/src/main/jni

include $(CLEAR_VARS)
LOCAL_MODULE := rama_inject
LOCAL_MODULE_FILENAME := librama_inject      # TANPA ekstensi (assert NDK build-executable); di-rename .so sesudah build
LOCAL_SRC_FILES := inject_main.cpp memory.cpp
LOCAL_LDLIBS := -llog
LOCAL_CPPFLAGS := -std=c++17 -O2 -Wall
LOCAL_CFLAGS := -O2
include $(BUILD_EXECUTABLE)
