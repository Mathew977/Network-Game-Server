#include "pch.h"
#include "ClassTCPServer.h"
#include "ClassUDPServer.h"
#include "Messages.h"
#include "PackUnpack.h"
#include "Enemy.h"
#include "Bullet.h"
#include "windows.h"

#pragma warning(disable : 4996)

#define ENEMYRADIUS 5 //Enemy's Radius
#define BULLETRADIUS 2 //Bullet's Radius
#define PLAYERRADIUS 5 //Player's Radius

ClassTCPServer tcp;
ClassUDPServer udp;
Player player;
Entity entity;
Enemy enemy;
Bullet bullet;

void setup(); //Does the initialisation of the game
void mainGame(); //Runs the main game loop on the server
void update(); //Handles the recieving of messages from the client
void enemyShoot(); //Handles the enemy's shooting

char name[DEFAULT_BUFLEN]; //Stores the Players Name
char enemyName[DEFAULT_BUFLEN]; //Stores the Enemy's Name

int enemyBulletCount = 0; //Holds the next bullet to fire for the enemy
int enemyBulCount = 0; //Holds the number of bullets fired for the enemy

int bulletCount = 0; //Holds the next bullet to fire for the player
int bulCount = 0; //Holds the number of bullets fired for the player

int terminate_app = 0;

bool finishGame = false;

#define MAXTHREADS 2 //number of Extra threads being used

LPCTSTR mutexname = (LPCTSTR)"FirstMutex";

//Thread handles the enemy's shooting
DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
	DWORD param = (DWORD)lpParameter; // Recover passed in value
	int index;
	HANDLE  mutexhdl;

	//Open the mutexhdl
	mutexhdl = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexname);

	//Loop until the game is finished
	while (!finishGame)
	{
		//Check if the enemy can fire any bullets yet
		if (enemyBulletCount != BULLETCOUNT)
		{
			WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used
			enemyShoot(); //Call the enemyShoot function which performs all the calculations for the enemy's bullets
			ReleaseMutex(mutexhdl); //Release the mutex
		}

		Sleep(1000); //Wait for 1 second so the enemy only shoots every second
	}

	return 0;
}

