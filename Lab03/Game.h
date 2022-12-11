#pragma once
#include "SDL2/SDL.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
class Actor;
class SpriteComponent;
class Vehicle;
class Frog;
class Log;

// TODO
class Game {
public:
	const int WIDTH = 896;
	const int HEIGHT = 1024;
	const float WINDOW_SCALE = 0.5;
	const char* TITLE = "Levi's Frogger";
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
	void AddVehicle(Vehicle* vehicle);
	void RemoveVehicle(Vehicle* vehicle);
	std::vector<Vehicle*>& GetVehicles();
	Frog* GetFrog();
	void SetFrog(Frog* frog);
	void AddLog(Log* log);
	void RemoveLog(Log* log);
	std::vector<Log*>& GetLogs();
	Actor* GetGoal();
private:
	const float BASE_X = 64.0f;
	const float BASE_Y = 160.0f;
	const float GRID_SIZE = 64.0f;
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<Actor*> actors;
	std::unordered_map<std::string, SDL_Texture*> textures;
	std::vector<SpriteComponent*> sprites;
	std::vector<Vehicle*> vehicles;
	Frog* frog;
	std::vector<Log*> logs;
	Actor* goal;
	int prevTime;
	bool gameRunning;
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
};