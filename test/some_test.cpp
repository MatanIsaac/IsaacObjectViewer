#include <gtest/gtest.h>
#include "Engine/Core/Engine.h"

using namespace isaacObjectLoader;

TEST(AppTest, Initialization) 
{
    const auto& engine = Engine::get();
    const bool check1 = engine->Init("Test Title",640,480);
    EXPECT_EQ(true,check1) << "App failed to initialize";
}