//Thread handles the movement of the enemy and all the bullets
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	DWORD param = (DWORD)lpParameter; // Recover passed in value
	int index;

	HANDLE  mutexhdl;

	//Open the mutexhdl
	mutexhdl = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexname);

	//Loop until the game is finished
	while (!finishGame)
	{
		WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used

		//Check if the enemy is close to the end of the scene in the x axis
		//If he is then make him go the other way by multiplying his speed by
		//-1
		if (entity.getX(enemy.getEnemyStats()) >= 480 || entity.getX(enemy.getEnemyStats()) <= 20)
			enemy.setEnemyInformation(enemyName, entity.getHealth(enemy.getEnemyStats()), entity.getX(enemy.getEnemyStats()), entity.getY(enemy.getEnemyStats()), entity.getSpeed(enemy.getEnemyStats()) * -1);

		//Update the enemy's x position by adding his speed to his x position
		enemy.updateEnemyPosition(entity.getX(enemy.getEnemyStats()) + entity.getSpeed(enemy.getEnemyStats()));

		//Send all the information for the enemy to the sendEnemyInfo function
		//in the udp server class that will send the information to the client
		udp.sendEnemyInfo(MESSAGE_CODE_ENEMY_UPDATE, enemyName, //Enemy message code and enemy name
			entity.getHealth(enemy.getEnemyStats()), //Enemy Health
			entity.getX(enemy.getEnemyStats()), //Enemy x position
			entity.getY(enemy.getEnemyStats()), //Enemy y position
			entity.getSpeed(enemy.getEnemyStats())); //Enemy speed
		ReleaseMutex(mutexhdl); //Release the mutex

		//Loop through for the total number of bullets
		for (int i = 0; i < BULLETCOUNT; i++)
		{
			//WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used
			bullet.removeBullet(i); //Handles the removing of bullets
			//ReleaseMutex(mutexhdl); //Release the mutex
		}

		//Check if a bullet has been fired by the player yet
		if (bulCount > 0)
		{
			//Loop through for the number of bullets the player has fired (max = 10)
			for (int i = 0; i < bulCount; i++)
			{
				//WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used
				bullet.moveBullet(i); //Handles the movement of the player's bullets
				//ReleaseMutex(mutexhdl); //Release the mutex
			}
		}

		//Check if a bullet has been fired by the enemy yet
		if (enemyBulCount > 0)
		{
			//Loop through for the number of bullets the enemy has fired (max = 10)
			for (int i = 0; i < enemyBulCount; i++)
			{
				//WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used
				bullet.moveEnemyBullet(i); //Handles the movement of the enemy's bullets
				//ReleaseMutex(mutexhdl); //Release the mutex
			}
		}

		//Check if the player has fired a bullet yet
		if (bulCount != 0)
		{
			//Loop through for the nubmer of bullets the player has created in the scene
			for (int i = 0; i < bulCount; i++)
			{
				WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used
				udp.sendBulletInfo(MESSAGE_CODE_BULLET_FIRED, name, //Bullet message code and the name of the player who made it
					entity.getX(bullet.getBulletStats(i)), //Bullet's x position
					entity.getY(bullet.getBulletStats(i)), //Bullet's y position
					i); //Bullet count - Keeps track of which bullet is being mentioned
				ReleaseMutex(mutexhdl); //Release the mutex
			}

			//Loop through for the nubmer of bullets the player has created in the scene
			for (int j = 0; j < bulCount; j++)
			{
				//Calculate the distance between the enemy and all the bullets
				float distance = ((entity.getX(bullet.getBulletStats(j)) - entity.getX(enemy.getEnemyStats())) * (entity.getX(bullet.getBulletStats(j)) - entity.getX(enemy.getEnemyStats()))) + ((entity.getY(bullet.getBulletStats(j)) - entity.getY(enemy.getEnemyStats()) * (entity.getY(bullet.getBulletStats(j)) - entity.getY(enemy.getEnemyStats()))));
				
				//Check if the distance is less than the combined radiuses of the enemy and bullet
				if (distance < (ENEMYRADIUS + BULLETRADIUS) * (ENEMYRADIUS + BULLETRADIUS))
				{
					WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used
					
					//Update the player's points by increasing it by 1 for hitting the enemy with a bullet
					player.setPlayerInformation(name, entity.getHealth(player.getPlayerStats()), player.getPoints() + 1);

					//Send the updated player information
					udp.sendPlayerInfo(MESSAGE_CODE_PLAYER_UPDATE, name,//Player message code and player name
						entity.getHealth(player.getPlayerStats()), //Player Health
						entity.getX(player.getPlayerStats()), //Player x position
						entity.getY(player.getPlayerStats()), //Player Y position
						entity.getSpeed(player.getPlayerStats()), //Player speed
						player.getPoints()); //Player Points
					//finishGame = true; //Set the finishGame variable to true
					ReleaseMutex(mutexhdl); //Release the mutex
				}
			}

			//Loop through for the nubmer of bullets the player has created in the scene
			for (int j = 0; j < enemyBulCount; j++)
			{
				//Calculate the distance between the enemy and all the bullets
				float distance = ((entity.getX(bullet.getEnemyBulletStats(j)) - entity.getX(player.getPlayerStats())) * (entity.getX(bullet.getEnemyBulletStats(j)) - entity.getX(player.getPlayerStats()))) + ((entity.getY(bullet.getEnemyBulletStats(j)) - entity.getY(player.getPlayerStats()) * (entity.getY(bullet.getEnemyBulletStats(j)) - entity.getY(player.getPlayerStats()))));

				//Check if the distance is less than the combined radiuses of the enemy and bullet
				if (distance < (ENEMYRADIUS + BULLETRADIUS) * (ENEMYRADIUS + BULLETRADIUS))
				{
					WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used

					//Update the player's points by subtracting it by 1 for being hit by the enemy's bullet
					player.setPlayerInformation(name, entity.getHealth(player.getPlayerStats()), player.getPoints() - 1);

					//Send the updated player information
					udp.sendPlayerInfo(MESSAGE_CODE_PLAYER_UPDATE, name,//Player message code and player name
						entity.getHealth(player.getPlayerStats()), //Player Health
						entity.getX(player.getPlayerStats()), //Player x position
						entity.getY(player.getPlayerStats()), //Player Y position
						entity.getSpeed(player.getPlayerStats()), //Player speed
						player.getPoints()); //Player Points
					//finishGame = true; //Set the finishGame variable to true
					ReleaseMutex(mutexhdl); //Release the mutex
				}
			}
		}

		//Check if the enemy has fired a bullet yet
		if (enemyBulCount != 0)
		{
			//Loop through for the nubmer of bullets the enemy has created in the scene
			for (int i = 0; i < enemyBulCount; i++)
			{
				//WaitForSingleObject(mutexhdl, INFINITE); //Wait until the mutex can be used
				udp.sendBulletInfo(MESSAGE_CODE_BULLET_FIRED, enemyName, //Bullet message code and the name of the enemy who made it
					entity.getX(bullet.getEnemyBulletStats(i)), //Enemy's bullet x position
					entity.getY(bullet.getEnemyBulletStats(i)), //Enemy's bullet y position
					i); //Enemy bullet count - keeps track of which bullet is being mentioned
				//ReleaseMutex(mutexhdl); //Release the mutex
			}
		}

		
		if (terminate_app)
		{
			printf("[%x] Thread terminating", GetCurrentThreadId());
			break;
		}
	}

	return 0;
}

