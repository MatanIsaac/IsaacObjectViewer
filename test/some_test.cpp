#include <gtest/gtest.h>
#include "Engine/Core/Engine.h"

using namespace isaacGraphicsEngine;

TEST(AppTest, Initialization) 
{
    const auto& engine = Engine::GetInstance();
    const bool check1 = engine->Init("Test Title",640,480);
    EXPECT_EQ(true,check1) << "App failed to initialize";
}