# Copyright (C) 2019 The Huawei Source Project
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.1 or lower.

cmake_minimum_required(VERSION 3.4.1 FATAL_ERROR)

set(SRC_MAIN_DIR src/main/jni)
set(SRC_OTHER_DIR src/main/jni/vulkan)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++11 \
                     -DVK_USE_PLATFORM_ANDROID_KHR")

file(GLOB EXAMPLE_SRC "${SRC_MAIN_DIR}/*.cpp")
file(GLOB ADD_SOURCE "${SRC_OTHER_DIR}/*.cpp")

add_library(vulkan-fbo SHARED ${EXAMPLE_SRC} ${ADD_SOURCE})				 

# build native_app_glue as a static lib
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)

add_library( app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

include_directories(${SRC_OTHER_DIR})
include_directories(${APP_GLUE_DIR})


target_link_libraries(vulkan-fbo app-glue log android)

