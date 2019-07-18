#include "pch.h"
#include "Bullet.h"

Bullet::Bullet()
{
	//Loop through for the max number of bullets
	for (int i = 0; i < BULLETCOUNT; i++)
	{
		//Set all the bullets to be offscreen
		offScreen[i] = true;
		enemyOffScreen[i] = true;

		//Set the start player bullets information
		bulletStats[i].x = -255;
		bulletStats[i].y = -255;
		bullet[i].xSpeed = 0;
		bullet[i].ySpeed = 0;

		//Set the start enemy bullets information
		enemyBulletStats[i].x = -255;
		enemyBulletStats[i].y = -255;
		enemyBullet[i].xSpeed = 0;
		enemyBullet[i].ySpeed = 0;
	}
}

Bullet::~Bullet()
{
}

//Set the player bullets
void Bullet::setBulletArray(float x, float y, float xSpeed, float ySpeed)
{
	//Loop through for the max number of bullets
	for (int i = 0; i < BULLETCOUNT; i++)
	{
		//Check if the bullet being looked at is off screen as you don't want to teleport the bullets back to the player
		if (offScreen[i])
		{
			//Set the bullet's information
			bulletStats[i].x = x;
			bulletStats[i].y = y;
			bullet[i].xSpeed = xSpeed * bulletStats[i].speed;
			bullet[i].ySpeed = ySpeed * bulletStats[i].speed;

			offScreen[i] = false; //Make the bullet appear on screen
			bulletCounter++; //Increment the bulletCounter by 1

			return;
		}
	}
}

void Bullet::setEnemyBulletArray(float x, float y, float xSpeed, float ySpeed)
{
	//Loop through for the max number of bullets
	for (int i = 0; i < BULLETCOUNT; i++)
	{
		//Check if the bullet being looked at is off screen as you don't want to teleport the bullets back to the enemy
		if (enemyOffScreen[i])
		{
			//Set the bullet's information
			enemyBulletStats[i].x = x;
			enemyBulletStats[i].y = y;
			enemyBullet[i].xSpeed = xSpeed * enemyBulletStats[i].speed;
			enemyBullet[i].ySpeed = ySpeed * enemyBulletStats[i].speed;

			enemyOffScreen[i] = false; //Make the bullet appear on screen
			enemyBulletCounter++; //Increment the bulletCounter by 1

			return;
		}
	}
}

//Handles moving the player's bullets
void Bullet::moveBullet(int count)
{
	//Check if the bullet is on screen as we don't have to move the ones off screen
	if (!offScreen[count])
	{
		bulletStats[count].x += bullet[count].xSpeed;
		bulletStats[count].y += bullet[count].ySpeed;
	}
}

//Handles moving the enemy's bullets
void Bullet::moveEnemyBullet(int count)
{
	//Check if the bullet is on screen as we don't have to move the ones off screen
	if (!enemyOffScreen[count])
	{
		enemyBulletStats[count].x += enemyBullet[count].xSpeed;
		enemyBulletStats[count].y += enemyBullet[count].ySpeed;
	}
}

//Handles setting the bullets to appear off screen to the server
void Bullet::removeBullet(int count)
{
	//Check if the player bullets have left the scene
	if (bulletStats[count].x > SCREEN_WIDTH || bulletStats[count].x < 0)
		offScreen[count] = true;
	if (bulletStats[count].y > SCREEN_HEIGHT || bulletStats[count].y < 0)
		offScreen[count] = true;

	//Check if the enemy bullets have left the scene
	if (enemyBulletStats[count].x > SCREEN_WIDTH || enemyBulletStats[count].x < 0)
		enemyOffScreen[count] = true;
	if (enemyBulletStats[count].y > SCREEN_HEIGHT || enemyBulletStats[count].y < 0)
		enemyOffScreen[count] = true;
}

//return the stats of the player's bullets
stats Bullet::getBulletStats(int count)
{
	return bulletStats[count]; //Return all the player bullet's information
}

//return the stats of the enemy's bullets
stats Bullet::getEnemyBulletStats(int count)
{
	return enemyBulletStats[count]; //Return all the enemy bullet's information
}