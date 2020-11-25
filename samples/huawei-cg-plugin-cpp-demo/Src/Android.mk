LOCAL_PATH := $(call my-dir)
SDK_PATH := ../PluginSdk

include $(CLEAR_VARS)
LOCAL_MODULE := libPluginDemo
LOCAL_SRC_FILES = ./PluginDemo.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDK_PATH)/Include/
LOCAL_LDFLAGS := -L$(LOCAL_PATH)/$(SDK_PATH)/libs/$(TARGET_ARCH_ABI)/
LOCAL_LDLIBS := -llog -lPluginInterface
LOCAL_CFLAGS += -Wall -O2 -DCGKIT_PLUGIN
include $(BUILD_SHARED_LIBRARY)
