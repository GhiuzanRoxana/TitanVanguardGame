#pragma once
#include <cstdint>
#include <optional>
#include <utility>
import bomb;

enum  WallType : std::uint8_t
{
    DestructibleWall = 2,
    DestructibleWallWithBomb = 3,
    NonDestructibleWall = 4
};

class Wall {

public:
    Wall(std::pair<std::uint16_t, std::uint16_t> position, WallType type, std::uint16_t durability, bool destructible, std::optional<Bomb> bomb = std::nullopt);

    WallType GetType() const noexcept;
    const std::pair<std::uint16_t, std::uint16_t>& GetPosition() const noexcept;
    std::uint16_t GetDurability() const noexcept;
    bool GetDestructible() const noexcept;
    void ReduceDurability() noexcept;
    bool IsDestructible() const noexcept;
    void Destroy() noexcept;

    bool HasBomb() const noexcept;
    const std::optional<Bomb>& GetBomb() const noexcept;
private:
    std::pair<std::uint16_t, std::uint16_t> position;
    WallType type;
    std::uint16_t durability;
    bool destructible;
    std::optional<Bomb> bomb;
};