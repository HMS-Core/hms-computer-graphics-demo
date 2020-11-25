/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: BufferDescriptor
 */
#include <jni.h>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include "OSRPluginInterface.h"
#include "OSRPluginCommon.h"
#include "help.h"

using namespace std;
using namespace CGKit;

constexpr int MAX_STR_LEN = 1024;
constexpr int MODE_QUERY_SS = 0;
constexpr int MODE_EXECUTE_SS_SYNC = 0;
constexpr int MODE_EXECUTE_IE_SYNC = 1;
constexpr int MODE_EXECUTE_SS_ASYNC = 2;
constexpr int MODE_EXECUTE_IE_ASYNC = 3;
constexpr float DEFAULT_SHARPNESS = 0.6;
constexpr int TIMEOUT = 10000;
constexpr int SS_ASYNC_WAIT_SECOND = 5;
constexpr int IE_ASYNC_WAIT_SECOND = 2;

bool g_initFlag = false;
float g_ieSharpness = DEFAULT_SHARPNESS;
bool g_ieToneMapping = true;

std::unordered_map<PixelFormat, const char *> g_lutPixelFormat2String = {
    {PIXEL_FORMAT_R8G8B8_UNORM, "RGB888"},
    {PIXEL_FORMAT_R8G8B8A8_UNORM, "RGBA8888"}
};

static bool RealPath(const string &pathIn, string &pathOut)
{
    char *pRealPathIn = realpath(pathIn.c_str(), 0);
    if (pRealPathIn == nullptr) {
        return false;
    }
    pathOut = string(pRealPathIn);
    free(pRealPathIn);
    pRealPathIn = nullptr;
    return true;
}

void CallBack(bool success)
{
    return;
}

void Initialize()
{
    CInitialize();
    g_initFlag = true;
    LOGI("OSRDemo::Initialize", "Initialize Plugin Successfully");
}

void Uninitialize()
{
    if (g_initFlag) {
        CUninitialize();
        g_initFlag = false;
        LOGI("OSRDemo::Uninitialize", "Fail to Un-Initialize Plugin");
    } else {
        LOGI("OSRDemo::Uninitialize", "Plugin was not Initialized");
    }
}

void QuerySuperSamplingConfig(const string &inPath, int &outW, int &outH, PixelFormat &outFormat, int &estimatedTime)
{
    outW = -1;
    outH = -1;
    estimatedTime = -1;
    string realInPath;
    if (!RealPath(inPath, realInPath)) {
        LOGI("OSRDemo::QuerySuperSamplingConfig", "Illegal path");
        return;
    }
    BufferDescriptor inBuffer;
    if (!ReadBuffer(inBuffer, realInPath)) {
        LOGI("OSRDemo::QuerySuperSamplingConfig", "Cannot read path");
        return;
    }
    PluginConfig pluginConfig;
    CQuerySuperSamplingPluginConfig(inBuffer.width, inBuffer.height, inBuffer.format, pluginConfig);
    outW = pluginConfig.output.width;
    outH = pluginConfig.output.height;
    outFormat = pluginConfig.output.format;
    estimatedTime = pluginConfig.estimatedCostTime;
    DeleteBuffer(inBuffer);
}

void QueryImageEnhancingConfig(const string &inPath, int &outW, int &outH, PixelFormat &outFormat, int &estimatedTime)
{
    outW = -1;
    outH = -1;
    estimatedTime = -1;
    string realInPath;
    if (!RealPath(inPath, realInPath)) {
        LOGI("OSRDemo::QueryImageEnhancingConfig", "Illegal path %s", inPath.c_str());
        return;
    }
    BufferDescriptor inBuffer;
    if (!ReadBuffer(inBuffer, realInPath)) {
        LOGI("OSRDemo::QueryImageEnhancingConfig", "Cannot read path");
        return;
    }
    PluginConfig pluginConfig;
    CQueryImageEnhancingPluginConfig(inBuffer.width, inBuffer.height, inBuffer.format, pluginConfig);
    outW = pluginConfig.output.width;
    outH = pluginConfig.output.height;
    outFormat = pluginConfig.output.format;
    estimatedTime = pluginConfig.estimatedCostTime;
    DeleteBuffer(inBuffer);
}

