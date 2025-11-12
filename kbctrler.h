#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct KeyboardControls {
    bool w = false;
    bool s = false;
    bool a = false;
    bool d = false;
    bool sp = false;
    bool lctrl = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool lbracket = false;
    bool rbracket = false;
    bool o = false;
    bool i = false;
    bool f = false;
    bool g = false;

    bool q = false;
    bool e = false;
    bool r = false;
    bool t = false;
    bool y = false;
    bool u = false;
    bool p = false;

    bool lshift = false;
    //bool lctrl = false;
    bool lalt = false;

    void glfwUpdate(GLFWwindow* window) {
        w = glfwGetKey(window, GLFW_KEY_W);
        s = glfwGetKey(window, GLFW_KEY_S);
        a = glfwGetKey(window, GLFW_KEY_A);
        d = glfwGetKey(window, GLFW_KEY_D);
        o = glfwGetKey(window, GLFW_KEY_O);
        i = glfwGetKey(window, GLFW_KEY_I);
        f = glfwGetKey(window, GLFW_KEY_F);
        g = glfwGetKey(window, GLFW_KEY_G);
        sp = glfwGetKey(window, GLFW_KEY_SPACE);
        lctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
        left = glfwGetKey(window, GLFW_KEY_LEFT);
        right = glfwGetKey(window, GLFW_KEY_RIGHT);
        up = glfwGetKey(window, GLFW_KEY_UP);
        down = glfwGetKey(window, GLFW_KEY_DOWN);
        lbracket = glfwGetKey(window, GLFW_KEY_LEFT_BRACKET);
        rbracket = glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET);

        q = glfwGetKey(window, GLFW_KEY_Q);
        e = glfwGetKey(window, GLFW_KEY_E);
        r = glfwGetKey(window, GLFW_KEY_R);
        t = glfwGetKey(window, GLFW_KEY_T);
        y = glfwGetKey(window, GLFW_KEY_Y);
        u = glfwGetKey(window, GLFW_KEY_U);
        p = glfwGetKey(window, GLFW_KEY_P);

        lshift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        //lctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
        lalt = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
    }
};