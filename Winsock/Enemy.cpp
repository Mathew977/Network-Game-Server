#include "pch.h"
#include "Enemy.h"

#pragma warning(disable : 4996)

Enemy::Enemy()
{
	enemyStats.speed = 1.0; //Set the default speed of the enemy - to be changed later
}

Enemy::~Enemy()
{
}

//Handles setting the information of the enemy
void Enemy::setEnemyInformation(char* eName, int hp, float x, float y, float speed)
{
	strcpy(enemyStats.name, eName); //Set the enemy's name
	enemyStats.health = hp; //Set the enemy's health
	this->enemyStats.x = x; //Set the enemy's x position
	this->enemyStats.y = y; //Set the enemy's y position
	this->enemyStats.speed = speed; //Set the enemy's speed
}

//Handles updating the enemy's position
void Enemy::updateEnemyPosition(float x)
{
	//y position isn't altered for the same reason as the player - refer to the player.cpp for more information
	enemyStats.x = x; //Update the enemy's x position
}

//Return the enemy object
stats Enemy::getEnemyStats()
{
	return enemyStats; //Return the object containing the enemy's information
}