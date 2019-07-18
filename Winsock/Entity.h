#pragma once
#include "pch.h"

//Base stats of all entities in the game
struct stats
{
	char name[64]; //The name of the controlling character eg. Player or Enemy
	int health = 100; //Health of the player or enemy
	float x = 0.0; //x position of the entity
	float y = 0.0; //y position of the enetity
	float speed = 3.0; //speed of the entity
};

class Entity
{
public:

	int getHealth(stats); //Returns the health
	float getX(stats); //returns the x position
	float getY(stats); //returns the y position
	float getSpeed(stats); //returns the speed

	Entity();
	~Entity();
};


