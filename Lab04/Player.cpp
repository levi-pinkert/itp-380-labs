#include "Player.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "CollisionComponent.h"
#include "PlayerMove.h"

Player::Player(Game* game) : Actor(game) {
	//Create animations
	spriteComp = new AnimatedSprite(this, 200);
	std::vector<SDL_Texture*> idleAnim{ mGame->GetTexture("Assets/Mario/Idle.png") };
	spriteComp->AddAnimation("idle", idleAnim);
	std::vector<SDL_Texture*> deadAnim{ mGame->GetTexture("Assets/Mario/Dead.png") };
	spriteComp->AddAnimation("dead", deadAnim);
	std::vector<SDL_Texture*> jumpLeftAnim{ mGame->GetTexture("Assets/Mario/JumpLeft.png") };
	spriteComp->AddAnimation("jumpLeft", jumpLeftAnim);
	std::vector<SDL_Texture*> jumpRightAnim{ mGame->GetTexture("Assets/Mario/JumpRight.png") };
	spriteComp->AddAnimation("jumpRight", jumpRightAnim);
	std::vector<SDL_Texture*> runLeftAnim{
		mGame->GetTexture("Assets/Mario/RunLeft0.png"),
		mGame->GetTexture("Assets/Mario/RunLeft1.png"),
		mGame->GetTexture("Assets/Mario/RunLeft2.png")
	};
	spriteComp->AddAnimation("runLeft", runLeftAnim);
	std::vector<SDL_Texture*> runRightAnim{
		mGame->GetTexture("Assets/Mario/RunRight0.png"),
		mGame->GetTexture("Assets/Mario/RunRight1.png"),
		mGame->GetTexture("Assets/Mario/RunRight2.png")
	};
	spriteComp->AddAnimation("runRight", runRightAnim);
	spriteComp->SetAnimation("idle");
	
	//Instantiate collision and move components
	collisionComp = new CollisionComponent(this);
	collisionComp->SetSize(COLLISION_SIZE, COLLISION_SIZE);
	moveComp = new PlayerMove(this);

	//Update game's player pointer
	game->SetPlayer(this);
}

Player::~Player() {
	//Update game's player pointer
	mGame->SetPlayer(nullptr);
}
