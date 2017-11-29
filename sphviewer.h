#ifndef SPHVIEWER_H
#define SPHVIEWER_H

#include <FreeImage.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <utility>
#include <cstring>

#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <chrono>
#include <algorithm>
#include <vector>
#include <png.h>

#include "includes/constant.h"
#include "includes/Camera.h"
#include "includes/sphere.h"
#include "includes/shader.h"
#include "includes/cube.h"
#include "includes/scene.h"
#include "SPH/SPH.h"

#define SQUARE_SIDE 20
#define MIN_ALT 0.5f
#define MAX_ALT 100.0f

typedef unsigned long long ulong64_t;

// Constants for defining the rendering scene

const float sphereSize = 0.03f; // Changes the rendered sphere size, not used for physics calc
const GLfloat x_min = -SQUARE_SIDE;
const GLfloat x_max = SQUARE_SIDE;
const GLfloat y_min = MIN_ALT;
const GLfloat y_max = MAX_ALT;
const GLfloat z_min = -SQUARE_SIDE;
const GLfloat z_max = SQUARE_SIDE;

struct ObjectData {
    GLuint ModelArrayID, ModelVBO, ModelColorVBO, ModelNormalVBO, EBO, indexSize;
};

// Vertices used for defining the cube visually which defines the particles.
// We make a unit cube and scale it to match the bounds of the particles.

float boundingCubeVertices[] = {
    -0.5,-0.5,0.5,
    0.5,-0.5,0.5,
    0.5,0.5,0.5,
    -0.5,0.5,0.5,
    -0.5,-0.5,-0.5,
    0.5,-0.5,-0.5,
    0.5,0.5,-0.5,
    -0.5,0.5,-0.5
};

// Order in which vertices should be joined for creating the bounding cube.
// We use an EBO for specifying the order of vertices.

unsigned int boundingCubeIndices[] = {
    0,1,
    1,2,
    2,3,
    3,0,
    4,5,
    5,6,
    6,7,
    7,4,
    0,4,
    1,5,
    2,6,
    3,7
};

// The boundary of the bounding cube is set to black.

GLfloat boundingCubeColors[] = {
    0.0f, 0.0f, 0.0f
};

// Color used for the sphere. The color ends up looking slightly different from the framebuffer.

GLfloat colorArray[] = {
    0.3f, 0.2f, 0.6f
};

// Size of the arrays used for the bounding sphere. Needed for creating VAOs.

const int boundingCubeSize = 8;
const int boundingIndicesSize = 24;

// Overloaded function to create an std::vector of VAOs for Mesh objects

void setupMeshVAO(Mesh mesh, GLfloat* color_vector, vector<ObjectData> &objectVector);

// Overloaded function to create an single ObjectData instance with VAO data bindings

void setupMeshVAO(Mesh mesh, GLfloat* color_vector, ObjectData &object);

// Used for setting up VAOs for primitive objects like the bounding cube which are defined by indices

void setupPrimitiveVAO(GLfloat* vertices, GLuint* indices, GLfloat* color_vector, int vertexCount, ObjectData &object);

// Functions hooked up to OpenGL callbacks. These are used for giving the control to the camera for movement.

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Initialises the FBO for offscreen rendering.
// Creates color and depth buffers and pixels are read by glReadPixels from COLOR_ATTACHMENT0.
// Only executed if OFFSCREEN is set to true.

void initFrameBuffers(GLuint &fbo, GLuint &colorBuffer);

// Initialises all OpenGL callbacks for the context.
// Only used when OFFSCREEN is set to false.

void setCallBacks(GLFWwindow* window);

// Draw an object from its VertexArrayObject (VAO) and its modification vectors.
// Vectors for modifying the scale, translation, rotation of the object can be passed.

void drawGenericObject(GLuint &VAO, GLuint programID,
                        glm::mat4 proj,
                        glm::mat4 view,
                        int size,
                        bool elemental,
                        int elmentMode = 0,
                        glm::vec3 translationVector = glm::vec3(0,0,0),
                        glm::vec3 scaleVector = glm::vec3(1,1,1),
                        GLfloat rotationAngle = 0,
                        glm::vec3 rotationAxis = glm::vec3(1,0,0));

bool initOpenGL();


#endif
