//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Random.h"
#include "Player.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "HeightMap.h"
#include "Enemy.h"

Game::Game()
:mIsRunning(true), mPlayer(nullptr), mHeightMap(nullptr), mStartTimer(INIT_START_TIMER), mRaceStarted(false), mMusicChannel(-1)
{
	
}

bool Game::Initialize()
{
	Random::Init();

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT)) {
		SDL_Log("Renderer failed to initialize");
		return false;
	}

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	LoadData();

	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.01667f)
	{
		deltaTime = 0.01667f;
	}
	mTicksCount = SDL_GetTicks();

	//Start race
	if (!mRaceStarted) {
		mStartTimer -= deltaTime;
		if (mStartTimer <= 0.0f) {
			mPlayer->SetState(ActorState::Active);
			mEnemy->SetState(ActorState::Active);
			mMusicChannel = Mix_PlayChannel(-1, GetSound("Assets/Sounds/Music.ogg"), -1);
			mRaceStarted = true;
		}
	}
	
	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any actors to destroy to a temp vector
	std::vector<Actor*> destroyActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroyActors.emplace_back(actor);
		}
	}

	// Delete the destroyed actors (which will
	// remove them from mActors)
	for (auto actor : destroyActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::LoadData()
{
	//Initialize projection and view matrices
	Matrix4 projectionMatrix = Matrix4::CreatePerspectiveFOV(1.22f, WINDOW_WIDTH, WINDOW_HEIGHT, 10.0f, 10000.0f);
	mRenderer->SetProjectionMatrix(projectionMatrix);

	//Load Height Map
	mHeightMap = new HeightMap();

	//Create Track
	Actor* track = new Actor(this);
	track->SetRotation(Math::Pi);
	MeshComponent* trackMesh = new MeshComponent(track);
	trackMesh->SetMesh(GetRenderer()->GetMesh("Assets/Track.gpmesh"));

	//Create Player
	mPlayer = new Player(this);
	mPlayer->SetState(ActorState::Paused);

	//Create Enemy
	mEnemy = new Enemy(this);
	mEnemy->SetState(ActorState::Paused);

	//Load all the sounds
	GetSound("Assets/Sounds/RaceStart.wav");
	GetSound("Assets/Sounds/Music.ogg");
	GetSound("Assets/Sounds/FinalLap.wav");
	GetSound("Assets/Sounds/MusicFast.ogg");
	GetSound("Assets/Sounds/Won.wav");
	GetSound("Assets/Sounds/Lost.wav");

	//Play the starting music
	Mix_PlayChannel(-1, GetSound("Assets/Sounds/RaceStart.wav"), 0);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy sounds
	for (auto s : mSounds)
	{
		Mix_FreeChunk(s.second);
	}
	mSounds.clear();
}

Mix_Chunk* Game::GetSound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

void Game::Shutdown()
{
	UnloadData();
	Mix_CloseAudio();
	mRenderer->Shutdown();
	delete mRenderer;
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mActors.end() - 1;
		std::iter_swap(iter, iter2);
		mActors.pop_back();
	}
}

void Game::StartFinalLap() {
	Mix_FadeOutChannel(mMusicChannel, MUSIC_FADE_TIME);
	Mix_PlayChannel(-1, GetSound("Assets/Sounds/FinalLap.wav"), 0);
	mMusicChannel = Mix_FadeInChannel(-1, GetSound("Assets/Sounds/MusicFast.ogg"), 0, FASTMUSIC_FADEIN_TIME);
}

void Game::EndMusic(bool won) {
	Mix_FadeOutChannel(mMusicChannel, MUSIC_FADE_TIME);
	Mix_PlayChannel(-1, GetSound(won ? "Assets/Sounds/Won.wav" : "Assets/Sounds/Lost.wav"), 0);
}