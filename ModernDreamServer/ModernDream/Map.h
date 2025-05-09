#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <array>
#include "Wall.h"
#include "../MapGenerator/MapGenerator/MapGenerator.h"



class Map
{
private:

    size_t height;
    size_t width;

    std::vector<std::unique_ptr<Wall>> walls;
    std::array<std::unique_ptr<Bomb>, 3> bombs;
    std::vector<std::vector<int>> mapMatrix;
    

public:
    Map();
    Map(MapGenerator& generator);
    bool IsPositionFree(std::pair<uint16_t, uint16_t> position) const;
    bool IsMovable(uint16_t x, uint16_t y) const;
    const std::vector<std::unique_ptr<Wall>>& GetWalls();
    const std::array<std::unique_ptr<Bomb>, 3>& GetBombs();
    size_t GetHeight() const;
    size_t GetWidth() const;
    Wall* GetWallAt(uint16_t x, uint16_t y);
    std::unique_ptr<Bomb>* GetBombAt(uint16_t x, uint16_t y);
    void SetFreePosition(uint16_t x, uint16_t y);
    void SetWalls(std::vector<std::unique_ptr<Wall>> newWalls);
    void SetBombs(const std::array<std::unique_ptr<Bomb>, 3>& newBombs);
    const std::vector<std::vector<int>>& GetMapMatrix() const;
    std::vector<std::pair<uint16_t, uint16_t>> GetPlayerStartPositions() const;
    int GetCellValue(uint16_t x, uint16_t y) const;
    void SetCellValue(uint16_t x, uint16_t y, uint16_t value);

};