bool SuperSamplingExecuteSync(const string &inPath, const string &outPath)
{
    if (!g_initFlag) {
        LOGE("OSRDemo::SuperSamplingExecuteSync", "Plugin was not Initialized");
        return false;
    }
    string realInPath, realOutPath;
    if (!RealPath(inPath, realInPath)) {
        LOGE("OSRDemo::SuperSamplingExecuteSync", "Illegal path");
        return false;
    }
    int outW, outH, estimatedTime;
    PixelFormat outFormat;
    QuerySuperSamplingConfig(inPath, outW, outH, outFormat, estimatedTime);
    if (outW < 0 || outH < 0 || estimatedTime < 0) {
        LOGE("OSRDemo::SuperSamplingExecuteSync", "Inappropriate Size or Unsupported Hardware");
        return false;
    }
    BufferDescriptor inBuffer, outBuffer;
    if (!ReadBuffer(inBuffer, realInPath)) {
        LOGE("OSRDemo::SuperSamplingExecuteSync", "Cannot read path");
        return false;
    }
    if (!CreateBuffer(outBuffer, outW, outH, outFormat)) {
        LOGE("OSRDemo::SuperSamplingExecuteSync", "Cannot create buffer");
        DeleteBuffer(inBuffer);
        return false;
    }
    bool success = CSuperSamplingSyncExecute(&inBuffer, &outBuffer, TIMEOUT);
    DeleteBuffer(inBuffer);
    if (!success) {
        LOGE("OSRDemo::SuperSamplingExecuteSync", "Execution Fails");
        DeleteBuffer(outBuffer);
        return false;
    }
    if (!WriteBuffer(outBuffer, outPath)) {
        LOGE("OSRDemo::SuperSamplingExecuteSync", "Wrtie Fails");
        DeleteBuffer(outBuffer);
        return false;
    }
    LOGI("OSRDemo::SuperSamplingExecuteSync", "Execution Succeeds");
    DeleteBuffer(outBuffer);
    return true;
}

bool ImageEnhancingExecuteSync(const string &inPath, const string &outPath, float sharpness, bool tonemapping)
{
    if (!g_initFlag) {
        LOGI("OSRDemo::ImageEnhancingExecuteSync", "Plugin was not Initialized");
        return false;
    }
    string realInPath, realOutPath;
    if (!RealPath(inPath, realInPath)) {
        LOGE("OSRDemo::ImageEnhancingExecuteSync", "Illegal path %s", inPath.c_str());
        return false;
    }
    int outW, outH, estimatedTime;
    PixelFormat outFormat;
    QueryImageEnhancingConfig(inPath, outW, outH, outFormat, estimatedTime);
    if (outW < 0 || outH < 0 || estimatedTime < 0) {
        LOGE("OSRDemo::ImageEnhancingExecuteSync", "Inappropriate Size or Unsupported Hardware");
        return false;
    }
    BufferDescriptor inBuffer, outBuffer;
    if (!ReadBuffer(inBuffer, realInPath)) {
        LOGE("OSRDemo::ImageEnhancingExecuteSync", "Cannot read path");
        return false;
    }
    if (!CreateBuffer(outBuffer, outW, outH, outFormat)) {
        LOGE("OSRDemo::ImageEnhancingExecuteSync", "Cannot create buffer");
        DeleteBuffer(inBuffer);
        return false;
    }
    bool success = CImageEnhancingSyncExecute(&inBuffer, &outBuffer, sharpness, tonemapping, TIMEOUT);
    DeleteBuffer(inBuffer);
    if (!success) {
        LOGE("OSRDemo::ImageEnhancingExecuteSync", "Execution Fails");
        DeleteBuffer(outBuffer);
        return false;
    }
    if (!WriteBuffer(outBuffer, outPath)) {
        LOGE("OSRDemo::ImageEnhancingExecuteSync", "Wrtie Fails");
        DeleteBuffer(outBuffer);
        return false;
    }
    LOGI("OSRDemo::ImageEnhancingExecuteSync", "Execution Succeeds");
    DeleteBuffer(outBuffer);
    return true;
}

