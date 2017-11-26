#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

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

float isolevel = 0.5;

int getParticleBitmask(float *cubeDelta)
{
    int bitmask = 0;
    if (cubeDelta[0] < isolevel) bitmask |= 1;
    if (cubeDelta[1] < isolevel) bitmask |= 2;
    if (cubeDelta[2] < isolevel) bitmask |= 4;
    if (cubeDelta[3] < isolevel) bitmask |= 8;
    if (cubeDelta[4] < isolevel) bitmask |= 16;
    if (cubeDelta[5] < isolevel) bitmask |= 32;
    if (cubeDelta[6] < isolevel) bitmask |= 64;
    if (cubeDelta[7] < isolevel) bitmask |= 128;
    return bitmask;
}




#endif