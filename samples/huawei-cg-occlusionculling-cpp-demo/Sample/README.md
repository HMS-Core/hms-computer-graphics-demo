# Occlusion Culling

## Table of Contents
 * [Introduction](#introduction)
 * [Getting Started](#getting-started)
 * [Supported Environments](#supported-environments)
 * [Result](#result)
 * [License](#license)

## Introduction
Occlusion Culling sample code provides reference of how to use APIs and configurable items of Occlusion Culling.

## Getting Started
1. Download the [cgsdk-plugin](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Library-V5/sdk-download-0000001050441521-V5) and unpackage the cgsdk-plugin-occlusionculling directory to the sample directory.

2. Build the sample:

    Execute the "ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application.mk APP_BUILD_SCRIPT=Android.mk" command in the sample directory.

3. Run the sample:

    Execute the "adb push" command to push "libpartythirdengine_plugin_oc.so" and "Sample" in the "Sample\obj\local\arm64-v8a" folder to the "/data/local/tmp" directory of   the phone.

    Execute the `export LD_LIBRARY_PATH=/data/local/tmp` command in the shell.

    Execute `chmod a+x Sample` command in the "/data/local/tmp" directory.

    Execute the `./Sample` command in the "/data/local/tmp" directory.

## Supported Environments
1. Devices with Android 7.0 or later.

## Result
<img src="OcclusionCullingResult.jpg" width="30%" height="30%">

## License
The sample of Occlusion Culling has obtained the [Apache 2.0 license.](http://www.apache.org/licenses/LICENSE-2.0).
