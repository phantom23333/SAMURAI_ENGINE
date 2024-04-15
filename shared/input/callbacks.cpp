#include "callbacks.h"
#include <string.h>

#include "input.h"
#include "windowGraphic/window.h"

#if !SAMURAI_SDL

void addTypedInput(samurai::Input &input, unsigned int c)
{
	if (c < 127)
	{
		auto l = strlen(input.typedInput);
		if (l < sizeof(input.typedInput) - 1)
		{
			input.typedInput[l++] = c;
			input.typedInput[l] = 0;
		}
	}
}

void characterCallback(GLFWwindow *window, unsigned int codepoint)
{
	auto ptr = glfwGetWindowUserPointer(window);
	samurai::Window &pikaWindow = *(samurai::Window *)ptr;

	addTypedInput(pikaWindow.input, codepoint);
}

void windowFocusCallback(GLFWwindow *window, int focused)
{
	auto ptr = glfwGetWindowUserPointer(window);
	samurai::Window &pikaWindow = *(samurai::Window *)ptr;

	if (focused)
	{
		pikaWindow.input.hasFocus = 1;
	}
	else
	{
		pikaWindow.input.hasFocus = 0;
	}
}

static void processAButton(samurai::Button &b, int action)
{
	if (action == GLFW_PRESS)
	{
		b.setHeld(true);

	}
	else if (action == GLFW_RELEASE)
	{
		b.setHeld(false);
	}
	else if (action == GLFW_REPEAT)
	{
		b.setHeld(true);
		b.setTyped(true);
	}

};

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	auto ptr = glfwGetWindowUserPointer(window);
	samurai::Window &pikaWindow = *(samurai::Window *)ptr;

	if ((action == GLFW_REPEAT || action == GLFW_PRESS) && key == GLFW_KEY_BACKSPACE)
	{
		addTypedInput(pikaWindow.input, 8);
	}

	if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
	{
		int index = key - GLFW_KEY_A;
		processAButton(pikaWindow.input.buttons[samurai::Button::A + index], action);
	}else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
	{
		int index = key - GLFW_KEY_0;
		processAButton(pikaWindow.input.buttons[samurai::Button::NR0 + index], action);
	}else
	{

		if (key == GLFW_KEY_SPACE)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::Space], action);
		}
		else
		if (key == GLFW_KEY_ENTER)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::Enter], action);
		}
		else
		if (key == GLFW_KEY_ESCAPE)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::Escape], action);
		}
		else
		if (key == GLFW_KEY_UP)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::Up], action);
		}
		else
		if (key == GLFW_KEY_DOWN)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::Down], action);
		}
		else
		if (key == GLFW_KEY_LEFT)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::Left], action);
		}
		else
		if (key == GLFW_KEY_RIGHT)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::Right], action);
		}
		else
		if (key == GLFW_KEY_LEFT_CONTROL)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::LeftCtrl], action);
		}else
		if (key == GLFW_KEY_TAB)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::Tab], action);
		}else
		if (key == GLFW_KEY_LEFT_ALT)
		{
			processAButton(pikaWindow.input.buttons[samurai::Button::LeftAlt], action);
		}
	}

}

void mouseCallback(GLFWwindow *window, int key, int action, int mods)
{

	auto ptr = glfwGetWindowUserPointer(window);
	samurai::Window &pikaWindow = *(samurai::Window *)ptr;

	if (key == GLFW_MOUSE_BUTTON_LEFT)
	{
		processAButton(pikaWindow.input.lMouse, action);
	}
	else if(key == GLFW_MOUSE_BUTTON_RIGHT)
	{
		processAButton(pikaWindow.input.rMouse, action);
	}


}
#endif