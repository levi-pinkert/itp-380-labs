#include "TiledBGComponent.h"
#include <vector>
#include <string>
#include "Actor.h"
#include "CSVHelper.h"
#include <fstream>
#include "Game.h"
#include "SpriteComponent.h"
#include "Math.h"

TiledBGComponent::TiledBGComponent(Actor* owner, int drawOrder) : SpriteComponent(owner, drawOrder) {
	mTileWidth = 0;
	mTileWidth = 0;
}

void TiledBGComponent::Draw(SDL_Renderer* renderer) {
	if (mTexture)
	{
		Vector2& camPos = mOwner->GetGame()->GetCameraPos();
		int texWidth = GetTexWidth();
		for (size_t tileY = 0; tileY < tilemap.size(); tileY++) {
			for (size_t tileX = 0; tileX < tilemap[tileY].size(); tileX++) {
				//Don't draw anything if it's -1
				int tile = tilemap[tileY][tileX];
				if (tile != -1) {
					//Find location of source rect
					SDL_Rect sourceR;
					sourceR.w = mTileWidth;
					sourceR.h = mTileHeight;
					sourceR.x = tile * mTileWidth % texWidth;
					sourceR.y = tile / (texWidth / mTileWidth) * mTileHeight;

					//Find location of destination rect
					SDL_Rect destR;
					destR.w = mTileWidth;
					destR.h = mTileHeight;
					destR.x = (int)tileX * mTileWidth - (int)(camPos.x);
					destR.y = (int)tileY * mTileHeight - (int)(camPos.y);

					//Draw
					SDL_RenderCopyEx(renderer,
						mTexture,
						&sourceR,
						&destR,
						0.0,
						nullptr,
						SDL_FLIP_NONE);
				}
			}
		}
		
	}
}

void TiledBGComponent::LoadTileCSV(const std::string& fileName, int tileWidth, int tileHeight) {
	mTileWidth = tileWidth;
	mTileHeight = tileHeight;
	//Try to open the file
	std::fstream file;
	file.open(fileName);
	if (file.is_open()) {
		//Go through all rows
		while (!file.eof()) {
			//Copy the line into the tilemap, converting strings to ints
			std::string line;
			std::getline(file, line);
			if (!line.empty()) {
				std::vector<std::string> row = CSVHelper::Split(line);
				std::vector<int> rowInts;
				for (size_t i = 0; i < row.size(); i++) {
					rowInts.push_back(std::stoi(row[i]));
				}
				tilemap.push_back(rowInts);
			}
		}
	}
}