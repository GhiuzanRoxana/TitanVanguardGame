
#include "Wall.h"

Wall::Wall(std::pair<std::uint16_t, std::uint16_t> position, WallType type, std::uint16_t durability, bool destructible, std::optional<Bomb> bomb)
    : position(std::move(position)),
    type(type),
    durability(durability),
    destructible(destructible),
    bomb(std::move(bomb))
{
}

WallType Wall::GetType() const noexcept {
    return type;
}

const std::pair<std::uint16_t, std::uint16_t>& Wall::GetPosition() const noexcept {
    return position;
}

std::uint16_t Wall::GetDurability() const noexcept {
    return durability;
}

bool Wall::GetDestructible() const noexcept {
    return destructible;
}

void Wall::ReduceDurability() noexcept {
    if (destructible && durability > 0) {
        --durability;
    }
}

bool Wall::IsDestructible() const noexcept {
    return destructible;
}

void Wall::Destroy() noexcept {
    if (destructible) {
        durability = 0;
    }
}

bool Wall::HasBomb() const noexcept {
    return bomb.has_value();
}

const std::optional<Bomb>& Wall::GetBomb() const noexcept {
    return bomb;
}
