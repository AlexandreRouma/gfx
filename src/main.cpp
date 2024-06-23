#include <stdio.h>
#include "flog/flog.h"
#include "backend/opengl/painter.h"
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <chrono>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
#endif

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197
#endif

int main() {
    try {
        // Init GLFW
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialized GLFW");
        }

        // Save primary monitor (TODO: Check highest framerate for all monitor instead)
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        int msWait = floorf(1000.0f / (float)mode->refreshRate);

        // Enable 8x antialiasing
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 16);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Select OpenGL version
#ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
        bool swapNeeded = true;

        gfx::Size winSize(1920, 1080);

        // Create windows
        GLFWwindow* glfwWindow = glfwCreateWindow(winSize.x, winSize.y, "GFX Demo", NULL, NULL);

        // Bind OpenGL context
        glfwMakeContextCurrent(glfwWindow);

        // Init GLAD
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        // Define viewport
        glViewport(0, 0, winSize.x, winSize.y);

        // Create painter
        auto painter = gfx::OpenGL::Painter(winSize);

        // Load the fonts
        painter.fc->loadFont("../vendor/res/NotoSansJP-Regular.ttf");
        painter.fc->loadFont("../vendor/res/Roboto-Thin.ttf");
        painter.fc->loadFont("../vendor/res/Roboto-Regular.ttf");
        painter.fc->loadFont("../vendor/res/Roboto-Medium.ttf");
        painter.fc->loadFont("../vendor/res/Roboto-Bold.ttf");
        painter.fc->loadFont("../vendor/res/Roboto-Black.ttf");
        painter.fc->loadFont("C:/Windows/Fonts/arial.ttf");
        painter.fc->loadFont("C:/Windows/Fonts/segoeui.ttf");
        painter.fc->loadFont("C:/Windows/Fonts/consola.ttf");
        painter.fc->loadFont("C:/Users/ryzerth/Downloads/helvetica-bold.ttf");

        int frameCount = 0;
        auto lastTime = std::chrono::high_resolution_clock::now();

        gfx::Font font("Roboto Medium", 14);
        gfx::Color color(21.0/255.0, 132.0/255.0, 224.0/255.0, 1);

        // Create a checkmark
        std::vector<gfx::Pointf> checkmarkVerts = {
            gfx::Pointf(0.3671875, 0.8671875),
            gfx::Pointf(0.0, 0.5234375),
            gfx::Pointf(0.1328125, 0.3984375),
            gfx::Pointf(0.3671875, 0.625),
            gfx::Pointf(0.84375, 0.125),
            gfx::Pointf(1.0, 0.25),
        };
        gfx::Polygon checkmark(checkmarkVerts);

        while (true) {
            glfwPollEvents();
            // Check if the window should exit
            if (glfwWindowShouldClose(glfwWindow)) {
                break;
            }

            // Wait for VSYNC
            glfwSwapInterval(1);

            // Clear frame
            glDisable(GL_SCISSOR_TEST);
            // glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Draw things
            painter.beginRender();
            // for (int i = 0; i < 1000; i++) {
            //     painter.drawText(gfx::Point(10, 15+i*19), "According to all known laws of aviation, there is no way a bee should be able to fly. Its wings are too small to get its fat little body off the ground. The bee of course, flies anyway, because bees don't care what humans think is impossible.", font, color);
            // }

            //font = gfx::Font("Roboto Medium", 16 + (frameCount%64)/4);
            //painter.drawText(gfx::Point(10, 60), "Hello!", font, color);

            // Draw a checkmark
            painter.fillRect(gfx::Rect(gfx::Point(98, 98), gfx::Point(118, 118)), gfx::Color(0.15, 0.15, 0.15, 1.0));
            painter.fillPolygon(gfx::Point(100, 100), checkmark, gfx::Size(17, 17), color);
            painter.drawText(gfx::Point(123, 114), "Checkbox!", font, gfx::Color(1.0, 1.0, 1.0, 1.0));

            painter.fc->atlas.pushTexture();
            painter.endRender();

            // Flush commands and swap buffers
            glFinish();
            glfwSwapBuffers(glfwWindow);

            // Check if enough time has elapsed and print framerate
            frameCount++;
            auto now = std::chrono::high_resolution_clock::now();
            int ns = (now - lastTime).count();
            if (ns >= 1e9) {
                int frames =  frameCount;
                frameCount = 0;
                lastTime = now;
                double fps = 1e9 * (double)frames / (double)ns;
                flog::debug("FPS: {} => Glyphs per Second: {}", 1e9 * (double)frames / (double)ns, fps*242000.0);
            }
        }

        
    }
    catch (const std::exception& e) {
        flog::error(e.what());
        return -1;
    }
    
    return 0;
}