bool SuperSamplingExecuteAsync(const string &inPath, const string &outPath)
{
    if (!g_initFlag) {
        LOGI("OSRDemo::SuperSamplingExecuteAsync", "Plugin was not Initialized");
        return false;
    }
    string realInPath, realOutPath;
    if (!RealPath(inPath, realInPath)) {
        LOGE("OSRDemo::SuperSamplingExecuteAsync", "Illegal path");
        return false;
    }
    int outW, outH, estimatedTime;
    PixelFormat outFormat;
    QuerySuperSamplingConfig(inPath, outW, outH, outFormat, estimatedTime);
    if (outW < 0 || outH < 0 || estimatedTime < 0) {
        LOGE("OSRDemo::SuperSamplingExecuteAsync", "Inappropriate Size or Unsupported Hardware");
        return false;
    }
    BufferDescriptor inBuffer, outBuffer;
    if (!ReadBuffer(inBuffer, realInPath)) {
        LOGE("OSRDemo::SuperSamplingExecuteAsync", "Cannot read path");
        return false;
    }
    if (!CreateBuffer(outBuffer, outW, outH, outFormat)) {
        LOGE("OSRDemo::SuperSamplingExecuteAsync", "Cannot create buffer");
        DeleteBuffer(inBuffer);
        return false;
    }
    bool success = CSuperSamplingAsyncExecute(&inBuffer, &outBuffer, CallBack);
    std::this_thread::sleep_for(std::chrono::seconds(SS_ASYNC_WAIT_SECOND));
    DeleteBuffer(inBuffer);
    if (!success) {
        LOGE("OSRDemo::SuperSamplingExecuteAsync", "Execution Fails");
        DeleteBuffer(outBuffer);
        return false;
    }
    if (!WriteBuffer(outBuffer, outPath)) {
        LOGE("OSRDemo::SuperSamplingExecuteAsync", "Wrtie Fails");
        DeleteBuffer(outBuffer);
        return false;
    }
    LOGI("OSRDemo::SuperSamplingExecuteAsync", "Execution Succeeds");
    DeleteBuffer(outBuffer);
    return true;
}

bool ImageEnhancingExecuteAsync(const string &inPath, const string &outPath, float sharpness, bool tonemapping)
{
    if (!g_initFlag) {
        LOGI("OSRDemo::ImageEnhancingExecuteAsync", "Plugin was not Initialized");
        return false;
    }
    string realInPath, realOutPath;
    if (!RealPath(inPath, realInPath)) {
        LOGE("OSRDemo::ImageEnhancingExecuteAsync", "Illegal path");
        return false;
    }
    int outW, outH, estimatedTime;
    PixelFormat outFormat;
    QueryImageEnhancingConfig(inPath, outW, outH, outFormat, estimatedTime);
    if (outW < 0 || outH < 0 || estimatedTime < 0) {
        LOGE("OSRDemo::ImageEnhancingExecuteAsync", "Inappropriate Size or Unsupported Hardware");
        return false;
    }
    BufferDescriptor inBuffer, outBuffer;
    if (!ReadBuffer(inBuffer, realInPath)) {
        LOGE("OSRDemo::ImageEnhancingExecuteAsync", "Cannot read path");
        return false;
    }
    if (!CreateBuffer(outBuffer, outW, outH, outFormat)) {
        LOGE("OSRDemo::ImageEnhancingExecuteAsync", "Cannot create buffer");
        DeleteBuffer(inBuffer);
        return false;
    }
    bool success = CImageEnhancingAsyncExecute(&inBuffer, &outBuffer, sharpness, tonemapping, CallBack);
    std::this_thread::sleep_for(std::chrono::seconds(IE_ASYNC_WAIT_SECOND));
    DeleteBuffer(inBuffer);
    if (!success) {
        LOGE("OSRDemo::ImageEnhancingExecuteAsync", "Execution Fails");
        DeleteBuffer(outBuffer);
        return false;
    }
    if (!WriteBuffer(outBuffer, outPath)) {
        LOGE("OSRDemo::ImageEnhancingExecuteAsync", "Wrtie Fails");
        DeleteBuffer(outBuffer);
        return false;
    }
    LOGI("OSRDemo::ImageEnhancingExecuteAsync", "Execution Succeeds");
    DeleteBuffer(outBuffer);
    return true;
}


