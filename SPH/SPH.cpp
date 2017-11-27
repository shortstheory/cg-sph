#include "SPH.h"
#include <math.h>
#include <vector>
#include "Octree.hpp"
using namespace std;

void SPH::move() {
    shm_t+=DELTA_TIME/1000.0f;
    bound[0]=0.8+0.2*cos(10*shm_t);
    bound_v[0]=-2*sin(10*shm_t);
    // bound[0]-=0.01;
    int l = particle_list.size();
    // get the list of neighbors that is close enough for each particle with the distance
    vector<vector<Particle> > neighbour_list(l, vector<Particle>(0));
    vector<vector<double> > dis_list(l, vector<double>(0));
    int i = 0, j;
    for (auto it0 = particle_list.begin(); it0 != particle_list.end(); ++i) {
        Particle p0 = *it0;
        j = i + 1;
        for (auto it1 = ++it0; it1 != particle_list.end(); ++it1, ++j) {
            Particle p1 = *it1;
            double r = getMagnitude((p0.getPosition() - p1.getPosition()));

            if (r < SMOOTHING_WIDTH) {
                neighbour_list[i].push_back(p1);
                dis_list[i].push_back(r);
                neighbour_list[j].push_back(p0);
                dis_list[j].push_back(r);
            }
        }
    }

    // count density
    i = 0;
    for (auto it0 = particle_list.begin(); it0 != particle_list.end(); ++it0, ++i) {
        (*it0).countDensity(neighbour_list[i], dis_list[i]);
    }

    // count pressure
    i = 0;
    for (auto it0 = particle_list.begin(); it0 != particle_list.end(); ++it0, ++i) {
        (*it0).countPressure(neighbour_list[i], dis_list[i]);
    }

    // count different force
    i = 0;
    for (auto it0 = particle_list.begin(); it0 != particle_list.end(); ++it0, ++i) {
        (*it0).countForce(neighbour_list[i], dis_list[i]);
        (*it0).countViscosity(neighbour_list[i], dis_list[i]);
        (*it0).countColorfield(neighbour_list[i], dis_list[i]);
    }

    vector <vec3> planes;
    vector <double> offset;
    vector <vec3> velocity;
    planes.push_back(vec3(1.0f, 0.0f, 0.0f));
    offset.push_back(-1.0f+0.7*cos(10*shm_t));
    velocity.push_back(vec3(-7*sin(10*shm_t),1.0f,0.1f));
    planes.push_back(vec3(-1.0f, 0.0f, 0.0f));
    offset.push_back(-1.7f);
    velocity.push_back(vec3(0,0,0));
    planes.push_back(vec3(0.0f, 1.0f, 0.0f));
    offset.push_back(-1.0f);
    velocity.push_back(vec3(0,0,0));
    planes.push_back(vec3(0.0f, -1.0f, 0.0f));
    offset.push_back(-2.4f);
    velocity.push_back(vec3(0,0,0));
    planes.push_back(vec3(0.0f, 0.0f, 1.0f));
    offset.push_back(-1.7f);
    velocity.push_back(vec3(0,0,0));
    planes.push_back(vec3(0.0f, 0.0f, -1.0f));
    offset.push_back(-1.7f);
    velocity.push_back(vec3(0,0,0));
    // planes.push_back(vec3(1.0f, 1.0f, 1.0f));
    // offset.push_back(-0.8f);

    // count final velocity and move
    for (auto it0 = particle_list.begin(); it0 != particle_list.end(); ++it0) {
        (*it0).countVelocity(base_move);
        (*it0).check_obstacle(planes,offset,velocity);
        // (*it0).check(bound,bound_v);
        (*it0).move();
    }

    // reset container movement
    base_move = vec3(0.0f, 0.0f, 0.0f);
}
