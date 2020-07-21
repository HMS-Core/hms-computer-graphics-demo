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

#include <jni.h>
#include <string>
#include <vector>
#include <set>
#include <array>
#include <android/log.h>
#include <android_native_app_glue.h>
#include "vulkan/vulkan_wrapper.h"
#include "vulkan/VulkanDeleter.h"
#include "vulkan/VulkanValidation.h"
#include <android/asset_manager.h>

#ifndef NDEBUG
#define USE_DEBUG_EXTENTIONS 1
#endif
/* Enable huawei prerotation extension */
#ifndef ENABLE_HUAWEI_EXTENSION
    #define ENABLE_HUAWEI_EXTENSION 1
#else
#undef ENABLE_HUAWEI_EXTENSION
    #define ENABLE_HUAWEI_EXTENSION 1
#endif
/* Enable huawei prerotation extension */
#ifndef ENABLE_HUAWEI_PREROTATION
    #define ENABLE_HUAWEI_PREROTATION 1
#else
#undef ENABLE_HUAWEI_PREROTATION
    #define ENABLE_HUAWEI_PREROTATION 1
#endif
#if ENABLE_HUAWEI_PREROTATION
    #define VK_HUAWEI_PREROTATION_EXTENSION_NAME      "VK_HUAWEI_prerotation"
#endif

/* Enable huawei smart cache */
#ifndef ENABLE_HUAWEI_SMART_CACHE
    #define ENABLE_HUAWEI_SMART_CACHE 1
#else
#undef ENABLE_HUAWEI_SMART_CACHE
    #define ENABLE_HUAWEI_SMART_CACHE 1
#endif
#if ENABLE_HUAWEI_SMART_CACHE
    #define VK_HUAWEI_SMARTCACHE_EXTENSION_NAME       "VK_HUAWEI_smart_cache"
#endif

#ifndef ENABLE_HUAWEI_SMART_CACHE_FOR_FBO
    #define ENABLE_HUAWEI_SMART_CACHE_FOR_FBO 1
#else
#undef ENABLE_HUAWEI_SMART_CACHE_FOR_FBO
    #define ENABLE_HUAWEI_SMART_CACHE_FOR_FBO 0
#endif

/* Android log function wrappers */
static const char* FBOTAG = "Vulkan-fbo";
static const size_t MAX_SHADER_SIZE = 0xA00000; /* 10MB */

#define HWLOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, FBOTAG, __VA_ARGS__))
#define HWLOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, FBOTAG, __VA_ARGS__))
#define HWLOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, FBOTAG, __VA_ARGS__))
#define HWLOGD(...) \
  ((void)__android_log_print(ANDROID_LOG_DEBUG, FBOTAG, __VA_ARGS__))

/* Vulkan call wrapper */
#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, "Vulkan-fbo-app ",         \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
  }

