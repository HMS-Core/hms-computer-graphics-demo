@echo off
set APP_ALLOW_MISSING_DEPS=true
set NDK_TOOLS=D:\AndroidSDK\ndk\21.0.6113669
call %NDK_TOOLS%\build\ndk-build.cmd NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk NDK_APPLICATION_MK=Application.mk
pause