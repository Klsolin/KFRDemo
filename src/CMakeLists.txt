find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(glm REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/lib/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/tinyobjloader)

file(GLOB IMGUI_SRC ${CMAKE_SOURCE_DIR}/lib/imgui/*.cpp)

set(ALL_SRC
    ${IMGUI_SRC}
    ${CMAKE_SOURCE_DIR}/lib/glad/src/glad.c
    ${CMAKE_SOURCE_DIR}/src/Main.cpp
    ${CMAKE_SOURCE_DIR}/src/Timer.cpp
    ${CMAKE_SOURCE_DIR}/src/Shader.cpp
    ${CMAKE_SOURCE_DIR}/src/Texture.cpp
    ${CMAKE_SOURCE_DIR}/src/GBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/ObjModel.cpp
    ${CMAKE_SOURCE_DIR}/src/FrameBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/MeshDrawer.cpp
    ${CMAKE_SOURCE_DIR}/src/CircleDrawer.cpp
    ${CMAKE_SOURCE_DIR}/src/Texture2DDrawer.cpp
    ${CMAKE_SOURCE_DIR}/src/Texture3DDrawer.cpp
    ${CMAKE_SOURCE_DIR}/src/ForwardDrawer.cpp
    ${CMAKE_SOURCE_DIR}/src/KFRPass1Drawer.cpp
    ${CMAKE_SOURCE_DIR}/src/KFRPass2Drawer.cpp
    ${CMAKE_SOURCE_DIR}/src/DeferredGeometryDrawer.cpp
    ${CMAKE_SOURCE_DIR}/src/DeferredLightingDrawer.cpp
    ${CMAKE_SOURCE_DIR}/src/KFREffect.cpp)

set(ALL_LIB
    OpenGL::GL
    glfw)

add_executable(${PROJECT_NAME} ${ALL_SRC})
target_link_libraries(${PROJECT_NAME} ${ALL_LIB})
