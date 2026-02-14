# Liquid Simulation

A real-time 2D fluid simulator using **Smoothed Particle Hydrodynamics (SPH)** and OpenGL. Up to 5 000 particles interact under gravity, pressure, and viscosity forces, rendered as a continuous fluid surface with Phong lighting.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue) ![OpenGL 3.3](https://img.shields.io/badge/OpenGL-3.3-green) ![GLFW 3.4](https://img.shields.io/badge/GLFW-3.4-orange)

## Controls

| Input | Action |
|---|---|
| **Left-click + drag** | Push water away from cursor |
| **Hold F** | Spawn particles at cursor |
| **R** | Reset simulation |
| **Esc** | Quit |

## How It Works

### Physics — SPH Algorithm

The simulation runs 8 substeps per frame for stability:

1. **Spatial hashing** — particles are bucketed into a grid (cell size = smoothing radius) so neighbor lookups are O(1) instead of O(n²).
2. **Density & pressure** — for each particle, nearby neighbors contribute to a density estimate via a Poly6 kernel. Pressure is derived from density using a stiffness coefficient; only positive pressures are kept.
3. **Force accumulation** — pressure forces (Spiky gradient kernel) push particles apart to maintain incompressibility. Viscosity forces (Laplacian kernel) smooth out velocity differences for realistic flow.
4. **Integration** — forces and gravity update velocities and positions. Particles bounce off container walls with energy loss.

### Rendering — Two-Pass Technique

1. **Splat pass** — each particle is drawn as a Gaussian circle into an off-screen framebuffer with additive blending, producing a smooth density field.
2. **Fluid pass** — the density texture is sampled to extract the fluid surface (threshold at 0.35). Surface normals are computed from density gradients, and Phong lighting with specular highlights is applied. Color interpolates from light blue (shallow) to dark blue (deep).

## Building from Source

### Requirements

- CMake 3.20+
- A C++17 compiler (MSVC, GCC, or Clang)
- OpenGL 3.3 capable GPU

GLFW 3.4 is fetched automatically by CMake — no manual dependency setup needed.

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The executable will be at `build/Release/WaterSimulation.exe` (Windows) or `build/WaterSimulation` (Linux/macOS).

## Project Structure

```
src/
  config.h        — tunable physics and rendering parameters
  main.cpp        — window creation, input handling, main loop
  simulation.h/cpp — SPH physics engine
  renderer.h/cpp  — OpenGL multi-pass fluid renderer
  gl_loader.h/cpp — manual OpenGL function pointer loading
```
