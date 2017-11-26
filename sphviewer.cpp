#include "sphviewer.h"

#define OFFSCREEN true

const int particleSize = 15;

using namespace std;
using namespace glm;
using namespace chrono;

glm::vec3 lightPos = glm::vec3(0, 10, 0);

static Camera camera(WINDOW_WIDTH, WINDOW_WIDTH, x_min, x_max, y_min, y_max, z_min, z_max);

SPH sph;

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

    for (i = 0; i < v.size();) {
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
    glBufferData(GL_ARRAY_BUFFER, size, ModelColorArray, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &(object.ModelNormalVBO));
    glBindBuffer(GL_ARRAY_BUFFER, object.ModelNormalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ModelNormalArray), ModelNormalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(0);

    objectVector.push_back(object);
}

void setupMeshVAO(Mesh mesh, GLfloat* color_vector, ObjectData &object)
{
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
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(0);
}

void setupPrimitiveVAO(GLfloat* vertices, GLuint* indices, GLfloat* color_vector, int vertexCount, int indicesCount, ObjectData &object)
{
    object.indexSize = indicesCount; //# of vertices = arraysize/3 (x,y,z)
    int size = vertexCount*sizeof(GLfloat)*3;

    GLfloat ModelColorArray[vertexCount*3];
    // GLfloat ModelNormalArray[normals.size()*3];

    ulong64_t i = 0;
 
 
    // Working with a regular array is easier than working with vectors for passing on to the VS/FS
    // for (auto it = normals.begin(); it != normals.end(); it++) {
    //     ModelNormalArray[i++] = it->x;
    //     ModelNormalArray[i++] = it->y;
    //     ModelNormalArray[i++] = it->z;
    // }

    for (i = 0; i < vertexCount*3;) {
        for (int ctr = 0; ctr < 3; ctr++) {
            ModelColorArray[i++] = color_vector[ctr];
        }
    }

    // We are only generating one VA at a time for each object
    glGenVertexArrays(1, &(object.ModelArrayID));
    glBindVertexArray(object.ModelArrayID);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // glEnableVertexAttribArray(2);

    // Separate buffers are created for the vertex buffer, color buffer, and normal buffer of the 3D object
    // These buffers store the data and forward it on to the GPU to be processed further by the VS/FS
    glGenBuffers(1, &(object.ModelVBO));
    glBindBuffer(GL_ARRAY_BUFFER, object.ModelVBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &(object.ModelColorVBO));
    glBindBuffer(GL_ARRAY_BUFFER, object.ModelColorVBO);
    glBufferData(GL_ARRAY_BUFFER, size, ModelColorArray, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &(object.EBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexSize*sizeof(GLuint), indices, GL_STATIC_DRAW);

    // glGenBuffers(1, &(object.ModelNormalVBO));
    // glBindBuffer(GL_ARRAY_BUFFER, object.ModelNormalVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(ModelNormalArray), ModelNormalArray, GL_STATIC_DRAW);
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(0);
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
    if (!OFFSCREEN) {
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    }
}

void drawGenericObject(GLuint &VAO, GLuint programID,
                        glm::mat4 proj,
                        glm::mat4 view,
                        int size,
                        bool elemental,
                        int elementMode,
                        glm::vec3 translationVector,
                        glm::vec3 scaleVector,
                        GLfloat rotationAngle,
                        glm::vec3 rotationAxis)
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
        if (elementMode == 0) {
            glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
        } else if (elementMode == 1) {
            glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
        }
    } else {
        glDrawArrays(GL_TRIANGLES, 0, size*3);
    }
    // Unbinds the vertex array
    glBindVertexArray(0);
}

void initFrameBuffers(GLuint &fbo, GLuint &colorBuffer, GLuint &depthBuffer, 
                        GLuint &stencilBuffer, GLuint &renderedTexture)
{
    if (OFFSCREEN) {
        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenRenderbuffers(1,&colorBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, WINDOW_WIDTH, WINDOW_HEIGHT);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        // glGenRenderbuffers(1, &stencilBuffer);
        // glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer);
        // glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, WINDOW_WIDTH, WINDOW_HEIGHT);
        // glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);
        // glEnable(GL_DEPTH_TEST);
        // glDepthFunc(GL_LESS);
    }
}

