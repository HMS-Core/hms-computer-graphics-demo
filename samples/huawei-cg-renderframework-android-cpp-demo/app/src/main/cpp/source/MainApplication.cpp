/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: A sample that illustrates the rendering of a demo scene.
 */

// Define the CGKIT_LOG macro to enable logging.
#define CGKIT_LOG
#include "MainApplication/MainApplication.h"
#include "OSRPlugin/OSRPlugin.h"
#include <sys/time.h>

using namespace CGKit;

const u32 HALF_CORE_NUM = 4;

// Set the interval of double-tap events, in milliseconds.
const u64 DOUBLE_TAP_INTERVAL_MS = 500;

MainApplication::MainApplication() {}

MainApplication::~MainApplication() {}

// Start CG Kit and configure system event listening.
void MainApplication::Start(void* param)
{
    gCGKitInterface.SetSceneManagerType(SCENE_MANAGER_TYPE_QUADTREE);
    BaseApplication::Start(param);

    // Set the log level to LOG_VERBOSE to overwrite the default log level of CG Kit.
    Log::SetLogLevel(LOG_VERBOSE);
}

// Initialize the resource manager, scene manager, and renderer of CG Kit.
void MainApplication::Initialize(const void* winHandle, u32 width, u32 height)
{
    // Set the number of threads.
    gCGKitInterface.SetThreadCount(HALF_CORE_NUM);
    // Turn on the multi-threaded rendering switch, the default is false to not star.
    gCGKitInterface.SetMultiThreadRendering(true);
    BaseApplication::Initialize(winHandle, width, height);
}

// Destroy allocations.
void MainApplication::Uninitialize()
{
    // Destroy the camera object.
    if (m_cameraObject != nullptr) {
        gSceneManager.DeleteObject(m_cameraObject);
        m_mainCamera = nullptr;
        m_cameraObject = nullptr;
    }

    // Destroy the model.
    if (m_modelObject != nullptr) {
        gSceneManager.DeleteObject(m_modelObject);
        m_modelObject = nullptr;
    }

    // Destroy the skybox.
    if (m_skyObject != nullptr) {
        gSceneManager.DeleteObject(m_skyObject);
        m_skyObject = nullptr;
    }

    // Destroy the directional light source.
    if (m_directionalLightObject != nullptr) {
        gSceneManager.DeleteObject(m_directionalLightObject);
        m_directionalLightObject = nullptr;
    }

    // Destroy the point light source.
    if (m_pointLightObject != nullptr) {
        gSceneManager.DeleteObject(m_pointLightObject);
        m_pointLightObject = nullptr;
    }

    // Destroy all your allocations and then call the destruction method of CG Kit to uninitialize the system.
    BaseApplication::Uninitialize();
}

/*
 * Initialize the scene.
 * Add a camera, set the default model, set the skybox, and add directional light and point light sources.
 */
void MainApplication::InitScene()
{
    LOGINFO("MainApplication InitScene.");
    BaseApplication::InitScene();
    const u32 sceneWidth = 1024;
    const u32 sceneHeight = 1024;
    const u32 sceneDepth = 1024;
    const Vector3 center = { 0.0, 0.0, 0.0 };
    gSceneManager.InitScene(center, sceneWidth, sceneHeight, sceneDepth);

    // Set the camera.
    if(!SetupCamera()) {
        return;
    }

    // Set the default model.
    if(!SetupDefaultModel()) {
        return;
    }

    // Set the skybox.
    m_skyObject = CreateSkybox();
    if (m_skyObject != nullptr) {
        // The skybox needs to be large enough to contain all the objects in the scene.
        m_skyObject->SetScale(Vector3(100.f, 100.f, 100.f));
    }

    // Add a directional light source.
    AddDirectionalLight();

    // Add a point light source.
    AddPointLight();
}

