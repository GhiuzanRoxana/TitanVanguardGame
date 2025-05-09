#pragma once
#include <iostream>

struct DataUser
{
	std::string username;
	int score = 0;
	int points = 0;
	bool canDoubleBulletSpeed = false;
	int canReduceReloadTime = -1;
};

