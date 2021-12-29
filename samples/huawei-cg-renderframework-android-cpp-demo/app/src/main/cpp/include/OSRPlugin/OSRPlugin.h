/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:
 * Author: lufei
 * Create: 2020-9-7
 */

#ifndef OSRPLUGIN_H
#define OSRPLUGIN_H

#include "OSRPlugin/OSRPluginCommon.h"
#include "MainApplication/MainApplication.h"

namespace CGKit {
// Describes pixel format.
enum OSROperationCode {
    QUERY_SUPER_SAMPLING = 0,
    QUERY_IMAGE_ENHANCING = 1,
    SYNC_SUPER_SAMPLING = 2,
    SYNC_IMAGE_ENHANCING = 3,
    ASYNC_SUPER_SAMPLING = 4,
    ASYNC_IMAGE_ENHANCING = 5,
    SET_ASSETS_DIR = 6
};

static inline u64 GetCurrentTimeMilliSecond()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return ms.count();
}

#ifdef CG_ANDROID_PLATFORM
class OSRPlugin {
public:
    OSRPlugin();
    ~OSRPlugin();
    void ExecuteOSR(const String localDir);

private:
    u8* ReadPPM(const String& path, u32& width, u32& height);
    s32 Format2Channel(PixelFormat format);
    bool WritePPM(const String& path, const u8* pixels, u32 width, u32 height);
    bool CreateBuffer(BufferDescriptor& buffer, u32 w, u32 h, PixelFormat format);
    bool ReadBuffer(BufferDescriptor& buffer, const String& path);
    bool WriteBuffer(const BufferDescriptor& buffer, const String& path);
    void DeleteBuffer(BufferDescriptor& buffer);
    bool SetAssetsDir(const String& localDir);
    bool QueryImage(BufferDescriptor& inBuffer, PluginConfig* pluginConfig);
    bool ImageEnhancingSync(BufferDescriptor& inBuffer, BufferDescriptor& outBuffer);
    bool PluginInit();
    void PluginDeInit();

private:
    IPlugin* plugin = nullptr;
    const String pluginName = "OfflineSupRes";
};
#endif

}  // namespace CGKit
#endif