/* normal call wrapper */
#define CALL_ASSERT(func)                                             \
  if (true != (func)) {                                               \
    __android_log_print(ANDROID_LOG_ERROR, "Vulkan-fbo-app ",         \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
  }

/* Store image mem view */
struct DepthStencil {
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

const int WIDTH = 800;
const int HEIGHT = 600;

enum Result
{
    /* Success */
            RESULT_SUCCESS = 0,

    /* Generic error without any particular information */
            RESULT_ERROR_GENERIC = -1,

    /* Returned by the swapchain when the swapchain is invalid and should be
       recreated */
            RESULT_ERROR_OUTDATED_SWAPCHAIN = -2,

    /* Generic input/output errors */
            RESULT_ERROR_IO = -3,

    /* Memory allocation errors */
            RESULT_ERROR_OUT_OF_MEMORY = -4
};

struct QueueFamilyIndices {
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool isComplete() {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

/* We will call this function the window is opened.
   This is where we will initialise everything */
bool initialized = false;
bool active = false;
bool Initialize(android_app* app);

bool smartCacheSupported = false;

/* Functions interacting with Android native activity */
void android_main(struct android_app* state);
void Terminate();
void HandleCmd(android_app* app, int32_t cmd);
bool ValidationLayerSupport(const std::vector<const char*>);

/* Global variables */
void DrawNeededFrame(unsigned imageIndex);

void CreateNeededCommandPool();

void RenderNeededFrame();

Result AcquireNeededNextImage(unsigned *image);

void CreateNeededGraphicsPipeline(android_app *pApp);

void CreateNeededRenderPass();

void CreateNeededLogicalDevice();

void CreateNeededSwapChain();

void PickNeededPhysicalDevice();

bool DeviceSuitable(VkPhysicalDevice device);

void SetupNeededDebugCallback();

void CreateNeededSurface(android_app *pApp);

void CreateNeededInstance();

bool EnabledValidationLayers();

bool CheckNeededDeviceExtensionSupport(VkPhysicalDevice device);

void CreateNeededImageViews();

void CreateNeededFramebuffers();

void CreateNeededSemaphores();


VkSurfaceFormatKHR ChooseNeededSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

VkPresentModeKHR ChooseNeededSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

VkExtent2D ChooseNeededSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

QueueFamilyIndices FindNeededQueueFamilies(VkPhysicalDevice device);

SwapChainSupportDetails QueryNeededSwapChainSupport(VkPhysicalDevice device);


VulkanDeleter<VkInstance> instance {vkDestroyInstance};
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VulkanDeleter<VkDevice> device{vkDestroyDevice}; /* logical device */
VulkanDeleter<VkDebugReportCallbackEXT> callback {instance, DestroyDebugReportCallbackEXT};
VulkanDeleter<VkSurfaceKHR> surface{instance, vkDestroySurfaceKHR};
VulkanDeleter<VkSwapchainKHR> swapChain{device, vkDestroySwapchainKHR};
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;
std::vector<VkImage> swapChainImages;
std::vector<VulkanDeleter<VkImageView>> swapChainImageViews;
struct DepthStencil depthStencil;
std::vector<VulkanDeleter<VkFramebuffer>> swapChainFramebuffers;

VulkanDeleter<VkShaderModule> vertShaderModule{device, vkDestroyShaderModule};
VulkanDeleter<VkShaderModule> fragShaderModule{device, vkDestroyShaderModule};
VulkanDeleter<VkPipelineLayout> pipelineLayout{device, vkDestroyPipelineLayout};
VulkanDeleter<VkRenderPass> renderPass{device, vkDestroyRenderPass};
VulkanDeleter<VkPipeline> graphicsPipeline{device, vkDestroyPipeline};

VulkanDeleter<VkCommandPool> commandPool{device, vkDestroyCommandPool};
std::vector<VkCommandBuffer> commandBuffers;

VulkanDeleter<VkSemaphore> imageAvailableSemaphore{device, vkDestroySemaphore};
VulkanDeleter<VkSemaphore> renderFinishedSemaphore{device, vkDestroySemaphore};

VkQueue graphicsQueue;
VkQueue presentQueue;

const std::vector<const char*> USABLELAYERS = {
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_swapchain",
        "VK_LAYER_LUNARG_image",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_device_limits",
        "VK_LAYER_GOOGLE_unique_objects"
};

const std::vector<const char*> USABLEDEVICEEXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/* typical Android NativeActivity entry function */
void android_main(struct android_app* app)
{
    app->onAppCmd = HandleCmd;
    for (;;) {
        int events;
        android_poll_source* source = nullptr;

        while (ALooper_pollAll(active? 0: -1, nullptr, &events, (void**)&source) >= 0) {
            if (source != NULL) {
                source->process(app, source);
            }
            if (app->destroyRequested != 0) {
                return;
            }
        }
        if (initialized && active) {
            unsigned index;
            Result res = AcquireNeededNextImage(&index);
            DrawNeededFrame(index);
        }
    }
}

void DestroyExistObject()
{
    vkQueueWaitIdle(graphicsQueue);
    vkDestroyImage(device.object, depthStencil.image, nullptr);
    vkFreeMemory(device.object, depthStencil.mem, nullptr);
    vkDestroyImageView(device.object, depthStencil.view, nullptr);
    swapChainFramebuffers.clear();
    swapChainImageViews.clear();
    vkDestroyRenderPass(device.object, renderPass.object, nullptr);
    vkDestroyPipeline(device.object, graphicsPipeline.object, nullptr);
    vkDestroyPipelineLayout(device.object, pipelineLayout.object, nullptr);
}

bool Initialize(android_app* app)
{
    /* Load Android vulkan and retrieve vulkan API function pointers */
    if (!InitVulkan()) {
    HWLOGE("Vulkan is unavailable, install vulkan and re-start");
    return false;
    }

    CreateNeededInstance();

    CreateNeededSurface(app);

    PickNeededPhysicalDevice();

    CreateNeededLogicalDevice();

    CreateNeededSwapChain();

    CreateNeededRenderPass();

    CreateNeededGraphicsPipeline(app);

    CreateNeededImageViews();

    CreateNeededFramebuffers();

    CreateNeededCommandPool();

    CreateNeededSemaphores();

    initialized = true;
    return true;
}

bool extensionSupported(std::string extension, std::vector<std::string> &supportedExtensions)
{
    return (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end());
}

VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat)
{
    /* Since all depth formats may be optional, we need to find a suitable depth format to use
       Start with the highest precision packed format */
    std::vector<VkFormat> depthFormats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM
    };

    for (auto& format : depthFormats) {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
        /* Format must support depth stencil attachment for optimal tiling */
        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            *depthFormat = format;
            return true;
        }
    }

    return false;
}
bool DeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = FindNeededQueueFamilies(device);

    bool extensionsSupported =  CheckNeededDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = QueryNeededSwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool CheckNeededDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(USABLEDEVICEEXTENSIONS.begin(), USABLEDEVICEEXTENSIONS.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices FindNeededQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }
        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

SwapChainSupportDetails QueryNeededSwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,
                                              surface,
                                              &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR ChooseNeededSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    uint32_t i = 0;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                if (memTypeFound) {
                    *memTypeFound = true;
                }
                return i;
            }
        }
        typeBits >>= 1;
    }

    if (memTypeFound) {
        *memTypeFound = true;
        return i;
    } else {
        return 0;
    }
}

