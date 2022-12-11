//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "Actor.h"
#include <SDL2/SDL_image.h>
#include "SpriteComponent.h"
#include "Random.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "SDL2/SDL_mixer.h"
#include "TiledBGComponent.h"
#include "CSVHelper.h"
#include "Collider.h"
#include "Bush.h"
#include "Soldier.h"
#include "PathFinder.h"
#include "PathNode.h"
#include "EnemyComponent.h"

Game::Game() {
	window = nullptr;
	renderer = nullptr;
	gameRunning = false;
	player = nullptr;
	musicChannel = 0;
	startMusicActive = true;
	pathFinder = nullptr;
}

bool Game::Initialize() {
	//Initialize SDL, Window, and Renderer
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		return false;
	}

	int actualWidth = (int)(WIDTH * WINDOW_SCALE);
	int actualHeight = (int)(HEIGHT * WINDOW_SCALE);
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, actualWidth, actualHeight, 0);
	if (window == nullptr) {
		return false;
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	//Initialize SDL Image Loader
	if (IMG_Init(IMG_INIT_PNG) == 0) {
		return false;
	}
	
	//Initialize audio mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
		return false;
	}

	//Initialize RNG
	Random::Init();

	//Initialize game variables
	prevTime = SDL_GetTicks();
	gameRunning = true;

	//Create initial actors
	LoadData();
	return true;
}

void Game::Shutdown() {
	//Destroy actors
	UnloadData();
	
	//Shut down SDL
	Mix_CloseAudio();
	IMG_Quit();
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

	//Process keyboard state
	const Uint8* keyState = SDL_GetKeyboardState(nullptr);
	if (keyState[SDL_SCANCODE_ESCAPE]) {
		gameRunning = false;
	}

	//Call ProcessInput on all actors
	std::vector<Actor*> actorsCopy = actors;
	for (size_t i = 0; i < actorsCopy.size(); i++) {
		actorsCopy[i]->ProcessInput(keyState);
	}
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

	//Update All Actors
	std::vector<Actor*> actorsCopy = actors;
	for (size_t i = 0; i < actorsCopy.size(); i++) {
		actorsCopy[i]->Update(deltaTime);
	}

	//Destroy Actors
	std::vector<Actor*> actorsToDestroy;
	for (size_t i = 0; i < actors.size(); i++) {
		if (actors[i]->GetState() == ActorState::Destroy) {
			actorsToDestroy.push_back(actors[i]);
		}
	}
	for (size_t i = 0; i < actorsToDestroy.size(); i++) {
		delete actorsToDestroy[i];
	}

	//Transition from start to normal music
	if (startMusicActive && !Mix_Playing(musicChannel)) {
		musicChannel = Mix_PlayChannel(-1, GetSound("Assets/Sounds/MusicLoop.ogg"), -1);
		startMusicActive = false;
	}
}

void Game::GenerateOutput() {
	//Clear background
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	//Draw all sprites
	for (SpriteComponent* sprite : sprites) {
		if (sprite->IsVisible()) {
			sprite->Draw(renderer);
		}
	}

	//Present the rendered screen
	SDL_RenderPresent(renderer);
}

void Game::AddActor(Actor* actor) {
	actors.push_back(actor);
}

void Game::RemoveActor(Actor* actor) {
	std::vector<Actor*>::const_iterator it = std::find(actors.begin(), actors.end(), actor);
	if (it != actors.end()) {
		actors.erase(it);
	}
}

void Game::LoadData() {
	//Load the tilemap background
	Actor* tileMap = new Actor(this);
	TiledBGComponent* bgComp = new TiledBGComponent(tileMap);
	bgComp->SetTexture(GetTexture("Assets/Map/Tiles.png"));
	bgComp->LoadTileCSV("Assets/Map/Tiles.csv", 32, 32);

	//Create PathFinder
	pathFinder = new PathFinder(this);

	//Load the actors
	std::fstream objectFile;
	objectFile.open("Assets/Map/Objects.csv");
	if (objectFile.is_open()) {
		//Go through all rows
		std::string line;
		std::getline(objectFile, line);	//discard first line
		while (!objectFile.eof()) {
			//Get the line and use its data to make an object
			std::getline(objectFile, line);
			if (!line.empty()) {
				LoadObject(line);
			}
		}
	}

	//Start the music
	musicChannel = Mix_PlayChannel(-1, GetSound("Assets/Sounds/MusicStart.ogg"), 0);
	startMusicActive = true;
}

