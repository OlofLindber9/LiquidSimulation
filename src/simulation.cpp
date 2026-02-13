#include "simulation.h"
#include <cmath>
#include <cstring>
#include <cstdlib>

static constexpr float PI = 3.14159265358979323846f;

SPHSimulation::SPHSimulation(int width, int height)
    : width(width), height(height)
{
    h  = cfg::SMOOTHING_RADIUS;
    h2 = h * h;
    cellSize = h;

    poly6Coeff     =  4.0f  / (PI * powf(h, 8));
    spikyGradCoeff = -30.0f / (PI * powf(h, 5));
    viscLapCoeff   =  40.0f / (PI * powf(h, 5));

    std::memset(posX, 0, sizeof(posX));
    std::memset(posY, 0, sizeof(posY));
    std::memset(velX, 0, sizeof(velX));
    std::memset(velY, 0, sizeof(velY));
}

void SPHSimulation::addParticle(float x, float y, float vx, float vy) {
    if (count >= cfg::MAX_PARTICLES) return;
    int i = count++;
    posX[i] = x;  posY[i] = y;
    velX[i] = vx; velY[i] = vy;
}

void SPHSimulation::initDamBreak() {
    count = 0;
    float spacing = h * 0.5f;
    float startX = spacing * 2.0f;
    float startY = spacing * 2.0f;
    float blockW = width * 0.3f;
    float blockH = (float)height - spacing * 4.0f;

    for (float y = startY; y < startY + blockH; y += spacing) {
        for (float x = startX; x < startX + blockW; x += spacing) {
            addParticle(
                x + ((rand() / (float)RAND_MAX) - 0.5f) * spacing * 0.1f,
                y + ((rand() / (float)RAND_MAX) - 0.5f) * spacing * 0.1f
            );
        }
    }

    // Compute rest density from initial packed configuration
    buildGrid();
    computeDensityPressure();
    float total = 0.0f;
    for (int i = 0; i < count; i++) total += density[i];
    restDensity = total / (float)count;
}

// ---- Spatial hash ----

int SPHSimulation::cellKey(int cx, int cy) const {
    return (cx * 73856093) ^ (cy * 19349663);
}

void SPHSimulation::buildGrid() {
    grid.clear();
    for (int i = 0; i < count; i++) {
        int cx = (int)(posX[i] / cellSize);
        int cy = (int)(posY[i] / cellSize);
        grid[cellKey(cx, cy)].push_back(i);
    }
}

// ---- SPH kernels & forces ----

void SPHSimulation::computeDensityPressure() {
    const float mass = cfg::PARTICLE_MASS;

    for (int i = 0; i < count; i++) {
        float rho = 0.0f;
        float px = posX[i], py = posY[i];
        int cx = (int)(px / cellSize);
        int cy = (int)(py / cellSize);

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                auto it = grid.find(cellKey(cx + dx, cy + dy));
                if (it == grid.end()) continue;
                const auto& cell = it->second;

                for (int k = 0; k < (int)cell.size(); k++) {
                    int j = cell[k];
                    float diffX = px - posX[j];
                    float diffY = py - posY[j];
                    float r2 = diffX * diffX + diffY * diffY;
                    if (r2 < h2) {
                        float w = h2 - r2;
                        rho += mass * poly6Coeff * w * w * w;
                    }
                }
            }
        }

        density[i]  = rho;
        pressure[i] = stiffness * (rho - restDensity);
    }
}

void SPHSimulation::computeForces() {
    const float mass = cfg::PARTICLE_MASS;

    for (int i = 0; i < count; i++) {
        float fx = 0.0f, fy = 0.0f;
        float px = posX[i], py = posY[i];
        float pi_p = pressure[i];
        float vxi = velX[i], vyi = velY[i];
        int cx = (int)(px / cellSize);
        int cy = (int)(py / cellSize);

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                auto it = grid.find(cellKey(cx + dx, cy + dy));
                if (it == grid.end()) continue;
                const auto& cell = it->second;

                for (int k = 0; k < (int)cell.size(); k++) {
                    int j = cell[k];
                    if (i == j) continue;

                    float diffX = px - posX[j];
                    float diffY = py - posY[j];
                    float r2 = diffX * diffX + diffY * diffY;

                    if (r2 < h2 && r2 > 1e-6f) {
                        float r  = sqrtf(r2);
                        float hr = h - r;
                        float dj = density[j];

                        // Pressure force (Spiky gradient kernel)
                        float pMag = -mass * (pi_p + pressure[j]) / (2.0f * dj)
                                     * spikyGradCoeff * hr * hr / r;
                        fx += pMag * diffX;
                        fy += pMag * diffY;

                        // Viscosity force (Viscosity laplacian kernel)
                        float vMag = viscosity * mass / dj * viscLapCoeff * hr;
                        fx += vMag * (velX[j] - vxi);
                        fy += vMag * (velY[j] - vyi);
                    }
                }
            }
        }

        forceX[i] = fx;
        forceY[i] = fy;
    }
}

void SPHSimulation::integrate(float dt) {
    const float damping = cfg::BOUND_DAMPING;
    const float pad = 4.0f;

    for (int i = 0; i < count; i++) {
        float rho = density[i];
        if (rho > 1e-6f) {
            velX[i] += dt * forceX[i] / rho;
            velY[i] += dt * (forceY[i] / rho + gravity);
        }

        posX[i] += dt * velX[i];
        posY[i] += dt * velY[i];

        // Boundary collisions
        if (posX[i] < pad)                { posX[i] = pad;                velX[i] *= damping; }
        else if (posX[i] > width  - pad)  { posX[i] = (float)width  - pad; velX[i] *= damping; }
        if (posY[i] < pad)                { posY[i] = pad;                velY[i] *= damping; }
        else if (posY[i] > height - pad)  { posY[i] = (float)height - pad; velY[i] *= damping; }
    }
}

void SPHSimulation::applyMouseForce(float mx, float my, bool active) {
    if (!active) return;
    float r2max = cfg::MOUSE_RADIUS * cfg::MOUSE_RADIUS;
    float str   = cfg::MOUSE_STRENGTH;
    float dt    = cfg::DT / (float)cfg::SUBSTEPS;

    for (int i = 0; i < count; i++) {
        float dx = posX[i] - mx;
        float dy = posY[i] - my;
        float d2 = dx * dx + dy * dy;
        if (d2 < r2max && d2 > 1.0f) {
            float d = sqrtf(d2);
            float factor = str * (1.0f - d / cfg::MOUSE_RADIUS) / d * dt;
            velX[i] += factor * dx;
            velY[i] += factor * dy;
        }
    }
}

void SPHSimulation::step(float dt) {
    buildGrid();
    computeDensityPressure();
    computeForces();
    integrate(dt);
}

void SPHSimulation::update() {
    float subDt = cfg::DT / (float)cfg::SUBSTEPS;
    for (int s = 0; s < cfg::SUBSTEPS; s++) {
        step(subDt);
    }
}
