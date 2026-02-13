#pragma once

#include "gl_loader.h"
#include "config.h"

class SPHSimulation;   // forward decl

class FluidRenderer {
public:
    FluidRenderer(int width, int height);
    ~FluidRenderer();
    void render(const SPHSimulation& sim);

private:
    int width, height;

    // Shader programs
    GLuint splatProg, fluidProg, bgProg;

    // Geometry
    GLuint particleVAO, particleVBO;
    GLuint quadVAO, quadVBO;

    // Framebuffer for the splat pass
    GLuint splatFBO, splatTex;

    // Temp buffer for uploading positions
    float posData[cfg::MAX_PARTICLES * 2];

    // Uniform locations
    GLint splat_uRes, splat_uPtSize;
    GLint fluid_uTex, fluid_uRes, fluid_uThresh;
    GLint bg_uRes, bg_uWall;

    GLuint compileProgram(const char* vsSrc, const char* fsSrc);
    void   setupGeometry();
    void   setupFBO();
};
