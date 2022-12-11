#pragma once
#include "SDL2/SDL.h"

// TODO
class Game {
public:
	const int WIDTH = 1024;
	const int HEIGHT = 768;
	const char* TITLE = "Levi Pong";
	Game();
	bool Initialize();
	void Shutdown();
	void RunLoop();
private:
	const int WALL_THICKNESS = 16;
	const int PADDLE_WIDTH = 16;
	const int PADDLE_HEIGHT = 150;
	const int BALL_SIZE = 16;
	const float MOVE_SPEED = 400.0;
	const int INIT_BALL_SPEED = 250;
	const float SCREEN_SHAKE_DECAY = 15.0f;
	const float HIT_SCREEN_SHAKE = 3.0f;
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool gameRunning;
	SDL_Point paddlePos;
	SDL_Point ballPos;
	SDL_Point ballVelocity;
	int prevTime;
	int moveDir;
	float screenShakeMagnitude;
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
};