# 图形计算服务插件示例代码

## 目录
 * [简介](#简介)
 * [开发准备](#开发准备)
 * [环境要求](#环境要求)
 * [授权许可](#授权许可)
## 简介
华为图形计算服务（HUAWEI Computer Graphics Kit）插件示例代码演示了插件开发过程，并提供示例程序供您参考。<br>
下面介绍本示例代码的头文件。

SDK说明([SDK DOWNLOAD](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Library-V5/sdk-download-0000001050441521-V5)):

[cgsdk-plugin-base SDK] 路径: cgsdk-plugin.zip/cgsdk-plugin-base.zip/cgsdk-plugin-base/<br>
 文件|描述
 ---|---
 Include/Core:|实例化API的头文件。
 Include/Math:|Math API的头文件。
 Include/nlohmann:|JSON API的头文件。
 Include/PluginManager:|PluginManager接口头文件。
 Include/Utils:|Param API的头文件。
 libs/arm64-v8a/libPluginInterface.so:|64位
 libs/armeabi-v7a/libPluginInterface.so:|32位

## 开发准备
1. 将"cgsdk-plugin-base"SDK添加到工程中（PluginSdk）。<br>
 (1) 将SDK中的include目录添加到工程中（PluginSdk/Include）。<br>
 (2) 将SDK中的`libs/arm64-v8a/libPluginInterface.so`添加到工程中（PluginSdk/libs/arm64-v8a）。<br>
 (3) 将SDK中的`libs/armeabi-v7a/libPluginInterface.so`添加到工程中（PluginSdk/libs/armeabi-v7a）。<br>

2. 检查NDK开发环境。推荐使用NDK r21及以上版本。<br>
3. 配置build.bat，设置NDK_TOOLS路径。<br>
4. 执行build.bat生成插件。<br>

## 环境要求
1. Android 8.0及以上的设备。<br>
2. EMUI和Vulkan 1.0或1.1版本的设备。<br>

## 授权许可
图形计算服务插件示例代码已获得[Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0)授权许可。
