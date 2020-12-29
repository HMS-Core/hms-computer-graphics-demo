/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: A sample that illustrates the rendering of a demo scene.
 */

// 启用日志，不定义CGKIT_LOG则没有日志输出
#define CGKIT_LOG
#include "MainApplication/MainApplication.h"
#include "OSRPlugin/OSRPlugin.h"
#include <sys/time.h>

using namespace CGKit;

const u32 HALF_CORE_NUM = 4;
// 设置双击事件间隔，单位毫秒
const u64 DOUBLE_TAP_INTERVAL_MS = 500;

MainApplication::MainApplication() {}

MainApplication::~MainApplication() {}

// 启动cgkit并配置系统事件监听
void MainApplication::Start(void* param)
{
    BaseApplication::Start(param);

    // 这里设置日志等级为VERBOSE，用以覆盖cgkit默认的日志级别
    Log::SetLogLevel(LOG_VERBOSE);
}

// 初始化cgkit的资源管理器，场景管理器以及渲染器等
void MainApplication::Initialize(const void* winHandle, u32 width, u32 height)
{
    gCGKitInterface.SetThreadCount(HALF_CORE_NUM);
    gCGKitInterface.SetMultiThreadRendering(true);
    BaseApplication::Initialize(winHandle, width, height);
}

// 销毁有关资源
void MainApplication::UnInitialize()
{
    // 销毁相机
    if (m_cameraObject != nullptr) {
        gSceneManager.DeleteObject(m_cameraObject);
        m_mainCamera = nullptr;
        m_cameraObject = nullptr;
    }

    // 销毁模型
    if (m_modelObject != nullptr) {
        gSceneManager.DeleteObject(m_modelObject);
        m_modelObject = nullptr;
    }

    // 销毁天空盒
    if (m_skyObject != nullptr) {
        gSceneManager.DeleteObject(m_skyObject);
        m_skyObject = nullptr;
    }

    // 销毁平行光源
    if (m_directionalLightObject != nullptr) {
        gSceneManager.DeleteObject(m_directionalLightObject);
        m_directionalLightObject = nullptr;
    }

    // 销毁点光源
    if (m_pointLightObject != nullptr) {
        gSceneManager.DeleteObject(m_pointLightObject);
        m_pointLightObject = nullptr;
    }

    // 前面先销毁用户自己申请的有关资源，然后调用cgkit系统的销毁方法释放系统资源
    BaseApplication::UnInitialize();
}

// 初始化场景
// 添加相机，设置默认模型，设置天空盒，添加平行光和点光源
void MainApplication::InitScene()
{
    LOGINFO("MainApplication InitScene.");
    BaseApplication::InitScene();

    // 设置相机
    if(!SetupCamera()) {
        return;
    }

    // 设置默认模型
    if(!SetupDefaultModel()) {
        return;
    }

    // 设置天空盒
    m_skyObject = CreateSkybox();
    if (m_skyObject != nullptr) {
        // 天空盒大小需要设置足够大，以包含场景内的物体
        m_skyObject->SetScale(Vector3(100.f, 100.f, 100.f));
    }

    // 添加平行光
    m_directionalLightObject = CG_NEW SceneObject(nullptr);
    if (m_directionalLightObject != nullptr) {
        // SceneObject需要添加Light组件得到Light对象
        Light* lightCom = m_directionalLightObject->AddComponent<Light>();
        if (lightCom != nullptr) {
            // 设置平行光为白色
            lightCom->SetColor(Vector3::ONE);

            // 设置方向
            const Vector3 lightDir(0.1f, 0.2f, 1.0f);
            lightCom->SetDirection(lightDir);

            // 设置类型为平行光
            lightCom->SetLightType(LIGHT_TYPE_DIRECTIONAL);
        } else {
            LOGERROR("Failed to create directional light.");
        }
    } else {
        LOG_ALLOC_ERROR("Failed to create directional light.");
    }

    // 添加点光源
    m_pointLightObject = CG_NEW SceneObject(nullptr);
    if (m_pointLightObject != nullptr) {
        // SceneObject需要添加Light组件得到Light对象
        Light* lightCom = m_pointLightObject->AddComponent<Light>();
        if (lightCom != nullptr) {
            const Vector3 lightColor(0.0f, 1.0f, 1.0f);
            lightCom->SetColor(lightColor);
            lightCom->SetIntensity(10000.f);
            lightCom->SetLightType(LIGHT_TYPE_POINT);
        } else {
            LOGERROR("Failed to create point light.");
        }
    } else {
        LOG_ALLOC_ERROR("Failed to create point light.");
    }
}