//Handles the creation of the enemy's bullets
void enemyShoot()
{
	int dirX = 1.0; //Determines the x direction of the enemy bullet
	int dirY = 1.0; //Determines the y direction of the enemy bullet

	//Calculates the difference between the x positions of the player and the enemy and adds it to the xSpeed
	float xSpeed = entity.getX(player.getPlayerStats()) - entity.getX(enemy.getEnemyStats());
	//Calculates the difference between the y positions of the player and the enemy and adds it to the ySpeed
	float ySpeed = entity.getY(player.getPlayerStats()) - entity.getY(enemy.getEnemyStats());

	//Check if the xSpeed is negative
	if (xSpeed < 0)
	{
		dirX *= -1; //Reverse the preset direction that the bullet moves in the x
		xSpeed *= dirX; //Make the xSpeed positive (This is to find the total xSpeed and ySpeed later on)
	}
	//Check if the ySpeed is negative
	if (ySpeed < 0)
	{
		dirY *= -1; //Reverse the preset direction that the bullet moves in the y
		ySpeed *= dirY; //Make the ySpeed positive (This is to find the total xSpeed and ySpeed later on)
	}

	//Stores the total of the combined xSpeed and ySpeed
	int totalSpeed;

	//Calculate the total combined differences to normalise the speeds below
	totalSpeed = xSpeed + ySpeed;

	xSpeed /= totalSpeed; //Make the xSpeed between 0 and 1

	xSpeed *= dirX; //Multiply the xSpeed by the x direction to set it back to a negative if it was changed earlier

	ySpeed /= totalSpeed; //Make the ySpeed between 0 and 1

	ySpeed *= dirY; //Multiply the ySpeed by the y direction to set it back to a negative if it was changed earlier

	//Call the setEnemyBulletArray to set the new calculated values to the newly created bullet 
	bullet.setEnemyBulletArray(entity.getX(enemy.getEnemyStats()), entity.getY(enemy.getEnemyStats()), xSpeed, ySpeed);

	enemyBulCount++; //Increment the enemyBulCount by 1
	enemyBulletCount++; //Increment the enemyBulletCount by 1

	//Check if the enemy's bullet that was just fired was bullet number 9
	if (enemyBulletCount == BULLETCOUNT)
		enemyBulletCount = 0; //Set the next bullet to be fired to be bullet number 0

	//Check if the enemy has fired 10 bullets yet
	if (enemyBulCount >= BULLETCOUNT)
		enemyBulCount = BULLETCOUNT; //Set the enemyBulCount back to the max number of bullets the enemy can fire
}

//Overview of all the steps in the program for the server
int main(int argc, char* argv[])
{
	strcpy(name, tcp.tcp()); //Runs the tcp server and returns the name of the player that logged in
	udp.udp(name); //Run the udp server and passes in the name of the player

	player.login(name); //Call the function that logs the player in and sets the starting information for the player

	setup(); //Initialises all the values for the game and sends them to the client

	mainGame(); //Enter the main game loop on the server side

	udp.closeUDP(); //Closes the udp server

	return 0; //Return 0
}