int main(int argc, char **argv) {
    sph = SPH(FRAME_LENGTH);
    sph.add(Particle(vec3(0, 0, 0), vec3(0, 0, 0)));
    for (int i = 0; i < particleSize; i+=1 )
        for (int j = 0; j < particleSize; j+=1)
            for (int k = 0; k < particleSize; k+=1)
                sph.add(Particle(vec3(-0.00 + i * sphereSize, 0.0 + j * sphereSize, -0.00 + k * sphereSize), vec3(0, 0, 0)));

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
    static GLuint fbo, colorBuffer, depthBuffer, stencilBuffer, renderedTexture;
    initFrameBuffers(fbo, colorBuffer, depthBuffer, stencilBuffer, renderedTexture);
    glClearColor(0.5f, 0.1f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

    glm::mat4 proj;
    glm::mat4 view;


    vector<ObjectData> spheres;
    vector<ObjectData> floorObjectVector;
    ObjectData boundingCube;
    ObjectData marchingCube;
    Scene floorScene;
    floorScene.addFloor(vec4(x_min, x_max, z_min, z_max));
    vector<Mesh> floorMesh = floorScene.getMesh();
    vector<vector<GLfloat>> floorColors = floorScene.getColors();

    for (int i = 0; i < particleSize*particleSize*particleSize; i++) {
        setupMeshVAO(Sphere(sphereSize, 3).getMesh(), colorArray, spheres);
    }

    for (int i = 0; i < floorMesh.size(); i++) {
        setupMeshVAO(floorMesh.at(i), &floorColors.at(i)[0], floorObjectVector);
    }

    // setupMeshVAO(Cube(FRAME_LENGTH[0], FRAME_LENGTH[1], FRAME_LENGTH[2]).getMesh(), colorArray, boundingCube);
    setupPrimitiveVAO(boundingCubeVertices, boundingCubeIndices, boundingCubeColors, boundingCubeSize, boundingIndicesSize, boundingCube);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    glUseProgram(programID);
    int k = 0;

    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
            currentFrame = lastFrame;
            sph.move();
        // Used for capturing the WASD keys and mouse
        if (!OFFSCREEN) {
            camera.processInput(window, deltaTime);
        }
        // Defines what can be seen by the camera along with the clip boundaries of the scene
        proj = glm::perspective(glm::radians(camera.getFOV()), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.2f, 500.0f);
        view = camera.getCameraView();
        int i = 0;
        list<Particle> particle_list = sph.getList();

        //rendering part
        if (OFFSCREEN) {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        } else {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(5.0f);
        drawGenericObject(boundingCube.ModelArrayID, programID, proj, view, boundingCube.indexSize, true, 0, vec3(0,FRAME_LENGTH[0],0), vec3(FRAME_LENGTH[0]*2,FRAME_LENGTH[0]*2,FRAME_LENGTH[0]*2));
        for (auto it = floorObjectVector.begin(); it != floorObjectVector.end(); it++) {
            drawGenericObject(it->ModelArrayID, programID, proj, view, it->indexSize, false);
        }
        glLineWidth(1.0f);
        for (Particle &particle : particle_list) {
            if (i < spheres.size()) {
                drawGenericObject(spheres.at(i).ModelArrayID, programID, proj, view, spheres.at(i).indexSize, false, 0, particle.getPosition()+vec3(0,1.8f,0));
            }
            i++;
        }

        if (OFFSCREEN) {
            uint8_t data[WINDOW_WIDTH*WINDOW_HEIGHT*3];
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,GL_RGB,GL_UNSIGNED_BYTE,&data[0]);

            // cout << glGetError();

            k++;
            char imgNum[10];
            string imVal;
            sprintf(imgNum, "img-%03d",k);
            for (int u = 0; imgNum[u] != '\0'; u++) {
                imVal += imgNum[u];
            }
            string imgName = "img/" + imVal + ".bmp";
            cout<<imgName<<endl;

            FIBITMAP* image = FreeImage_ConvertFromRawBits(data, WINDOW_WIDTH, WINDOW_HEIGHT, 3 * WINDOW_WIDTH, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);

            FreeImage_Save(FIF_BMP, image, imgName.c_str(), 0);
        }
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	glDeleteProgram(programID);
    // glDeleteFramebuffers(1,&fbo);
    // glDeleteRenderbuffers(1,&colorBuffer);
	// glDeleteRenderbuffers(1,&depthBuffer);
    glfwTerminate();
    return 0;
}
