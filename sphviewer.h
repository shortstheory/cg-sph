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
#include <vector>
#include <png.h>

#include "includes/constant.h"
#include "includes/Camera.h"
#include "includes/sphere.h"
#include "includes/shader.h"
#include "includes/cube.h"
#include "includes/scene.h"
#include "SPH/SPH.h"
#include "MarchingCube/MarchingCube.h"

#define SQUARE_SIDE 20
#define MIN_ALT 0.5f
#define MAX_ALT 100.0f

typedef unsigned long long ulong64_t;

const int particleSize = 5;
const float sphereSize = 0.03f;
const GLfloat x_min = -SQUARE_SIDE;
const GLfloat x_max = SQUARE_SIDE;
const GLfloat y_min = MIN_ALT;
const GLfloat y_max = MAX_ALT;
const GLfloat z_min = -SQUARE_SIDE;
const GLfloat z_max = SQUARE_SIDE;

struct ObjectData {
    GLuint ModelArrayID, ModelVBO, ModelColorVBO, ModelNormalVBO, EBO, indexSize;
};

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

GLfloat boundingCubeColors[] = {
    0.0f, 0.0f, 0.0f
};

GLfloat colorArray[] = {
    1.0f, 1.0f, 1.0f
};


const int boundingCubeSize = 8;
const int boundingIndicesSize = 24;

void setupMeshVAO(Mesh mesh, GLfloat* color_vector, vector<ObjectData> &objectVector);
void setupMeshVAO(Mesh mesh, GLfloat* color_vector, ObjectData &object);
void setupPrimitiveVAO(GLfloat* vertices, GLuint* indices, GLfloat* color_vector, int vertexCount, ObjectData &object);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void initFrameBuffers(GLuint &fbo, GLuint &colorBuffer, GLuint &depthBuffer);
void setCallBacks(GLFWwindow* window);
void drawGenericObject(GLuint &VAO, GLuint programID,
                        glm::mat4 proj,
                        glm::mat4 view,
                        int size,
                        bool elemental,
                        glm::vec3 translationVector = glm::vec3(0,0,0),
                        glm::vec3 scaleVector = glm::vec3(1,1,1),
                        GLfloat rotationAngle = 0,
                        glm::vec3 rotationAxis = glm::vec3(1,0,0));

bool initOpenGL();




#endif