// Set the update logic every frame.
void MainApplication::Update(float deltaTime)
{
    // deltaTime indicates the interval between consecutive frames, in seconds.
    m_deltaTime = deltaTime;
    m_deltaAccumulate += deltaTime;

    // Set the rotation and scale for each frame. The m_objectRotation and m_objectScale variables are dynamically changed in gesture events.
    if (m_modelObject != nullptr) {
        m_modelObject->SetRotation(Vector3(0.0, m_objectRotation, 0.0));
        m_modelObject->SetScale(SCENE_OBJECT_SCALE * m_objectScale);
    }

    // Adjust the light position in real time.
    const f32 moveRatioX = 0.2f;
    const f32 moveRatioY = 0.5f;
    const f32 pointLightCircle = 50.f;
    if (m_pointLightObject != nullptr) {
        m_pointLightObject->SetPosition(Vector3(sin(m_deltaAccumulate * moveRatioX) * pointLightCircle,
            sin(m_deltaAccumulate * moveRatioY) * pointLightCircle + pointLightCircle,
            cos(m_deltaAccumulate * moveRatioX) * pointLightCircle));
    }

    // Call the default update logic of CG Kit.
    BaseApplication::Update(deltaTime);
}

// Deal with window size changes.
void MainApplication::Resize(u32 width, u32 height)
{
    /*
     * Set the viewport.
     * If the viewport is not set, the original viewport is used after the scene is rotated, resulting in the failure to fill the entire screen with the rendered image.
     */
    if (m_mainCamera != nullptr) {
        m_mainCamera->SetViewport(0, 0, width, height);
    }

    // Continue with the default processing logic of CG Kit. Pay attention to the calling sequence, that is, set the viewport, and then pass the updated arguments to CG Kit to match the changed window size.
    BaseApplication::Resize(width, height);
}

/*
 * Process input events.
 * CG Kit supports multi-touch events. This demo demonstrates only single-touch events.
 * Realize the support of model rotation by dragging the model side to side, and model scaling by dragging the model up or down.
 */
void MainApplication::ProcessInputEvent(const InputEvent* inputEvent)
{
    if (inputEvent == nullptr) {
        LOGERROR("Received null event.");
        return;
    }

    // Call the default processing logic of CG Kit.
    BaseApplication::ProcessInputEvent(inputEvent);

    // Customize input events.
    EventSource source = inputEvent->GetSource();
    static bool tapDetected = false;
    static s64 lastTime = 0;

    // This demo demonstrates only touch events.
    if(source != EVENT_SOURCE_TOUCHSCREEN) {
        return;
    }

    const TouchInputEvent* touchEvent = reinterpret_cast<const TouchInputEvent*>(inputEvent);

    // This demo demonstrates only the single-finger slide and scaling functions.
    if (touchEvent->GetTouchCount() > 1) {
        return;
    }

    // Touch action: press
    if (touchEvent->GetAction() == TOUCH_ACTION_DOWN) {
        LOGINFO("Action move start.");
        m_touchBegin = true;
        // Two taps happening within the double-tap interval constitute a double-tap action.
        if (!m_osrPluginExecuted && tapDetected && (abs(GetCurrentTime() - lastTime) < DOUBLE_TAP_INTERVAL_MS)) {
            ExecuteOSRPlugin();
        }
    } else if (touchEvent->GetAction() == TOUCH_ACTION_MOVE) {  // Touch action: move
        // Ensure that the touch is started to obtain the coordinates of last touch.
        if (!m_touchBegin) {
            return;
        }

        // Obtain the difference between the coordinates of the current and previous touch events for calculating the model change.
        f32 touchPosDeltaX = touchEvent->GetPosX(touchEvent->GetTouchIndex()) - m_touchPosX;
        f32 touchPosDeltaY = touchEvent->GetPosY(touchEvent->GetTouchIndex()) - m_touchPosY;

        // A valid event is determined only when the difference between the coordinates of the current and previous events exceeds the preset value, to prevent unintentional vibration.
        if (fabs(touchPosDeltaX) > 2.f) {
            if (touchPosDeltaX > 0.f) {
                m_objectRotation -= 2.f * m_deltaTime;
            } else {
                m_objectRotation += 2.f * m_deltaTime;
            }
            LOGINFO("Set rotation start.");
        }

        if (fabs(touchPosDeltaY) > 3.f) {
            if (touchPosDeltaY > 0.f) {
                m_objectScale -= 0.25f * m_deltaTime;
            } else {
                m_objectScale += 0.25f * m_deltaTime;
            }

            // Set the model size to a value between 0.75 and 1.25.
            m_objectScale = std::min(1.25f, std::max(0.75f, m_objectScale));
            LOGINFO("Set scale start.");
        }
    } else if (touchEvent->GetAction() == TOUCH_ACTION_UP) {  // Touch action: release
        LOGINFO("Action up.");
        m_touchBegin = false;
        if (!m_osrPluginExecuted) {
            tapDetected = true;
            lastTime = GetCurrentTime();
        }
    } else if (touchEvent->GetAction() == TOUCH_ACTION_CANCEL) {  // Cancel the touch action.
        LOGINFO("Action cancel.");
        m_touchBegin = false;
    }

    // Record the coordinates of the current touch event for calculating the next movement.
    m_touchPosX = touchEvent->GetPosX(touchEvent->GetTouchIndex());
    m_touchPosY = touchEvent->GetPosY(touchEvent->GetTouchIndex());
}

