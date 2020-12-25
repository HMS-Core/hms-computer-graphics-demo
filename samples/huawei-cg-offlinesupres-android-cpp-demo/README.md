# CG Kit

## Table of Contents
 * [Introduction](#introduction)
 * [Getting Started](#getting-started)
 * [Supported Environments](#supported-environments)
 * [Result](#result)
 * [License](#license)
## Introduction
OSRPlugin sample code provides sample programs for reference or usage of offline super-resolution plugin.<br>
The following describes heads files of sample code.
    
([SDK DOWNLOAD](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Library-V5/sdk-download-0000001050441521-V5))The path in the compressed package is: cgsdk-plugin.zip/cgsdk-plugin-offlinesuperresolution.zip/cgsdk-plugin-offlinesuperresolution/pkg-for-thirdpartyengine/<br>
 File|Description
 ---|---
   include/OSRPluginCommon.h:|Head file of necessary enumerations and structures.<br>
   include/OSRPluginInterface.h:|Head file of apis.<br>
   libs/arm64-v8a/libthirdpartyengine_plugin_offlineSupRes.so:|64 bit

## Getting Started
1. Push the test image file(.ppm) to your mobile.<br><br>
2. Push asset files(ie_data.bin, and ss_data.om if required) to `/sdcard/osr` on mobile.<br><br>
3. Copy folder `cgsdk-plugin-offlinesuperresolution/pkg-for-thirdpartyengine/libs` in SDK to "app" derectory in this demo.<br><br>
4. Check whether the Android studio development environment is ready. Open the sample code project directory with file "build.gradle" in Android Studio. Run on your device or simulator.<br><br>
5. Run the test app.<br>
 (1) Initialize the plugin.<br><br>
 (2) Select a test image.<br><br>
 (3) Run super-sampling.<br>
　 (a) Query super-sampling support.<br>
　 If supported:<br>
　 (b) Run super-sampling synchronously. The result will be saved to `/sdcard/osr/SS_Sync.ppm`<br>
　 (c) Or run super-sampling asynchronously. The result will be saved to `/sdcard/osr/SS_Async.ppm`<br><br>
 (4) Or run image-enhancing.<br>
　 (a) Query image-enhancing support.<br>
　 If supported:<br>
　 (b) Set sharpness and tone-mapping.<br>
　 (c) Run image-enhancing synchronously. The result will be saved to `/sdcard/osr/IE_Sync.ppm`<br>
　 (d) Or run image-enhancing asynchronously. The result will be saved to `/sdcard/osr/IE_Async.ppm`<br><br>
6. Uninitialize the plugin.<br>

## Supported Environments
1. Devices with Android 8.0 or later.<br>
2. Devices with OpenGL ES 3.x.<br>
3. Devices with arm64-v8a platform.<br>

## License
　　The sample of OSRPlugin has obtained the [Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0).