void Game::LoadObject(std::string& csvLine) {
	//Parse the fields from the string
	std::vector<std::string> fields = CSVHelper::Split(csvLine);
	if (fields.size() < 5) {
		return;
	}

	//Figure out size and position
	Vector2 size(std::stof(fields[3]), std::stof(fields[4]));
	Vector2 pos(std::stof(fields[1]), std::stof(fields[2]));
	pos += size * 0.5f;

	//Create the actor
	Actor* newObj = nullptr;
	if (fields[0] == "Player") {
		newObj = new Player(this);
	}
	else if (fields[0] == "Collider") {
		Collider* newCollider = new Collider(this, size.x, size.y);
		colliders.push_back(newCollider);
		newObj = newCollider;
	}
	else if (fields[0] == "Bush") {
		newObj = new Bush(this);
	}
	else if (fields[0] == "Soldier") {
		PathNode* start = pathFinder->GetPathNode((size_t)std::stoi(fields[5]), (size_t)std::stoi(fields[6]));
		PathNode* end = pathFinder->GetPathNode((size_t)std::stoi(fields[7]), (size_t)std::stoi(fields[8]));
		newObj = new Soldier(this, start, end);
	}

	//Set its position
	if (newObj != nullptr) {
		newObj->SetPosition(pos);
	}
}

void Game::UnloadData() {
	//Deallocate all Actors
	while (!actors.empty()) {
		delete actors.back();
	}

	//Deallocate all sounds
	for (std::pair<std::string, Mix_Chunk*> soundPair : sounds) {
		Mix_FreeChunk(soundPair.second);
	}
	sounds.clear();

	//Deallocate all textures
	for (std::pair<std::string, SDL_Texture*> tex : textures) {
		SDL_DestroyTexture(tex.second);
	}
	textures.clear();
}

SDL_Texture* Game::GetTexture(std::string filename) {
	//Check if this texture is already loaded
	std::unordered_map<std::string, SDL_Texture*>::iterator texIt = textures.find(filename);
	if (texIt != textures.end()) {
		return texIt->second;
	}

	//Load the texture
	SDL_Surface* surf = IMG_Load(filename.c_str());
	if (surf == nullptr) {
		SDL_Log("failed to load texture %s", filename.c_str());
		return nullptr;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);

	//Save for future use
	textures.insert(std::make_pair(filename, tex));
	return tex;
}

Mix_Chunk* Game::GetSound(const std::string& filename) {
	//Check if this texture is already loaded
	std::unordered_map<std::string, Mix_Chunk*>::iterator soundIt = sounds.find(filename);
	if (soundIt != sounds.end()) {
		return soundIt->second;
	}

	//Load the sound
	Mix_Chunk* sound = Mix_LoadWAV(filename.c_str());
	if (sound == nullptr) {
		SDL_Log("failed to load sound %s", filename.c_str());
		return nullptr;
	}

	//Save for future use
	sounds.insert(std::make_pair(filename, sound));
	return sound;
}

void Game::AddSprite(SpriteComponent* sprite) {
	sprites.push_back(sprite);
	std::sort(sprites.begin(), sprites.end(),
		[](SpriteComponent* a, SpriteComponent* b) {
			return a->GetDrawOrder() < b->GetDrawOrder();
		});
}

void Game::RemoveSprite(SpriteComponent* sprite) {
	std::vector<SpriteComponent*>::const_iterator it = std::find(sprites.begin(), sprites.end(), sprite);
	if (it != sprites.end()) {
		sprites.erase(it);
	}
}

Vector2& Game::GetCameraPos() {
	return cameraPos;
}

void Game::SetPlayer(Player* newPlayer) {
	player = newPlayer;
}

Player* Game::GetPlayer() {
	return player;
}

void Game::StopMusic() {
	Mix_HaltChannel(musicChannel);
}

const std::vector<Collider*>& Game::GetColliders() {
	return colliders;
}

PathFinder* Game::GetPathFinder() {
	return pathFinder;
}

std::vector<EnemyComponent*>& Game::GetEnemies() {
	return enemies;
}