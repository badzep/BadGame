#include "Simulation.h"
#include <ode/ode.h>


void initialize_simulation(Simulation* simulation) {
    simulation->world = dWorldCreate();
    simulation->space = dSimpleSpaceCreate(0);
    simulation->contacts = dJointGroupCreate(0);
    dWorldSetGravity(simulation->world, 0.0, -9.8, 0.0);
}

void nearCallback (void *data, dGeomID geometry1, dGeomID geometry2) {
    Simulation* simulation = (Simulation*) data;
    if (dGeomIsSpace(geometry1) || dGeomIsSpace(geometry2))
    {
        dSpaceCollide2(geometry1, geometry2, data, &nearCallback);
        return;
    }
    const int N = 32;
    dContact contact[N];
    int n = dCollide(geometry1, geometry2, N, &(contact[0].geom), sizeof(dContact));
    if (n > 0) {
        for (int i = 0; i < n; i++)
        {
            contact[i].surface.mode = dContactSoftERP | dContactSoftCFM | dContactApprox1 | dContactSlip1 | dContactSlip2;
            contact[i].surface.slip1 = 0.01;
            contact[i].surface.slip2 = 0.01;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.mu2 = dInfinity;
            contact[i].surface.soft_erp = 0.4; // [0.1, 0.8] recommended
            contact[i].surface.soft_cfm = 0.0004;
            dJointID joint = dJointCreateContact (simulation->world, simulation->contacts, &contact[i]);
            dJointAttach (joint, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
        }
    }
}
