#include "SpriteComponent.h"
#include <vector>
#include <string>

class TiledBGComponent : public SpriteComponent {
public:
	TiledBGComponent(class Actor* owner, int drawOrder = 50);
	virtual void Draw(SDL_Renderer* renderer) override;
	void LoadTileCSV(const std::string& fileName, int tileWidth, int tileHeight);
private:
	int mTileWidth;
	int mTileHeight;
	std::vector<std::vector<int>> tilemap;
};