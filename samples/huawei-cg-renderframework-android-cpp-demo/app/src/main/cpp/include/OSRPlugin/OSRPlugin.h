/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:
 */
#ifndef OSRPLUGIN_H
#define OSRPLUGIN_H
#include "OSRPluginCommon.h"
#include "MainApplication.h"

namespace CGKit {
/**
 * @brief Describes pixel format.
 */
enum OSROperationCode {
    QUERY_SUPER_SAMPLING = 0,
    QUERY_IMAGE_ENHANCING = 1,
    SYNC_SUPER_SAMPLING = 2,
    SYNC_IMAGE_ENHANCING = 3,
    ASYNC_SUPER_SAMPLING = 4,
    ASYNC_IMAGE_ENHANCING = 5,
    SET_ASSETS_DIR = 6
};

static inline int64_t GetCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

class OSRPlugin {
public:
    OSRPlugin();
    ~OSRPlugin();
    void PluginTest();

private:
    unsigned char* ReadPPM(const String &path, int &width, int &height);
    int Format2Channel(PixelFormat format);
    bool WritePPM(const String &path, const unsigned char *pixels, int width, int height);
    bool CreateBuffer(BufferDescriptor &buffer, int w, int h, PixelFormat format);
    bool ReadBuffer(BufferDescriptor &buffer, const String &path);
    bool WriteBuffer(const BufferDescriptor &buffer, const String &path);
    void DeleteBuffer(BufferDescriptor &buffer);
    bool SetAssetsDir(const String& localDir);
    bool QueryImage(BufferDescriptor& inBuffer, PluginConfig *pluginConfig);
    bool ImageEnhancingSync(BufferDescriptor& inBuffer, BufferDescriptor& outBuffer);
    bool PluginInit();
    void PluginDeInit();
    IPlugin *plugin {nullptr};
    const String pluginName {"OfflineSupRes"};
};
}  // namespace CGKit
#endif