// 处理每一帧更新逻辑
void MainApplication::Update(float deltaTime)
{
    // deltaTime为两帧之间的时间间隔，单位是秒
    m_deltaTime = deltaTime;
    m_deltaAccumulate += deltaTime;

    // 这里每一帧设置旋转和大小，m_objectRotation和m_objectScale变量会在手势事件中被动态改变
    if (m_modelObject != nullptr) {
        m_modelObject->SetRotation(Vector3(0.0, m_objectRotation, 0.0));
        m_modelObject->SetScale(SCENE_OBJECT_SCALE * m_objectScale);
    }

    // 实时调整灯光空间位置
    const f32 moveRatioX = 0.2f;
    const f32 moveRatioY = 0.5f;
    const f32 pointLightCircle = 50.f;
    if (m_pointLightObject != nullptr) {
        m_pointLightObject->SetPosition(Vector3(sin(m_deltaAccumulate * moveRatioX) * pointLightCircle,
            sin(m_deltaAccumulate * moveRatioY) * pointLightCircle + pointLightCircle,
            cos(m_deltaAccumulate * moveRatioX) * pointLightCircle));
    }

    // 调用cgkit默认更新逻辑
    BaseApplication::Update(deltaTime);
}

// 处理窗口大小变化
void MainApplication::Resize(u32 width, u32 height)
{
    // 设置对应视口
    // 如果不进行设置，旋转屏幕后则继续使用原来的视口，可能导致渲染画面无法填充满整个屏幕
    if (m_mainCamera != nullptr) {
        m_mainCamera->SetViewport(0, 0, width, height);
    }

    // 继续cgkit默认处理逻辑
    BaseApplication::Resize(width, height);
}

// 处理输入事件
// cgkit支持多手指触摸事件，本demo仅演示单指触摸事件
// 通过单个手指横向滑动旋转模型，纵向滑动缩放模型
void MainApplication::ProcessInputEvent(const InputEvent* inputEvent)
{
    if (inputEvent == nullptr) {
        LOGERROR("Received null event.");
        return;
    }

    // 调用cgkit默认事件处理逻辑
    BaseApplication::ProcessInputEvent(inputEvent);

    // 针对收到的事件进行自定义设置
    EventSource source = inputEvent->GetSource();
    static bool tapDetected = false;
    static s64 lastTime = 0;

    // 本demo仅演示触摸事件
    if(source != EVENT_SOURCE_TOUCHSCREEN) {
        return;
    }

    const TouchInputEvent* touchEvent = reinterpret_cast<const TouchInputEvent*>(inputEvent);

    // 本demo仅演示单指滑动和缩放
    if (touchEvent->GetTouchCount() > 1) {
        return;
    }

    // 触摸事件按压
    if (touchEvent->GetAction() == TOUCH_ACTION_DOWN) {
        LOGINFO("Action move start.");
        m_touchBegin = true;
        // 两次点击间隔在双击间隔内，作为双击
        if (!m_osrPluginExecuted && tapDetected && (abs(GetCurrentTime() - lastTime) < DOUBLE_TAP_INTERVAL_MS)) {
            ExecuteOSRPlugin();
        }
    } else if (touchEvent->GetAction() == TOUCH_ACTION_MOVE) {  // 触摸事件移动
        // 确保已经是开始触摸的状态，以获取上一次的触摸坐标信息
        if (!m_touchBegin) {
            return;
        }

        // 获取事件坐标和前一次记录纵横坐标差值，用于计算模型变化数据
        f32 touchPosDeltaX = touchEvent->GetPosX(touchEvent->GetTouchIndex()) - m_touchPosX;
        f32 touchPosDeltaY = touchEvent->GetPosY(touchEvent->GetTouchIndex()) - m_touchPosY;

        // 与上次事件触摸坐标距离超过设定值才认定为有效事件，以防止误触抖动
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

            // 模型大小设定为0.75-1.25之间
            m_objectScale = std::min(1.25f, std::max(0.75f, m_objectScale));
            LOGINFO("Set scale start.");
        }
    } else if (touchEvent->GetAction() == TOUCH_ACTION_UP) {  // 触摸事件抬起
        LOGINFO("Action up.");
        m_touchBegin = false;
        if (!m_osrPluginExecuted) {
            tapDetected = true;
            lastTime = GetCurrentTime();
        }
    } else if (touchEvent->GetAction() == TOUCH_ACTION_CANCEL) {  // 触摸事件取消
        LOGINFO("Action cancel.");
        m_touchBegin = false;
    }

    // 记录本次触摸事件坐标，以便下一次参考计算移动量
    m_touchPosX = touchEvent->GetPosX(touchEvent->GetTouchIndex());
    m_touchPosY = touchEvent->GetPosY(touchEvent->GetTouchIndex());
}

