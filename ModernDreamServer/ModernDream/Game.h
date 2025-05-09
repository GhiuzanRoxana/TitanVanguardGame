
#pragma once
#include "Map.h"
#include "Player.h"
#include <array>
#include <deque>
#include "Bullet.h"
#include <map>

class Game {
private:
    Map map;
    std::array<std::unique_ptr<Player>, 4> players;
    std::deque<Bullet> bullets;
    std::vector<std::pair<std::uint16_t, std::uint16_t>> updatedCells; 
    std::map<std::string, std::pair<std::uint16_t, std::uint16_t>> playerPositions;
    std::map<std::uint32_t, std::string> bulletToPlayerMap; 
    std::uint32_t bulletCounter = 0;  

public:
    Game() = default;
    Game(Map map, const std::map<std::string, std::unique_ptr<Player>>& sessionPlayers);

    const Map& GetMap() const;
    std::array<std::unique_ptr<Player>, 4>& GetPlayers();
    void DetermineWinner();
    void WinGame();
    void FinishSecond();
    void CheckAndApplyWeaponUpgrade();
    void GenerateMap(std::uint8_t numPlayers);  
    void ShootBullet(const Player& player);
    void UpdateBullets();
    const std::deque<Bullet>& GetBullets() const;
    const std::vector<std::pair<std::uint16_t, std::uint16_t>>& GetUpdatedCells() const;
    void ClearUpdatedCells();
    std::map<std::string, std::pair<std::uint16_t, std::uint16_t>> GetPlayerPositions() const;
    void UpdatePlayerPosition(const std::string& username, std::uint16_t x, std::uint16_t y);
};
