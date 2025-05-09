
#pragma once

#ifdef MAPGENERATOR_EXPORTS
#define MAPGEN_API __declspec(dllexport)
#else
#define MAPGEN_API __declspec(dllimport)
#endif

#include <vector>
#include <random>
#include <utility>
#include <array> 




class MAPGEN_API MapGenerator {
private:

    void InitializeMapMatrix();
    void GenerateClusters();
    void PlaceConnectorWalls();
    void SetPlayerStartPosition(int numPlayers);
    void PlaceBombs(); 
    void GenerateNonDestructibleWalls();

    static const size_t kMinHeight = 13;
    static const size_t kMaxHeight = 25;
    static const size_t kMinWidth = 20;
    static const size_t kMaxWidth = 40;
    size_t currentHeight;
    size_t currentWidth;
    std::vector<std::vector<int>> mapMatrix;

    std::vector<std::pair<int, int>> wallPositions;
    std::vector<int> wallDurabilities;
    std::vector<bool> wallDestructibleFlags;

    std::vector<std::pair<int, int>> bombPositions;
    std::vector<bool> bombStatuses;
public:
    enum MapTile
    {
        PlayerPosition = 0,
        FreeSpace = 1,
        DestructibleWall = 2,
        DestructibleWallWithBomb = 3,
        NonDestructibleWall = 4
    };

    MapGenerator();

    ~MapGenerator();

    void GenerateMap(int numPlayers);
    void DisplayMap() const;

    std::vector<std::pair<int, int>> GetWallPositions() const;
    std::vector<int> GetWallDurabilities() const;
    std::vector<bool> GetWallDestructibleFlags() const;

    std::vector<std::pair<int, int>> GetBombPositions() const;
    std::vector<bool> GetBombStatuses() const;

    size_t GetHeightG() const;

    
    size_t GetWidthG() const;

    
    const std::vector<std::vector<int>>& GetMapMatrix() const { return mapMatrix; }

};