void setDepthStencil(struct DepthStencil &referencedDepthStencil)
{
    VkImageCreateInfo imageCI{};
    imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCI.imageType = VK_IMAGE_TYPE_2D;
    /* Depth buffer format */
    VkFormat depthFormat;
    /* Find a suitable depth format */
    CALL_ASSERT(getSupportedDepthFormat(physicalDevice, &depthFormat));
    imageCI.format = depthFormat;
    imageCI.extent = { swapChainExtent.width, swapChainExtent.height, 1 };
    imageCI.mipLevels = 1;
    imageCI.arrayLayers = 1;
    imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    CALL_VK(vkCreateImage(device, &imageCI, nullptr, &referencedDepthStencil.image));
    VkMemoryRequirements memReqs{};
    vkGetImageMemoryRequirements(device, referencedDepthStencil.image, &memReqs);

    VkMemoryAllocateInfo memAllloc{};
    memAllloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllloc.allocationSize = memReqs.size;
    /* Enable huawei smart cache for fbo */
    #if ENABLE_HUAWEI_EXTENSION && ENABLE_HUAWEI_SMART_CACHE && ENABLE_HUAWEI_SMART_CACHE_FOR_FBO
        if(smartCacheSupported) {
            memAllloc.memoryTypeIndex = 3;
            HWLOGD("Enable huawei smart cache for fbo successfully");
        }
    #else
        memAllloc.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    #endif
    CALL_VK(vkAllocateMemory(device, &memAllloc, nullptr, &referencedDepthStencil.mem));
    CALL_VK(vkBindImageMemory(device, referencedDepthStencil.image, referencedDepthStencil.mem, 0));

    VkImageViewCreateInfo imageViewCI{};
    imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCI.image = referencedDepthStencil.image;
    imageViewCI.format = depthFormat;
    imageViewCI.subresourceRange.baseMipLevel = 0;
    imageViewCI.subresourceRange.levelCount = 1;
    imageViewCI.subresourceRange.baseArrayLayer = 0;
    imageViewCI.subresourceRange.layerCount = 1;
    imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    /* Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT */
    if (depthFormat >= VK_FORMAT_D16_UNORM_S8_UINT) {
        imageViewCI.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    CALL_VK(vkCreateImageView(device, &imageViewCI, nullptr, &referencedDepthStencil.view));
}

VkResult CreateNeededShaderModule(android_app* androidAppCtx, VkDevice  device, const char* filePath, VkShaderModule* shaderOut)
{
    /* Read the file */
    AAsset* file = AAssetManager_open(androidAppCtx->activity->assetManager, filePath, AASSET_MODE_BUFFER);
    size_t fileLength = AAsset_getLength(file);
    VkResult result = VK_ERROR_OUT_OF_HOST_MEMORY;

    if (fileLength < MAX_SHADER_SIZE) {
        char* fileContent = new char[fileLength];
        if (fileContent != nullptr) {
            AAsset_read(file, fileContent, fileLength);

            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = fileLength;
            createInfo.pCode = (uint32_t*) fileContent;
            VkResult result = vkCreateShaderModule( device, &createInfo, nullptr, shaderOut);
            delete[] fileContent;
            CALL_VK(result);
        } else {
            HWLOGE("out of memory");
        }
    } else {
        HWLOGE("shader size more than defined max size");
    }
    return result;
}

void CreateNeededLogicalDevice()
{
    QueueFamilyIndices indices = FindNeededQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = (uint32_t) queueCreateInfos.size();

    createInfo.pEnabledFeatures = &deviceFeatures;
    /* Enable huawei  extension */
    #if ENABLE_HUAWEI_EXTENSION
        std::vector<std::string> supportedExtensions;
        std::vector<const char*> temDeviceExtensions(USABLEDEVICEEXTENSIONS);
        uint32_t extCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
        if (extCount > 0) {
            std::vector<VkExtensionProperties> extensions(extCount);
            if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS) {
                for (auto ext : extensions) {
                    supportedExtensions.push_back(ext.extensionName);
                }
            }
        }
        /* whether huawei prerotation is supported */
        #if ENABLE_HUAWEI_PREROTATION
            if (extensionSupported(VK_HUAWEI_PREROTATION_EXTENSION_NAME, supportedExtensions)) {
                temDeviceExtensions.push_back(VK_HUAWEI_PREROTATION_EXTENSION_NAME);
                HWLOGD("Huawei prerotation is supported");
            }
        #endif
        /* whether huawei prerotation is supported */
        #if ENABLE_HUAWEI_SMART_CACHE
            smartCacheSupported = extensionSupported(VK_HUAWEI_SMARTCACHE_EXTENSION_NAME, supportedExtensions);
            if (smartCacheSupported) {
                temDeviceExtensions.push_back(VK_HUAWEI_SMARTCACHE_EXTENSION_NAME);
                HWLOGD("Huawei smart cache is supported");
            }
        #endif
        createInfo.enabledExtensionCount = (uint32_t)temDeviceExtensions.size();
        createInfo.ppEnabledExtensionNames = temDeviceExtensions.data();
    #else
        createInfo.enabledExtensionCount = USABLEDEVICEEXTENSIONS.size();
        createInfo.ppEnabledExtensionNames = USABLEDEVICEEXTENSIONS.data();
    #endif

    if ( EnabledValidationLayers()) {
        createInfo.enabledLayerCount = USABLELAYERS.size();
        createInfo.ppEnabledLayerNames = USABLELAYERS.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, device.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);

}