bool MainApplication::SetupCamera()
{
    /*
     * Add a camera.
     * Create a SceneObject object.
     */
    m_cameraObject = gSceneManager.CreateSceneObject();
    if (m_cameraObject == nullptr) {
        LOGERROR("Failed to create camera object.");
        return false;
    }

    // Add the Camera component to the SceneObject object to obtain the Camera object.
    m_mainCamera = m_cameraObject->AddComponent<Camera>();
    if (m_mainCamera == nullptr) {
        CG_SAFE_DELETE(m_cameraObject);
        LOGERROR("Failed to create camera component.");
        return false;
    }

    // Set camera parameters.
    const f32 FOV = 60.f;
    const f32 NEAR = 0.1f;
    const f32 FAR = 500.0f;
    const Vector3 EYE_POSITION(0.0f, 0.0f, 0.0f);

    // Initialize the camera position to the origin.
    m_cameraObject->SetPosition(EYE_POSITION);

    // The camera type is initialized to perspective projection, to simulate human vision.
    m_mainCamera->SetProjectionType(ProjectionType::PROJECTION_TYPE_PERSPECTIVE);

    // Set perspective projection parameters, including the FOV, aspect ratio, near clipping plane, and far clipping plane.
    m_mainCamera->SetPerspective(FOV, gCGKitInterface.GetAspectRatio(), NEAR, FAR);

    // Set the camera viewport. By default, the camera viewport is displayed in full screen.
    m_mainCamera->SetViewport(0, 0, gCGKitInterface.GetScreenWidth(), gCGKitInterface.GetScreenHeight());
    gSceneManager.SetMainCamera(m_mainCamera);
    LOGINFO("Setup main camera done.");
    return true;
}

bool MainApplication::SetupDefaultModel()
{
    m_modelObject = CreateObject("models/Avatar/body.obj", "material/Avatar.cgmat");
    if (m_modelObject == nullptr) {
        LOGERROR("Create scene object failed");
        return false;
    }

    // Set the default model position and size.
    m_modelObject->SetPosition(SCENE_OBJECT_POSITION);
    m_modelObject->SetScale(SCENE_OBJECT_SCALE);

    LOGINFO("Load default model done.");
    return true;
}

// Create a skybox.
SceneObject* MainApplication::CreateSkybox()
{
    SceneObject* skybox = CreateObject("models/test-cube.obj", "material/skybox.cgmat");
    if (skybox != nullptr) {
        skybox->SetScale(Vector3(1.f, 1.f, 1.f));
    }
    return skybox;
}

