#include "gl_loader.h"
#include "config.h"
#include "simulation.h"
#include "renderer.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand((unsigned)std::time(nullptr));

    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(cfg::WIDTH, cfg::HEIGHT,
                                          "Liquid Simulation", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window (OpenGL 3.3 required)\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // vsync

    if (!loadGL()) {
        fprintf(stderr, "Failed to load OpenGL functions\n");
        glfwTerminate();
        return 1;
    }

    SPHSimulation sim(cfg::WIDTH, cfg::HEIGHT);
    sim.initDamBreak();

    FluidRenderer renderer(cfg::WIDTH, cfg::HEIGHT);

    // FPS tracking
    double lastTime = glfwGetTime();
    int    frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // ---- Input ----
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        bool mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            sim.initDamBreak();

        // Faucet — hold F to pour particles at cursor
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && sim.count < cfg::MAX_PARTICLES) {
            for (int i = 0; i < 4; i++) {
                sim.addParticle(
                    (float)mx + ((rand() / (float)RAND_MAX) - 0.5f) * 10.0f,
                    (float)my + ((rand() / (float)RAND_MAX) - 0.5f) * 10.0f,
                    ((rand() / (float)RAND_MAX) - 0.5f) * 50.0f,
                    200.0f + (rand() / (float)RAND_MAX) * 100.0f
                );
            }
        }

        // ---- Simulate ----
        sim.applyMouseForce((float)mx, (float)my, mouseDown);
        sim.update();

        // ---- Render ----
        renderer.render(sim);
        glfwSwapBuffers(window);

        // ---- FPS title bar ----
        frameCount++;
        double now = glfwGetTime();
        if (now - lastTime >= 0.5) {
            int fps = (int)(frameCount / (now - lastTime));
            char title[128];
            snprintf(title, sizeof(title),
                     "Liquid Simulation  |  FPS: %d  |  Particles: %d  |  [Click] push  [F] pour  [R] reset  [Esc] quit",
                     fps, sim.count);
            glfwSetWindowTitle(window, title);
            frameCount = 0;
            lastTime = now;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
