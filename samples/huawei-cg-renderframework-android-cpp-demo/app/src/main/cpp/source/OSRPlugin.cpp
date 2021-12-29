/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: A sample that illustrates the rendering of a demo scene.
 */

#define CGKIT_LOG
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include "OSRPlugin/OSRPlugin.h"

#ifdef CG_ANDROID_PLATFORM
using namespace std;
using namespace CGKit;
static constexpr s32 CHANNELS_RGB = 3;
static constexpr u32 MAX_WIDTH = 4096;
static constexpr u32 MAX_HEIGHT = 4096;

OSRPlugin::OSRPlugin() {}

OSRPlugin::~OSRPlugin() {}

void CallBack(bool success)
{
    return;
}

u8* OSRPlugin::ReadPPM(const String& path, u32& width, u32& height)
{
    ifstream fIn(path, ios::in | ios::binary);
    if (!fIn.is_open()) {
        return nullptr;
    }
    u8* pixels = nullptr;
    u32 pixelsSize = 0;
    String line;
    if (getline(fIn, line) && getline(fIn, line)) {
        stringstream ss(line);
        if ((ss >> width) && (width > 0) && (width <= MAX_WIDTH) &&
            (ss >> height) && (height > 0) && (height <= MAX_HEIGHT) &&
            getline(fIn, line)) {
            pixelsSize = width * height * CHANNELS_RGB;
            pixels = CG_NEW_ARRAY(u8, pixelsSize);
            if (pixels == nullptr) {
                fIn.close();
                return nullptr;
            }
        } else {
            fIn.close();
            return nullptr;
        }
    } else {
        fIn.close();
        return nullptr;
    }

    if (fIn.read(reinterpret_cast<c8*>(pixels), pixelsSize)) {
        fIn.close();
        return pixels;
    } else {
        CG_DELETE_ARRAY(u8, pixels);
        fIn.close();
        return nullptr;
    }
}

bool OSRPlugin::WritePPM(const String& path, const u8* pixels, u32 width, u32 height)
{
    if (pixels == nullptr || width <= 0 || height <= 0 ||
        width > MAX_WIDTH || height > MAX_HEIGHT) {
        return false;
    }
    ofstream fOut(path, ios::out | ios::binary);
    if (!fOut.is_open()) {
        return false;
    }
    fOut << "P6\n" << width << ' ' << height << "\n255\n";
    fOut.write(reinterpret_cast<const c8*>(pixels), width * height * CHANNELS_RGB);
    fOut.close();
    return true;
}

s32 OSRPlugin::Format2Channel(PixelFormat format)
{
    if (format == PIXEL_FORMAT_R8G8B8_UNORM) {
        return CHANNELS_RGB;
    } else {
        return -1;
    }
}

bool OSRPlugin::CreateBuffer(BufferDescriptor& buffer, u32 w, u32 h, PixelFormat format)
{
    s32 c = Format2Channel(format);
    if (w <= 0 || h <= 0 || c < 0) {
        return false;
    }
    u8* pixels = CG_NEW_ARRAY(u8, w * h * c);
    if (pixels == nullptr) {
        return false;
    }
    buffer.addr = static_cast<void *>(pixels);
    buffer.width = w;
    buffer.height = h;
    buffer.len = w * h * c;
    buffer.format = format;
    return true;
}

bool OSRPlugin::ReadBuffer(BufferDescriptor& buffer, const String& path)
{
    u32 w, h;
    u8* pixels = ReadPPM(path, w, h);
    if (pixels == nullptr) {
        return false;
    }
    buffer.addr = static_cast<void*>(pixels);
    buffer.width = w;
    buffer.height = h;
    buffer.len = w * h * CHANNELS_RGB;
    buffer.format = PIXEL_FORMAT_R8G8B8_UNORM;
    return true;
}

bool OSRPlugin::WriteBuffer(const BufferDescriptor& buffer, const String& path)
{
    if (WritePPM(path, static_cast<u8*>(buffer.addr), buffer.width, buffer.height)) {
        return true;
    } else {
        return false;
    }
}

void OSRPlugin::DeleteBuffer(BufferDescriptor& buffer)
{
    if (buffer.addr != nullptr) {
        CG_DELETE_ARRAY(u8, buffer.addr);
    }
    buffer.addr = nullptr;
    buffer.width = buffer.height = buffer.len = 0;
    buffer.format = PIXEL_FORMAT_MAX;
}

bool OSRPlugin::SetAssetsDir(const String& localDir)
{
    Param paramIn, paramOut;
    c8* dir = const_cast<c8*>(localDir.c_str());

    Param pi0;
    // set opCode
    pi0.Set<s32>(SET_ASSETS_DIR);
    Param pi1;
    // set val
    pi1.Set<void*>(static_cast<void*>(dir));
    // set paramIn
    paramIn.Set(0, pi0);
    paramIn.Set(1, pi1);

    Param po0;
    // set val
    po0.Set<void*>(static_cast<void*>(dir));
    // set paramOut
    paramOut.Set(0, po0);

    // call fn:SET_ASSETS_DIR
    bool success = plugin->Execute(paramIn, paramOut);
    if (success) {
        LOGINFO("Set assets dir %s success", dir);
        return true;
    } else {
        LOGERROR("Set assets dir %s failed", dir);
        return false;
    }
}

