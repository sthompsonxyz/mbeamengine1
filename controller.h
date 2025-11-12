#pragma once


//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


struct ps4ControllerState {
	bool dpadup;
	bool dpaddown;
	bool dpadright;
	bool dpadleft;
	bool triangle;
	bool circle;
	bool cross;
	bool square;
    bool back; //share
    bool start; //options
    bool guide; //ps button
	bool l1; // LB
	bool r1; // RB
    bool l3; // L3
    bool r3; // R3
    float lstickLR;
    float lstickUD;
    float rstickLR;
    float rstickUD;
	float l2; // LT
	float r2; // RT
};


struct ps4Controller {

	ps4Controller() {
        ctrlrstate = ps4ControllerState{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    }
	~ps4Controller() {
        //delete ctrlrstate;
    }

    ps4ControllerState ctrlrstate;
    GLFWgamepadstate gpState;
    //GLFWgamepadstate gpState2;


    void updatecontrollerstate() {
        //glfwGetGamepadState(1, &gpState2);
        if (glfwGetGamepadState(0, &gpState)) {
            //std::cout << "got gp state" << std::endl;
            //aux buttons
            ctrlrstate.back = gpState.buttons[GLFW_GAMEPAD_BUTTON_BACK];
            ctrlrstate.start = gpState.buttons[GLFW_GAMEPAD_BUTTON_START];
            ctrlrstate.guide = gpState.buttons[GLFW_GAMEPAD_BUTTON_GUIDE];
            //dpad
            ctrlrstate.dpadup = gpState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
            ctrlrstate.dpadright = gpState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
            ctrlrstate.dpaddown = gpState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
            ctrlrstate.dpadleft = gpState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
            //buttons
            ctrlrstate.triangle = gpState.buttons[GLFW_GAMEPAD_BUTTON_TRIANGLE];
            ctrlrstate.circle = gpState.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE];
            ctrlrstate.cross = gpState.buttons[GLFW_GAMEPAD_BUTTON_CROSS];
            ctrlrstate.square = gpState.buttons[GLFW_GAMEPAD_BUTTON_SQUARE];
            ctrlrstate.l1 = gpState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
            ctrlrstate.r1 = gpState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
            ctrlrstate.l3 = gpState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB];
            ctrlrstate.r3 = gpState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB];
            //sixaxes
            ctrlrstate.lstickLR = gpState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            ctrlrstate.lstickUD = gpState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
            ctrlrstate.rstickLR = gpState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
            ctrlrstate.rstickUD = gpState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
            ctrlrstate.l2 = gpState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
            ctrlrstate.r2 = gpState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
        }
    }
};