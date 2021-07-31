# CMake最低版本为3.10.2
CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)

# 头文件包含目录
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/CGRenderingFramework)

# libmain.so源码文件清单
ADD_LIBRARY(
        main-lib
        SHARED
        source/Main.cpp
        source/MainApplication.cpp
        source/OSRPlugin.cpp)

# libcgkit.so导入
ADD_LIBRARY(
        cgkit
        SHARED
        IMPORTED)

# libcgkit.so导入路径
SET_TARGET_PROPERTIES(cgkit
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI}/libcgkit.so
        )

# Vulkan头文件包含路径
SET(
        VULKAN_INCLUDE_DIR
        "$ENV{VULKAN_SDK}/include")
        #"${ANDROID_NDK}/sources/third_party/vulkan/src/include")

INCLUDE_DIRECTORIES(${VULKAN_INCLUDE_DIR})

# native_app_glue路径
SET(
        NATIVE_APP_GLUE_DIR
        "${ANDROID_NDK}/sources/android/native_app_glue")

FILE(
        GLOB NATIVE_APP_GLUE_FILLES
        "${NATIVE_APP_GLUE_DIR}/*.c"
        "${NATIVE_APP_GLUE_DIR}/*.h")

# 导入native_app_glue
ADD_LIBRARY(native_app_glue
        STATIC
        ${NATIVE_APP_GLUE_FILLES})

TARGET_INCLUDE_DIRECTORIES(
        native_app_glue
        PUBLIC
        ${NATIVE_APP_GLUE_DIR})

# 依赖log模块
FIND_LIBRARY(
        log-lib
        log )

TARGET_LINK_LIBRARIES(
        main-lib
        cgkit
        native_app_glue
        android
        ${log-lib} )

SET(
        CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
