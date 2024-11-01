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

void drawButton(gfx::Painter& painter, gfx::Font& font, gfx::Point pos, gfx::HRef href, gfx::VRef vref) {
    // TODO: Figure out why the +1 is required for it to look correct...
    // TODO: Change rect to use .A() and .B() for conciseness
    // TODO: Use float as default to remove f suffix

    // Draw the background
    gfx::Rect btn(pos, gfx::Size(80.0f, 22.0f));
    painter.fillRect(btn, gfx::Color(0.15, 0.15, 0.15, 1.0));

    // Select the position of the text depending on its reference
    gfx::Point textPos;
    switch (href) {
    case gfx::H_REF_LEFT:
        textPos.x = btn.A().x;
        break;
    case gfx::H_REF_CENTER:
        textPos.x = ((btn.A() + btn.B()) * 0.5f).x;
        break;
    case gfx::H_REF_RIGHT:
        textPos.x = btn.B().x;
        break;
    default:
        break;
    }
    switch (vref) {
    case gfx::V_REF_TOP:
        textPos.y = btn.A().y;
        break;
    case gfx::V_REF_CENTER:
        textPos.y = ((btn.A() + btn.B()) * 0.5f).y;
        break;
    case gfx::V_REF_BOTTOM:
        textPos.y = btn.B().y;
        break;
    default:
        break;
    }
    painter.drawText(textPos, "Refreshj", font, gfx::Color(1.0, 1.0, 1.0, 1.0), href, vref);
}

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

        gfx::Sizei winSize(1280, 720);

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
        painter.fc->loadFont("../vendor/res/Roboto-Medium.ttf");
        painter.fc->loadFont("../vendor/res/Roboto-Regular.ttf");
        painter.fc->loadFont("../vendor/res/arial.ttf");
        painter.fc->loadFont("../vendor/res/OpenSans-Medium.ttf");
        painter.fc->loadFont("../vendor/res/OpenSans-Regular.ttf");
        painter.fc->loadFont("../vendor/res/NotoSans-Medium.ttf");
        painter.fc->loadFont("../vendor/res/NotoSans-Regular.ttf");

        int frameCount = 0;
        auto lastTime = std::chrono::high_resolution_clock::now();

        gfx::Font font("Open Sans Medium", 14);
        gfx::Color color(21.0/255.0, 132.0/255.0, 224.0/255.0, 1);

        // Create a checkmark
        std::vector<gfx::Point> checkmarkVerts = {
            gfx::Point(0.3671875, 0.8671875),
            gfx::Point(0.0, 0.5234375),
            gfx::Point(0.1328125, 0.3984375),
            gfx::Point(0.3671875, 0.625),
            gfx::Point(0.84375, 0.125),
            gfx::Point(1.0, 0.25),
        };
        gfx::Polygon checkmark(checkmarkVerts);

        double counter = -M_PI * 0.5;
        float test = 0;

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
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Begin the render
            painter.beginRender();

            float input = (sin(counter) + 1.0)*0.5;

            // Draw a checkmark
            painter.fillRect(gfx::Rect(gfx::Point(98, 98+60), gfx::Point(118, 118+60)), gfx::Color(0.15, 0.15, 0.15, 1.0));
            if ((input > 0.25f && input < 0.5f) || (input > 0.75f && input < 1.0f)) {
                painter.fillPolygon(gfx::Point(100, 100+60), checkmark, gfx::Size(17, 17), color);
            }
            painter.drawText(gfx::Point(123, 114+60), "The quick brown fox jumps over the lazy dog.", font, gfx::Color(1.0, 1.0, 1.0, 1.0));

            // painter.pushStencil(gfx::Rect(gfx::Point(500, 500), gfx::Size(128, 128)));
            // painter.pushOffset(gfx::Point(0, 0));
            painter.drawArc(gfx::Point(500, 500), 128, -30.0*(M_PI/180.0), 210.0*(M_PI/180.0), gfx::Color(0.15, 0.15, 0.15, 1.0), 20);
            painter.drawArc(gfx::Point(500, 500), 128, -30.0*(M_PI/180.0), (input*240.0 - 30)*(M_PI/180.0), color, 20);
            char str[128];
            sprintf(str, "%0.1f%%", input*100.0f);
            painter.drawText(gfx::Point(500, 500), str, font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_BASELINE);
            painter.fillRect(gfx::Rect(gfx::Point(200, 200), gfx::Point(420, 207)), gfx::Color(0.15, 0.15, 0.15, 1.0), 3);
            // painter.popOffset();
            // painter.popStencil();

            painter.drawLine(gfx::Point(0 + test, 100), gfx::Point(400 + test, 100), gfx::Color(0.0, 1.0, 0.0, 1.0));
            painter.drawText(gfx::Point(50 + test, 100), "Ag!", font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_BOTTOM);
            painter.drawText(gfx::Point(150 + test, 100), "Ag!", font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_BASELINE);
            painter.drawText(gfx::Point(250 + test, 100), "Ag!", font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_CENTER);
            painter.drawText(gfx::Point(350 + test, 100), "Ag!", font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_TOP);

            painter.drawText(gfx::Point(50 + test, 140), "Bottom", font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_BASELINE);
            painter.drawText(gfx::Point(150 + test, 140), "Baseline", font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_BASELINE);
            painter.drawText(gfx::Point(250 + test, 140), "Center", font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_BASELINE);
            painter.drawText(gfx::Point(350 + test, 140), "Top", font, gfx::Color(1.0, 1.0, 1.0, 1.0), gfx::H_REF_CENTER, gfx::V_REF_BASELINE);

            test += 0.25f;
            if (test >= 600.0f) { test = 0.0f; }
            counter += 0.01;
            while (counter > M_PI) { counter -= 2.0*M_PI; }
            while (counter < -M_PI) { counter += 2.0*M_PI; }

            // TODO: Workaround for buggy behavior
            painter.fc->atlas.pushTexture();

            // Finish the render
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
                flog::debug("FPS: {}", 1e9 * (double)frames / (double)ns);
            }
        }        
    }
    catch (const std::exception& e) {
        flog::error(e.what());
        return -1;
    }
    
    return 0;
}