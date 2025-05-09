#pragma once
#include <iostream>
#include <vector>
#include <string>

class Weapon
{

public:
	Weapon() = default;
	Weapon(float waitingTime);
	void Shoot();
	bool CanShoot() const;
	void UpgradeWaitingTime(float reduction);
	float GetWaitingTime() const;
	float GetLastShot() const;

private:
	float waitingTime{ 4.0f };
	float lastShot{ 0.0f };
};
