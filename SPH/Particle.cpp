#include "Particle.h"
#include <stdio.h>
#include <iostream>

const double KERNAL_POLY_CONSTANT = 4.921875;
const double KERNAL_POLY_GRAD_CONSTANT = -9.4000888;
const double KERNAL_POLY_LAP_CONSTANT = 9.4000888;
const double KERNAL_SPIK_GRAD_CONSTANT = -14.323944878;
const double KERNAL_VISC_LAPLACIAN_CONSTANT = 14.323944878;

using namespace std;

Particle::Particle(): position(0.0f, 0.0f, 0.0f), velocity(0.0f, 0.0f, 0.0f) {}
Particle::Particle(const vec3 &_position, const vec3 &_velocity): position(_position), velocity(_velocity) {}

void Particle::countDensity(const vector<Particle> &neighbour, const vector<double> &r) {
    density = mass * KERNAL_POLY_CONSTANT / pow(SMOOTHING_WIDTH, 3);
    int l = neighbour.size();
    for (int i = 0; i < l; ++i) {
        density += KernelPoly(neighbour[i], r[i]) * mass;
    }
}

void Particle::countPressure(const vector<Particle> &neighbour, const vector<double> &r) {
    // pressure =  1000 * (density - 12);
    pressure = pow(density / 1000.0, 7) - 1;
}

void Particle::countForce(const vector<Particle> &neighbour, const vector<double> &r) {
    force = vec3(0, 0, 0);
    int l = neighbour.size();
    // printf("LVALUE %d\n", l);	
    for (int i = 0; i < l; ++i) {
        force += (float)(mass * 0.5 / density *  (pressure + neighbour[i].getPressure())) * KernalSpik(neighbour[i], r[i]);
    }
}

void Particle::countViscosity(const vector<Particle> &neighbour, const vector<double> &r) {
    viscosity = vec3(0.0f, 0.0f, 0.0f);
    int l = neighbour.size();		
    for (int i = 0; i < l; ++i) {
        viscosity += (float)(mass / density * KernalVisc(neighbour[i], r[i]) * 3e-4) * (neighbour[i].getVelocity() - velocity);
    }
}

void Particle::countColorfield(const vector<Particle> &neighbour, const vector<double> &r) {
    vec3 color_grad(0.0f, 0.0f, 0.0f);
    double color_lap = 0;
    int l = neighbour.size();
    float ratio = mass / density;
    for (int i = 0; i < l; ++i) {
        color_grad += ratio * KernalPolyGrad(neighbour[i], r[i]);
        color_lap += ratio * KernalPolyLap(neighbour[i], r[i]);
    }
    if (getMagnitude(color_grad) > 1e-6) tenssion = - (float)(color_lap * 1e-4) * getUnitVector(color_grad);
    else tenssion = vec3(0.0f, 0.0f, 0.0f);
}

void Particle::countVelocity(const vec3 &base_move) {
    vec3 acce = (force + viscosity + tenssion) * 1.0f/(float)density;
    acce[1] += g;
    velocity += acce * (float)(DELTA_TIME/1000.0f) + base_move * (float)(1.0f/DELTA_TIME) * 200.0f;	
}

void Particle::move() {
    // printf("force: %lf, %lf, %lf\n", force[0], force[1], force[2]);
    // printf("viscosity: %lf, %lf, %lf\n", viscosity[0], viscosity[1], viscosity[2]);
    // printf("tenssion: %lf, %lf, %lf\n", tenssion[0], tenssion[1], tenssion[2]);
    // printf("velocity0: %lf, %lf, %lf\n", velocity[0], velocity[1], velocity[2]);		
    position += velocity * (float)(DELTA_TIME / 1000.0f);
    // printf("position: %lf, %lf, %lf\n", position[0], position[1], position[2]);
    // printf("density: %lf\n", density);
    // printf("pressure: %lf\n", pressure);
}
void Particle::check(const vec3 &bound) {
    for (int i = 0; i < 3; ++i) {
        if (abs(position[i]) > bound[i] - 1e-6) {
            position[i] = bound[i] * (position[i] / abs(position[i])) * 2 - position[i];
            velocity[i] *= COEFICIENT_OF_RESTITUTION;
        }
        /*
           if (abs(position[i]) > bound[i] - 1e-4 && velocity[i] * position[i] > 0) {
           velocity[i] *= -0.5f;
           }*/
    }
}

double Particle::KernelPoly(const Particle &_particle, double r) const {
    return KERNAL_POLY_CONSTANT / pow(SMOOTHING_WIDTH, 9) * pow(SMOOTHING_WIDTH2 - r * r, 3);
}

double Particle::KernalPolyLap(const Particle &_particle, double r) const {
    return KERNAL_POLY_LAP_CONSTANT / pow(SMOOTHING_WIDTH, 9) * (SMOOTHING_WIDTH2 - r * r) * (7.0f * r * r - 3.0f * SMOOTHING_WIDTH2);
}

vec3 Particle::KernalPolyGrad(const Particle &_particle, double r) const {
    return (float)(KERNAL_POLY_GRAD_CONSTANT / pow(SMOOTHING_WIDTH, 9) * pow(SMOOTHING_WIDTH2 - r * r, 2)) * (_particle.getPosition() - position);
}

vec3 Particle::KernalSpik(const Particle &_particle, double r) const {
    return (float)(KERNAL_SPIK_GRAD_CONSTANT / pow(SMOOTHING_WIDTH, 6) * pow(SMOOTHING_WIDTH - r, 2)) * (_particle.getPosition() - position);
}

double Particle::KernalVisc(const Particle &_particle, double r) const {
    return KERNAL_VISC_LAPLACIAN_CONSTANT / pow(SMOOTHING_WIDTH, 6) * (SMOOTHING_WIDTH - r);		
}
