#pragma once

namespace cfg {
    constexpr int   WIDTH           = 800;
    constexpr int   HEIGHT          = 600;
    constexpr int   MAX_PARTICLES   = 5000;

    // Rendering
    constexpr float POINT_SIZE       = 45.0f;
    constexpr float THRESHOLD        = 0.35f;

    // SPH
    constexpr float SMOOTHING_RADIUS = 16.0f;
    constexpr float PARTICLE_MASS    = 1.0f;
    constexpr float STIFFNESS        = 18000.0f;
    constexpr float VISCOSITY        = 250.0f;
    constexpr float GRAVITY          = 200.0f;    // px/s²
    constexpr float DT               = 1.0f / 60.0f;
    constexpr int   SUBSTEPS         = 8;
    constexpr float BOUND_DAMPING    = -0.5f;
    constexpr float BOUND_PAD        = POINT_SIZE * 0.55f; // keep particle splats inside walls
    constexpr float WALL_THICKNESS   = 6.0f;      // visual wall width in pixels

    // Mouse interaction
    constexpr float MOUSE_RADIUS     = 100.0f;
    constexpr float MOUSE_STRENGTH   = 8000.0f;
}