bool OSRPlugin::QueryImage(BufferDescriptor& inBuffer, PluginConfig* pluginConfig)
{
    Param paramIn, paramOut;

    Param pi0;
    // set opcode
    pi0.Set<s32>(QUERY_IMAGE_ENHANCING);
    // set val
    Param pi1;
    pi1.Set<s32>(inBuffer.width);
    Param pi2;
    pi2.Set<s32>(inBuffer.height);
    Param pi3;
    pi3.Set<s32>(inBuffer.format);
    // set paramIn
    paramIn.Set(0, pi0);
    paramIn.Set(1, pi1);
    paramIn.Set(2, pi2);
    paramIn.Set(3, pi3);

    Param po0;
    // set val
    po0.Set<void*>(static_cast<void*>(pluginConfig));
    // set paramOut
    paramOut.Set(0, po0);

    // call fn:QUERY_IMAGE_ENHANCING
    bool success = plugin->Execute(paramIn, paramOut);
    if (success) {
        LOGINFO("Query ImageEnhancing success");
        return true;
    } else {
        LOGERROR("Query ImageEnhancing failed");
        return false;
    }
}

bool OSRPlugin::ImageEnhancingSync(BufferDescriptor& inBuffer, BufferDescriptor& outBuffer)
{
    Param paramIn, paramOut;
    const f32 sharpness = 0.9f;
    const bool toneMapping = true;
    const u32 timeOut = 10000;

    Param pi0;
    // set opcode
    pi0.Set<s32>(SYNC_IMAGE_ENHANCING);
    // set val
    Param pi1;
    pi1.Set<void*>(static_cast<void*>(&inBuffer));
    Param pi2;
    pi2.Set<f32>(sharpness);
    Param pi3;
    pi3.Set<bool>(toneMapping);
    Param pi4;
    pi4.Set<s32>(timeOut);
    // set paramIn
    paramIn.Set(0, pi0);
    paramIn.Set(1, pi1);
    paramIn.Set(2, pi2);
    paramIn.Set(3, pi3);
    paramIn.Set(4, pi4);

    Param po0;
    // set val
    po0.Set<void*>(static_cast<void*>(&outBuffer));
    // set paramOut
    paramOut.Set(0, po0);

    // call fn:SYNC_IMAGE_ENHANCING
    bool success = plugin->Execute(paramIn, paramOut);
    if (success) {
        LOGINFO("Sync ImageEnhancing success");
        return true;
    } else {
        LOGERROR("Sync ImageEnhancing failed");
        return false;
    }
}

bool OSRPlugin::PluginInit()
{
    vector<String> pluginList;

    gPluginManager.Initialize();
    pluginList = gPluginManager.GetPluginList();
    if (find(pluginList.begin(), pluginList.end(), pluginName) == pluginList.end()) {
        LOGERROR("Cannot find plugin %s!", pluginName.c_str());
        gPluginManager.Uninitialize();
        return false;
    }

    plugin = gPluginManager.LoadPlugin(pluginName);
    if (plugin == nullptr) {
        LOGERROR("No plugin loaded!");
        gPluginManager.Uninitialize();
        return false;
    }
    LOGINFO("Load Plugin %s successfully!", pluginName.c_str());

    if (!plugin->IsPluginActive()) {
        LOGERROR("Plugin %s was not activated!", pluginName.c_str());
        PluginDeInit();
        return false;
    }
    return true;
}

void OSRPlugin::PluginDeInit()
{
    gPluginManager.UnloadPlugin(pluginName);
    gPluginManager.Uninitialize();
}

void OSRPlugin::ExecuteOSR(const String localDir)
{
    bool ret = false;
    ret = PluginInit();
    if (!ret) {
        return;
    }

    // push ie_data.bin & input image here
    ret = SetAssetsDir(localDir);
    if (!ret) {
        PluginDeInit();
        return;
    }

    // setup inBuffer
    const String inputFn = "input.ppm";
    String imgPath = localDir;
    if(imgPath.length() != 0 && imgPath[imgPath.length() - 1] != '/') {
        imgPath += "/";
    }
    imgPath += inputFn;
    BufferDescriptor inBuffer, outBuffer;
    if (!ReadBuffer(inBuffer, imgPath)) {
        LOGERROR("Setup inBuffer failed!");
        PluginDeInit();
        return;
    }
    LOGINFO("inBuffer info:\ninput.width %d\ninput.height %d\ninput.len %d\n",
            inBuffer.width, inBuffer.height, inBuffer.len);

    // query image
    PluginConfig* pluginConfig = static_cast<PluginConfig*>(malloc(sizeof(PluginConfig)));
    if (pluginConfig == nullptr) {
        LOGERROR("malloc failed!");
        DeleteBuffer(inBuffer);
        PluginDeInit();
        return;
    }
    ret = QueryImage(inBuffer, pluginConfig);
    if (!ret) {
        free(pluginConfig);
        DeleteBuffer(inBuffer);
        PluginDeInit();
        return;
    }

    // setup outBuffer
    u32 outW, outH, estimatedTime;
    PixelFormat outFormat;
    outW = pluginConfig->output.width;
    outH = pluginConfig->output.height;
    outFormat = pluginConfig->output.format;
    estimatedTime = pluginConfig->estimatedCostTime;
    free(pluginConfig);
    LOGINFO("Query ImageEnhancing Info:\noutput.width %d\noutput.height %d\noutput.time %d\n",
            outW, outH, estimatedTime);
    if (!CreateBuffer(outBuffer, outW, outH, outFormat)) {
        LOGERROR("Setup outBuffer failed!");
        DeleteBuffer(inBuffer);
        PluginDeInit();
        return;
    }

    // output image with ppm format
    const String outputFnIESync = "output_ie_sync.ppm"; /* sync image enhance */
    LOGINFO("Test Sync ImageEnhancing!");
    String outPath = localDir + "/" + outputFnIESync;
    ret = ImageEnhancingSync(inBuffer, outBuffer);
    if (ret) {
        WriteBuffer(outBuffer, outPath);
        LOGINFO("Save to %s", outPath.c_str());
    }

    DeleteBuffer(inBuffer);
    DeleteBuffer(outBuffer);
    PluginDeInit();
}
#endif