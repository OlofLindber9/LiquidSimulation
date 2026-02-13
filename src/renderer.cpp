#include "renderer.h"
#include "simulation.h"
#include <cstdio>
#include <cstdlib>

// ============================================================
// GLSL shaders (OpenGL 3.3 Core)
// ============================================================

static const char* SPLAT_VS = R"(
#version 330 core
layout(location = 0) in vec2 a_position;
uniform vec2  u_resolution;
uniform float u_pointSize;
void main() {
    vec2 clip = (a_position / u_resolution) * 2.0 - 1.0;
    clip.y = -clip.y;
    gl_Position = vec4(clip, 0.0, 1.0);
    gl_PointSize = u_pointSize;
}
)";

static const char* SPLAT_FS = R"(
#version 330 core
out vec4 fragColor;
void main() {
    vec2 c = gl_PointCoord - 0.5;
    float r2 = dot(c, c);
    if (r2 > 0.25) discard;
    float a = exp(-r2 * 18.0);
    fragColor = vec4(a, a, a, 1.0);
}
)";

static const char* QUAD_VS = R"(
#version 330 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoord;
out vec2 v_texCoord;
void main() {
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_texCoord  = a_texCoord;
}
)";

static const char* FLUID_FS = R"(
#version 330 core
uniform sampler2D u_texture;
uniform vec2  u_resolution;
uniform float u_threshold;
in  vec2 v_texCoord;
out vec4 fragColor;

void main() {
    float density = texture(u_texture, v_texCoord).r;
    if (density < u_threshold) discard;

    // Surface normal from density gradient
    vec2 texel = 1.0 / u_resolution;
    float dL = texture(u_texture, v_texCoord - vec2(texel.x, 0)).r;
    float dR = texture(u_texture, v_texCoord + vec2(texel.x, 0)).r;
    float dD = texture(u_texture, v_texCoord - vec2(0, texel.y)).r;
    float dU = texture(u_texture, v_texCoord + vec2(0, texel.y)).r;
    vec3 normal = normalize(vec3(dL - dR, dD - dU, 0.18));

    // Lighting
    vec3 lightDir = normalize(vec3(0.4, -0.5, 0.8));
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(reflect(-lightDir, normal), vec3(0, 0, 1)), 0.0), 48.0);

    // Water colour — shallow to deep
    vec3 shallow = vec3(0.15, 0.50, 0.90);
    vec3 deep    = vec3(0.02, 0.10, 0.32);
    float depthFactor = smoothstep(u_threshold, u_threshold + 2.0, density);
    vec3 waterColor = mix(shallow, deep, depthFactor);

    vec3 color = waterColor * (0.35 + 0.55 * diff) + vec3(0.85, 0.92, 1.0) * spec * 0.7;
    float alpha = smoothstep(u_threshold, u_threshold + 0.12, density) * 0.93;
    fragColor = vec4(color, alpha);
}
)";

static const char* BG_FS = R"(
#version 330 core
uniform vec2 u_resolution;
in  vec2 v_texCoord;
out vec4 fragColor;

void main() {
    vec3 bg = mix(vec3(0.07, 0.07, 0.11), vec3(0.03, 0.03, 0.06), v_texCoord.y);

    // Container walls
    vec2 px = gl_FragCoord.xy;
    float wall = 3.0;
    float edge = 0.0;
    if (px.x < wall || px.x > u_resolution.x - wall ||
        px.y < wall || px.y > u_resolution.y - wall) {
        edge = 1.0;
    }
    vec3 wallColor = vec3(0.22, 0.28, 0.38);
    fragColor = vec4(mix(bg, wallColor, edge), 1.0);
}
)";

// ============================================================
// Implementation
// ============================================================

FluidRenderer::FluidRenderer(int w, int h) : width(w), height(h) {
    splatProg = compileProgram(SPLAT_VS, SPLAT_FS);
    fluidProg = compileProgram(QUAD_VS,  FLUID_FS);
    bgProg    = compileProgram(QUAD_VS,  BG_FS);

    // Cache uniform locations
    glUseProgram(splatProg);
    splat_uRes    = glGetUniformLocation(splatProg, "u_resolution");
    splat_uPtSize = glGetUniformLocation(splatProg, "u_pointSize");

    glUseProgram(fluidProg);
    fluid_uTex    = glGetUniformLocation(fluidProg, "u_texture");
    fluid_uRes    = glGetUniformLocation(fluidProg, "u_resolution");
    fluid_uThresh = glGetUniformLocation(fluidProg, "u_threshold");

    glUseProgram(bgProg);
    bg_uRes = glGetUniformLocation(bgProg, "u_resolution");

    glUseProgram(0);

    setupGeometry();
    setupFBO();

    glEnable(GL_PROGRAM_POINT_SIZE);
}

FluidRenderer::~FluidRenderer() {
    glDeleteProgram(splatProg);
    glDeleteProgram(fluidProg);
    glDeleteProgram(bgProg);
    glDeleteVertexArrays(1, &particleVAO);
    glDeleteBuffers(1, &particleVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(1, &splatFBO);
    glDeleteTextures(1, &splatTex);
}

void FluidRenderer::setupGeometry() {
    // Particle VAO/VBO (dynamic positions uploaded each frame)
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posData), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);

    // Fullscreen quad VAO/VBO
    float quadVerts[] = {
        // pos       texcoord
        -1, -1,     0, 0,
         1, -1,     1, 0,
        -1,  1,     0, 1,
         1,  1,     1, 1,
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);  // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);  // texcoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void FluidRenderer::setupFBO() {
    glGenTextures(1, &splatTex);
    glBindTexture(GL_TEXTURE_2D, splatTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &splatFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, splatFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, splatTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer is not complete!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FluidRenderer::render(const SPHSimulation& sim) {
    // Upload particle positions
    for (int i = 0; i < sim.count; i++) {
        posData[i * 2]     = sim.posX[i];
        posData[i * 2 + 1] = sim.posY[i];
    }
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sim.count * 2 * sizeof(float), posData);

    // ---- Pass 1: Splat particles to FBO (additive) ----
    glBindFramebuffer(GL_FRAMEBUFFER, splatFBO);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(splatProg);
    glUniform2f(splat_uRes, (float)width, (float)height);
    glUniform1f(splat_uPtSize, cfg::POINT_SIZE);

    glBindVertexArray(particleVAO);
    glDrawArrays(GL_POINTS, 0, sim.count);

    // ---- Pass 2: Draw to screen ----
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);

    // Background
    glUseProgram(bgProg);
    glUniform2f(bg_uRes, (float)width, (float)height);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Fluid surface
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(fluidProg);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, splatTex);
    glUniform1i(fluid_uTex, 0);
    glUniform2f(fluid_uRes, (float)width, (float)height);
    glUniform1f(fluid_uThresh, cfg::THRESHOLD);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisable(GL_BLEND);
}

// ---- Shader compilation ----

GLuint FluidRenderer::compileProgram(const char* vsSrc, const char* fsSrc) {
    auto compile = [](GLenum type, const char* src) -> GLuint {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);

        GLint ok;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[512];
            glGetShaderInfoLog(s, sizeof(log), nullptr, log);
            fprintf(stderr, "Shader compile error:\n%s\n", log);
        }
        return s;
    };

    GLuint vs = compile(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        fprintf(stderr, "Program link error:\n%s\n", log);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}
