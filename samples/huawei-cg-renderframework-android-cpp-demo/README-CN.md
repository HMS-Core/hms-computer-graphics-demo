# 华为图形计算服务示例代码

## 目录
 * [简介](#简介)
 * [开发准备](#开发准备)
 * [环境要求](#环境要求)
 * [运行结果](#运行结果)
 * [授权许可](#授权许可)
## 简介
华为图形计算服务（HUAWEI Computer Graphics Kit）示例代码封装了Vulkan接口，提供示例程序供您参考。<br>
下面介绍本示例代码的头文件。<br>

请先从([SDK DOWNLOAD](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Library-V5/sdk-download-0000001050441521-V5))获取SDK包。本示例在压缩包中的路径为: cgsdk-vulkanframework.zip/cgsdk-vulkanframework。<br>
cgsdk-vulkanframework 包含如下头文件:
 文件|描述
 ---|---
 include/CGRenderingFramework/Application:|Application API头文件。
 include/CGRenderingFramework/Core:|Core API的头文件。
 include/CGRenderingFramework/Log:|Log API的头文件。
 include/CGRenderingFramework/Math:|Math API的头文件。
 include/CGRenderingFramework/nolhmann:|JSON API的头文件。
 include/CGRenderingFramework/Rendering:|Rendering API的头文件。
 include/CGRenderingFramework/Resource:|Resource API的头文件。
 include/CGRenderingFramework/Scene:|Scene API的头文件。
 include/CGRenderingFramework/Utils:|Param API的头文件。

 cgsdk-vulkanframework 包含如下库文件：
 文件|描述
 ---|---
 libs/arm64-v8a/libcgkit.so:|64 位
 libs/armeabi-v7a/libcgkit.so:|32 位

 cgsdk-plugin 包含如下基础库文件：
 文件|描述
 ---|---
 libs/arm64-v8a/libPluginInterface.so:|64 位
 libs/armeabi-v7a/libPluginInterface.so:|32 位

 下表为cgsdk-plugin的其他头文件和库文件，其在压缩包中的路径为:cgsdk-plugin.zip/cgsdk-plugin-offlinesuperresolution.zip/cgsdk-plugin-offlinesuperresolution/cgsdk-plugin-offlinesuperresolution/.
 文件|描述
 ---|---
 pkg-for-cgsdk/include/OSRPluginCommon.h|离线超分插件API头文件。
 pkg-for-cgsdk/assets/ie_data.bin|离线超分插件的资源文件。
 pkg-for-cgsdk/assets/pluginList/plugin.json|离线超分插件的配置文件。
 pkg-for-cgsdk/libs/arm64-v8a/libcgkit_plugin_offlineSupRes.so|64 位

## 开发准备
1. 检查Android Studio开发环境。在Android Studio中打开示例代码工程目录。在设备或虚拟机上运行TestApp。本工程依赖于Huawei Mobile Service（HMS）。<br><br>
2. 注册 [华为账号](https://developer.huawei.com/consumer/en/)。<br><br>
3. 创建应用，生成签名证书指纹，配置AppGallery Connect。<br>
   详细操作请参见: [开发准备](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Guides/environment-req-0000001050200019)。<br><br>
4. 编译此demo前，请先将demo导入Android Studio（3.5及以上版本）。<br><br>
5. 配置示例代码：<br>
 ● 打开示例工程的应用级build.gradle文件，修改applicationid为应用包名。<br>
 ● 在根目录下添加签名证书（.jks），修改示例工程应用级build.gradle文件中的signingConfigs参数。<br>
 ● 参照[华为图形计算服务开发指南](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Guides/demo-data-process-0000001050200023)创建模型和材质<br>
 ● 添加CG渲染框架SDK、cgsdk-plugin-base SDK、cgsdk-plugin-offlinessupersolution SDK到以下目录。<br>
       
　　　[渲染框架SDK]<br>
　　　◉ 将SDK中的include目录添加到工程中（src/cpp/include）。<br>
　　　◉ 将SDK中的`libs/arm64-v8a/libcgkit.so`添加到工程中（libs/arm64-v8a）。<br>
　　　◉ 将SDK中的`libs/armeabi-v7a/libcgkit.so`添加到工程中（libs/armeabi-v7a）。<br>
       
　　　[cgsdk-plugin-base SDK]<br>
　　　◉ 将SDK中的`libs/arm64-v8a/libPluginInterface.so`添加到工程中（libs/arm64-v8a）。<br>
　　　◉ 将SDK中的`libs/armeabi-v7a/libPluginInterface.so`添加到工程中（libs/libPluginInterface-v7a）。<br>
       
　　　[cgsdk-plugin-offlinesuperresolution SDK]<br>
　　　◉ 将SDK中的`pkg-for-cgsdk/include/OSRPluginCommon.h`添加到工程中（src/main/cpp/include/OSRPlugin）。<br>
　　　◉ 将SDK中的`pkg-for-cgsdk/assets/ie_data.bin`添加到工程中（src/main/assets/resource）。<br>
　　　◉ 将SDK中的pluginList目录添加到工程中（`app/src/main/assets`）。<br>
　　　◉ 将SDK中的`libs/arm64-v8a/libcgkit_plugin_offlineSupRes.so`添加到工程中（`app/OSRPlugin/arm64-v8a`）。<br><br>

6. 在Android设备或虚拟机上运行Vulkan渲染demo。<br><br>
7. 在Android设备或虚拟机上运行离线超分插件demo。<br>
 ● 在app/src/main/assets/resource目录下运行run.bat。<br>
 ● 双击屏幕。<br>
 ● 从/sdcard/Android/data/com.hisi.CGRenderFrameworkDemo/output_ie_sync.ppm中拉取结果文件。<br>

## 环境要求
1. Android 8.0及以上的设备。<br>
2. Vulkan 1.0或Vulkan 1.1的设备。<br>

## 运行结果
 <img src="CGRenderResult.jpg" width="30%" height="30%">

## 授权许可
图形计算服务示例代码已获得[Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0)授权许可。
