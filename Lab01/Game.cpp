//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <iostream>
#include <algorithm>

Game::Game() {
	window = nullptr;
	renderer = nullptr;
	gameRunning = false;
}

bool Game::Initialize() {
	//Initialize SDL, Window, and Renderer
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		return false;
	}

	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	if (window == nullptr) {
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//Initialize game
	prevTime = 0;
	paddlePos.x = WALL_THICKNESS + (PADDLE_WIDTH / 2);
	paddlePos.y = HEIGHT / 2;
	ballPos.x = WIDTH / 2;
	ballPos.y = HEIGHT / 2;
	ballVelocity.x = INIT_BALL_SPEED;
	ballVelocity.y = INIT_BALL_SPEED;
	moveDir = 0;
	screenShakeMagnitude = 0.0f;
	return true;
}

void Game::Shutdown() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Game::RunLoop() {
	gameRunning = true;
	while (gameRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() {
	//Process individual events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			gameRunning = false;
			break;
		}
	}

	//Process keybaord state
	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	if (keys[SDL_SCANCODE_ESCAPE]) {
		gameRunning = false;
	}
	moveDir = keys[SDL_SCANCODE_DOWN] - keys[SDL_SCANCODE_UP];
}

void Game::UpdateGame() {
	//Limit Framerate
	while (SDL_GetTicks() - prevTime < 16);

	//Calculate Delta Time
	int time = SDL_GetTicks();
	float deltaTime = ((float)(time - prevTime)) / 1000.0f;
	prevTime = time;

	//Limit Delta Time
	deltaTime = std::min(deltaTime, 0.033f);

	//Move Paddle
	paddlePos.y += (int)(deltaTime * moveDir * MOVE_SPEED);
	paddlePos.y = std::max(paddlePos.y, WALL_THICKNESS + (PADDLE_HEIGHT / 2));
	paddlePos.y = std::min(paddlePos.y, HEIGHT - WALL_THICKNESS - (PADDLE_HEIGHT / 2));

	//Move and Bounce Ball
	ballPos.x += (int)(deltaTime * ballVelocity.x);
	ballPos.y += (int)(deltaTime * ballVelocity.y);

	if (ballPos.x > WIDTH - WALL_THICKNESS - (BALL_SIZE / 2)) {
		ballPos.x = WIDTH - WALL_THICKNESS - (BALL_SIZE / 2);
		ballVelocity.x *= -1;
		screenShakeMagnitude = HIT_SCREEN_SHAKE;
	}
	if (ballPos.y < WALL_THICKNESS + (BALL_SIZE / 2)) {
		ballPos.y = WALL_THICKNESS + (BALL_SIZE / 2);
		ballVelocity.y *= -1;
		screenShakeMagnitude = HIT_SCREEN_SHAKE;
	}
	if (ballPos.y > HEIGHT - WALL_THICKNESS - (BALL_SIZE / 2)) {
		ballPos.y = HEIGHT - WALL_THICKNESS - (BALL_SIZE / 2);
		ballVelocity.y *= -1;
		screenShakeMagnitude = HIT_SCREEN_SHAKE;
	}
	if (ballVelocity.x < 0
		&& ballPos.x > paddlePos.x - (PADDLE_WIDTH / 2) - (BALL_SIZE / 2)
		&& ballPos.x < paddlePos.x + (PADDLE_WIDTH / 2) + (BALL_SIZE / 2)
		&& ballPos.y > paddlePos.y - (PADDLE_HEIGHT / 2) - (BALL_SIZE / 2)
		&& ballPos.y < paddlePos.y + (PADDLE_HEIGHT / 2) + (BALL_SIZE / 2)) {
		ballVelocity.x *= -1;
		screenShakeMagnitude = HIT_SCREEN_SHAKE;
	}

	//Check for Game Over
	if (ballPos.x < -(BALL_SIZE / 2)) {
		gameRunning = false;
	}

	//Decay Screen Shake
	screenShakeMagnitude = std::max(0.0f, screenShakeMagnitude - deltaTime*SCREEN_SHAKE_DECAY);
}

void Game::GenerateOutput() {
	//Clear background
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	SDL_RenderClear(renderer);

	//Get Screenshake Offset
	SDL_Point screenshake;
	int maxScreenShake = (int)(ceilf(screenShakeMagnitude));
	if (maxScreenShake == 0) {
		screenshake.x = screenshake.y = 0;
	}
	else {
		screenshake.x = std::rand() % (maxScreenShake * 2) - maxScreenShake;
		screenshake.y = std::rand() % (maxScreenShake * 2) - maxScreenShake;
	}

	//Draw Walls
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect topWall, bottomWall, rightWall;
	topWall.x = bottomWall.x = screenshake.x;
	topWall.y = rightWall.y = screenshake.y;
	topWall.h = bottomWall.h = rightWall.w = WALL_THICKNESS;
	topWall.w = bottomWall.w = WIDTH;
	bottomWall.y = HEIGHT - WALL_THICKNESS + screenshake.y;
	rightWall.x = WIDTH - WALL_THICKNESS + screenshake.x;
	rightWall.h = HEIGHT;
	SDL_RenderFillRect(renderer, &topWall);
	SDL_RenderFillRect(renderer, &bottomWall);
	SDL_RenderFillRect(renderer, &rightWall);

	//Draw Paddle
	SDL_Rect paddle;
	paddle.x = paddlePos.x - (PADDLE_WIDTH / 2) + screenshake.x;
	paddle.y = paddlePos.y - (PADDLE_HEIGHT / 2) + screenshake.y;
	paddle.w = PADDLE_WIDTH;
	paddle.h = PADDLE_HEIGHT;
	SDL_RenderFillRect(renderer, &paddle);

	//Draw Ball
	SDL_Rect ball;
	ball.x = ballPos.x - (BALL_SIZE / 2) + screenshake.x;
	ball.y = ballPos.y - (BALL_SIZE / 2) + screenshake.y;
	ball.w = BALL_SIZE;
	ball.h = BALL_SIZE;
	SDL_RenderFillRect(renderer, &ball);

	//Present the rendered screen
	SDL_RenderPresent(renderer);
}