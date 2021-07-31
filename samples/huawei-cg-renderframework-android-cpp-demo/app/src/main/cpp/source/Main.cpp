/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: The entry point class in sample, use different defined to distinct platform handle.
 */

#include "MainApplication/MainApplication.h"

using namespace CGKit;

void android_main(android_app* state)
{
    // Instantiate the demo home page.
    auto app = CreateMainApplication();
    if (app == nullptr) {
        return;
    }
    // Start platform rendering.
    app->Start(reinterpret_cast<void*>(state));
    // Start the rendering main loop.
    app->MainLoop();
    CG_SAFE_DELETE(app);
}