//Handles initialising all the values for the game and sending them to the client
void setup()
{
	//Recieves the preamble from the client and sets it to recvdbuf
	unsigned char* recvdbuf = (unsigned char*)udp.recvMessage();

	MESSAGE_PREAMBLE  preMsg; //Will hold the unpacked information from the preamble

	//Unpack the preamble from the recvdbuf into the preMsg
	udp._UnpackPreamble(&preMsg, recvdbuf);

	printf("\nPreamble Code: %d\n", preMsg.code);

	//Pass all the player information that has been pre set in the player.login function 
	//Into the sendPlayerInfo function in the udp class that handles sending the player information to the client
	udp.sendPlayerInfo(MESSAGE_CODE_PLAYER_UPDATE, name,//Player message code and player name
		entity.getHealth(player.getPlayerStats()), //Player Health
		entity.getX(player.getPlayerStats()), //Player x position
		entity.getY(player.getPlayerStats()), //Player Y position
		entity.getSpeed(player.getPlayerStats()), //Player speed
		player.getPoints()); //Player Points

	strcpy(enemyName, "BadGuy"); //Set the enemy's name to "BadGuy"

	//Set all the initial values for the enemy
	enemy.setEnemyInformation(enemyName, //Sets the name of the enemy
		entity.getHealth(player.getPlayerStats()), //Set the health of the enemy
		entity.getX(player.getPlayerStats()), //Set the x position of the enemy to be the same as the player
		//Set the y position of the enemy to be the same as the player subtracted from the max height of the 
		//scene to put the enemy on the opposite side of the screen from the player
		SCREEN_HEIGHT -  entity.getY(player.getPlayerStats()), 
		1.0); //Set the base value of the enemy's speed

	//Send the enemy information to the client
	udp.sendEnemyInfo(MESSAGE_CODE_ENEMY_UPDATE, enemyName, //Send the code for the enemy update and the enemy's name
		entity.getHealth(enemy.getEnemyStats()), //Send the health of the enemy
		entity.getX(enemy.getEnemyStats()), //Send the x position of the enemy
		entity.getY(enemy.getEnemyStats()), //Send the y position of the enemy
		entity.getSpeed(enemy.getEnemyStats())); //Send the speed of the enemy
}

//Creates and starts the extra threads being used and runs the game loop that takes user inputs from the client
void mainGame()
{
	HANDLE  threadhdl[MAXTHREADS];
	DWORD   threadid;
	int     index = 0;
	DWORD	param = 2;  // Some value to pass to thread
	HANDLE  mutexhdl;

	//Create the mutex mutexhdl
	mutexhdl = CreateMutex(NULL, FALSE, mutexname);

	//Create the first extra thread
	threadhdl[0] = CreateThread(
		NULL,               // No security attributes reqd
		0,                  // Use default thread stack size
		ThreadProc,         // Thread function
		(LPVOID)param,      // Pass parameter to function
		0,                  // Creation flags (e.g. CREATE_SUSPENDED)
		&threadid);         // Return value of thread ID

	//Create the second extra thread
	threadhdl[1] = CreateThread(
		NULL,               // No security attributes reqd
		0,                  // Use default thread stack size
		ThreadProc2,         // Thread function
		(LPVOID)param,      // Pass parameter to function
		0,                  // Creation flags (e.g. CREATE_SUSPENDED)
		&threadid);         // Return value of thread ID

	//Loop while the game hasn't ended
	while (!finishGame)
	{
		update(); //Handles the recieving of messages from the client, in this case the user inputs
	}

	//TerminateThread(threadhdl, 0); // Kill thread
	terminate_app = 1;

#if MAXTHREADS > 1
	WaitForMultipleObjects(MAXTHREADS,
		threadhdl,
		TRUE,
		INFINITE);
#else
	WaitForSingleObject(threadhdl[0], INFINITE);
#endif // MAXTHREADS > 1

	//Loops through the number of threads used to close them
	for (index = 0; index < MAXTHREADS; index++)
	{
		CloseHandle(threadhdl[index]); //Close all the threads
	}

	CloseHandle(mutexhdl); //CLoses the mutex created earlier

	getchar(); //Pause

}

