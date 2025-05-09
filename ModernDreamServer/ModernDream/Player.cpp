#include "Player.h"
#include <conio.h>

Player::Player(const std::string& name, std::unique_ptr<Weapon> weapon, std::pair<uint16_t, uint16_t> position)
	: name(name), weapon(std::move(weapon)), position(position), initialPosition(position){
}
 char Player::GetDirection() const
{
	return direction;
}


void Player::Movement(const Map& mapMatrix, char direction) 
{
	int newX = position.first;
	int newY = position.second;
	this->direction = direction;

	auto updatePosition = [&](int dx, int dy) {
		newX += dx;
		newY += dy;
		};

	switch (direction) {
	case 'w': updatePosition(-1, 0); break;
	case 's': updatePosition(1, 0); break;
	case 'a': updatePosition(0, -1); break;
	case 'd': updatePosition(0, 1); break;
	default: std::cout << "Invalid direction!\n"; return;
	}

	if (newX >= 0 && newX < mapMatrix.GetHeight() && newY >= 0 && newY < mapMatrix.GetWidth()) {
		if (mapMatrix.IsMovable(newX, newY)) {
			position = { newX, newY };
			std::cout << "Player " << name << " moved to position ("
				<< position.first << ", " << position.second << ")\n";
		}
		else {
			std::cout << "Cannot move to wall or occupied space!\n";
		}
	}
	else {
		std::cout << "Cannot move outside map boundaries!\n";
	}
}


void Player::ResetPosition()
{
	std::cout << "[DEBUG] Resetting player " << GetName() << " to initial position ("
		<< initialPosition.first << ", " << initialPosition.second << ").\n";
	position = initialPosition;
}

void Player::Hit()
{

	if (lifes > 0)
	{
		--lifes;
		std::cout << "Player :" << name << "Was hit and lost a life.Lifes remaining:" << lifes << std::endl;

	}
	else
	{

		std::cout << "Game over for player" << name << "No more lifes " << std::endl;
		Eliminate();

	}
}

int Player::GetLifes() const
{
	return lifes;
}

int Player::GetPoints() const
{
	return points;
}

std::pair<int, int> Player::GetPosition() const 
{
	return position;
}


const std::string& Player::GetName() const
{
	return name;
}

int Player::GetScore() const
{
	return score;
}

void Player::SetScore(int newScore) 
{
	score = newScore;
}

void Player::SetPoints(int newPoints)
{
	points = newPoints;
}

bool Player::IsSpeedBoostUsed() const 
{
	return speedBoostUsed;
}

void Player::SetSpeedBoostUsed(bool used) 
{
	speedBoostUsed = used;
}

Weapon& Player::GetWeapon() 
{ 
	return *weapon; 
}

void Player::SetPosition(std::pair<uint16_t, uint16_t> pos) {
	position = pos; 
}


bool Player::IsEliminated() const {
	return isEliminated;
}

void Player::Eliminate() {
	isEliminated = true;
	std::cout << "Player " << name << " has been eliminated.\n";
}

void Player::SetInitialPosition(const std::pair<uint16_t, uint16_t>& position)
{
	initialPosition = position;
}

void Player::AddPoints(int points) 
{
	this->points += points;
	std::cout << "[DEBUG] Player " << GetName() << " gained " << points << " points. Total: " << score << "\n";
}