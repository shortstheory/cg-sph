#include "scene.h"

Scene::Scene()
{
}

vector<Mesh> Scene::getMesh()
{
    return meshes;
}

vector<vector<GLfloat>> Scene::getColors()
{
    return colors;
}

void Scene::addCube(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color_vec)
{
    Cube cube = Cube(dimensions.x, dimensions.y, dimensions.z);
    Mesh mesh = cube.getMesh();
    int len = (mesh.getVertices()).size();

    vector<GLfloat> color_vector = getColorVector(color_vec, len*2);

    glm::mat4 T;
    T = glm::translate(T, center);
    T = glm::translate(T, glm::vec3(-dimensions.x/2, -dimensions.y/2, -dimensions.z/2));
    mesh.transform(T);

    meshes.push_back(mesh);
    colors.push_back(color_vector);
}

void Scene::addSphere(glm::vec3 center, float radius, glm::vec3 color_vec, int detail_level)
{
    Sphere sphere = Sphere(radius, detail_level);
    Mesh mesh  = sphere.getMesh();
    int len = (mesh.getVertices()).size();

    vector<GLfloat> color_vector = getColorVector(color_vec, len);

    glm::mat4 T;
    T = glm::translate(T, center);
    mesh.transform(T);

    meshes.push_back(mesh);
    colors.push_back(color_vector);
}

void Scene::addCylinder(glm::vec3 center, float height, float radius, glm::vec3 color_vec, glm::vec3 orientation)
{
    Cylinder cylinder = Cylinder(height, radius, orientation);
    Mesh mesh = cylinder.getMesh();
    int len = (mesh.getVertices()).size();

    vector<GLfloat> color_vector = getColorVector(color_vec, len);

    glm::mat4 T;
    T = glm::translate(T, center);
    mesh.transform(T);

    meshes.push_back(mesh);
    colors.push_back(color_vector);
}

void Scene::addFloor(glm::vec4 bounds) //x-z bounds
{
    for (float i = bounds.x; i <= bounds.y; i+=1)
        addCylinder(glm::vec3(i, 0, bounds.z+bounds.w), (float) bounds.w-bounds.z, (float) 0.005, glm::vec3(0.7, 0.7, 0.7), glm::vec3(0, 0, 1));

    for (float i = bounds.z; i <= bounds.w; i+=1)
        addCylinder(glm::vec3(bounds.x+bounds.y, 0, i), (float) bounds.x-bounds.y, (float) 0.005, glm::vec3(0.7, 0.7, 0.7), glm::vec3(1, 0, 0));
    addCube(glm::vec3(0, -0.1, 0), glm::vec3(bounds.y-bounds.x + 10, 0.1, bounds.w-bounds.z + 10), glm::vec3(0.41, 0.65, 0.45));
}
