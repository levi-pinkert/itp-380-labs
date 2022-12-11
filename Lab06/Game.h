#pragma once
#include "SDL2/SDL.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
#include "Math.h"
#include "SDL2/SDL_mixer.h"
class Actor;
class SpriteComponent;
class Player;
class Collider;
class PathFinder;
class EnemyComponent;

// TODO
class Game {
public:
	const int WIDTH = 512;
	const int HEIGHT = 448;
	const float WINDOW_SCALE = 1.0f;
	const char* TITLE = "Levi's Zelda";
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
	Vector2& GetCameraPos();
	Mix_Chunk* GetSound(const std::string& filename);
	void SetPlayer(Player* player);
	Player* GetPlayer();
	void StopMusic();
	const std::vector<Collider*>& GetColliders();
	PathFinder* GetPathFinder();
	std::vector<EnemyComponent*>& GetEnemies();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<Actor*> actors;
	std::unordered_map<std::string, SDL_Texture*> textures;
	std::vector<SpriteComponent*> sprites;
	std::unordered_map<std::string, Mix_Chunk*> sounds;
	int prevTime;
	bool gameRunning;
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	Vector2 cameraPos;
	Player* player;
	int musicChannel;
	bool startMusicActive;
	void LoadObject(std::string& csvLine);
	std::vector<Collider*> colliders;
	PathFinder* pathFinder;
	std::vector<EnemyComponent*> enemies;
};