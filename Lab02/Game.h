#pragma once
#include "SDL2/SDL.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
class Actor;
class SpriteComponent;
class Asteroid;

// TODO
class Game {
public:
	const int WIDTH = 1024;
	const int HEIGHT = 768;
	const char* TITLE = "Levi Asteroids";
	Game();
	bool Initialize();
	void Shutdown();
	void RunLoop();
	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);
	void LoadData();
	void UnloadData();
	SDL_Texture* GetTexture(std::string filename);
	void AddSprite(SpriteComponent* sprite);
	void RemoveSprite(SpriteComponent* sprite);
	void AddAsteroid(Asteroid* asteroid);
	void RemoveAsteroid(Asteroid* asteroid);
	std::vector<Asteroid*>& GetAsteroids();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<Actor*> actors;
	std::unordered_map<std::string, SDL_Texture*> textures;
	std::vector<SpriteComponent*> sprites;
	std::vector<Asteroid*> asteroids;
	int prevTime;
	bool gameRunning;
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
};