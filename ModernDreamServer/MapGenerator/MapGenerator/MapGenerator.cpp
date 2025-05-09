
#include "MapGenerator.h"
#include<random>
#include <algorithm>
#include <iostream>


MapGenerator::MapGenerator() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> heightDist(kMinHeight, kMaxHeight);
    std::uniform_int_distribution<> widthDist(kMinWidth, kMaxWidth);

    currentHeight = heightDist(gen);
    currentWidth = widthDist(gen);
} 

void MapGenerator::GenerateMap(int numPlayers)
{

    std::cout << "Inițializăm matricea cu dimensiunea: (" << currentHeight << ", " << currentWidth << ")\n";

    InitializeMapMatrix();
    GenerateClusters();
    PlaceConnectorWalls();
    SetPlayerStartPosition(numPlayers);
    PlaceBombs();
    DisplayMap();
    GenerateNonDestructibleWalls();
  
}


void MapGenerator::InitializeMapMatrix() {
    if (currentHeight <= 0 || currentWidth <= 0) {
        throw std::runtime_error("Dimensiunile matricei sunt nevalide!");
    }

    mapMatrix.resize(currentHeight, std::vector<int>(currentWidth, FreeSpace));
}

void MapGenerator::GenerateNonDestructibleWalls() { 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 100);

    for (size_t i = 1; i < currentHeight - 1; ++i) {
        for (size_t j = 1; j < currentWidth - 1; ++j) { 
            if (dist(gen) < 10) {  
                mapMatrix[i][j] = NonDestructibleWall;
                wallPositions.emplace_back(std::make_pair(i, j));
                wallDurabilities.push_back(99999); 
                wallDestructibleFlags.push_back(false);
            }
        }
    }
}
void MapGenerator::GenerateClusters() {
    std::random_device rd;
    std::mt19937 gen(rd());

    int numClusters = std::uniform_int_distribution<>(6, 12)(gen);
    std::uniform_int_distribution<> clusterSizeDist(3, 5);
    std::uniform_int_distribution<> wallTypeDist(0, 100);

    for (int cluster = 0; cluster < numClusters; cluster++) {
        int startY = std::uniform_int_distribution<>(2,MapGenerator::currentHeight  - 4)(gen);
        int startX = std::uniform_int_distribution<>(2, MapGenerator::currentWidth - 4)(gen);

        int clusterHeight = clusterSizeDist(gen);
        int clusterWidth = clusterSizeDist(gen);

        bool canPlace = true;
        for (int x = startX - 1; x <= startX + clusterWidth && canPlace; x++) {
            for (int y = startY - 1; y <= startY + clusterHeight && canPlace; y++) {
                if (x >= 0 && x < MapGenerator::currentHeight && y >= 0 && y < MapGenerator::currentWidth) {
                    if (mapMatrix[x][y] != FreeSpace) {
                        canPlace = false;
                    }
                }
            }
        }

        if (canPlace) {
            bool isHollow = std::uniform_int_distribution<>(0, 1)(gen);
            for (int x = startX; x < startX + clusterWidth && x < MapGenerator::currentHeight - 1; x++) {
                for (int y = startY; y < startY + clusterHeight && y < MapGenerator::currentWidth - 1; y++) {
                    if (!isHollow || x == startX || x == startX + clusterWidth - 1 ||
                        y == startY || y == startY + clusterHeight - 1) {

                        if (wallTypeDist(gen) < 90) {  
                            mapMatrix[x][y] = DestructibleWall;
                            wallPositions.emplace_back(std::make_pair(x, y));
                            wallDurabilities.push_back(1);
                            wallDestructibleFlags.push_back(true);
                        }

                    }
                }
            }
        }
    }
}

void MapGenerator::PlaceConnectorWalls() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> connectorDist(0, 2);
    std::uniform_int_distribution<> wallTypeDist(0, 80);

    for (int x = 1; x < MapGenerator::currentHeight - 1; x++) {
        for (int y = 1; y < MapGenerator::currentWidth - 1; y++) {
                if (mapMatrix[x][y] == FreeSpace) {
                    bool hasNearbyWalls = false;
                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            if (x + dx >= 0 && x + dx < MapGenerator::currentHeight && y + dy >= 0 && y + dy < MapGenerator::currentWidth) {
                                if (mapMatrix[x + dx][y + dy] == DestructibleWall) {
                                    hasNearbyWalls = true;
                                    break;
                                }
                            }
                        }
                    }
                
                if (hasNearbyWalls && connectorDist(gen) == 0) {
                    mapMatrix[y][x] = DestructibleWall;
                    bool isDestructible = (wallTypeDist(gen) == 1);
                    wallPositions.emplace_back(std::make_pair(x, y));
                    wallDurabilities.push_back(1); 
                    wallDestructibleFlags.push_back(isDestructible);
                }
            }
        }
    }
}

void MapGenerator::SetPlayerStartPosition(int numPlayers) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<std::pair<int, int>> startPositions = {
        {0, 0}, {MapGenerator::currentHeight - 1, 0},
        {0, MapGenerator::currentWidth - 1}, {MapGenerator::currentHeight - 1, MapGenerator::currentWidth - 1}
    };
    std::shuffle(startPositions.begin(), startPositions.end(), gen);

    for (int i = 0; i < numPlayers; i++) {
        int px = startPositions[i].first;
        int py = startPositions[i].second;
        mapMatrix[px][py] = PlayerPosition;
    }
}

void MapGenerator::PlaceBombs() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> bombCountDist(0, 3);
    int bombCount = bombCountDist(gen);

    std::vector<size_t> eligibleWalls;
    for (size_t i = 0; i < wallPositions.size(); i++) {
        if (wallDestructibleFlags[i]) {
            eligibleWalls.push_back(i);
        }
    }

    if (!eligibleWalls.empty()) {
        std::shuffle(eligibleWalls.begin(), eligibleWalls.end(), gen);
        int actualBombCount = std::min(bombCount, static_cast<int>(eligibleWalls.size()));

        for (int i = 0; i < actualBombCount; i++) {
            size_t wallIndex = eligibleWalls[i];
            auto position = wallPositions[wallIndex];
            bombPositions.emplace_back(position);
            mapMatrix[position.first][position.second] = DestructibleWallWithBomb;
            bombStatuses.push_back(true);  
        }
    }
}

void MapGenerator::DisplayMap() const {
    std::cout << "Map generated with size (" << MapGenerator::currentHeight << ", " << MapGenerator::currentWidth << ")." << std::endl;
    for (const auto& row : mapMatrix) {
        for (int cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}
std::vector<std::pair<int, int>> MapGenerator::GetWallPositions() const {
    return wallPositions;
}

std::vector<int> MapGenerator::GetWallDurabilities() const {
    return wallDurabilities;
}

std::vector<bool> MapGenerator::GetWallDestructibleFlags() const {
    return wallDestructibleFlags;
}

std::vector<std::pair<int, int>> MapGenerator::GetBombPositions() const {
    return bombPositions;
}

std::vector<bool> MapGenerator::GetBombStatuses() const {
    return bombStatuses;
}
size_t MapGenerator::GetHeightG() const
{
    return currentHeight;
}
size_t MapGenerator::GetWidthG() const
{
    return currentWidth; 
}
MapGenerator::~MapGenerator() {}
