#ifndef MARCHINGCUBE_H
#define MARCHINGCUBE_H

#include <list>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "../includes/constant.h"
#include "lookup_list.h"
#include "../SPH/Particle.h"
// #include "../includes/vec3.h"

using namespace std;
using namespace glm;

class MarchingCube {
    public:
        MarchingCube() {}
        MarchingCube(const vec3 _bound, double _l, list<Particle> *_particles);
        ~MarchingCube();

        // count the mesh
        void count(vector<vec3> &vertexs, vector<int> &tri_index);

    private:
        // return the closeset particle if there is a particle contain this vertex, otherwise NULL
        Particle* check(vec3 v) const;

        // if one in one not, return the one in, otherwise, return NULL
        Particle* diff(Particle* a, Particle* b) const;

        // return the intersect point one the edge
        vec3* countInter(const vec3 &v, const vec3 &u, Particle* tmp) const;

    private:
        // position and size of the container
        vec3 base, bound;
        // edge length of the cube
        double l;
        // number of edges on each coordinate
        int total_edge[3];
        // total number of vertex and edge
        int sum_v, sum_e;

        // list of particles
        list<Particle> *particles;

        /*	list of all edges for cubes
            point to the cut point if intersect with the mesh, otherwise NULL
            edge are numbered by related vetex towards z, y, x
         */
        vector<vec3*> intersections;
        // related index offset for twelve edges in a cube
        vector<int> offset_edge;
        // related index offset for two edge that is one step different on each coordinate
        int dx, dy, dz;
        // map edge to index of final mesh vertex
        vector<int> mapping;

        // list of all vertexs for cubes, point to the closest particle if it is inside, otherwise NULL
        vector<Particle*> inside;
        // related index offset for eight vertex in a cube
        vector<int> offset;

};

#endif