bool MainApplication::SetupCamera()
{
    // 添加相机
    // 相机是SceneObject类型，需先创建一个SceneObject
    m_cameraObject = CG_NEW SceneObject(nullptr);
    if (m_cameraObject == nullptr) {
        LOGERROR("Failed to create camera object.");
        return false;
    }

    // 然后在该SceneObject对象上添加Camera组件，得到Camera对象
    m_mainCamera = m_cameraObject->AddComponent<Camera>();
    if (m_mainCamera == nullptr) {
        CG_SAFE_DELETE(m_cameraObject);
        LOGERROR("Failed to create camera component.");
        return false;
    }

    // 设置相机有关参数
    const f32 FOV = 60.f;
    const f32 NEAR = 0.1f;
    const f32 FAR = 500.0f;
    const Vector3 EYE_POSITION(0.0f, 0.0f, 0.0f);

    // 相机位置初始化为原点
    m_cameraObject->SetPosition(EYE_POSITION);

    // 相机类型初始化为透视投影类型，与人眼观看近大远小效果一致
    m_mainCamera->SetProjectionType(ProjectionType::PROJECTION_TYPE_PERSPECTIVE);

    // 设置透视投影的参数，包括FOV，屏幕宽高比以及近平面和远平面
    m_mainCamera->SetPerspective(FOV, gCGKitInterface.GetAspectRatio(), NEAR, FAR);

    // 设置相机视口，默认为全屏显示
    m_mainCamera->SetViewport(0, 0, gCGKitInterface.GetScreenWidth(), gCGKitInterface.GetScreenHeight());
    gSceneManager.SetMainCamera(m_mainCamera);
    LOGINFO("Setup main camera done.");
    return true;
}

