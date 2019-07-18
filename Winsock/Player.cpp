#include "pch.h"
#include "Player.h"

#pragma warning(disable : 4996)

Player::Player()
{
}


Player::~Player()
{
}

//Handle the reading of the palyer save file and setting of the player information
void Player::login(char* name)
{
	char buffer[64]; //Used to read the file

	strcpy(buffer, "Character Save Files\\"); //Name of the folder that contains the save files
	strcat(buffer, name); //Contains the name of the character being logged in
	strcat(buffer, ".txt"); //Adds .txt to allow it open the .txt file - change this if other formatted files are to be used

	//Open the file containing the information for the logged in character
	FILE *ptr = fopen(buffer, "r");

	fscanf(ptr, "%s", buffer); //Read line from the file
	strcpy(playerStats.name, buffer); //Read in the name of the player

	fscanf(ptr, "%s", buffer); //Read line from the file
	playerStats.health = atoi(buffer); //Read in health stat

	fscanf(ptr, "%s", buffer); //Read line from the file
	player.points = atoi(buffer); //Read in defence stat

	playerStats.speed = 5.0; //Set the default speed of the player
	playerStats.x = 250.0; //Set the default starting x position of the player
	playerStats.y = 20.0; //Set the default starting y position of the player

	fclose(ptr); //Close the file
}

//Handles updating the player's hp and points
void Player::setPlayerInformation(char* eName, int hp, int points)
{
	strcpy(playerStats.name, eName); //Set name
	playerStats.health = hp; //Set health
	this->player.points = points; //Set points
}

//Handles updating the player's information
void Player::updatePlayerPosition(float x)
{
	//Y position isn't updated to the nature of the game, so the player can only move left and right
	playerStats.x += x; //Update the player's x position
}

//Handles grabbing the object containing the player's stats
stats Player::getPlayerStats()
{
	return playerStats; //return the player's stats object
}

//Handles return the player's points
int Player::getPoints()
{
	return player.points; //Return the player's points
}