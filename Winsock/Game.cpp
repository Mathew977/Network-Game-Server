#include "pch.h"
#include "Game.h"

#pragma warning(disable : 4996)
Game::Game()
{
}


Game::~Game()
{
}

//Handles opening the leaderboard file to show the user
char* Game::getLeadboard()
{
	//Open the leaderboard file
	FILE *ptr = fopen("Leaderboard.txt", "r");

	char* buffer; //Used for storing the leaderboard

	buffer = new char[DEFAULT_BUFLEN];

	char tempBuffer[124]; //Used to read the file

	strcpy(buffer, "LEADERBOARD:");

	do
	{
		strcat(buffer, "\n"); //Add \n to the buffer - this is used to make the output later on more presentable for the user
		fscanf(ptr, "%s", tempBuffer); //Read the next line from the file

		strcat(buffer, tempBuffer); //Add the line read from the file to the end of the buffer

	} while (!feof(ptr)); //Loop until end of the file has been found

	fclose(ptr); //Close the file

	return buffer; //Return the buffer to be sent to the leaderboard later
}