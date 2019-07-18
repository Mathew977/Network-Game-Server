#pragma once
#include "Entity.h"
#include "pch.h"

class Enemy
{
private:
	stats enemyStats; //Contains all the stats of the enemy
public:

	//Sets the enemy information
	void setEnemyInformation(char*, int, float, float, float);

	//Update the player's values
	void updateEnemyPosition(float);

	stats getEnemyStats();

	Enemy();
	~Enemy();
};

