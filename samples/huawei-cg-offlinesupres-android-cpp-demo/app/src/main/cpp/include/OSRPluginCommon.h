/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:
 */
#ifndef PLUGINCOMMON_H
#define PLUGINCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif
namespace CGKit {
/**
 * @brief Describes pixel format.
 */
enum PixelFormat {
    // RGB
    PIXEL_FORMAT_R8G8B8_UNORM,
    // RGBA
    PIXEL_FORMAT_R8G8B8A8_UNORM,

    PIXEL_FORMAT_MAX
};

/**
 * @brief Describes buffer for plugin inoutput.
 */
struct BufferDescriptor {
    /** buffer address */
    void *addr;
    /** buffer length */
    int len;
    /** image width */
    int width;
    /** image height */
    int height;
    /** pixel format */
    PixelFormat format;
};

/**
 * @brief Describes buffer for plugin inoutput.
 */
struct PluginConfig {
    struct {
        int width;
        int height;
        PixelFormat format;
    } input, output;
    /** Estimated cost time(ms) of plugin */
    float estimatedCostTime;
};
}  // namespace CGKit

#ifdef __cplusplus
}
#endif
#endif
