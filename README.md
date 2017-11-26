# SPH Computer Graphics



Computing fluid dynamics is a difficult problem as the physics involved are very computationally expensive. However, in many computer graphics applications, it is sufficient to use an approximation of the dynamics of the fluids for rendering purposes. One such method used in several areas is called the Smoothed Particle Hydrodynamics (SPH) method. In this method, the forces acting on a particle are "smoothed" using a kernal fucntion that approximates the forces on the particle in relation to its neighbours.

This method is extensively used in animation and can also be used to simulate cloth movements.

# Implementation

In this project, we have modified the SPH algorithm for implementation in a discretised case. The particles in this case are assumed to be spherical in spahe