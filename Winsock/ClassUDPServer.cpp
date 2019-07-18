#include "pch.h"
#include "ClassUDPServer.h"
#include "Packing.h"

#pragma warning(disable : 4996)

ClassUDPServer::ClassUDPServer()
{
}

ClassUDPServer::~ClassUDPServer()
{
}

//Handles calling the PackPreamble
unsigned short ClassUDPServer::_PackPreamble(MESSAGE_PREAMBLE *preamb, unsigned char *buf)
{
	return(PackPreamble(preamb, buf));
}

//Handles calling the UnpackPreamble
unsigned short ClassUDPServer::_UnpackPreamble(MESSAGE_PREAMBLE *preamb, unsigned char *buf)
{
	return(UnpackPreamble(preamb, buf));
}

//Handles calling the PackPlayer
unsigned short ClassUDPServer::_PackPlayer(PLAYER_INFORMATION *player, unsigned char *buf)
{
	return(PackPlayer(player, buf));
}

//Handles calling the UnpackMovement
void ClassUDPServer::_UnpackMovement(PLAYER_MOVEMENT_INFORMATION *move, unsigned char *buf)
{
	UnpackMovement(move, buf);
}

//Handles calling the PackBullet
unsigned short ClassUDPServer::_PackBullet(MESSAGE_BULLET_FIRED *bullet, unsigned char *buf)
{
	return(PackBullet(bullet, buf));
}

//Handles calling the UnpackBullet
void ClassUDPServer::_UnpackBullet(MESSAGE_BULLET_FIRED *bullet, unsigned char *buf)
{
	UnpackBullet(bullet, buf);
}

//Handles setting up the udp server
void ClassUDPServer::udp(char* character)
{
	//Do this only once at startup
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
	}

	UdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (UdpSocket == INVALID_SOCKET)
	{
		printf("socket failed: %ld\n", WSAGetLastError());
		WSACleanup();
	}

	rx_addr.sin_family = AF_INET;
	rx_addr.sin_port = htons((u_short)atol(DEFAULT_PORT));
	rx_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//Bind the server's udp socket
	iResult = bind(UdpSocket, (SOCKADDR *)&rx_addr, sizeof(rx_addr));

	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed: %d\n", WSAGetLastError());
		
		closeUDP();
	}
}

//Handles sending bullet information to the client
void ClassUDPServer::sendBulletInfo(int code, char* user, float x, float y, int count)
{
	unsigned char sendbuf[DEFAULT_BUFLEN]; //The message being sent to the client - information being sent will be packed into this unsigned char*
	unsigned short buflen; //Length of the message being sent

	MESSAGE_BULLET_FIRED msg; //Will contain all the bullet information to be sent to the client

	//Fill the msg with the information passed into the function
	msg.code = code; //Code of the msg being sent
	strcpy((char*)msg.username, user); //Username of the one making use of the message e.g. player's name or enemy's name
	snprintf(msg.bulletX, MAX_MESSAGE_CHAR_LENGTH, "%f", x); //Add the x position float for the bullet as a char* to the bulletX
	snprintf(msg.bulletY, MAX_MESSAGE_CHAR_LENGTH, "%f", y); //Add the y position float for the bullet as a char* to the bulletY
	msg.count = count; //Add the count, which is used to tell the client and server what bullet it is

	//Pack the message structure into the send buffer.
	buflen = PackBullet(&msg, (unsigned char*)sendbuf);

	sendPreamble(code, buflen); //Send the code and the length of the message to the sendPreamble message to send to the client

	sendMessage(sendbuf, (int)buflen); //Send the packed message and the length of the message ot the sendMessage function where the message is sent to the client
}

