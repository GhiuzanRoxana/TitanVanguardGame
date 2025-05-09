#include "Bullet.h"


Bullet::Bullet(const Coordinates& position, char direction) : position(position), direction(direction) {}

void Bullet::SetIsInactive()
{
	isActive = false;
}
void Bullet::SetIsActive()
{
	isActive = true;
}
void Bullet::SetPosition(const Coordinates &pos)
{
	position = pos;
}

float Bullet::GetSpeed() const
{
	return speed;
}

Coordinates Bullet::GetPosition() const
{
	return position;
}

bool Bullet::GetIsActive() const
{
	return isActive;
}

void Bullet::SetDoubleSpeed()
{
	speed *= 2;
}

void Bullet::Movement(std::uint16_t maxHeight, std::uint16_t maxWidth) {
	if (isActive == true)
	{
		switch (direction) 
		{
		case 'w': position.first -= 1; break; 
		case 's': position.first += 1; break;  
		case 'a': position.second -= 1; break; 
		case 'd': position.second += 1; break; 
		}
	}
	std::cout << std::format("New coordinates: ({}, {})\n", position.first, position.second);


	if (position.first < 0 || position.second < 0 || position.first >= maxHeight || position.second >= maxWidth) 
	{
		isActive = false;  
		std::cout << "Bullet is out of bounds\n\n";
	}
}

char Bullet::GetDirection() const
{
	return direction;
}
