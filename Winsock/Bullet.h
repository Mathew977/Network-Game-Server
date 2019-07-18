#pragma once
#define BULLETCOUNT 10 //Set the max number of bullets
#define SCREEN_HEIGHT 500 //Set the max screen height being rendered on the client
#define SCREEN_WIDTH 500 //Set the max screen width being rendered on the client
#include "Entity.h"

class Bullet
{
private:
	//Holds the information for the bullet that only the bullet uses
	struct bulletSpecificStats
	{
		float xSpeed; //Speed of the bullet in the x 
		float ySpeed; //Speed of the bullet in the y
	};

	bulletSpecificStats bullet[BULLETCOUNT]; //Stores the information that only the player bullet uses
	stats enemyBulletStats[BULLETCOUNT]; //Stores an array of information for the player's bullets

	bulletSpecificStats enemyBullet[BULLETCOUNT]; //Stores the information that only the enemy bullet uses
	stats bulletStats[BULLETCOUNT]; //Stores an array of information for the enemy's bullets

	int bulletCounter = 0; //Holds the current number of player bullets
	int enemyBulletCounter = 0; //Holds the current number of enemy bullets

	bool offScreen[BULLETCOUNT]; //Used to say if the player bullets are on screen or not
	bool enemyOffScreen[BULLETCOUNT]; //Used to say if the enemy bullets are on screen or not

public:

	void setBulletArray(float, float, float, float); //Set the player bullets
	void setEnemyBulletArray(float, float, float, float); //Set the enemy bullets

	void moveBullet(int); //Move the player bullets
	void moveEnemyBullet(int); //Move the enemy bullets

	void removeBullet(int); //Remove the bullets that go off screen
	
	stats getBulletStats(int); //Return the object containing all the information of the player's bullets
	stats getEnemyBulletStats(int); //Return the object containing all the information of the enemy's bullets

	Bullet();
	~Bullet();
};

