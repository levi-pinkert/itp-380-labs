#include "HeightMap.h"
#include "CSVHelper.h"
#include <fstream>
#include "Math.h"
#include "SDL2/SDL.h"

HeightMap::HeightMap() {
	//Load the file
	std::fstream file;
	file.open(FILE_NAME);
	if (file.is_open()) {
		//Parse each row into a vector and push it onto the end of the heightMap
		while (!file.eof()) {
			std::string line;
			std::getline(file, line);
			if (line.empty()) { continue; }
			std::vector<std::string> strs = CSVHelper::Split(line);
			std::vector<int> row;
			for (std::string str : strs) {
				row.push_back(std::stoi(str));
			}
			heightMap.push_back(row);
		}
	}
}

bool HeightMap::IsCellOnTrack(int row, int col) {
	if (row < 0 || (unsigned long)row >= heightMap.size() || col < 0 || (unsigned long)col >= heightMap[0].size()) {
		return false;
	}
	return heightMap[row][col] != -1;
}

float HeightMap::GetHeightFromCell(int row, int col) {
	return heightMap[row][col] * HEIGHT_MULT + HEIGHT_OFFSET;
}

Vector3 HeightMap::CellToWorld(int row, int col) {
	if (!IsCellOnTrack(row, col)) { return Vector3::Zero; }
	Vector3 result = Vector3::Zero;
	result.x = GRID_TOP - CELL_SIZE * row;
	result.y = GRID_LEFT + CELL_SIZE * col;
	result.z = GetHeightFromCell(row, col);
	return result;
}

Vector2 HeightMap::WorldToCell(const Vector2& pos) {
	Vector2 result = Vector2::Zero;
	result.x = (Math::Abs(pos.x - GRID_TOP) + CELL_SIZE / 2.0f) / CELL_SIZE;
	result.y = (Math::Abs(pos.y - GRID_LEFT) + CELL_SIZE / 2.0f) / CELL_SIZE;
	return result;
}

bool HeightMap::IsOnTrack(const Vector2& pos) {
	Vector2 cell = WorldToCell(pos);
	return IsCellOnTrack((int)cell.x, (int)cell.y);
}

float HeightMap::GetHeight(const Vector2& pos) {
	if (!IsOnTrack(pos)) { return OFFTRACK_HEIGHT; }
	Vector2 cell = WorldToCell(pos);
	return GetHeightFromCell((int)cell.x, (int)cell.y);
}