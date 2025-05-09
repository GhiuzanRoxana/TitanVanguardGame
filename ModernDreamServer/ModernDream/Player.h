#pragma once
#include <iostream>
#include <string>
#include <utility>
#include "Weapon.h"
#include "Map.h" 


class Player
{
public:
    Player() = default;
    Player(const std::string &name, std::unique_ptr<Weapon> weapon, std::pair<uint16_t, uint16_t> position);
    void SetDirection(char newDirection) { direction = newDirection; }
    char GetDirection() const;
    void ResetPosition();
    void Hit();
    int GetLifes() const;
    int GetPoints() const;
    std::pair<int, int> GetPosition() const;
    const std::string& GetName() const;
    int GetScore() const;
    void Movement(const Map& mapMatrix, char direction);
    Weapon& GetWeapon();
    void SetScore(int newScore);
    void SetPoints(int newPoints);
    bool IsSpeedBoostUsed() const;
    void SetSpeedBoostUsed(bool used);
    void SetPosition(std::pair<uint16_t, uint16_t> pos);
    bool IsEliminated() const;
    void SetInitialPosition(const std::pair<uint16_t, uint16_t>& position);
    void AddPoints(int points);
    void Eliminate();
    Player(const Player&) = delete;  
    Player& operator=(const Player&) = delete;
    Player(Player&&) = default;     
    Player& operator=(Player&&) = default;
    ~Player() = default;

private:
    std::string name;
    std::unique_ptr<Weapon> weapon;
    int points{ 0 };
    int lifes{ 3 };
    std::pair<uint16_t, uint16_t> position{ 0, 0 };
    std::pair<uint16_t, uint16_t> initialPosition{ 0, 0 };
    int score{ 0 };
    bool speedBoostUsed{ false };
    bool isEliminated{ false };
    char direction{ ' ' };

};