//Handles sending the player information to the client
void ClassUDPServer::sendPlayerInfo(int code, char* user, int hp, float x, float y, float speed, int points)
{
	unsigned char sendbuf[DEFAULT_BUFLEN]; //The message being sent to the client - information being sent will be packed into this unsigned char*
	unsigned short buflen; //Length of the message being sent

	PLAYER_INFORMATION msg; //Will contain all the player information to be sent to the client

	//Fill the msg with the information passed into the function
	msg.code = code; //Code of the msg being sent           
	strcpy((char*)msg.username, user); //Username of the player
	msg.health = hp; //Health of the player
	snprintf(msg.startX, MAX_MESSAGE_CHAR_LENGTH, "%f", x); //Add the x position float for the player as a char* to the message
	snprintf(msg.startY, MAX_MESSAGE_CHAR_LENGTH, "%f", y); //Add the y position float for the player as a char* to the message
	snprintf(msg.speed, MAX_MESSAGE_CHAR_LENGTH, "%f", speed); //Add the speed float for the player as a char* to the message
	msg.points = points; //Add the points of the player to the message

	//Pack the message structure into the send buffer.
	buflen = PackPlayer(&msg, (unsigned char*)sendbuf);

	sendPreamble(code, buflen); //Send the code of the message and the length of the message being sent to the sendPreamble function to be sent to the client

	sendMessage(sendbuf, (int)buflen); //Pass the packed messaged and the length of the message to the sendMessage function that will handle sending the packed message to the client
}

//Handles sending the enemy information to the client
void ClassUDPServer::sendEnemyInfo(int code, char* user, int hp, float x, float y, float speed)
{
	unsigned char sendbuf[DEFAULT_BUFLEN]; //The message being sent to the client = information being sent will be packed into this unsigned char*
	unsigned short buflen; //Length of the message being sent

	ENEMY_INFORMATION msg; //Will contain all the enemy inforamtion to be sent to the client

	//Fill the msg with the information passed into the function
	msg.code = code; //Code of the msg being sent
	strcpy((char*)msg.username, user); //Username of the enemy
	msg.health = hp; //Health of the enemy
	snprintf(msg.startX, MAX_MESSAGE_CHAR_LENGTH, "%f", x); //Add the x position float for the enemy as a char* to the message
	snprintf(msg.startY, MAX_MESSAGE_CHAR_LENGTH, "%f", y); //Add the y position float for the enemy as a char* to the message
	snprintf(msg.speed, MAX_MESSAGE_CHAR_LENGTH, "%f", speed); //Add the speed float for the enemy as a char* to the message

	//Pack the message structure into the send buffer.
	buflen = PackEnemy(&msg, (unsigned char*)sendbuf);

	sendPreamble(code, buflen); //Send the code of the message and the length of the message being sent to the sendPreamble function to be sent to the client

	sendMessage(sendbuf, (int)buflen); //Pass the packed messaged and the length of the message to the sendMessage function that will handle sending the packed message to the client
}

//Handles sending the preamble information to the client
void ClassUDPServer::sendPreamble(int code, int buflen)
{
	unsigned char presendbuf[DEFAULT_BUFLEN]; //The message being sent to the client = information being sent will be packed into this unsigned char*
	unsigned short prebuflen; //Length of the message being sent

	MESSAGE_PREAMBLE preMsg; //Will contain all the preamble information to be sent to the client

	//Fill the preMsg with information passed into the function
	preMsg.code = code; //Code of the next message to be sent to the client
	preMsg.length = buflen; //Length of the next message to be sent to the client

	//Pack the message structure into the presendbuf
	prebuflen = (PackPreamble(&preMsg, (unsigned char*)presendbuf));

	sendMessage(presendbuf, (int)prebuflen); //Pass the packed messaged and the length of the message to the sendMessage function that will handle sending the packed message to the client
}

//Handles sending the packed messages to the client
void ClassUDPServer::sendMessage(unsigned char* sendbuf, int buflen)
{
	//Send the packed message to the client
	iResult = sendto(UdpSocket, (char *)sendbuf, buflen, 0, (SOCKADDR *)&sender_addr, iSenderAddrSize);

	//Check if the message failed to send
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed: %d\n", WSAGetLastError());
		freeaddrinfo(addressList);

		closeUDP(); //Close UDP server
	}
}

//Handles recieving the packed messages from the client
char* ClassUDPServer::recvMessage()
{
	//Recieve the packed message from the client
	iResult = recvfrom(UdpSocket, recvbuf, recvbuflen, 0, (SOCKADDR *)&sender_addr, &iSenderAddrSize);

	//Check if the server failed to recieve the message from the client
	if (iResult == SOCKET_ERROR)
	{
		printf("recv failed: %d\n", WSAGetLastError());
		closeUDP(); //Close UDP server
	}

	return recvbuf; //Return the recieved message
}

void ClassUDPServer::clearAdr()
{
	freeaddrinfo(addressList); //Avoid memory leak
}

void ClassUDPServer::closeUDP()
{
	//Cleanup
	closesocket(UdpSocket);

	//Do this only once at the end
	WSACleanup();
}