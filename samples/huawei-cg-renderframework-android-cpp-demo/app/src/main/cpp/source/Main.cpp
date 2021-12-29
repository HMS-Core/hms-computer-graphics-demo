/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: The entry point class in sample, use different defined to distinct platform handle.
 */

#include "MainApplication/MainApplication.h"

#ifdef CG_ANDROID_PLATFORM
#include <android_native_app_glue.h>
#endif

using namespace CGKit;

#ifdef CG_WINDOWS_PLATFORM
void main()
#elif defined CG_ANDROID_PLATFORM
void android_main(android_app* state)
#endif
{
    // Instantiate the demo home page.
    auto app = CreateMainApplication();
    if (app == nullptr) {
        return;
    }
    // Start platform rendering.
#ifdef CG_WINDOWS_PLATFORM
    app->Start(nullptr);
#else
    app->Start(reinterpret_cast<void*>(state));
#endif
    // Start the rendering main loop.
    app->MainLoop();
    CG_SAFE_DELETE(app);
}
