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

using namespace std;
using namespace glm;

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

// initiate, set color and lighting
void init() {
    for (int i = 0; i < 3; ++i) frame_base[i] = FRAME_BASE_INIT[i];

    srand(time(NULL));
    sph = SPH(FRAME_LENGTH);

    GLfloat light_position[2][4] = {{0, 0, 0.5, 0}, {(float)frame_base[0], (float)frame_base[1], (float)frame_base[2], 1}};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position[0]);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position[1]);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);	glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0, GL_AMBIENT, I02);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, I08);
    glLightfv(GL_LIGHT0, GL_SPECULAR, I);
    glLightfv(GL_LIGHT1, GL_AMBIENT, I02);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, I08);
    glLightfv(GL_LIGHT1, GL_SPECULAR, I);

    glMaterialfv(GL_FRONT, GL_SPECULAR, I);
    glMaterialf (GL_FRONT, GL_SHININESS, 40);

    glClearColor(color4_black[0], color4_black[1], color4_black[2], color4_black[3]);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}


void reshape(GLsizei width, GLsizei height) {
    glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    width = WINDOW_WIDTH; height = WINDOW_HEIGHT;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(height <= 0) {
        height = 1;
    }
    gluPerspective(75.0, (GLdouble)width/(GLdouble)height, 0.5, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// draw edge for container
void drawEdge(const double _pos[], const double _scale[]) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color4_white);

    glTranslatef(_pos[0] + frame_base[0], _pos[1] + frame_base[1], _pos[2] + frame_base[2]);
    glRotated(0, 0, 0, 0);
    glScalef(_scale[0], _scale[1], _scale[2]);
    //glScalef(FRAME_SCALE[0], FRAME_SCALE[1], FRAME_SCALE[2]);		
    glutSolidCube(1);

    glPopMatrix();
}

// draw sphere for particle mode/debug mode
void drawSphere(const vec3 _pos) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color4_sphere);

    glTranslatef(_pos[0] + frame_base[0], _pos[1] + frame_base[1], _pos[2] + frame_base[2]);
    //glScalef(FRAME_SCALE[0], FRAME_SCALE[1], FRAME_SCALE[2]);	
    glRotated(0, 0, 0, 0);
    glutSolidSphere(0.3f, 100, 100);

    glPopMatrix();	
}

// draw the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // container
    for (int i = 0; i < 12; i++) {
        drawEdge(WALL_EDGE[i], WALL_SCALE[i]);
    }

    if (debug) {
        // spheres for debug mode
        list<Particle> particle_list = sph.getList();
        for (Particle &particle : particle_list) {
            drawSphere(particle.getPosition());
        }
    } else {
        // mesh for fuild simulation
        list<Particle> particle_list = sph.getList();
        vector<vec3> v_list;
        vector<int> index_list;
        MarchingCube marching_cube(FRAME_LENGTH, GRID_LENGTH, &particle_list);
        marching_cube.count(v_list, index_list);
        // printf("size: %d\n", (int)index_list.size());

        glEnable(GL_COLOR_MATERIAL);
        glBegin(GL_TRIANGLES);
        glColor4f(color4_sphere[0], color4_sphere[1], color4_sphere[2], color4_sphere[3]);
        for (int i = 0; i < index_list.size(); ++i) {
            int p = index_list[i];
            glVertex3f(v_list[p][0] + frame_base[0], v_list[p][1] + frame_base[1], v_list[p][2] + frame_base[2]);
        }
        glEnd();	
        glDisable(GL_COLOR_MATERIAL);
    }
    glFlush();
    glutSwapBuffers();
}

// update for each frame
void update(int value) {
    // move speed
    if (pos_now[0] != pos_pre[0] || pos_now[1] != pos_pre[1] || pos_now[2] != pos_pre[2]) {
        vec3 delta = pos_now - pos_pre;
        vec3 base_new = vec3(frame_base) + delta;
        for (int i = 0; i < 2; ++i) {
            if (abs(base_new[i]) > FRAME_LENGTH[i] * 2) {
                base_new[i] = base_new[i] / abs(base_new[i]) * (FRAME_LENGTH[i] - 0.0001);
            }
        }
        delta = base_new - frame_base;
        for (int i = 0; i < 3; ++i) frame_base[i] = base_new[i];
        pos_pre = pos_now;
        sph.setBase(delta);

        		// printf("base %lf, %lf, %lf\n", frame_base[0], frame_base[1], frame_base[2]);
    }

    // cout movement
    sph.move();
    glutTimerFunc(DELTA_TIME, update, 0);
    glutPostRedisplay();
}

vec3 coordinateTrans(int x, int y) {
    x -= WINDOW_WIDTH / 2; y -= WINDOW_HEIGHT / 2;
    return vec3((x + 0.0) / WINDOW_WIDTH * (FRAME_LENGTH[0] * 2.2f), (y + 0.0) / WINDOW_HEIGHT * (FRAME_LENGTH[1] * 2.2f) * -1, 0);
}

bool initOpenGL()
{
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
}

void renderLoop()
{

}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("SPH - ^.^");

    init();

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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


    const int particleSize = 10;
    sph.add(Particle(vec3(0, 0, 0), vec3(0, 0, 0)));
    for (int i = 0; i < particleSize; i+=1 )
        for (int j = 0; j < particleSize; j+=1)
            for (int k = 0; k < particleSize; k+=1)
                sph.add(Particle(vec3(-0.50 + i * 0.03, -0.2 + j * 0.03, -0.05 + k * 0.03), vec3(0, 0, 0)));

    vector<ObjectData> spheres;
    GLfloat colorArray[] = {1.0f, 0.0f, 0.0f};
    for (int i = 0; i < particleSize*particleSize*particleSize; i++) {
        setupMeshVAO(Sphere(0.03f, 3).getMesh(), colorArray, spheres);
    }

    glutDisplayFunc(display);

    glutTimerFunc(DELTA_TIME, update, 0);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}