void CreateNeededSwapChain()
{
    vkDeviceWaitIdle(device.object);
    SwapChainSupportDetails swapChainSupport = QueryNeededSwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = ChooseNeededSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseNeededSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseNeededSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    VkSwapchainKHR oldSwapchain = swapChain.object;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindNeededQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {(uint32_t) indices.graphicsFamily, (uint32_t) indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    /* To specify that you do not want any transformation, simply specify the current transformation. */
    if(swapChainSupport.capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    }
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = oldSwapchain;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, swapChain.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed to create swap chain!");
    }

    if (oldSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device.object, oldSwapchain, nullptr);
    }

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
    CALL_VK(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr));
    swapChainImages.resize(imageCount);
    CALL_VK(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data()));
}

void SetupNeededDebugCallback()
{
    if (! EnabledValidationLayers()) return;
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    createInfo.pfnCallback = LayerDebugCallback;

    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr && func(instance, &createInfo, nullptr, callback.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed to set up debug callback!");
    }
}

void CreateNeededSurface(android_app *app)
{
    /* if we create a surface, we need the surface extension */
    VkAndroidSurfaceCreateInfoKHR createInfo {
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .window = app->window };
    if (vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr,
                                  surface.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed creating android surface");
    }
}

void CreateNeededInstance()
{
    if ( EnabledValidationLayers() && !ValidationLayerSupport(USABLELAYERS)) {
        HWLOGE("Validation layers requested, but not available!");
    }

    /* CreateNeededInstance() */
    VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .apiVersion = VK_MAKE_VERSION(1, 0, 0),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .pApplicationName = "vulkan fbo",
            .pEngineName = "VulkanEngine",
    };