//Handles the recieving of any messages and the updating of some information
void update()
{
	//Recieves the preamble from the client and sets it to recvdbuf
	unsigned char* recvdbuf = (unsigned char*)udp.recvMessage();

	MESSAGE_PREAMBLE preMsg; //Will hold the unpacked information from the preamble

	//Unpack the preamble from the recvdbuf into the preMsg
	udp._UnpackPreamble(&preMsg, recvdbuf);

	printf("\nPreamble Code: %d\n", preMsg.code);

	//Run different updates and unpack the message in the way that corresponds to the code in the preamble
	switch (preMsg.code)
	{
	case MESSAGE_CODE_PLAYER_UPDATE: //Enters if the preamble's code matches the player update code
	{
		//PLAYER_INFORMATION playerMsg;

		//unsigned char* recvdbuf = (unsigned char*)udp.recvMessage();

		//udp._UnpackPlayer(&playerMsg, recvdbuf);

		//printf("Message Recieved:\n Code: %d\n Character: %s\n Health: %d\n X Position: %f\n Y Position: %f\n Speed: %f\n Points: %d\n",
		//	playerMsg.code, playerMsg.username, playerMsg.health, atof(playerMsg.startX),
		//	atof(playerMsg.startY), atof(playerMsg.speed), playerMsg.points);

		//player.setPlayerInformation(name, playerMsg.health, atof(playerMsg.startX), atof(playerMsg.startY), atof(playerMsg.speed), playerMsg.points);

		break;
	}
	case MESSAGE_CODE_ENEMY_UPDATE: //Enters if the preamble's code matches the enemy update code
	{
		//ENEMY_INFORMATION enemyMsg;
		//unsigned char* recvdbuf = (unsigned char*)udp.recvMessage();

		//udp._UnpackEnemy(&enemyMsg, recvdbuf);

		//printf("Message Recieved:\n Code: %d\n Character: %s\n Health: %d\n X Position: %f\n Y Position: %f\n Speed: %f\n",
		//	enemyMsg.code, enemyMsg.username, enemyMsg.health, atof(enemyMsg.startX),
		//	atof(enemyMsg.startY), atof(enemyMsg.speed));

		//strcpy(enemyName, enemyMsg.username);

		//enemy.setEnemyInformation(enemyName, enemyMsg.health, atof(enemyMsg.startX), atof(enemyMsg.startY), atof(enemyMsg.speed));

		//endInit = true;
		break;
	}
	case MESSAGE_CODE_BULLET_FIRED: //Enters if the preamble's code matches the bullet fired code
	{
		MESSAGE_BULLET_FIRED bulletMsg; //Will hold the unpacked information from the bullet fired message

		//Recieves the bullet fired message from the client and sets it to the recvdbuf
		unsigned char* recvdbuf = (unsigned char*)udp.recvMessage();

		//Unpack the recvdbuf into the bulletMsg
		udp._UnpackBullet(&bulletMsg, recvdbuf);

		//Print out all the recieved information from the bullet message
		printf("Message Recieved:\n Code: %d\n Character: %s\n Bullet X Position: %f\n Bullet Y Position: %f\n Bullet Number: %d\n",
			bulletMsg.code, bulletMsg.username, atof(bulletMsg.bulletX), atof(bulletMsg.bulletY), bulletMsg.count);

		int dirX = 1.0; //Determines the x direction of the player bullet
		int dirY = 1.0; //Determines the y direction of the player bullet

		//Set the starting y position of the player's bullet - 
		//(The glut positions y = 0 is in the bottom of the window displayed in the client, but the mouse click y = 0 is at the top of the window displayed in the client,
		//This is the reason for subtracting the mouse's click y position from the screen height)
		int changeY = (SCREEN_HEIGHT - atoi(bulletMsg.bulletY));

		//Calculate the distance between the mouses click in the x that was sent from the client in the bulletMsg and the player's x position
		float xSpeed = atoi(bulletMsg.bulletX) - entity.getX(player.getPlayerStats());

		//Calculate the distance between the changeY calculated above and the player's y position
		float ySpeed = changeY - entity.getY(player.getPlayerStats());

		//Check if the distance in the x is negative
		if (xSpeed < 0)
		{
			dirX *= -1; //reverse the x direction of the bullet
			xSpeed *= dirX; //Set the xSpeed to a positive value
		}
		//Check if the distance in the y is negative
		if (ySpeed < 0)
		{
			dirY *= -1; //reverse the y direction of the bullet
			ySpeed *= dirY; //Set the ySpeed to a positive value
		}

		int totalSpeed; //Will hold the total directions added together

		totalSpeed = xSpeed + ySpeed; //Add the difference in the x and y together

		xSpeed /= totalSpeed; //Make the xSpeed's value between 0 and 1

		xSpeed *= dirX; //Set the xSpeed back to a negative if it was changed earlier

		ySpeed /= totalSpeed; //Make the ySpeed's value between 0 and 1

		ySpeed *= dirY; //Set the ySpeed back to a negative if it was changed earlier

		//Set the bullet values
		bullet.setBulletArray(entity.getX(player.getPlayerStats()), entity.getY(player.getPlayerStats()), xSpeed, ySpeed);

		//Send the bullet information to the client
		udp.sendBulletInfo(MESSAGE_CODE_BULLET_FIRED, name, //Send the bullet fired code and the name of the person who fired it
			entity.getX(bullet.getBulletStats(bulletMsg.count)), //Send the x position of the bullet
			entity.getY(bullet.getBulletStats(bulletMsg.count)), //Send the y position of the bullet
			bulletMsg.count); //Send the number corresponding to the bullet being fired

		bulletCount++; //Increment the bulletCount by 1
		bulCount++; //Increment the bulCount by 1

		//Check if the last bullet fired was bullet 9
		if (bulletCount >= BULLETCOUNT)
			bulletCount = 0; //Set the next bullet to be fired to be bullet 0
		//Check if the bulCount is above or equal to the max number of bullets
		if (bulCount >= BULLETCOUNT)
			bulCount = BULLETCOUNT; //Set the bulCount to be equal to the max number of bullets

		break;
	}
	case MESSAGE_CODE_PLAYER_MOVE: //Enter if the preamble code matched the player move code
	{
		PLAYER_MOVEMENT_INFORMATION moveMsg; //Will hold the unpacked movement information

		//Recieves the player movement message from the client and sets it to the recvdbuf
		unsigned char* recvdbuf = (unsigned char*)udp.recvMessage();
		float move = 0.0; //Used to store the value to change the player's position by
		
		//Unpack the movement message into moveMsg
		udp._UnpackMovement(&moveMsg, recvdbuf);

		printf("Message Recieved:\n Code: %d\n Character: %s\n Input: %s\n",
			moveMsg.code, moveMsg.username, moveMsg.input);

		//Check if the player's x position is less than 480 as the player shouldn't move to the right past that point
		if (entity.getX(player.getPlayerStats()) < 480)
		{
			//Check if the message recieved contains "d" or "D"
			if (strcmp(moveMsg.input, "d") == 0 || strcmp(moveMsg.input, "D") == 0)
				move += entity.getSpeed(player.getPlayerStats()); //Add the player's speed to the move variable to be used later
		}

		//Check if the player's x position is greater than 20 as the player shouldn't move to the left past that point
		if (entity.getX(player.getPlayerStats()) > 20)
		{
			//Check if the message recieved contains "a" or "A"
			if (strcmp(moveMsg.input, "a") == 0 || strcmp(moveMsg.input, "A") == 0)
				move -= entity.getSpeed(player.getPlayerStats()); //Subtract the player's speed from the move variable to be used later
		}

		//Call the function to update the player's position using the move value determined above
		player.updatePlayerPosition(move);

		//Send the updated player information to the client
		udp.sendPlayerInfo(MESSAGE_CODE_PLAYER_UPDATE, name, //Send the player move code and the player's name
			entity.getHealth(player.getPlayerStats()), //Send the player's health
			entity.getX(player.getPlayerStats()), //Send the player's x position
			entity.getY(player.getPlayerStats()), //Send the player's y position
			entity.getSpeed(player.getPlayerStats()), //Send the player's speed
			player.getPoints()); //Send the player's points
		break;
	}
	default:
		break;
	}
}