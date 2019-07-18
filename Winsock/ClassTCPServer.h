#pragma once

#define _CRT_SECURE_NO_WARNINGS  1

#include "pch.h"
#include "Game.h"
#include "Login.h"
#include "Player.h"

using namespace std;

#define DEFAULT_PORT "27015" //Holds the dafult port used by the client and the server
#define DEFAULT_BUFLEN 512 //Holds the maximum buffer length to be used

class ClassTCPServer
{
private:
	WSADATA	wsaData;
	char	recvbuf[DEFAULT_BUFLEN];
	int		iResult;
	int		recvbuflen = DEFAULT_BUFLEN;

	struct addrinfo *addressList = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	Game game;

	Login character;

	Player player;
public:

	char* tcp(); //TCP startup
	void sendMessage(char*); //Handles sending messages
	char* recvMessage(); //Handles recieving messages
	void characterCheck(char*, char*, int); //Handles checking if the character exists
	
	void sendMenu(char*); //Handles sending the menu to the client
	bool menu(char*); //Handles the menu inputs from the client
	ClassTCPServer();
	~ClassTCPServer();
};

