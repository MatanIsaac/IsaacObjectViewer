# project_root/test/CMakeLists.txt

# Define the test executable
add_executable(TemplateTest
    some_test.cpp
)

# Include directories for the test executable
target_include_directories(TemplateTest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Utility
)

# Link the library and googletest to the test executable
target_link_libraries(TemplateTest PUBLIC
    gtest             # Googletest library
    gtest_main        # Main function for googletest
    isaac_object_loader  # Your application library
    imgui
    imgui_impl
)

# Add the test to CTest
add_test(NAME TemplateUnitTests COMMAND TemplateTest)
