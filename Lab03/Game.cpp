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
#include "Vehicle.h"
#include "Log.h"
#include "Frog.h"
#include "CollisionComponent.h"

Game::Game() {
	window = nullptr;
	renderer = nullptr;
	gameRunning = false;
	goal = nullptr;
	frog = nullptr;
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
	//Create background
	Actor* background = new Actor(this);
	background->SetPosition(Vector2((float)WIDTH / 2, (float)HEIGHT/ 2));
	SpriteComponent* bgSpriteComp = new SpriteComponent(background, 50);
	bgSpriteComp->SetTexture(GetTexture("Assets/Background.png"));

	//Load data from file
	std::fstream file;
	file.open("Assets/Level.txt");
	if (file.is_open()) {
		size_t j = 0;
		while (!file.eof()) {
			std::string line;
			std::getline(file, line);
			for (size_t i = 0; i < line.length(); i++) {
				char c = line[i];
				Vector2 pos(BASE_X + i * GRID_SIZE, BASE_Y + j * GRID_SIZE);
				if (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'T') {
					Vehicle* newCar = new Vehicle(this, c, Vector2(j%2 == 0 ? 1.0f : -1.0f, 0.0f));
					newCar->SetPosition(pos);
				}
				else if (c == 'X' || c == 'Y' || c == 'Z') {
					Log* newLog = new Log(this, c, Vector2(j % 2 == 0 ? 1.0f : -1.0f, 0.0f));
					newLog->SetPosition(pos);
				}
				else if (c == 'F') {
					Frog* newFrog = new Frog(this);
					newFrog->SetPosition(pos);
				}
				else if (c == 'G') {
					goal = new Actor(this);
					goal->SetPosition(pos);
					CollisionComponent* goalCollision = new CollisionComponent(goal);
					goalCollision->SetSize(GRID_SIZE, GRID_SIZE);
				}
			}
			j++;
		}
	}
}

void Game::UnloadData() {
	//Deallocate all Actors
	while (!actors.empty()) {
		delete actors.back();
	}

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

void Game::AddVehicle(Vehicle* vehicle) {
	vehicles.push_back(vehicle);
}

void Game::RemoveVehicle(Vehicle* vehicle) {
	std::vector<Vehicle*>::const_iterator it = std::find(vehicles.begin(), vehicles.end(), vehicle);
	if (it != vehicles.end()) {
		vehicles.erase(it);
	}
}

std::vector<Vehicle*>& Game::GetVehicles() {
	return vehicles;
}

Frog* Game::GetFrog() {
	return frog;
}

void Game::SetFrog(Frog* newFrog) {
	frog = newFrog;
}

void Game::AddLog(Log* log) {
	logs.push_back(log);
}

void Game::RemoveLog(Log* log) {
	std::vector<Log*>::const_iterator it = std::find(logs.begin(), logs.end(), log);
	if (it != logs.end()) {
		logs.erase(it);
	}
}

std::vector<Log*>& Game::GetLogs() {
	return logs;
}

Actor* Game::GetGoal() {
	return goal;
}