bool MainApplication::SetupDefaultModel()
{
    // 加载默认模型，目前只支持obj格式
    String modelName = "models/Avatar/body.obj";
    Model* model = dynamic_cast<Model*>(gResourceManager.Get(modelName));

    if (model == nullptr) {
        LOGERROR("Failed to load default model.");
        return false;
    }

    // 将模型添加到场景中
    MeshRenderer* meshRenderer = nullptr;
    m_modelObject = gSceneManager.CreateSceneObject();
    if (m_modelObject == nullptr) {
        LOGERROR("Failed to create scene object for default model.");
        return false;
    }

    // SceneObject对象要添加MeshRenderer组件才能渲染
    meshRenderer = m_modelObject->AddComponent<MeshRenderer>();
    if (meshRenderer == nullptr) {
        LOGERROR("Failed to add MeshRenderer for default model.");
        return false;
    }

    const Mesh* mesh = model->GetMesh();
    if (mesh == nullptr) {
        LOGERROR("Failed to get mesh from default model.");
        return false;
    }

    // 从obj模型中解析Mesh后设置到MeshRenderer以进行渲染
    meshRenderer->SetMesh(mesh);
    LOGINFO("Model submesh count %d.", mesh->GetSubMeshCount());
    LOGINFO("Model vertex count %d.", mesh->GetVertexCount());

    // 加载贴图，cgkit支持pbr效果，一般需要配置以下贴图
    String texAlbedo = "models/Avatar/Albedo_01.png";
    String texNormal = "models/Avatar/Normal_01.png";
    String texPbr = "models/Avatar/Pbr_01.png";
    String texEmissive = "shaders/pbr_brdf.png";

    u32 subMeshCnt = mesh->GetSubMeshCount();
    for (u32 i = 0; i < subMeshCnt; ++i) {
        SubMesh* subMesh = mesh->GetSubMesh(i);
        if (subMesh == nullptr) {
            LOGERROR("Failed to get submesh %d.", i);
            continue;
        }

        // 添加材质
        Material* material = dynamic_cast<Material*>(gResourceManager.Get(ResourceType::RESOURCE_TYPE_MATERIAL));
        if (material == nullptr) {
            LOGERROR("Failed to get default material.");
            return false;
        }

        // 初始化管线状态
        material->Init();

        // 每个SubMesh对应一个Material
        material->SetSubMesh(subMesh);

        // 设置模型材质的Albedo贴图及采样参数
        material->SetTexture(TextureType::TEXTURE_TYPE_ALBEDO, texAlbedo);
        material->SetSamplerParam(TextureType::TEXTURE_TYPE_ALBEDO, SAMPLER_FILTER_BILINEAR,
                                  SAMPLER_FILTER_BILINEAR, SAMPLER_MIPMAP_BILINEAR, SAMPLER_ADDRESS_CLAMP);

        // 设置模型材质的Normal贴图及采样参数
        material->SetTexture(TextureType::TEXTURE_TYPE_NORMAL, texNormal);
        material->SetSamplerParam(TextureType::TEXTURE_TYPE_NORMAL, SAMPLER_FILTER_BILINEAR,
                                  SAMPLER_FILTER_BILINEAR, SAMPLER_MIPMAP_BILINEAR, SAMPLER_ADDRESS_CLAMP);

        // 设置模型材质的PBR贴图及采样参数
        material->SetTexture(TextureType::TEXTURE_TYPE_PBRTEXTURE, texPbr);
        material->SetSamplerParam(TextureType::TEXTURE_TYPE_PBRTEXTURE, SAMPLER_FILTER_BILINEAR,
                                  SAMPLER_FILTER_BILINEAR, SAMPLER_MIPMAP_BILINEAR, SAMPLER_ADDRESS_CLAMP);

        // 设置模型材质的Emission贴图及采样参数
        material->SetTexture(TextureType::TEXTURE_TYPE_EMISSION, texEmissive);
        material->SetSamplerParam(TextureType::TEXTURE_TYPE_EMISSION, SAMPLER_FILTER_BILINEAR,
                                  SAMPLER_FILTER_BILINEAR, SAMPLER_MIPMAP_BILINEAR, SAMPLER_ADDRESS_CLAMP);

        // 设置模型材质的EnvMap贴图及采样参数
        material->SetTexture(TextureType::TEXTURE_TYPE_ENVIRONMENTMAP, m_envMap);
        material->SetSamplerParam(TextureType::TEXTURE_TYPE_ENVIRONMENTMAP, SAMPLER_FILTER_BILINEAR,
                                  SAMPLER_FILTER_BILINEAR, SAMPLER_MIPMAP_BILINEAR, SAMPLER_ADDRESS_CLAMP);

        // 设置模型材质的ShaderStage，即vert和frag
        material->AttachShaderStage(ShaderStageType::SHADER_STAGE_TYPE_VERTEX, "shaders/pbr_vert.spv");
        material->AttachShaderStage(ShaderStageType::SHADER_STAGE_TYPE_FRAGMENT, "shaders/pbr_frag.spv");

        // 设置背面剔除，并启用DepthTest和DepthWrite
        material->SetCullMode(CULL_MODE_BACK);
        material->SetDepthTestEnable(true);
        material->SetDepthWriteEnable(true);
        material->Create();
        meshRenderer->SetMaterial(i, material);
    }

    // 默认模型位置和大小
    m_modelObject->SetPosition(SCENE_OBJECT_POSITION);
    m_modelObject->SetScale(SCENE_OBJECT_SCALE);

    LOGINFO("Load default model done.");
    return true;
}

