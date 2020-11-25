/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: 
 */

#include "PluginDemo.h"

namespace CGKit {
const String pluginInfo = "PluginDemo:VERSION1.0,DES:VECTOR3 Dot&Cross operation";
static PluginDemo *pluginDemo;

PluginDemo::PluginDemo()
{
}

PluginDemo::~PluginDemo()
{
}

const String &PluginDemo::GetPluginInfo() const
{
    return pluginInfo;
}

bool PluginDemo::Initialize()
{
    LOGINFO("->PluginDemo: Initialize called");
    return true;
}

void PluginDemo::Uninitialize()
{
    LOGINFO("->PluginDemo: Uninitialize called");
}

bool PluginDemo::Execute(const Param &paramIn, Param &paramOut)
{
    LOGINFO("->PluginDemo: Execute called");
    if (!IsPluginActive()) {
        LOGERROR("->PluginDemo: Execute fail, not actived!");
        return false;
    }

    if (!paramIn.IsArray()) {
        LOGERROR("->PluginDemo: param not a array!");
        return false;
    }
    Param::Array arrayVal = paramIn.Get<Param::Array>();
    if (arrayVal.size() < 3) {
        LOGERROR("->PluginDemo: param size too short!");
        return false;
    }

    // get opcode
    Param p1 = arrayVal[0];
    if (!p1.IsInt()) {
        LOGERROR("->PluginDemo: p1 not a int!");
        return false;
    }
    int opCode = p1.Get<s32>();

    // get oprand 1
    Param p2 = arrayVal[1];
    if (!p2.IsVector3()) {
        LOGERROR("->PluginDemo: p2 not a Vector3!");
        return false;
    }

    // get oprand 2
    Param p3 = arrayVal[2];
    if (!p3.IsVector3()) {
        LOGERROR("->PluginDemo: p3 not a Vector3!");
        return false;
    }

    switch (opCode) {
        case OP_DOT: {
            Vector3 opRandDot1 = p2.Get<Vector3>();
            Vector3 opRandDot2 = p3.Get<Vector3>();
            LOGINFO("->PluginDemo: paramList: opCode %d, opRandDot1 %s, opRandDot2 %s",
                opCode, opRandDot1.ToString().c_str(), opRandDot2.ToString().c_str());
            f32 outDot = opRandDot1.Dot(opRandDot2);
            paramOut.Set<f32>(outDot);
            break;
        }
        case OP_CROSS: {
            Vector3 opRandCross1 = p2.Get<Vector3>();
            Vector3 opRandCross2 = p3.Get<Vector3>();
            LOGINFO("->PluginDemo: paramList: opCode %d, opRandCross1 %s, opRandCross2 %s",
                opCode, opRandCross1.ToString().c_str(), opRandCross2.ToString().c_str());
            Vector3 outCross = opRandCross1.Cross(opRandCross2);
            paramOut.Set<Vector3>(outCross);
            break;
        }
        default:
            LOGINFO("->PluginDemo: Unsupported code");
            return false;
    }
    return true;
}

extern "C" IPlugin *CreatePlugin(void)
{
    LOGINFO("->PluginDemo: CreatePlugin called");
    pluginDemo = new PluginDemo();
    return pluginDemo;
}

extern "C" void DestoryPlugin(IPlugin *plugin)
{
    LOGINFO("->PluginDemo: DestoryPlugin called");
    delete plugin;
}

}  // namespace CGKit
