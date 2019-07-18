#pragma once

#define MAX_MESSAGE_CHAR_LENGTH 20 //Max length of chars in the messages

//Used to send and recieve the preamble messages
typedef struct
{
	unsigned short code;	//Type of message
	unsigned short length;	//Length of message
}MESSAGE_PREAMBLE;

#define PREAMBLE_LENGTH 5

//Code values
#define MESSAGE_CODE_PLAYER_UPDATE 1 //Code that corresponds to the player update
#define MESSAGE_CODE_ENEMY_UPDATE 2 //Code that corresponds to the enemy update
#define MESSAGE_CODE_BULLET_FIRED 3 //Code that corresponds to the bullet fired
#define MESSAGE_CODE_PLAYER_MOVE 4 //Code that corresponds to the player move

//Used to send and recieve information about the bullet
typedef struct
{
	int code; //Type of message - Not required as the preamble is used for this
	char username[MAX_MESSAGE_CHAR_LENGTH]; //player or enemy name
	char bulletX[MAX_MESSAGE_CHAR_LENGTH]; //bullet x position
	char bulletY[MAX_MESSAGE_CHAR_LENGTH]; //bullet y position
	int count; //bullet counter
}MESSAGE_BULLET_FIRED;

//Used to send and recieve information about the player
typedef struct
{
	int code; //Type of message - Not required as the preamble is used for this
	char username[MAX_MESSAGE_CHAR_LENGTH]; //player name
	int health; //player health
	char startX[MAX_MESSAGE_CHAR_LENGTH]; //player x position
	char startY[MAX_MESSAGE_CHAR_LENGTH]; //player y position
	char speed[MAX_MESSAGE_CHAR_LENGTH]; //player speed
	int points; //player points
	int dataLen; //message data length
}PLAYER_INFORMATION;

//Used to send and recieve information about the movement of the player
typedef struct
{
	int code; //Type of message - Not required as the preamble is used for this
	char username[MAX_MESSAGE_CHAR_LENGTH]; //player name
	char input[MAX_MESSAGE_CHAR_LENGTH]; //user input
}PLAYER_MOVEMENT_INFORMATION;

//Used to send and recieve information about the enemy
typedef struct
{
	int code; //Type of message - Not required as the preamble is used for this
	char username[MAX_MESSAGE_CHAR_LENGTH]; //enemy name
	int health; //enemy health
	char startX[MAX_MESSAGE_CHAR_LENGTH]; //enemy x position
	char startY[MAX_MESSAGE_CHAR_LENGTH]; //enemy y position
	char speed[MAX_MESSAGE_CHAR_LENGTH]; //enemy speed
	int dataLen; //message data length
}ENEMY_INFORMATION;