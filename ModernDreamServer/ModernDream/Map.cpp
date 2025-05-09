#include "Map.h"


Map::Map(){}

Map::Map(MapGenerator& generator) {
    height = generator.GetHeightG();
    width = generator.GetWidthG();
    mapMatrix.resize(height, std::vector<int>(width));

    const auto& generatorMatrix = generator.GetMapMatrix();
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            mapMatrix[i][j] = generatorMatrix[i][j];
        }
    }

    const auto& wallPositions = generator.GetWallPositions();
    const auto& wallDurabilities = generator.GetWallDurabilities();
    const auto& wallDestructibles = generator.GetWallDestructibleFlags();

    for (size_t i = 0; i < wallPositions.size(); ++i) {
        walls.push_back(std::make_unique<Wall>(
            wallPositions[i],
            wallDestructibles[i] ? WallType::DestructibleWall : WallType::NonDestructibleWall,
            wallDurabilities[i],
            wallDestructibles[i]
        ));
    }

    const auto& bombPositions = generator.GetBombPositions();
    const auto& bombStatuses = generator.GetBombStatuses();

    size_t bombIndex = 0;
    for (size_t i = 0; i < bombPositions.size(); ++i) {
        if (bombStatuses[i] && bombIndex < bombs.size()) {
            bombs[bombIndex++] = std::make_unique<Bomb>(bombPositions[i]);
        }
    }
}
 
std::vector<std::pair<uint16_t, uint16_t>> Map::GetPlayerStartPositions() const {
    std::vector<std::pair<uint16_t, uint16_t>> startPositions;
    for (size_t i = 0; i < mapMatrix.size(); ++i) {
        for (size_t j = 0; j < mapMatrix[i].size(); ++j) {
            if (mapMatrix[i][j] == 0) { 
                startPositions.emplace_back(i, j);
            }
        }
    }
    return startPositions;
}


bool Map::IsPositionFree(std::pair<uint16_t, uint16_t> position) const
{
    for (const auto& wall : walls) { 
        if (wall->GetPosition() == position && wall->GetDurability() < 0) {
            return false;
        }
    }
    return true;
}

const std::vector<std::vector<int>>& Map::GetMapMatrix() const {
    return mapMatrix;
}


bool Map::IsMovable(uint16_t x, uint16_t y) const
{
    if (x < 0 || x >= height || y < 0 || y >= width) { 
        return false;
    }
    if (!IsPositionFree({ x, y })) {
        return false;
    }

    return mapMatrix[x][y] == 1;
}

const std::vector<std::unique_ptr<Wall>>& Map::GetWalls() 
{
    return walls;
}

const std::array<std::unique_ptr<Bomb>, 3>& Map::GetBombs()
{
    return bombs; 
}

size_t Map::GetHeight() const 
{
    return height;
}

size_t Map::GetWidth() const
{
    return width;
}


Wall* Map::GetWallAt(uint16_t x, uint16_t y) {
    for (auto& wall : walls) {
        if (wall->GetPosition() == std::make_pair(x, y)) {
            return wall.get();  
        }
    }
    return nullptr;
}

std::unique_ptr<Bomb>* Map::GetBombAt(uint16_t x, uint16_t y)
{
    for (auto& bomb : bombs)
    {
        if (bomb->GetPosition() == std::make_pair(x, y))
        {
            return &bomb;  
        }
    }
    return nullptr;
}

void Map::SetFreePosition(uint16_t x, uint16_t y)
{
    mapMatrix[x][y] = 1;
}

void Map::SetWalls(std::vector<std::unique_ptr<Wall>> newWalls) 
{
    walls = std::move(newWalls);
}


void Map::SetBombs(const std::array<std::unique_ptr<Bomb>, 3>& newBombs)
{
    for (size_t i = 0; i < bombs.size(); ++i) {
        bombs[i] = newBombs[i] ? std::make_unique<Bomb>(*newBombs[i]) : nullptr;
    }
}


int Map::GetCellValue(uint16_t x, uint16_t y) const {
    if (x < 0 || x >= height || y < 0 || y >= width) {
        return -1;  
    }
    return mapMatrix[x][y];
}

void Map::SetCellValue(uint16_t x, uint16_t y, uint16_t value) {
    if (x >= 0 && x < height && y >= 0 && y < width) {
        mapMatrix[x][y] = value;
    } 
}