SceneObject* MainApplication::CreateObject(const String& modelName, const String& cgmatName, SceneObject* father)
{
    SceneObject* sceneObj = nullptr;
    Model* model = dynamic_cast<Model*>(gResourceManager.Get(modelName));
    if (model == nullptr) {
        LOGERROR("load file %s failed", modelName.c_str());
        return nullptr;
    }
    const Mesh* mesh = model->GetMesh();
    if (mesh == nullptr) {
        gResourceManager.Delete(model);
        LOGERROR("mesh is nullptr");
        return nullptr;
    }
    if (father == nullptr) {
        sceneObj = gSceneManager.CreateSceneObject();
    } else {
        sceneObj = new SceneObject(father);
    }
    if (sceneObj == nullptr) {
        gResourceManager.Delete(model);
        LOGERROR("create SceneObject failed .");
        return nullptr;
    }
    MeshRenderer* meshRenderer = sceneObj->AddComponent<MeshRenderer>();
    if (meshRenderer == nullptr) {
        CG_SAFE_DELETE(sceneObj);
        gResourceManager.Delete(model);
        LOGERROR("create MeshRenderer failed .");
        return nullptr;
    }
    meshRenderer->SetMesh(model->GetMesh());

    u32 subMeshCnt = mesh->GetSubMeshCount();
    for (u32 i = 0; i < subMeshCnt; ++i) {
        const SubMesh* subMesh = mesh->GetSubMesh(i);
        if (subMesh == nullptr) {
            continue;
        }
        MaterialInstance* materialInstance = dynamic_cast<MaterialInstance*>(gResourceManager.Get(cgmatName.c_str()));
        if (materialInstance == nullptr) {
            continue;
        }
        materialInstance->SetSubMeshInfo(subMesh);
        materialInstance->Create();
        meshRenderer->SetMaterialInstance(i, materialInstance);
    }
    return sceneObj;
}

void MainApplication::AddDirectionalLight()
{
    m_directionalLightObject = CG_NEW(SceneObject, nullptr);
    if (m_directionalLightObject != nullptr) {
        // Add a Light component to the SceneObject to obtain the Light object.
        Light* lightCom = m_directionalLightObject->AddComponent<Light>();
        if (lightCom != nullptr) {
            // Set the color of the directional light to white.
            lightCom->SetColor(Vector3::ONE);

            // Set the direction.
            const Vector3 lightDir(0.1f, 0.2f, 1.0f);
            lightCom->SetDirection(lightDir);

            // Set the type to directional light.
            lightCom->SetLightType(LIGHT_TYPE_DIRECTIONAL);
        } else {
            LOGERROR("Failed to create directional light.");
        }
    } else {
        LOG_ALLOC_ERROR("Failed to create directional light.");
    }
}

void MainApplication::AddPointLight()
{
    m_pointLightObject = CG_NEW(SceneObject, nullptr);
    if (m_pointLightObject != nullptr) {
        // Add a Light component to the SceneObject to obtain the Light object.
        Light* lightCom = m_pointLightObject->AddComponent<Light>();
        if (lightCom != nullptr) {
            const Vector3 lightColor(0.0f, 10000.0f, 10000.0f);
            lightCom->SetColor(lightColor);
            lightCom->SetLightType(LIGHT_TYPE_POINT);
        } else {
            LOGERROR("Failed to create point light.");
        }
    } else {
        LOG_ALLOC_ERROR("Failed to create point light.");
    }
}

// Create a CG Kit app.
BaseApplication* CreateMainApplication()
{
    return new MainApplication();
}

// Execute the Offline Super-Resolution plug-in.
void MainApplication::ExecuteOSRPlugin()
{
    // Save super-resolution data in the sandbox directory, for example, /storage/emulated/0/Android/data/com.xxx.xxx/files.
    const String localDir = gCGKitInterface.GetProgramDirectory();
    OSRPlugin plugin;
    plugin.ExecuteOSR(localDir);
    m_osrPluginExecuted = true;
}
