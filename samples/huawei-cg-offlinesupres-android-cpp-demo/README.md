# CG Kit

## Table of Contents
 * [Introduction](#introduction)
 * [Getting Started](#getting-started)
 * [Supported Environments](#supported-environments)
 * [Result](#result)
 * [License](#license)
## Introduction
OSRPlugin sample code provides sample programs for reference or usage of offline super-resolution plugin.
The following describes heads files of sample code.
    
    [SDK DOWNLOAD](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Library-V5/sdk-download-0000001050441521-V5) in Software/lib
    include/OSRPluginCommon.h:                                       Head file of necessary enumerations and structures.
    include/OSRPluginInterface.h:                                    Head file of apis.
    
    libs/arm64-v8a/libthirdpartyengine_plugin_offlineSupRes.so:      64 bit

## Getting Started
    1. Push the test image file(.ppm) to your mobile.
    2. Push asset files(ie_data.bin, and ss_data.om if required) to "/sdcard/osr" on mobile.
    3. Copy folder "cgsdk-plugin-offlinesuperresolution/pkg-for-thirdpartyengine/libs" in SDK to "app" derectory in this demo
    4. Check whether the Android studio development environment is ready. Open the sample code project directory with file "build.gradle" in Android Studio. Run on your device or simulator.
    5. Run the test app.
       (1) Initialize the plugin.
       (2) Select a test image.
       (3) Run super-sampling.
          (a) Query super-sampling support.
          If supported:
          (b) Run super-sampling synchronously. The result will be saved to "/sdcard/osr/SS_Sync.ppm"
          (c) Or run super-sampling asynchronously. The result will be saved to "/sdcard/osr/SS_Async.ppm"
       (4) Or run image-enhancing.
          (a) Query image-enhancing support.
          If supported:
          (b) Set sharpness and tone-mapping.
          (c) Run image-enhancing synchronously. The result will be saved to "/sdcard/osr/IE_Sync.ppm"
          (d) Or run image-enhancing asynchronously. The result will be saved to "/sdcard/osr/IE_Async.ppm"
    6. Uninitialize the plugin.

## Supported Environments
    1. Devices with Android 8.0 or later.
    2. Devices with OpenGL ES 3.x.
    3. Devices with arm64-v8a platform.

## License
    The sample of OSRPlugin has obtained the [Apache 2.0 license.](http://www.apache.org/licenses/LICENSE-2.0).
