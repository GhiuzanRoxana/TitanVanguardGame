#include "Weapon.h"
#include "Bullet.h"

Weapon::Weapon(float speed) :waitingTime(waitingTime)
{}

void Weapon::Shoot()
{
	if (CanShoot())
	{
		lastShot = (float)std::time(nullptr);

	}
}

bool Weapon::CanShoot() const
{
	return (lastShot + waitingTime) <= (float)std::time(nullptr);
}

void Weapon::UpgradeWaitingTime(float reduction)
{
	waitingTime = std::max(1.0f, waitingTime - reduction);
	std::cout << std::format("Waiting time upgraded. New waiting time: {:.2f} seconds\n", waitingTime);
}

float Weapon::GetWaitingTime() const
{
	return waitingTime;
}

float Weapon::GetLastShot() const
{
	return lastShot;
}
