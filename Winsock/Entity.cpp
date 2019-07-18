#include "pch.h"
#include "Entity.h"

#pragma warning(disable : 4996)

Entity::Entity()
{
}

Entity::~Entity()
{
}

int Entity::getHealth(stats stat)
{
	return stat.health; //Return the health of the entity requesting it
}

float Entity::getX(stats stat)
{
	return stat.x; //Return the x position of the entity requesting it
}

float Entity::getY(stats stat)
{
	return stat.y; //Return the y position of the entity requesting it
}

float Entity::getSpeed(stats stat)
{
	return stat.speed; //Return the speed of the entity requesting it
}