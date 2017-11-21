 # sudo apt install libglfw3 libglew-dev glew-utils libglew2.0 assimp-utils libassimp-dev libassimp3v5 freeglut3-dev

cmake_minimum_required(VERSION 3.2)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra")
project(opengl_assignment)

add_executable(cg_sph main.cpp "SPH/Particle.cpp" "SPH/SPH.cpp" "MarchingCube/MarchingCube.cpp" "includes/shader.cpp" "includes/constant.h" "includes/Camera.cpp" 
                "includes/icosahedron.cpp" "includes/cube.cpp" "includes/mesh.cpp" "includes/utilities.cpp")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/includes/ColorFragmentShader.fragmentshader
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/includes/TransformVertexShader.vertexshader
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(cg_sph ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw GLEW assimp dl)
