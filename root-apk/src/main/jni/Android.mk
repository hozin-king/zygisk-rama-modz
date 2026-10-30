LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ramaroot
LOCAL_SRC_FILES := injector.cpp memory.cpp
LOCAL_LDLIBS := -llog
LOCAL_CPPFLAGS := -std=c++17 -O2 -fvisibility=hidden -Wall
LOCAL_CFLAGS := -O2 -fvisibility=hidden
include $(BUILD_SHARED_LIBRARY)
