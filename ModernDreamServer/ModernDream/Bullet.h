#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include <memory>
#include <algorithm>
#include <format>


using Coordinates = std::pair<std::uint16_t, std::uint16_t>;

class Bullet
{

public:
	Bullet() = default;
	Bullet(const Coordinates& position, char direction);
	void SetIsInactive();
	void SetIsActive();
	void SetPosition(const Coordinates &pos);
	float GetSpeed() const;
	Coordinates GetPosition() const;
	bool GetIsActive() const;
	void SetDoubleSpeed();
	void Movement(std::uint16_t maxHeight, std::uint16_t maxWidth);
	char GetDirection() const;

private:
	Coordinates position{ 0,0 };
	float speed{ 0.25f };
	bool isActive{ true };
	char direction{ ' ' };
};
