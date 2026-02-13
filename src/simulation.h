#pragma once

#include "config.h"
#include <unordered_map>
#include <vector>

class SPHSimulation {
public:
    SPHSimulation(int width, int height);

    void initDamBreak();
    void update();
    void applyMouseForce(float mx, float my, bool active);
    void addParticle(float x, float y, float vx = 0, float vy = 0);

    // Public particle data (read by renderer)
    int   count = 0;
    float posX[cfg::MAX_PARTICLES];
    float posY[cfg::MAX_PARTICLES];

    // Mutable runtime parameters
    float stiffness  = cfg::STIFFNESS;
    float viscosity  = cfg::VISCOSITY;
    float gravity    = cfg::GRAVITY;
    float restDensity = 0.0f;

private:
    int width, height;

    float velX[cfg::MAX_PARTICLES];
    float velY[cfg::MAX_PARTICLES];
    float density[cfg::MAX_PARTICLES];
    float pressure[cfg::MAX_PARTICLES];
    float forceX[cfg::MAX_PARTICLES];
    float forceY[cfg::MAX_PARTICLES];

    // SPH kernel pre-computed coefficients
    float h, h2;
    float poly6Coeff, spikyGradCoeff, viscLapCoeff;

    // Spatial hash grid
    float cellSize;
    std::unordered_map<int, std::vector<int>> grid;

    int  cellKey(int cx, int cy) const;
    void buildGrid();
    void computeDensityPressure();
    void computeForces();
    void integrate(float dt);
    void step(float dt);
};
