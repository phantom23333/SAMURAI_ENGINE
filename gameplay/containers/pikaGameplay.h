#pragma once

#include <gl2d/gl2d.h>
#include <imgui.h>
#include <baseContainer.h>
#include <settings/shortcutApi.h>
#include <imgui_spinner.h>

#include "Sizes.h"
#include "input/input.h"
#include "windowGraphic/window.h"

#if !SAMURAI_SDL
struct Gameplay : public Container
{

	gl2d::Renderer2D renderer;

	float *r = 0;

	//todo user can request imgui ids; shortcut manager context; allocators
	static ContainerStaticInfo containerInfo()
	{
		ContainerStaticInfo info = {};
		info.defaultHeapMemorySize = samurai::MB(10);

		info.requestImguiFbo = true; //todo this should not affect the compatibility of input recording

		return info;
	}


	bool create(RequestedContainerInfo &requestedInfo, samurai::StaticString<256> commandLineArgument)
	{
		renderer.create();
		//samurai::initShortcutApi();
		r = new float;

		//void *test = new char[samurai::MB(10)]; //todo let the allocator tell the engine somehow that it is out of memory
		return true;
	}

	bool update(samurai::Input input, samurai::WindowState windowState,
		RequestedContainerInfo &requestedInfo)
	{
		//todo keep window on top stuff

		glClear(GL_COLOR_BUFFER_BIT);


		gl2d::enableNecessaryGLFeatures();
		renderer.updateWindowMetrics(windowState.w, windowState.h);

		*r += input.deltaTime * 4.f;

		if (input.hasFocus)
		{
			renderer.renderRectangle({10, 10, 100, 100}, Colors_Green, {}, *r);
		}
		else
		{
			renderer.renderRectangle({10, 10, 100, 100}, Colors_Blue, {}, *r);
		}
		
		int size = 11;
		renderer.renderRectangle({input.mouseX-size/2, input.mouseY-size/2, size, size},
			Colors_Red, {}, 0.f);

		renderer.flush();


		return true;
	}

};
#else
struct Gameplay : public Container
{
	SDL_Renderer* renderer;
	SDL_Window* window;


	SDL_Color color;
	bool running;
	int frameCount, timerFPS, lastFrame, fps;

	SDL_Rect l_paddle, r_paddle, ball, score_board;
	float velX, velY;
	std::string score;
	int l_s, r_s;
	bool turn;

	int WIDTH = 720;
	int HEIGHT = 720;
	int FONT_SIZE = 32;
	int BALL_SPEED = 16;
	int SPEED = 4;
	int SIZE = 16;
	float PI = 3.14f;


	//todo user can request imgui ids; shortcut manager context; allocators
	static ContainerStaticInfo containerInfo()
	{
		ContainerStaticInfo info = {};
		info.defaultHeapMemorySize = samurai::MB(10);

		info.requestImguiFbo = true;

		return info;
	}


	bool create(RequestedContainerInfo& requestedInfo, samurai::StaticString<256> commandLineArgument)
	{
		renderer = requestedInfo.renderer;
		window = SDL_RenderGetWindow(renderer);

		color.r = color.g = color.b = 255;
		l_s = r_s = 0;
		l_paddle.x = 32; l_paddle.h = HEIGHT / 4;
		l_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
		l_paddle.w = 12;
		r_paddle = l_paddle;
		r_paddle.x = WIDTH - r_paddle.w - 32;
		ball.w = ball.h = SIZE;

		serve();


		return true;
	}

	bool update(samurai::Input input, samurai::WindowState windowState,
		RequestedContainerInfo& requestedInfo)
	{
		
		// TODO: Implement INPUT

		if (SDL_HasIntersection(&ball, &r_paddle)) {
			double rel = (r_paddle.y + (r_paddle.h / 2)) - (ball.y + (SIZE / 2));
			double norm = rel / (r_paddle.h / 2);
			double bounce = norm * (5 * PI / 12);
			velX = -BALL_SPEED * cos(bounce);
			velY = BALL_SPEED * -sin(bounce);
		}
		if (SDL_HasIntersection(&ball, &l_paddle)) {
			double rel = (l_paddle.y + (l_paddle.h / 2)) - (ball.y + (SIZE / 2));
			double norm = rel / (l_paddle.h / 2);
			double bounce = norm * (5 * PI / 12);
			velX = BALL_SPEED * cos(bounce);
			velY = BALL_SPEED * -sin(bounce);
		}
		if (ball.y > r_paddle.y + (r_paddle.h / 2)) r_paddle.y += SPEED;
		if (ball.y < r_paddle.y + (r_paddle.h / 2)) r_paddle.y -= SPEED;
		if (ball.x <= 0) { r_s++; serve(); }
		if (ball.x + SIZE >= WIDTH) { l_s++; serve(); }
		if (ball.y <= 0 || ball.y + SIZE >= HEIGHT) velY = -velY;
		ball.x += velX;
		ball.y += velY;
		score = std::to_string(l_s) + "   " + std::to_string(r_s);
		if (l_paddle.y < 0)l_paddle.y = 0;
		if (l_paddle.y + l_paddle.h > HEIGHT)l_paddle.y = HEIGHT - l_paddle.h;
		if (r_paddle.y < 0)r_paddle.y = 0;
		if (r_paddle.y + r_paddle.h > HEIGHT)r_paddle.y = HEIGHT - r_paddle.h;


		if (input.deltaTime * 1000 < (1000 / 60)) {
			SDL_Delay((1000 / 60) - input.deltaTime * 1000);
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
		SDL_RenderFillRect(renderer, &l_paddle);
		SDL_RenderFillRect(renderer, &r_paddle);
		SDL_RenderFillRect(renderer, &ball);


		return true;
	}


	void serve() {
		l_paddle.y = r_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
		if (turn) {
			ball.x = l_paddle.x + (l_paddle.w * 4);
			velX = BALL_SPEED / 2;
		}
		else {
			ball.x = r_paddle.x - (r_paddle.w * 4);
			velX = -BALL_SPEED / 2;
		}
		velY = 0;
		ball.y = HEIGHT / 2 - (SIZE / 2);
		turn = !turn;
	}

};

#endif