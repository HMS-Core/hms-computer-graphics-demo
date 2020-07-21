/*
 * Copyright (C) 2019 The Huawei Source Project
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef VULKAN_VALIDATION_H
#define VULKAN_VALIDATION_H

#include <vulkan/vulkan.h>
#include <android/log.h>

// Android log function wrappers
static const char* VALIDATION_LOG_TAG = "Vulkan-validation";
#define VALIDATION_LAYER_HWLOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, VALIDATION_LOG_TAG, __VA_ARGS__))
#define VALIDATION_LAYER_HWLOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, VALIDATION_LOG_TAG, __VA_ARGS__))
#define VALIDATION_LAYER_HWLOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, VALIDATION_LOG_TAG, __VA_ARGS__))
#define VALIDATION_LAYER_HWLOGD(...) \
  ((void)__android_log_print(ANDROID_LOG_DEBUG, VALIDATION_LOG_TAG, __VA_ARGS__))

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL LayerDebugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t object,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* message,
        void* userData) {

    if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0) {
        VALIDATION_LAYER_HWLOGE("Validation layer: %s", message);
    } else if ( ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0) || ((flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) != 0)) {
        VALIDATION_LAYER_HWLOGW("Validation layer: %s", message);
    } else if ( (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0) {
        VALIDATION_LAYER_HWLOGI("Validation layer: %s", message);
    } else if ((flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0) {
        VALIDATION_LAYER_HWLOGD("Validation layer: %s", message);
    } else {
        VALIDATION_LAYER_HWLOGW("Validation layer: %s", message);
    }
    return VK_FALSE;
}

#endif