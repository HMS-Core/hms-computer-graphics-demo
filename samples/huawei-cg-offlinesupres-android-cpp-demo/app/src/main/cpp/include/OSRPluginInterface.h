/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:
 */

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include "OSRPluginCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
     * @brief The callback of listener for asynchronous execution.
 */
typedef void (*PluginCallback)(bool success);

/**
     * @brief Initialize the plugin manager and all the plugins managed.
 */
void CInitialize();

/**
     * @brief Uninitialize the plugin manager and all the plugins managed.
 */
void CUninitialize();

/**
     * @brief Set the path to asset files.
 */
void CSetAssetsDir(const char *assetsPath);

/**
     * @brief Get the path to asset files.
 */
const char *CGetAssetsDir();

/**
     * @brief Synchronously execute super sampling.
     *
     * @param[in]      inBuffer     The descriptor of input buffer. The caller is responsible for allocating
     * and freeing buffer.
     * @param[in]      outBuffer    The descriptor of output buffer. The caller is responsible for allocating
     * and freeing buffer.
     * @param[in]      timeOut      Timeout millisecond.
     * @return @c true if super sampling is sucess, @c false otherwise.
 */
bool CSuperSamplingSyncExecute(const CGKit::BufferDescriptor *inBuffer, const CGKit::BufferDescriptor *outBuffer,
    int timeOut);

/**
     * @brief Synchronously execute image enhancement.
     *
     * @param[in]      inBuffer         The descriptor of input buffer. The caller is responsible for allocating
     * and freeing buffer.
     * @param[in]      outBuffer        The descriptor of output buffer. The caller is responsible for allocating
     * and freeing buffer.
     * @param[in]      sharpenStrength  Sharpen strength, range of which is between 0.0 and 1.0.
     * @param[in]      toneMapping      Decide whether to turn on the brightness adjustment function.
     * @param[in]      timeOut          Timeout millisecond.
     * @return @c true if super sampling is success, @c false otherwise.
 */
bool CImageEnhancingSyncExecute(const CGKit::BufferDescriptor *inBuffer, const CGKit::BufferDescriptor *outBuffer,
    float sharpenStrength, bool toneMapping, int timeOut);

/**
     * @brief Asynchronously execute super sampling. When super sampling is finished, the IPluginListener::OnFinish will
     * be called.
     *
     * @param[in]      inBuffer     The descriptor of input buffer.
     * @param[in]      outBuffer    The descriptor of output buffer.
     * @param[in]      callback     The listener for asynchronous execution.
     * @return @c true if super sampling task successful start, @c false otherwise.
 */
bool CSuperSamplingAsyncExecute(const CGKit::BufferDescriptor *inBuffer, const CGKit::BufferDescriptor *outBuffer,
    PluginCallback callback);

/**
     * @brief Asynchronously execute image enhancement. When image enhancement is finished, the
     * IPluginListener::OnFinish will be called.
     *
     * @param[in]      inBuffer         The descriptor of input buffer. The caller is responsible for allocating
     * and freeing buffer.
     * @param[in]      outBuffer        The descriptor of output buffer. The caller is responsible for allocating
     * and freeing buffer.
     * @param[in]      sharpenStrength  Sharpen strength, range of which is between 0.0 and 1.0.
     * @param[in]      toneMapping      Decide whether to turn on the brightness adjustment function.
     * @param[in]      callback         The listener for asynchronous execution.
     * @return @c true if image enhancement task successful start, @c false otherwise.
 */
bool CImageEnhancingAsyncExecute(const CGKit::BufferDescriptor *inBuffer, const CGKit::BufferDescriptor *outBuffer,
    float sharpenStrength, bool toneMapping, PluginCallback callback);

/**
 * @brief  Query ImageEnhancing Plugin config.
 *
 * @param[in]      inW              The input buffer width.
 * @param[in]      inH              The input buffer height.
 * @param[in]      inFormat         The input buffer format.
 * @param[out]     pluginConfig     Thes output buffer PluginConfig,include width,height,format and estimatedCostTime.
 */
void CQueryImageEnhancingPluginConfig(int inW, int inH, CGKit::PixelFormat inFormat,
    CGKit::PluginConfig &pluginConfig);

/**
 * @brief  Query SuperSampling Plugin config.
 *
 * @param[in]      inW              The input buffer width.
 * @param[in]      inH              The input buffer height.
 * @param[in]      inFormat         The input buffer format.
 * @param[out]     pluginConfig     Thes output buffer PluginConfig,include width,height,format and estimatedCostTime.
 */
void CQuerySuperSamplingPluginConfig(int inW, int inH, CGKit::PixelFormat inFormat,
    CGKit::PluginConfig &pluginConfig);

#ifdef __cplusplus
}
#endif
#endif // !PLUGININTERFACE_H
