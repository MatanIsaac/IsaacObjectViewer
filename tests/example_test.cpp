#include <gtest/gtest.h>
#include "Engine/Core/Engine.h"

using namespace isaacObjectViewer;

TEST(AppTest, Initialization)
{
    const auto& engine = Engine::GetInstance();
    const bool check1 = engine->Init("Test Title", 800,600);
    EXPECT_EQ(true,check1) << "App failed to initialize";
}

