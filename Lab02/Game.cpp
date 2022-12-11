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
#include "Actor.h"
#include <SDL2/SDL_image.h>
#include "SpriteComponent.h"
#include "Ship.h"
#include "Random.h"
#include "Asteroid.h"

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
	Actor* stars = new Actor(this);
	stars->SetPosition(Vector2(512.0f, 384.0f));
	SpriteComponent* sc4 = new SpriteComponent(stars, 50);
	sc4->SetTexture(GetTexture("Assets/Stars.png"));

	//Create ship
	Actor* ship = new Ship(this);
	ship->SetPosition(Vector2(512.0f, 384.0f));

	//Create asteroids
	for (int i = 0; i < 10; i++) {
		new Asteroid(this);
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
		SDL_Log("failed to load image %s", filename.c_str());
		return nullptr;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if (tex == nullptr) {
		SDL_Log("failed to load texture %s", filename.c_str());
		return nullptr;
	}

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

void Game::AddAsteroid(Asteroid* asteroid) {
	asteroids.push_back(asteroid);
}

void Game::RemoveAsteroid(Asteroid* asteroid) {
	std::vector<Asteroid*>::const_iterator it = std::find(asteroids.begin(), asteroids.end(), asteroid);
	if (it != asteroids.end()) {
		asteroids.erase(it);
	}
}

std::vector<Asteroid*>& Game::GetAsteroids() {
	return asteroids;
}
