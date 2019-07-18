#include "pch.h"
#include "Login.h"

#pragma warning(disable : 4996)

Login::Login()
{
}


Login::~Login()
{
}

//Handles reading the list of character names to see if the character being logged into exists
bool Login::loginChecker(char* name)
{
	//Open the list of characters created
	FILE *ptr = fopen("Character_Files.txt", "r");

	char buffer[64]; //used to read in lines from the text file

	do
	{
		fscanf(ptr, "%s", buffer); //Read line from the file

		if (strcmp(buffer, name) == 0) //Check if the line being read matches the input character name
		{
			fclose(ptr); //Close the file
			return true; //Return true to say that the character exists
		}

	} while (!feof(ptr)); //Loop until end of the file has been found

	fclose(ptr); //Close the file
	return false; //return false to say that the character doesn't exist yet
}

//Handles the creation of a new character
void Login::characterCreator(char* name)
{
	char buffer[64]; //Used to read in lines from the text file

	strcpy(buffer, "Character Save Files\\"); //Folder where all the character saves are located
	strcat(buffer, name); //Name of the new character being created
	strcat(buffer, ".txt"); //.txt is used because the file format is a .txt - this can be altered if different file formats are to be used

	//Open the list of characters created to add the new character's name to
	FILE *ptr = fopen("Character_Files.txt", "a");

	fprintf(ptr, "\n%s", name); //Add the new character's name to the file

	fclose(ptr); //Close the file
	
	//Create a new file to save the new characters data to
	ptr = fopen(buffer, "w");

	//Set the information for the new player to the newly created file
	fprintf(ptr, "%s\n%d\n%d\n", name, 100, 0); //Name of the player, the health and the number of poinst

	fclose(ptr); //Close the file
}