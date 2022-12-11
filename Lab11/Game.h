#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include "Math.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

class Player;
class Checkpoint;

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
#ifdef __EMSCRIPTEN__
	void EmRunIteration()
	{
		if (!mIsRunning)
		{
			emscripten_cancel_main_loop();
		}
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
#endif
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	Mix_Chunk* GetSound(const std::string& fileName);

	class Renderer* GetRenderer() {	return mRenderer; }
	Player* GetPlayer() { return mPlayer; }
	void SetPlayer(Player* player) { mPlayer = player; }
	std::vector<Actor*>& GetBlocks() { return mBlocks; }

	void AddCheckpoint(Checkpoint* newCheckpoint);
	void RemoveCheckpoint();
	Checkpoint* GetActiveCheckpoint();
	void SetNextLevel(std::string levelName) { mNextLevel = levelName; }
	void LoadNextLevel();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	std::unordered_map<std::string, Mix_Chunk*> mSounds;

	// All the actors in the game
	std::vector<class Actor*> mActors;

	class Renderer* mRenderer = nullptr;

	Uint32 mTicksCount = 0;
	bool mIsRunning;

	const float WINDOW_WIDTH = 1024.0f;
	const float WINDOW_HEIGHT = 768.0f;
	Player* mPlayer;
	std::vector<Actor*> mBlocks;
	std::queue<Checkpoint*> mCheckpoints;
	std::string mNextLevel;
};
