#include "AnimatedSprite.h"
#include "Actor.h"
#include "Game.h"

AnimatedSprite::AnimatedSprite(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
{
}

void AnimatedSprite::Update(float deltaTime)
{
	//Don't do anything if mAnimName is empty or not in our animations map
	if (mAnimName.empty()) { return; }
	std::unordered_map<std::string, std::vector<SDL_Texture*>>::iterator animIt = mAnims.find(mAnimName);
	if (animIt == mAnims.end()) { return; }

	//Update and wrap the timer
	if (!mIsPaused) {
		mAnimTimer += deltaTime * mAnimFPS;
	}
	std::vector<SDL_Texture*> anim = animIt->second;
	float animLength = (float)(anim.size());
	while (mAnimTimer >= animLength) {
		mAnimTimer -= animLength;
	}

	//Set the texture
	SetTexture(anim[(int)(mAnimTimer)]);
}

void AnimatedSprite::AddAnimation(const std::string& name,
								  const std::vector<SDL_Texture*>& images)
{
	mAnims.emplace(name, images);
}
