#include "PlayerMove.h"
#include "Actor.h"
#include "MoveComponent.h"
#include "Math.h"
#include "SDL2/SDL.h"
#include "Game.h"
#include "Renderer.h"
#include "SideBlock.h"
#include "Random.h"
#include "Bullet.h"
#include "Player.h"
#include "CollisionComponent.h"
#include "Block.h"

PlayerMove::PlayerMove(Actor* actor) : MoveComponent(actor) {
	//Initialize member variables
	mVelocity = Vector3::UnitX * MOVE_SPEED;
	wallsSpawned = 0;
	levelsLoaded = 1;
	lastShootInput = false;
	player = static_cast<Player*>(actor);
	spdMult = 1.0f;
	accelTimer = ACCEL_TIME;
	rollTimer = 0.0f;
	lastRollInput = false;
}

void PlayerMove::ProcessInput(const Uint8* keyState) {
	//Get the input as a vector
	Vector3 input(0.0f, (float)(keyState[SDL_SCANCODE_D] - keyState[SDL_SCANCODE_A]), (float)(keyState[SDL_SCANCODE_W] - keyState[SDL_SCANCODE_S]));

	//Set the velocity based on input
	mVelocity = spdMult * (Vector3::UnitX * MOVE_SPEED + input * STRAFE_SPEED);

	//Shoot
	if (!lastShootInput && keyState[SDL_SCANCODE_SPACE]) {
		Bullet* newBullet = new Bullet(mOwner->GetGame(), spdMult);
		newBullet->SetPosition(mOwner->GetPosition());
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Shoot.wav"), 0);
	}
	lastShootInput = keyState[SDL_SCANCODE_SPACE];

	//Barrel Roll
	if (!lastRollInput && keyState[SDL_SCANCODE_Q] && rollTimer == 0.0f) {
		rollTimer = ROLL_LENGTH;
		player->Heal();
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Boost.wav"), 0);
	}
	lastRollInput = keyState[SDL_SCANCODE_Q];
}

void PlayerMove::Update(float deltaTime) {
	//Accelerate and move
	EulerIntegrate(deltaTime);

	//Set Camera Position
	UpdateCameraPos();

	//Spawn Walls
	while (wallsSpawned * WALL_SIZE < mOwner->GetPosition().x + WALL_SPAWN_DIST) {
		CreateWallSet();
	}

	//Load more levels
	while (levelsLoaded * LEVEL_SEPARATION < mOwner->GetPosition().x + WALL_SPAWN_DIST) {
		int levelNum = levelsLoaded;
		if (levelNum > 20) { levelNum = Random::GetIntRange(1, 20); }
		mOwner->GetGame()->LoadBlocks("Assets/Blocks/" + std::to_string(levelNum) + ".txt", levelsLoaded * LEVEL_SEPARATION);
		levelsLoaded++;
	}

	//Collide with blocks
	CheckBlockCollision();

	//Barrel Roll
	if (rollTimer > 0.0f) {
		//Currently in a roll
		mOwner->SetRoll(mOwner->GetRoll() + ROLL_SPEED * deltaTime);
		rollTimer = std::max(0.0f, rollTimer - deltaTime);
	}
	else {
		mOwner->SetRoll(0.0f);
	}
}

void PlayerMove::CreateWallSet() {
	Game* game = mOwner->GetGame();
	float wallSpawnX = wallsSpawned * WALL_SIZE;
	SideBlock* block1 = new SideBlock(game, SIDE_PATTERN[wallsSpawned % 4]);
	block1->SetPosition(Vector3(wallSpawnX, WALL_SIZE, 0.0f));
	block1->SetRotation(Math::Pi);
	SideBlock* block2 = new SideBlock(game, SIDE_PATTERN[wallsSpawned % 4]);
	block2->SetPosition(Vector3(wallSpawnX, -WALL_SIZE, 0.0f));
	SideBlock* block3 = new SideBlock(game, 5);
	block3->SetPosition(Vector3(wallSpawnX, 0.0f, -WALL_SIZE));
	SideBlock* block4 = new SideBlock(game, wallsSpawned % 2 == 0 ? 6 : 7);
	block4->SetPosition(Vector3(wallSpawnX, 0.0f, WALL_SIZE));
	wallsSpawned++;
}

void PlayerMove::CheckBlockCollision() {
	std::vector<Block*>& blocks = mOwner->GetGame()->GetBlocks();
	bool hitBlock = false;
	for (Block* block : blocks) {
		if (player->GetCollisionComponent()->Intersect(block->GetCollision())) {
			//Destroy the block and count the hit
			block->Explode();
			hitBlock = true;
		}
	}
	if (hitBlock) {
		player->TakeDamage();
	}
}

void PlayerMove::EulerIntegrate(float deltaTime) {
	//Accelerate
	accelTimer -= deltaTime;
	if (accelTimer <= 0.0f) {
		accelTimer = ACCEL_TIME;
		spdMult += ACCEL_AMOUNT;
	}

	//Move according to velocity (and clamp position)
	Vector3 newPos = mOwner->GetPosition() + mVelocity * deltaTime;
	newPos.y = Math::Clamp(newPos.y, -MAX_Y, MAX_Y);
	newPos.z = Math::Clamp(newPos.z, -MAX_Z, MAX_Z);
	mOwner->SetPosition(newPos);
}

void PlayerMove::UpdateCameraPos() {
	Vector3 ownerPos = mOwner->GetPosition();
	Vector3 ownerFwd = mOwner->GetForward();
	Vector3 cameraPos = ownerPos - (ownerFwd * CAM_H_DIST);
	cameraPos.z = 0.0f;
	Vector3 targetPos = ownerPos + (ownerFwd * CAM_TARGET_DIST);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(Matrix4::CreateLookAt(cameraPos, targetPos, Vector3::UnitZ));
}