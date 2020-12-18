# CGKit Plugin sample

## Table of Contents
 * [Introduction](#introduction)
 * [Getting Started](#getting-started)
 * [Supported Environments](#supported-environments)
 * [License](#license)
## Introduction
CGKit Plugin sample code show development of plugin. It provides many sample programs for your reference or usage.
The following describes heads files of sample code.

[SDK DOWNLOAD](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Library-V5/sdk-download-0000001050441521-V5) in Software/lib

[cgsdk-plugin-base SDK] in Software/lib/cgsdk-plugin/cgsdk-plugin-base
    Include/CGRenderingFramework/Core:           Head file of instantiate apis.
    Include/CGRenderingFramework/Math:           Head file of math apis.
    Include/CGRenderingFramework/nlohmann:       Head file of json apis.
    Include/CGRenderingFramework/PluginManager:  Head file of PluginManager apis.
    Include/CGRenderingFramework/Utils:          Head file of Param apis.

    libs/arm64-v8a/libPluginInterface.so:                  64 bit
    libs/armeabi-v7a/libPluginInterface.so:                32 bit

## Getting Started
1. Add cgsdk-plugin-base SDK to to your own project(PluginSdk).

(1) Add include directory in SDK to your own project(PluginSdk/Include).

(2) Add libs/arm64-v8a/libPluginInterface.so in SDK to your own project(PluginSdk/libs/arm64-v8a).

(3) Add libs/armeabi-v7a/libPluginInterface.so in SDK to your own project(PluginSdk/libs/armeabi-v7a).

2. Check whether the NDK development environment is ready, recommand version r21 or higher.
3. Configure the build.bat, set you own NDK_TOOLS path.
4. Execute build.bat to generate the plugin.

## Supported Environments
1. Devices with Android 8.0 or later.
2. Devices with EMUI Vulkan1.0 or Vulkan1.1.

## License
The sample of CGKit Plugin has obtained the [Apache 2.0 license.](http://www.apache.org/licenses/LICENSE-2.0).
