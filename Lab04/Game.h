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
class Block;
class Goomba;
class Player;

// TODO
class Game {
public:
	const int WIDTH = 600;
	const int HEIGHT = 448;
	const float WINDOW_SCALE = 1.0f;
	const char* TITLE = "Levi's Mario";
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
	void AddBlock(Block* block);
	void RemoveBlock(Block* block);
	const std::vector<Block*>& GetBlocks() const;
	void AddGoomba(Goomba* goomba);
	void RemoveGoomba(Goomba* goomba);
	const std::vector<Goomba*>& GetGoombas() const;
	void SetPlayer(Player* player);
	Player* GetPlayer();
	void StopMusic();
private:
	const float BASE_X = 16.0f;
	const float BASE_Y = 16.0f;
	const float GRID_SIZE = 32.0f;
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
	std::vector<Block*> blocks;
	std::vector<Goomba*> goombas;
	Player* player;
	int musicChannel;
};