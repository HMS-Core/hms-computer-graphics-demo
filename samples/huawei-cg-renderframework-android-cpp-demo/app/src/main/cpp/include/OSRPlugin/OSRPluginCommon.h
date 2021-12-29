/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:
 * Author: lufei
 * Create: 2020-9-7
 */
#ifndef PLUGINCOMMON_H
#define PLUGINCOMMON_H
#include "Rendering/RenderCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace CGKit {
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
