#pragma once
#include "pch.h"
#include "Game.h"
#include "Player.h"
#include "Messages.h"
#include "PackUnpack.h"

#define DEFAULT_PORT "27015" //Holds the dafult port used by the client and the server
#define DEFAULT_BUFLEN 512 //Holds the maximum buffer length to be used

using namespace std;

class ClassUDPServer
{
private:
	//Packs the preamble message
	unsigned short PackPreamble(MESSAGE_PREAMBLE *preamb, unsigned char *buf)
	{
		PackShort(buf, preamb->code); //Pack the Preamble Code
		PackShort(buf, preamb->length); //Pack the Preamble Length
		return PREAMBLE_LENGTH;
	}

	//Unpacks the preamble message
	unsigned short UnpackPreamble(MESSAGE_PREAMBLE *preamb, unsigned char *buf)
	{
		UnpackShort(buf, preamb->code); //Unpack the Preamble Code
		UnpackShort(buf, preamb->length); //Unpack the Preamble Length
		return PREAMBLE_LENGTH;
	}

	//Packs the player information message
	unsigned short PackPlayer(PLAYER_INFORMATION *player, unsigned char *buf)
	{
		unsigned short msglen; //Holds the length of the message

		PackShort(buf, player->code); //Pack the player code
		PackBytes(buf, player->username, MAX_MESSAGE_CHAR_LENGTH);	//Pack the Player Name
		PackLong(buf, player->health);		//Pack the Player's Health
		PackBytes(buf, player->startX, MAX_MESSAGE_CHAR_LENGTH);	//Pack the Player's X Position
		PackBytes(buf, player->startY, MAX_MESSAGE_CHAR_LENGTH);	//Pack the Player's Y Position
		PackBytes(buf, player->speed, MAX_MESSAGE_CHAR_LENGTH);	//Pack the Player's speed
		PackShort(buf, player->points);	//Pack the Player's points

		//Calculate the max length of the message to be sent
		msglen = sizeof(unsigned short)
			+ sizeof(player->username)
			+ sizeof(unsigned short)
			+ sizeof(player->startX)
			+ sizeof(player->startY)
			+ sizeof(player->speed)
			+ sizeof(unsigned short);

		return msglen; //Return the length of the message
	}

	//Packs the enemy information message
	unsigned short PackEnemy(ENEMY_INFORMATION *enemy, unsigned char *buf)
	{
		unsigned short msglen; //Holds the length of the message

		PackShort(buf, enemy->code); //Pack the enemy code
		PackBytes(buf, enemy->username, MAX_MESSAGE_CHAR_LENGTH);	//Pack the enemy Name
		PackLong(buf, enemy->health);		//Pack the enemy's Health
		PackBytes(buf, enemy->startX, MAX_MESSAGE_CHAR_LENGTH);	//Pack the enemy's X Position
		PackBytes(buf, enemy->startY, MAX_MESSAGE_CHAR_LENGTH);	//Pack the enemy's Y Position
		PackBytes(buf, enemy->speed, MAX_MESSAGE_CHAR_LENGTH);	//Pack the enemy's speed

		//Calculate the max length of the message to be sent
		msglen = sizeof(unsigned short)
			+ sizeof(enemy->username)
			+ sizeof(unsigned short)
			+ sizeof(enemy->startX)
			+ sizeof(enemy->startY)
			+ sizeof(enemy->speed);

		return msglen; //Return the length of the message
	}

	//Unpacks the player movement message
	unsigned short UnpackMovement(PLAYER_MOVEMENT_INFORMATION *move, unsigned char *buf)
	{
		UnpackShort(buf, move->code); //Unpack the movement code
		UnpackBytes(buf, move->username, MAX_MESSAGE_CHAR_LENGTH); //Unpack the movement username
		UnpackBytes(buf, move->input, MAX_MESSAGE_CHAR_LENGTH); //Unpack the movement input
		
		return PREAMBLE_LENGTH;
	}

	//Packs the Bullet message
	unsigned short PackBullet(MESSAGE_BULLET_FIRED *bullet, unsigned char *buf)
	{
		unsigned short msglen;

		PackShort(buf, bullet->code); //Packs the Bullet Code
		PackBytes(buf, bullet->username, MAX_MESSAGE_CHAR_LENGTH); //Pack the Bullet Username
		PackBytes(buf, bullet->bulletX, MAX_MESSAGE_CHAR_LENGTH); //Pack the Bullet X
		PackBytes(buf, bullet->bulletY, MAX_MESSAGE_CHAR_LENGTH); //Pack the Bullet Y
		PackShort(buf, bullet->count); //Pack the Bullet Counter

		//Calculates the length of the message
		msglen = sizeof(unsigned short)
			+ sizeof(bullet->username)
			+ sizeof(bullet->bulletX)
			+ sizeof(bullet->bulletY)
			+ sizeof(unsigned short);

		return msglen; //Returns the length of the message
	}

	//Unpacks the bullet message
	void UnpackBullet(MESSAGE_BULLET_FIRED *bullet, unsigned char *buf)
	{
		UnpackShort(buf, bullet->code); //Unpack the Bullet Code
		UnpackBytes(buf, bullet->username, MAX_MESSAGE_CHAR_LENGTH); //Unpack the Bullet Username
		UnpackBytes(buf, bullet->bulletX, MAX_MESSAGE_CHAR_LENGTH); //Unpack the Bullet X
		UnpackBytes(buf, bullet->bulletY, MAX_MESSAGE_CHAR_LENGTH); //Unpack the Bullet Y
		UnpackShort(buf, bullet->count); //Unpack the Bullet Counter
	}

	WSADATA	wsaData;
	char	recvbuf[DEFAULT_BUFLEN];
	int		iResult;
	int		recvbuflen = DEFAULT_BUFLEN;
	SOCKET	UdpSocket = INVALID_SOCKET;

	struct addrinfo *addressList = NULL;
	struct addrinfo hints;
	int			iRcvdBytes = 0;
	sockaddr_in	rx_addr;
	sockaddr_in sender_addr;
	int			iSenderAddrSize = sizeof(sender_addr);

public:
	unsigned short _PackPreamble(MESSAGE_PREAMBLE*, unsigned char*); //Calls the PackPreamble function
	unsigned short _UnpackPreamble(MESSAGE_PREAMBLE*, unsigned char*); //Calls the UnpackPreamble function
	unsigned short _PackPlayer(PLAYER_INFORMATION*, unsigned char*); //Calls the PackPlayer function
	void _UnpackMovement(PLAYER_MOVEMENT_INFORMATION*, unsigned char*); //Calls the UnpackMovement function
	unsigned short _PackBullet(MESSAGE_BULLET_FIRED*, unsigned char*); //Calls the PackBullet function
	void _UnpackBullet(MESSAGE_BULLET_FIRED*, unsigned char*); //Calls the UnpackBullet function

	void udp(char*); //Udp startup

	void sendBulletInfo(int, char*, float, float, int); //Handles the information to be send for the bullet
	void sendPlayerInfo(int, char*, int, float, float, float, int); //Handles the information to be sent for the player
	void sendEnemyInfo(int, char*, int, float, float, float); //Handles the information to be sent for the enemy
	void sendPreamble(int, int); //Handles the information to be sent for the preamble

	void sendMessage(unsigned char*, int); //Handles the sending of messages to the client

	char* recvMessage(); //Handles receiving messages

	void clearAdr(); //Clears the address
	void closeUDP(); //Closes UDP

	ClassUDPServer();
	~ClassUDPServer();
};

