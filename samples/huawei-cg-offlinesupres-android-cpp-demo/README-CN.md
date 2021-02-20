# 华为图形计算服务示例代码

## 目录
 * [简介](#简介)
 * [开发准备](#开发准备)
 * [环境要求](#环境要求)
 * [授权许可](#授权许可)
## 简介
离线超分插件示例代码提供离线超分插件样例程序，供您参考。<br>
下表为示例代码头文件。
    
从[SDK DOWNLOAD](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Library-V5/sdk-download-0000001050441521-V5)获取SDK包。本示例代码头文件在压缩包中的路径为: cgsdk-plugin.zip/cgsdk-plugin-offlinesuperresolution.zip/cgsdk-plugin-offlinesuperresolution/pkg-for-thirdpartyengine/<br>
 文件|描述
 ---|---
   include/OSRPluginCommon.h:|必要的枚举和结构头文件。<br>
   include/OSRPluginInterface.h:|API头文件。<br>
   libs/arm64-v8a/libthirdpartyengine_plugin_offlineSupRes.so:|64位

## 开发准备
1. 将测试图片文件（.ppm）传输到设备上。<br><br>
2. 如有要求，将asset文件（ie_data.bin和ss_data.om）传输到设备的`/sdcard/osr`目录下。<br><br>
3. 将SDK包中的`cgsdk-plugin-offlinessupersolution/pkg-for-hirdpartyengine/libs`文件夹拷贝到demo的"app"目录下。<br><br>
4. 检查Android Studio开发环境。在Android Studio中打开示例代码工程目录下的"build.gradle"的文件。在设备或虚拟机上运行TestApp。<br><br>
5. 运行TestApp.<br>
 (1) 初始化插件。<br><br>
 (2) 选择测试图片。<br><br>
 (3) 执行超采样。<br>
　 (a) 查询是否支持超采样。<br>
　 如果支持:<br>
　 (b) 同步执行超采样。结果保存在`/sdcard/osr/SS_Sync.ppm`中。<br>
　 (c) 或异步执行超采样。结果保存在`/sdcard/osr/SS_Async.ppm`中。<br><br>
 (4) 或执行图像增强。<br>
　 (a) 查询是否支持图像增强。<br>
　  如果支持:<br>
　 (b) 设置清晰度和亮度调整。<br>
　 (c) 同步执行图像增强。结果保存在`/sdcard/osr/IE_Sync.ppm`中。<br>
　 (d) 或异步执行图像增强。结果保存在`/sdcard/osr/IE_Async.ppm`中。<br><br>
6. 反初始化插件。<br>

## 环境要求
1. Android 8.0及以上的设备。<br>
2. OpenGL ES 3.x.的设备。<br>
3. arm64-v8a架构的设备。<br>

## 授权许可
　　离线超分插件示例代码已获得 [Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0)授权许可。
