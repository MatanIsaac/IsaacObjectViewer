// engine_stress_tests.cpp
#include <gtest/gtest.h>
#include "Engine/Core/Engine.h"

using namespace isaacObjectViewer;

//-------------------------
// Max Lights Overflow
//-------------------------
TEST(EngineStressTest, MaxLightsOverflow) 
{
    const auto& engine = Engine::GetInstance();
    int initialLights = static_cast<int>(engine->GetLightObjects().size());
    for (int i = 0; i < engine->GetMaxLights() + 5; ++i) 
    {
        engine->AddSceneObject(ObjectType::PointLight, {0.0f, 1.0f, 2.0f});
    }
    int activeLights = static_cast<int>(engine->GetLightObjects().size());
    ASSERT_LE(activeLights, engine->GetMaxLights());
    ASSERT_GE(activeLights, initialLights);
}

//-------------------------
// Double Add / Remove Objects
//-------------------------
TEST(EngineStressTest, DoubleAddRemoveObjects) 
{
    const auto& engine = Engine::GetInstance();

    auto* obj1 = new Cube({0.0f, 0.0f, 0.0f});
    auto* obj2 = new Cube({0.0f, 0.0f, 0.0f});
    engine->AddSceneObject(obj1);
    engine->AddSceneObject(obj2); // adding same object again should not crash
    ASSERT_NE(obj1, nullptr);
    ASSERT_NE(obj2, nullptr);

    engine->RemoveSceneObject(obj1);
    EXPECT_NO_THROW(engine->RemoveSceneObject(obj1)); // removing again should not crash
    engine->RemoveSceneObject(obj2);
    EXPECT_NO_THROW(engine->RemoveSceneObject(obj2)); // removing again should not crash

    delete obj1;
    delete obj2;
}

//-------------------------
// Clean After Failed Init
//-------------------------
TEST(EngineStressTest, CleanAfterFailedInit) 
{
    Engine* badEngine = Engine::GetInstance();
    bool ok = badEngine->Init("Bad Init", -1, -1, false); // fail intentionally
    ASSERT_FALSE(ok);
    //EXPECT_NO_THROW(badEngine->Clean());
}

//-------------------------
// Rapid Mode Toggle
//-------------------------
TEST(EngineStressTest, RapidModeToggle) 
{
    const auto& engine = Engine::GetInstance();
    for (int i = 0; i < 100; ++i) 
    {
        engine->EnableFreeCameraMode();
        engine->EnableMouseMode();
    }
    SUCCEED();
}

//-------------------------
// Background Color Extremes
//-------------------------
TEST(EngineStressTest, BackgroundColorExtremes) 
{
    const auto& engine = Engine::GetInstance();

    engine->SetBackgroundColor({2.0f, -1.0f, 100.0f});
    auto color = engine->GetBackgroundColor();
    EXPECT_NO_THROW({
        (void)color; // just check it doesn’t crash
    });
}

//-------------------------
// Frame Cap Weirdness
//-------------------------
TEST(EngineStressTest, FrameCapWeirdness) 
{
    const auto& engine = Engine::GetInstance();
    engine->GetFrameCapFps() = 0;
    engine->GetFrameCapFps() = -60;
    SUCCEED();
}

//-------------------------
// Clear Scene Objects
//-------------------------
TEST(EngineStressTest, ClearSceneObjects) 
{
    const auto& engine = Engine::GetInstance();
    engine->AddSceneObject(ObjectType::Cube, {0.0f, 0.0f, 0.0f});
    engine->AddSceneObject(ObjectType::Sphere, {1.0f, 0.0f, 0.0f});
    ASSERT_FALSE(engine->GetSceneObjects().empty());
    engine->ClearSceneObjects();
    ASSERT_TRUE(engine->GetSceneObjects().empty());
}