// 创建天空盒
SceneObject* MainApplication::CreateSkybox()
{
    // 天空盒需要立方体模型，用以放置六个面的贴图
    String modelName = "models/test-cube.obj";
    Model* model = dynamic_cast<Model*>(gResourceManager.Get(modelName));
    if (model == nullptr) {
        LOGERROR("Failed to load model with path: %s.", modelName.c_str());
        return nullptr;
    }

    // 获取Mesh数据
    const Mesh* mesh = model->GetMesh();
    if (mesh == nullptr) {
        LOGERROR("Failed to get mesh from skybox model.");
        gResourceManager.Delete(model);
        return nullptr;
    }

    // 获取SubMesh个数
    u32 subMeshCnt = mesh->GetSubMeshCount();
    SceneObject* skyboxObject = gSceneManager.CreateSceneObject();
    if (skyboxObject == nullptr) {
        LOGERROR("Failed to create scene object for skybox.");
        return nullptr;
    }

    // SceneObject对象要添加Meshrenderer组件才能渲染
    MeshRenderer* meshRenderer = skyboxObject->AddComponent<MeshRenderer>();
    if (meshRenderer == nullptr) {
        LOGERROR("Failed to add MeshRenderer to skybox.");
        return nullptr;
    }

    // 从obj模型中解析Mesh后设置到MeshRenderer以进行渲染
    meshRenderer->SetMesh(mesh);
    for (u32 i = 0; i < subMeshCnt; ++i) {
        SubMesh* subMesh = mesh->GetSubMesh(i);
        if (subMesh == nullptr) {
            LOGERROR("Failed to get submesh %d.", i);
            continue;
        }
        // 添加材质
        Material* material = dynamic_cast<Material*>(gResourceManager.Get(ResourceType::RESOURCE_TYPE_MATERIAL));
        if (material == nullptr) {
            continue;
        }
        material->Init();
        material->SetSubMesh(subMesh);

        // 设置天空盒材质的环境贴图及采样参数
        material->SetTexture(TextureType::TEXTURE_TYPE_ENVIRONMENTMAP, m_envMap);
        material->SetSamplerParam(TextureType::TEXTURE_TYPE_ENVIRONMENTMAP, SAMPLER_FILTER_BILINEAR,
                                  SAMPLER_FILTER_BILINEAR, SAMPLER_MIPMAP_BILINEAR, SAMPLER_ADDRESS_CLAMP);

        // 设置天空盒材质的ShaderStage，即vert和frag
        material->AttachShaderStage(ShaderStageType::SHADER_STAGE_TYPE_VERTEX, "shaders/sky_vert.spv");
        material->AttachShaderStage(ShaderStageType::SHADER_STAGE_TYPE_FRAGMENT, "shaders/sky_frag.spv");

        // 因为在天空盒内，所以此处设置剔除模式为NONE
        material->SetCullMode(CULL_MODE_NONE);
        // 启用DepthTest和DepthWrite
        material->SetDepthTestEnable(true);
        material->SetDepthWriteEnable(true);
        material->Create();
        meshRenderer->SetMaterial(i, material);
    }

    // 设置天空盒大小，位置和旋转
    skyboxObject->SetScale(Vector3(1.0f, 1.0f, 1.0f));
    skyboxObject->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    skyboxObject->SetRotation(Vector3(0.0f, 0.0, 0.0));

    return skyboxObject;
}

// 创建cgkit应用程序
BaseApplication* CreateMainApplication()
{
    return new MainApplication();
}

// 执行超分插件
void MainApplication::ExecuteOSRPlugin()
{
    // 使用沙盒路径存放超分数据，类似"/storage/emulated/0/Android/data/com.xxx.xxx/files"
    const String localDir = gCGKitInterface.GetProgramDirectory();
    OSRPlugin plugin;
    plugin.ExecuteOSR(localDir);
    m_osrPluginExecuted = true;
}