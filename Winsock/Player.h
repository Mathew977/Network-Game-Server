#pragma once
#include "Entity.h"
#include "pch.h"

class Player
{
private:
	//Store any stats that only the player will have
	struct playerSpecificStats
	{
		int points; //Holds onto the number of points the player has
	};

	stats playerStats; //Has the player's stats
	playerSpecificStats player; //Hs the player only stats

public:

	//Sets the player's initial values
	void login(char*);

	//Sets the player information
	void setPlayerInformation(char*, int, int);

	//Update the player's information
	void updatePlayerPosition(float);

	stats getPlayerStats();

	int getPoints();

	Player();
	~Player();
};

