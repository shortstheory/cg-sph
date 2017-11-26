#ifndef PARTICLE_H
#define PARTICLE_H

#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
// #include "../includes/vector3D.h"
#include "../includes/constant.h"

using namespace std;
using namespace glm;

class Particle {
    public:
        Particle();
        Particle(const vec3 &_position, const vec3 &_velocity);

        // count different parameters based on its neighbors and distance to the neighbors
        void countDensity(const vector<Particle> &neighbour, const vector<double> &r);
        void countPressure(const vector<Particle> &neighbour, const vector<double> &r);
        void countForce(const vector<Particle> &neighbour, const vector<double> &r);
        void countViscosity(const vector<Particle> &neighbour, const vector<double> &r);
        void countColorfield(const vector<Particle> &neighbour, const vector<double> &r);
        void countVelocity(const vec3 &base_move);

        // move to new position
        void move();
        // check the position, reflect and damp the velocity based on the boundary
        void check(const vec3 &bound, const vec3 &bound_v);
        void check_obstacle(const vector <vec3> &plane_list, const vector<double> offset_list, const vector<vec3> velocity_list);


        // kernal functions
        double KernelPoly(const Particle &_particle, double r) const;
        double KernalPolyLap(const Particle &_particle, double r) const;
        vec3 KernalPolyGrad(const Particle &_particle, double r) const;
        vec3 KernalSpik(const Particle &_particle, double r) const;
        double KernalVisc(const Particle &_particle, double r) const;

        // Gets
        vec3 countMid(const vec3 &_pos) const {
            return (position + _pos) * 0.5f;
        }
        vec3 getPosition() const {
            return position;
        }
        vec3 getVelocity() const {
            return velocity;
        }
        double getPressure() const {
            return pressure;
        }

    private:
        // parameters for one particle
        vec3 position, velocity, viscosity, tenssion, force;
        double density, pressure;
};

#endif
