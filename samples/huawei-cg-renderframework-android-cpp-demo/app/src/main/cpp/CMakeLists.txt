# CMake最低版本为3.10.2
CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)

SET(NAME renderdemo)
PROJECT(${NAME} LANGUAGES C CXX)

# 头文件包含目录
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CGRenderingFramework)

# 源码文件清单
FILE(GLOB PROJECT_FILES
    include/MainApplication/MainApplication.h
    include/OSRPlugin/*.h
    source/Main.cpp
    source/MainApplication.cpp
    source/OSRPlugin.cpp)

IF(NOT CMAKE_VERSION VERSION_LESS 3.7.0)
    FIND_PACKAGE(Vulkan)
ENDIF()

IF(NOT Vulkan_FOUND)
    MESSAGE("Vulkan not found. You should configure your vulkan sdk to project by yourself.")
ELSE()
    SET(PLATFORM_INCLUDE ${Vulkan_INCLUDE_DIRS})
ENDIF()

# Vulkan头文件包含路径
INCLUDE_DIRECTORIES(${PLATFORM_INCLUDE})

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # windows header file conflicts with std min and max, so undefine them
    add_definitions(-DNOMINMAX)
    # link cgkit.dll for building stage
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${CMAKE_VS_PLATFORM_NAME})
    ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_FILES})
    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        cgkit)

    source_group("Source Files\\MainApplication\\" FILES ${PROJECT_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD" )
    set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} /MD" )
    set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} /MDd" )

    # copy cgkit.dll to output directory for running stage, so the exe will load cgkit.dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${CMAKE_VS_PLATFORM_NAME}/cgkit.dll
        ./$(Configuration)/cgkit.dll)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Android")
    ADD_LIBRARY(${PROJECT_NAME} SHARED ${PROJECT_FILES})
    ADD_LIBRARY(cgkit SHARED IMPORTED)
    SET_TARGET_PROPERTIES(cgkit PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${ANDROID_ABI}/libcgkit.so)

    # native_app_glue路径
    SET(NATIVE_APP_GLUE_DIR "${ANDROID_NDK}/sources/android/native_app_glue")

    FILE(
        GLOB NATIVE_APP_GLUE_FILLES
        "${NATIVE_APP_GLUE_DIR}/*.c"
        "${NATIVE_APP_GLUE_DIR}/*.h")

    # 导入native_app_glue
    ADD_LIBRARY(native_app_glue STATIC ${NATIVE_APP_GLUE_FILLES})
    TARGET_INCLUDE_DIRECTORIES(native_app_glue PUBLIC ${NATIVE_APP_GLUE_DIR})

    # 依赖log模块
    FIND_LIBRARY(log-lib log)

    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        cgkit
        native_app_glue
        android
        ${log-lib})

    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
ENDIF()

# unset this macro to use import functions
remove_definitions(-DCGKIT_LIB)