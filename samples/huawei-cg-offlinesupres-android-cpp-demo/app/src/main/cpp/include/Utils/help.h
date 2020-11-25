/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: BufferDescriptor
 */
#ifndef _HELP_H_
#define _HELP_H_
#include <string>
#include <android/log.h>
#include "OSRPluginCommon.h"

#define LOGI(TAG, ...)  __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);
#define LOGE(TAG, ...)  __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);

constexpr int CHANNELS_RGB = 3;

bool CreateBuffer(CGKit::BufferDescriptor& buffer, int w, int h, CGKit::PixelFormat format);
bool ReadBuffer(CGKit::BufferDescriptor& buffer, const std::string &path);
bool WriteBuffer(const CGKit::BufferDescriptor& buffer, const std::string &path);
void DeleteBuffer(CGKit::BufferDescriptor& buffer);


#endif

