 
# sudo apt install libglfw3 libglew-dev glew-utils libglew2.0 assimp-utils libassimp-dev libassimp3v5 freeglut3-dev

cmake_minimum_required(VERSION 3.2)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra")
project(opengl_assignment)
# include_directories("src/")
# add_executable(parkviewer parkviewer.cpp "src/shader.cpp" "src/Camera.cpp" "src/utilities.cpp" "src/mesh.cpp" "src/icosahedron.cpp" "src/sphere.cpp" "src/cube.cpp" "src/cylinder.cpp" "src/monkeybars.cpp" "src/seesaw.cpp" "src/slide.cpp" "src/scene.cpp" "src/grass.cpp")

add_executable(cg_sph main.cpp "SPH/Particle.cpp" "SPH/SPH.cpp" "MarchingCube/MarchingCube.cpp")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(cg_sph ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw GLEW assimp dl)
