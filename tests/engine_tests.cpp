#include <gtest/gtest.h>
#include "Engine/Core/Engine.h"

using namespace isaacObjectViewer;

//---------------------------------------------------------
// ENGINE INITIALIZATION TESTS
//---------------------------------------------------------

TEST(EngineTest, InitializationWorks) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("Test Window", 800, 600, false);
    EXPECT_TRUE(engine->Init("Test Title", 800, 600))
        << "Engine failed to initialize with valid parameters.";
}

TEST(EngineTest, ReinitializationDoesNotCrash) 
{
    const auto& engine = Engine::GetInstance();
    EXPECT_TRUE(engine->Init("Window1", 800, 600));
    EXPECT_TRUE(engine->Init("Window2", 1024, 768))
    << "Engine should handle being initialized twice without failure.";
}
    
TEST(EngineTest, InvalidWindowSize) 
{
    const auto& engine = Engine::GetInstance();
    bool result = engine->Init("BadSize", -640, -480);
    EXPECT_FALSE(result) << "Engine should reject negative dimensions.";    
    engine->Clean();
}


//---------------------------------------------------------
// SHUTDOWN & STATE HANDLING TESTS
//---------------------------------------------------------

TEST(EngineTest, ShutdownStopsRunning) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("Temp", 640, 480);
    engine->Clean();
    EXPECT_FALSE(engine->IsRunning())
    << "Engine should not be running after Clean.";
}

TEST(EngineTest, DoubleCleanIsSafe) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("Temp", 800, 600);
    engine->Clean();
    EXPECT_NO_THROW(engine->Clean())
    << "Calling Clean twice should not throw or crash.";
}


//---------------------------------------------------------
// SCENE OBJECT MANAGEMENT TESTS
//---------------------------------------------------------
TEST(SceneObjectTest, AddAndRetrieveObject) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("SceneTest", 800, 600);
    size_t startCount = engine->GetSceneObjects().size();
    engine->AddSceneObject(ObjectType::Cube);
    EXPECT_EQ(engine->GetSceneObjects().size(), startCount + 1);
}

TEST(SceneObjectTest, RemoveObjectShrinksList) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("SceneTest", 800, 600);
    engine->ClearSceneObjects();
    engine->AddSceneObject(ObjectType::Sphere);
    auto obj = engine->GetSelectedObject();
    engine->RemoveSceneObject(obj);
    EXPECT_TRUE(engine->GetSceneObjects().empty());
}

TEST(SceneObjectTest, ClearSceneObjectsEmptiesScene) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("SceneTest", 800, 600);
    engine->AddSceneObject(ObjectType::Cube);
    engine->AddSceneObject(ObjectType::Plane);
    engine->ClearSceneObjects();
    EXPECT_TRUE(engine->GetSceneObjects().empty());
}

//---------------------------------------------------------
// LIGHTING & SHADING TESTS
//---------------------------------------------------------
TEST(LightingTest, AddPointLightIncreasesLightList) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("LightTest", 800, 600);
    engine->ClearSceneObjects();
    size_t startLights = engine->GetLightObjects().size();
    LOG_INFO("Starting lights: {}", startLights);
    engine->AddSceneObject(ObjectType::PointLight);
    LOG_INFO("Ending lights: {}", engine->GetLightObjects().size());
    EXPECT_EQ(engine->GetLightObjects().size(), startLights+1);
}

TEST(LightingTest, RemovePointLightShrinksBothLists)
{
    const auto& engine = Engine::GetInstance();
    engine->Init("LightRemoveTest", 800, 600);
    engine->ClearSceneObjects();

    engine->AddSceneObject(ObjectType::PointLight);
    IObject* light = engine->GetSelectedObject();
    ASSERT_NE(light, nullptr);
    EXPECT_EQ(engine->GetSceneObjects().size(), 1u);
    EXPECT_EQ(engine->GetLightObjects().size(), 1u);

    // Must free the light exactly once (a double-free here would crash the runner).
    engine->RemoveSceneObject(light);
    EXPECT_TRUE(engine->GetSceneObjects().empty());
    EXPECT_TRUE(engine->GetLightObjects().empty());
    EXPECT_EQ(engine->GetSelectedObject(), nullptr);
}

TEST(LightingTest, SetShadingUpdatesFlag)
{
    const auto& engine = Engine::GetInstance();
    engine->Init("ShadeTest", 800, 600);
    engine->SetShading(ShadingType::BLINNPHONG);
    EXPECT_EQ(engine->GetShadingMode(), 2);
    EXPECT_TRUE(engine->GetBlinnPhongShading());

    engine->SetShading(ShadingType::PHONG);
    EXPECT_EQ(engine->GetShadingMode(), 1);
    EXPECT_FALSE(engine->GetBlinnPhongShading());

    engine->SetShading(ShadingType::NONE);
    EXPECT_EQ(engine->GetShadingMode(), 0);
    EXPECT_FALSE(engine->GetBlinnPhongShading());
}

//---------------------------------------------------------
// INPUT & STATE FLAG TESTS
//---------------------------------------------------------
TEST(InputModeTest, EnableMouseAndFreeCameraModes) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("InputTest", 800, 600);

    engine->EnableFreeCameraMode();
    EXPECT_TRUE(engine->IsFreeCameraModeEnabled());
    EXPECT_FALSE(engine->GetDisableInput());

    engine->EnableMouseMode();
    EXPECT_FALSE(engine->IsFreeCameraModeEnabled());
    EXPECT_TRUE(engine->GetDisableInput());
}

TEST(StateFlagTest, VSyncTogglePersists) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("VSyncTest", 800, 600);
    engine->IsVSyncEnabled() = false;
    EXPECT_FALSE(engine->IsVSyncEnabled());
    engine->IsVSyncEnabled() = true;
    EXPECT_TRUE(engine->IsVSyncEnabled());
}

//---------------------------------------------------------
// BACKGROUND COLOR & FRAME CAP TESTS
//---------------------------------------------------------
TEST(ConfigTest, BackgroundColorSetsCorrectly) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("ColorTest", 800, 600);
    glm::vec3 color(0.2f, 0.4f, 0.6f);
    engine->SetBackgroundColor(color);
    EXPECT_EQ(engine->GetBackgroundColor(), color);
}

TEST(ConfigTest, FrameCapFlagsAreMutable) 
{
    const auto& engine = Engine::GetInstance();
    engine->Init("Test Window", 800, 600, false);

    engine->Init("FrameCapTest", 800, 600);
    engine->GetFrameCapEnabled() = true;
    engine->GetFrameCapFps() = 120;
    EXPECT_TRUE(engine->GetFrameCapEnabled());
    EXPECT_EQ(engine->GetFrameCapFps(), 120);
}

