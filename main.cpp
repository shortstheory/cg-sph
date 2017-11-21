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

#include "includes/constant.h"
#include "includes/Camera.h"
#include "includes/sphere.h"
// #include "includes/vector3D.h"
#include "includes/shader.h"
#include "SPH/SPH.h"
#include "MarchingCube/MarchingCube.h"

#define SQUARE_SIDE 20
#define MIN_ALT 0.5f
#define MAX_ALT 100.0f
#define GLUT false

using namespace std;
using namespace glm;
using namespace chrono;

glm::vec3 lightPos = glm::vec3(0, 10, 20);

typedef unsigned long long ulong64_t;

const GLfloat x_min = -SQUARE_SIDE;
const GLfloat x_max = SQUARE_SIDE;
const GLfloat y_min = MIN_ALT;
const GLfloat y_max = MAX_ALT;
const GLfloat z_min = -SQUARE_SIDE;
const GLfloat z_max = SQUARE_SIDE;

static Camera camera(WINDOW_WIDTH, WINDOW_WIDTH, x_min, x_max, y_min, y_max, z_min, z_max);

SPH sph;

bool debug = true;
bool is_press = false;
vec3 pos_pre, pos_now;
vec3 frame_base;

struct ObjectData {
    GLuint ModelArrayID, ModelVBO, ModelColorVBO, ModelNormalVBO, EBO, indexSize;
};

void setupMeshVAO(Mesh mesh, GLfloat* color_vector, vector<ObjectData> &objectVector)
{
    ObjectData object;

    vector<GLfloat> v = mesh.getVertices();
    GLfloat* vertices = &v[0];
    object.indexSize = v.size()/3; //# of vertices = arraysize/3 (x,y,z)
    int size = v.size()*sizeof(GLfloat);

    vector<glm::vec3> normals = mesh.getNormals();
    GLfloat ModelColorArray[v.size()];
    GLfloat ModelNormalArray[normals.size()*3];

    ulong64_t i = 0;
    // Working with a regular array is easier than working with vectors for passing on to the VS/FS
    for (auto it = normals.begin(); it != normals.end(); it++) {
        ModelNormalArray[i++] = it->x;
        ModelNormalArray[i++] = it->y;
        ModelNormalArray[i++] = it->z;
    }

    for (i = 0; i < v.size(); i++) {
        for (int ctr = 0; ctr < 3; ctr++) {
            ModelColorArray[i++] = color_vector[ctr];
        }
    }

    // We are only generating one VA at a time for each object
    glGenVertexArrays(1, &(object.ModelArrayID));
    glBindVertexArray(object.ModelArrayID);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // Separate buffers are created for the vertex buffer, color buffer, and normal buffer of the 3D object
    // These buffers store the data and forward it on to the GPU to be processed further by the VS/FS
    glGenBuffers(1, &(object.ModelVBO));
    glBindBuffer(GL_ARRAY_BUFFER, object.ModelVBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &(object.ModelColorVBO));
    glBindBuffer(GL_ARRAY_BUFFER, object.ModelColorVBO);
    glBufferData(GL_ARRAY_BUFFER, size, color_vector, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &(object.ModelNormalVBO));
    glBindBuffer(GL_ARRAY_BUFFER, object.ModelNormalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ModelNormalArray), ModelNormalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    objectVector.push_back(object);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.scroll_callback(window, xoffset, yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouse_callback(window, xpos, ypos);
}

bool initOpenGL()
{
    // sph = SPH(FRAME_LENGTH);

    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}

void setCallBacks(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void renderLoop()
{

}

void drawGenericObject(GLuint &VAO, GLuint programID,
                        glm::mat4 proj,
                        glm::mat4 view,
                        int size,
                        bool elemental,
                        glm::vec3 translationVector = glm::vec3(0,0,0),
                        glm::vec3 scaleVector = glm::vec3(1,1,1),
                        GLfloat rotationAngle = 0,
                        glm::vec3 rotationAxis = glm::vec3(1,0,0))
{
    // Must match the name specified in the vertex shader.
    GLuint matrixID = glGetUniformLocation(programID, "MVP");
    GLuint modelID = glGetUniformLocation(programID, "model");
    GLuint lightID = glGetUniformLocation(programID, "LightPos");

    // Binding the vertex array is equivalent to setting a global variable for the rest of the gl calls here
    glBindVertexArray(VAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translationVector);
    model = glm::scale(model, scaleVector);
    model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
    glm::mat4 MVP = proj*view*model;

    // Pass our arrays over to the vertexshader for further transformations
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
    glUniform3fv(lightID, 1, &lightPos[0]);
    if (elemental) {
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, size*3);
    }
    // Unbinds the vertex array
    glBindVertexArray(0);
}

int main(int argc, char **argv) {
    const int particleSize = 10;
    sph = SPH(FRAME_LENGTH);
    sph.add(Particle(vec3(0, 0, 0), vec3(0, 0, 0)));
    for (int i = 0; i < particleSize; i+=1 )
        for (int j = 0; j < particleSize; j+=1)
            for (int k = 0; k < particleSize; k+=1)
                sph.add(Particle(vec3(-0.50 + i * 0.03, -0.2 + j * 0.03, -0.05 + k * 0.03), vec3(0, 0, 0)));

    if(!glfwInit()) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return false;
    }
    if (!initOpenGL()) {
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cubes!", NULL, NULL);
    if(window == NULL){
        glfwTerminate();
        return -1;
    }
    setCallBacks(window);
    glewExperimental = true;
    std::cout << glewInit();
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return false;
    }
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

    glm::mat4 proj;
    glm::mat4 view;



    vector<ObjectData> spheres;
    GLfloat colorArray[] = {1.0f, 0.0f, 0.0f};

    for (int i = 0; i < particleSize*particleSize*particleSize; i++) {
        setupMeshVAO(Sphere(0.03f, 3).getMesh(), colorArray, spheres);
    }

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        currentFrame = lastFrame;        
        sph.move();
        // Used for capturing the WASD keys and mouse
        camera.processInput(window, deltaTime);
        // Defines what can be seen by the camera along with the clip boundaries of the scene
        proj = glm::perspective(glm::radians(camera.getFOV()), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.2f, 500.0f);
        view = camera.getCameraView();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);
        int i = 0;
        list<Particle> particle_list = sph.getList();
        for (Particle &particle : particle_list) {
            if (i < spheres.size()) {
                drawGenericObject(spheres.at(i).ModelArrayID, programID, proj, view, spheres.at(i).indexSize, false, particle.getPosition()+vec3(0,4,0));
            }
            i++;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	glDeleteProgram(programID);

	glfwTerminate();
    return 0;
}