#ifndef NDEBUG
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 0);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    for (auto &extension : extensions) {
        HWLOGI("Extension found %s v%d" , extension.extensionName, extension.specVersion);
    }
#endif

    /* prepare necessary extensions: Vulkan on Android need these to function */
    std::vector<const char *> instanceExt;
    instanceExt.push_back("VK_KHR_surface");
    instanceExt.push_back("VK_KHR_android_surface");
    const bool enableValidationLayers =  EnabledValidationLayers();
    if (enableValidationLayers) {
        instanceExt.push_back("VK_EXT_debug_report");
    }

    /* Create the Vulkan instance */
    VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = static_cast<uint32_t>(instanceExt.size()),
            .ppEnabledExtensionNames = instanceExt.data()
    };
    if (enableValidationLayers) {
        instanceCreateInfo.enabledLayerCount = USABLELAYERS.size();
        instanceCreateInfo.ppEnabledLayerNames = USABLELAYERS.data();
    } else {
        instanceCreateInfo.enabledLayerCount = 0;
    }
    CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, instance.Replace()));

}

void PickNeededPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        HWLOGE("Failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if ( DeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        HWLOGE("Failed to find a suitable GPU!");
    }
}

void CreateNeededImageViews()
{
    swapChainImageViews.resize(swapChainImages.size(), VulkanDeleter<VkImageView>{device, vkDestroyImageView});

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, nullptr, swapChainImageViews[i].Replace()) != VK_SUCCESS) {
            HWLOGE("Failed to create image views!");
        }
    }
}


void CreateNeededRenderPass()
{
    std::array<VkAttachmentDescription, 2> attachments = {};
    // Color attachment
    attachments[0].format = swapChainImageFormat;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    /* Depth attachment */
    VkFormat depthFormat;
    /* Find a suitable depth format */
    VkBool32 validDepthFormat = getSupportedDepthFormat(physicalDevice, &depthFormat);
    attachments[1].format = depthFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;
    subpassDescription.pResolveAttachments = nullptr;

    /* Subpass dependencies for layout transitions */
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, renderPass.Replace()) != VK_SUCCESS) {
       HWLOGE("Failed to create render pass!");
    }
}

void CreateNeededGraphicsPipeline(android_app *app)
{
    if (CreateNeededShaderModule(app, device, "shaders/triangle.vert.spv", vertShaderModule.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed to create vertex shader module!");
    }

    if (CreateNeededShaderModule(app, device, "shaders/triangle.frag.spv", fragShaderModule.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed to create fragment shader module!");
    }

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainExtent.width;
    viewport.height = (float) swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    /* If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage.
       This basically disables any output to the framebuffer. */
    rasterizer.rasterizerDiscardEnable = VK_FALSE;

    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = 0;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                               pipelineLayout.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed to create pipeline layout!");
    }
    /* Depth and stencil state containing depth and stencil compare and test operations
       We only use depth tests and want depth tests and writes to be enabled and compare with less or equal */
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.front = depthStencilState.back;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;

    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, graphicsPipeline.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed to create graphics pipeline!");
    }
    vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(device, shaderStages[1].module, nullptr);
}

void CreateNeededFramebuffers()
{
    VkImageView attachments[2];

    /* Depth/Stencil attachment is the same for all frame buffers */
    setDepthStencil(depthStencil);
    attachments[1] = depthStencil.view;

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = NULL;
    frameBufferCreateInfo.renderPass = renderPass;
    frameBufferCreateInfo.attachmentCount = 2;
    frameBufferCreateInfo.pAttachments = attachments;

    SwapChainSupportDetails swapChainSupport = QueryNeededSwapChainSupport(physicalDevice);
    VkExtent2D extent = ChooseNeededSwapExtent(swapChainSupport.capabilities);
    frameBufferCreateInfo.width =extent.width;
    frameBufferCreateInfo.height = extent.height;
    frameBufferCreateInfo.layers = 1;

    /* Create frame buffers for every swap chain image */
    swapChainFramebuffers.resize(swapChainImageViews.size(), VulkanDeleter<VkFramebuffer>{device, vkDestroyFramebuffer});
    for (uint32_t i = 0; i < swapChainFramebuffers.size(); i++) {
        attachments[0] = swapChainImageViews[i];
        if (vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, swapChainFramebuffers[i].Replace()) != VK_SUCCESS) {
            HWLOGE("Failed to create framebuffer!");
        }
    }
}

void CreateNeededCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = FindNeededQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = 0;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, commandPool.Replace()) != VK_SUCCESS) {
        HWLOGE("Failed to create command pool!");
    }
}

void RenderNeededFrame()
{
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        HWLOGE("Failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChainFramebuffers[i];

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChainExtent;

            VkClearValue clearColor = {0.2f, 0.8f, 0.8f, 1.0f};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

                vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            HWLOGE("Failed to record command buffer!");
        }
    }
}

void CreateNeededSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, imageAvailableSemaphore.Replace()) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, renderFinishedSemaphore.Replace()) != VK_SUCCESS) {

        HWLOGE("Failed to create semaphores!");
    }
}
Result AcquireNeededNextImage(unsigned *image)
{
    VkResult res = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, image);
    if (res != VK_SUCCESS) {
        HWLOGI("failed acquir next image res: %d", (int)res);
        vkQueueWaitIdle(graphicsQueue);
    }
    return RESULT_SUCCESS;
}

void DrawNeededFrame(unsigned imageIndex)
{
    RenderNeededFrame();
    HWLOGD("DrawNeededFrame");

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        HWLOGE("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr;

    vkQueuePresentKHR(presentQueue, &presentInfo);
}

VkPresentModeKHR ChooseNeededSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseNeededSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {WIDTH, HEIGHT};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

bool EnabledValidationLayers()
{
#ifdef NDEBUG
    return false;
#else
    return false;
#endif

}
void DestroySwapchain()
{
    if (device.object != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device.object);
    }
    if (swapChain.object != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device.object, swapChain.object, nullptr);
        swapChain.object = VK_NULL_HANDLE;
    }
}

void Terminate()
{
    HWLOGD("Terminate: %d", __LINE__);
    if (device.object != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device.object);
    }
    HWLOGD("Terminate: %d", __LINE__);
    if (!commandBuffers.empty()) {
        vkFreeCommandBuffers(device.object, commandPool.object, commandBuffers.size(), commandBuffers.data());
    }
    vkDestroyCommandPool(device.object, commandPool.object, nullptr);
    vkDestroySemaphore(device.object, imageAvailableSemaphore.object, nullptr);
    vkDestroySemaphore(device.object, renderFinishedSemaphore.object, nullptr);
    DestroySwapchain();

    if (surface.object != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance.object, surface.object, nullptr);
        surface.object = VK_NULL_HANDLE;
    }
    HWLOGD("Terminate: %d", __LINE__);
    if (device.object != VK_NULL_HANDLE) {
        vkDestroyDevice(device.object, nullptr);
        device.object = VK_NULL_HANDLE;
    }

    if (instance.object != VK_NULL_HANDLE) {
        vkDestroyInstance(instance.object, nullptr);
        instance.object = VK_NULL_HANDLE;
    }
    HWLOGD("Terminate: %d", __LINE__);
}

/* Process the next main command. */
void HandleCmd(android_app* app, int32_t cmd)
{
    switch (cmd) {
        case APP_CMD_RESUME:
            HWLOGD("APP_CMD_RESUME");
            active = true;
            if (initialized == true) {
                CreateNeededSwapChain();
                CreateNeededRenderPass();
                CreateNeededGraphicsPipeline(app);
                CreateNeededImageViews();
                CreateNeededFramebuffers();
            }
            break;
        case APP_CMD_INIT_WINDOW:
            /* The window is being shown, get it ready. */
            HWLOGD("APP_CMD_INIT_WINDOW");
            if (!Initialize(app)) {
                app->destroyRequested = 1;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            /* The window is being hidden or closed, clean it up. */
            HWLOGD("APP_CMD_TERM_WINDOW");

            if (initialized == true) {
                DestroyExistObject();
                initialized = false;
                Terminate();
            }
            break;
        case APP_CMD_PAUSE:
            active = false;
            HWLOGD("APP_CMD_PAUSE");
            DestroySwapchain();
            break;
        default:
            HWLOGI("Event not handled: %d", cmd);
    }
}

bool ValidationLayerSupport(const std::vector<const char*> usableLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    HWLOGI("%d validation layers available" , layerCount);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (auto &layer: availableLayers) {
        HWLOGI("Validation layer available %s v%d" , layer.layerName, layer.specVersion);
    }

    for (const char* layerName : usableLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }
    return true;
}