extern "C" {
JNIEXPORT jboolean JNICALL Java_huawei_apk_Interface_Execute(JNIEnv *env, jclass cls, jint mode, jstring imageInputPath)
{
    int modeId = mode;
    string imageInPath; // 直接赋值给string即可
    if (imageInputPath != NULL) {
        const char *tmpDn  = env->GetStringUTFChars(imageInputPath, NULL);
        imageInPath = string(tmpDn);
        env->ReleaseStringUTFChars(imageInputPath, tmpDn);
    } else {
        LOGE("OSRDemo::", "invalid input path");
        return false;
    }

    time_t t = time(0);
    bool ret = false;
    switch (mode) {
        case MODE_EXECUTE_SS_SYNC: {
            string imageOutPath = "/sdcard/osr/SS_Sync.ppm";
            LOGI("OSRDemo::", "save to %s", imageOutPath.c_str());
            ret = SuperSamplingExecuteSync(imageInPath, imageOutPath);
            break;
        }
        case MODE_EXECUTE_IE_SYNC: {
            string imageOutPath = "/sdcard/osr/IE_Sync.ppm";
            ret = ImageEnhancingExecuteSync(imageInPath, imageOutPath, g_ieSharpness,
                                            g_ieToneMapping);
            break;
        }
        case MODE_EXECUTE_SS_ASYNC: {
            string imageOutPath = "/sdcard/osr/SS_Async.ppm";
            LOGI("OSRDemo::", "save to %s", imageOutPath.c_str());
            ret = SuperSamplingExecuteAsync(imageInPath, imageOutPath);
            break;
        }
        case MODE_EXECUTE_IE_ASYNC: {
            string imageOutPath = "/sdcard/osr/IE_Async.ppm";
            ret = ImageEnhancingExecuteAsync(imageInPath, imageOutPath, g_ieSharpness,
                g_ieToneMapping);
            break;
        }
        default:
            ret = false;
    }
    return ret;
}

JNIEXPORT jcharArray JNICALL Java_huawei_apk_Interface_Query(JNIEnv *env, jclass cls, jint mode, jstring imageInputPath)
{
    int modeId = mode;
    string imageInPath; // 直接赋值给string即可
    if (imageInputPath != NULL) {
        const char *tmpDn  = env->GetStringUTFChars(imageInputPath, NULL);
        imageInPath = string(tmpDn);
        env->ReleaseStringUTFChars(imageInputPath, tmpDn);
    } else {
        LOGE("OSRDemo::", "invalid input path");
        jcharArray info = env->NewCharArray(1);
        jchar *pInfo = env->GetCharArrayElements(info, NULL);
        pInfo[0] = '\0';
        env->ReleaseCharArrayElements(info, pInfo, 0);
        return info;
    }

    int outW;
    int outH;
    PixelFormat outFormat;
    int estimatedTime;
    if (mode == MODE_QUERY_SS) {
        QuerySuperSamplingConfig(imageInPath, outW, outH, outFormat, estimatedTime);
    } else {
        QueryImageEnhancingConfig(imageInPath, outW, outH, outFormat, estimatedTime);
    }

    string format = "Unsupprted";
    if (g_lutPixelFormat2String.find(outFormat) != g_lutPixelFormat2String.end()) {
        format = g_lutPixelFormat2String[outFormat];
    }

    jcharArray info = env->NewCharArray(MAX_STR_LEN);
    jchar *pInfo = env->GetCharArrayElements(info, NULL);
    string s = "Output Width: " + to_string(outW) +
        "\nOutput Height: " + to_string(outH) +
        "\nOutput Format: " + format +
        "\nReference Time: " + to_string(estimatedTime) + "\n";
    for (int ii = 0; ii <= s.length(); ++ii) {
        pInfo[ii] = s[ii];
    }
    env->ReleaseCharArrayElements(info, pInfo, 0);
    return info;
}

JNIEXPORT jboolean  JNICALL Java_huawei_apk_Interface_Initialize(JNIEnv *env, jclass cls)
{
    Initialize();
    return true;
}

JNIEXPORT jboolean  JNICALL Java_huawei_apk_Interface_Uninitialize(JNIEnv *env, jclass cls)
{
    Uninitialize();
    return true;
}

JNIEXPORT void  JNICALL Java_huawei_apk_Interface_SetSharpness(JNIEnv *env, jclass cls, jfloat s)
{
    g_ieSharpness = s;
    return;
}

JNIEXPORT jboolean  JNICALL Java_huawei_apk_Interface_SwitchToneMapping(JNIEnv *env, jclass cls)
{
    g_ieToneMapping = !g_ieToneMapping;
    return g_ieToneMapping;
}
}

