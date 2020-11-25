/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: 
 */

#ifndef PLUGIN_DEMO_H
#define PLUGIN_DEMO_H

#include "Utils/Param.h"
#include "PluginManager/IPlugin.h"
#include "Math/Vector3.h"
#include <android/log.h>

#define LOG_TAG      "PluginDemo"
#define LOGINFO(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGERROR(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

#define OP_DOT   1
#define OP_CROSS 2

namespace CGKit {

class PluginDemo : public IPlugin {
public:
    PluginDemo();

    virtual ~PluginDemo() override;

    virtual const String &GetPluginInfo() const override;

    virtual bool Execute(const Param &paramIn, Param &paramOut) override;

private:
    virtual bool Initialize() override;

    virtual void Uninitialize() override;
};

}  // namespace CGKit

#endif
