/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: A sample that illustrates the rendering of a triangle.
 */

#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#define CGKIT_LOG
#include "Application/CGKitHeaders.h"

enum TouchCountNum {
    TOUCH_COUNT_MOVE = 1,
    TOUCH_COUNT_ZOOM,
    TOUCH_COUNT_MULTI_FINGER,
    TOUCH_COUNT_MAX = TOUCH_COUNT_MULTI_FINGER
};

class MainApplication : public CGKit::BaseApplication {
public:
    MainApplication();
    virtual ~MainApplication();
    virtual void Start(void* param);
    virtual void Initialize(void* winHandle, CGKit::u32 width, CGKit::u32 height);
    virtual void UnInitialize();
    virtual void InitScene();
    virtual void Update(CGKit::f32 deltaTime);
    virtual void Resize(CGKit::u32 width, CGKit::u32 height);
    virtual void ProcessInputEvent(const CGKit::InputEvent* inputEvent);

private:
    CGKit::SceneObject* CreateSkybox();
    void ProcessSingleTap(int64_t lastTouchDownTime, int64_t preTouchUpTime);
private:
    bool m_touchBegin;
    bool m_pluginTested {false};
    float m_touchPosX;
    float m_touchPosY;
    float m_deltaTime;
    float m_deltaAccumulate = 0.0f;
    float m_objectRotation = 0.0f;
    float m_objectScale = 1.0f;
    const CGKit::Vector3 SCENE_OBJECT_POSITION = CGKit::Vector3(0.0f, -18.0f, 40.0f);
    const CGKit::Vector3 SCENE_OBJECT_SCALE = CGKit::Vector3(30.f, 30.f, 30.f);
    CGKit::SceneObject* m_cameraObject = nullptr;
    CGKit::SceneObject* m_sceneObject = nullptr;
    CGKit::SceneObject* m_lightObject = nullptr;
    CGKit::SceneObject* m_skyObject = nullptr;
    CGKit::SceneObject* m_pointLightObject = nullptr;
    CGKit::Camera* m_mainCamera = nullptr;
    CGKit::String m_envMap = "cubemaps/env/cubemap.cub";
};

CGKit::BaseApplication* CreateMainApplication();

#endif