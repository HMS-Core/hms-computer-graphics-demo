LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := HiCulling
LOCAL_SRC_FILES := cgsdk-plugin-occlusionculling/pkg-for-thirdpartyengine/libs/$(TARGET_ARCH_ABI)/libthirdpartyengine_plugin_oc.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Sample
LOCAL_C_INCLUDES := cgsdk-plugin-occlusionculling/pkg-for-thirdpartyengine/include/
LOCAL_SRC_FILES := Sample.cpp
LOCAL_SHARED_LIBRARIES := HiCulling

include $(BUILD_EXECUTABLE)