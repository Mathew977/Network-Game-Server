#include "pch.h"
#include "ClassTCPServer.h"

#pragma warning(disable : 4996)

ClassTCPServer::ClassTCPServer()
{
}

ClassTCPServer::~ClassTCPServer()
{
}

//Handles starting up the tcp server
char* ClassTCPServer::tcp()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; //Passive opens
	getaddrinfo(NULL, DEFAULT_PORT, &hints, &addressList);

	//Create a socket for connecting to the server
	listenSocket = socket(addressList->ai_family, addressList->ai_socktype, addressList->ai_protocol);

	BOOL optionValue = TRUE;
	int optionLength = sizeof(BOOL);
	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optionValue, optionLength);

	//Set socket to listen on the port number provided (DEFAULT_PORT)
	bind(listenSocket, addressList->ai_addr, (int)addressList->ai_addrlen);

	freeaddrinfo(addressList);

	listen(listenSocket, SOMAXCONN);

	char characterName[DEFAULT_BUFLEN]; //Holds the player's name

	char sendbuf[DEFAULT_BUFLEN]; //Holds the message being sent

	bool leaveTCP = false; //Determines if the server needs to stop running the tcp server and switch to the udp server

	//Loop forever
	while(1)
	{
		char* recvbuf = recvMessage(); //Recieves the message from the client
		char* last_letter = &recvbuf[strlen(recvbuf) - 1]; //Grabs the last character from the recieved message

		int messageNumber = atoi(last_letter); //Converts the last character to an integer

		//Reacts differently to the client based on the messageNumber
		switch (messageNumber)
		{
		case 1:
		{
			//Sends the message to the client telling the player to input a character name
			strcpy(sendbuf, "Please Enter a Character's name\n");

			sendMessage(sendbuf);

			break;
		}
		case 2:
		{
			//Checks if the character exists
			characterCheck(characterName, sendbuf, 0);

			break;
		}
		case 3:
		{
			//Handles the player's input regarding whether they want to create a character or not
			characterCheck(characterName, sendbuf, 1);

			break;
		}
		case 4:
		{
			//Sets leaveTCP to true or false depending on the player's selection in the menu
			leaveTCP = menu(sendbuf);

			break;
		}
		case 5:
		{
			//Handles sending the menu to the client
			sendMenu(sendbuf);

			break;
		}
		default:
			break;
		}

		messageNumber = 0; //Rest the messageNumber

		if (leaveTCP)
		{
			closesocket(listenSocket); //Server no longer required

			WSACleanup();

			return characterName; //Return the logged in character's name
		}

	}
}

//Handles sending messages from the server to the client
void ClassTCPServer::sendMessage(char* msg)
{
	//Send the msg to the client
	iResult = send(clientSocket, msg, (int)strlen(msg) + 1, 0);

	shutdown(clientSocket, SD_SEND); //Forces send
	closesocket(clientSocket);
}

//Handles receiving messages from the client
char* ClassTCPServer::recvMessage()
{
	int iRecvdBytes = 0;

	printf("Listening for incoming connections\n");

	//BLOCK waiting for somebody to talk to me!!
	clientSocket = accept(listenSocket, NULL, NULL);

	//received the message from the client and set it to recvbuf
	iResult = recv(clientSocket, recvbuf, recvbuflen, 0);

	//Check if the message was recieved
	if (iResult > 0)
	{
		//iResult is the number of bytes received
		//Assumes NUL char is included in recvbuf
		//to terminate the string
		printf("Bytes received from client: %d msg: %s\n", iResult, recvbuf);
		iRecvdBytes = iResult;

	}
	//Check if the client disconnected
	else if (iResult == 0)
	{
		printf("Client closing connection\n");
		strcpy(recvbuf, "Client Closed");
	}
	//Check if the message failed to be received
	else
	{
		printf("recv failed: %d\n", WSAGetLastError());
		strcpy(recvbuf, "Error");
		WSACleanup();
	}

	return recvbuf; //Return the received message
}

//Handles the character checker and the character creator
void ClassTCPServer::characterCheck(char* characterName, char* sendbuf, int loginState)
{
	int i;

	//Check if the player just tried to log in with a character that doesn't exist
	//loginState = 0 if input character does not exist
	//loginState = 1 if the character is selecting whether they want to create a new character or not
	if (loginState == 0)
	{
		bool characterExists = false; //Determines if the character to be logged into exists

		//Loop through the recvbuf
		for (i = 0; i < (int)strlen(recvbuf) - 1; i++)
		{
			characterName[i] = recvbuf[i]; //Set characterName to be the same as the recvbuf except for the last character
		}

		characterName[i] = '\0'; //Add the terminating character to the characterName

		characterExists = character.loginChecker(characterName); //Pass the name of the character into the loginChecker function which will return true if the character exists and false if the character doesn't

		//Check if the character was determined to exist
		if (!characterExists)
		{
			strcpy(sendbuf, "Character does not exist\nWould you like to create a new character with the given name? (y/n)\n");

			sendMessage(sendbuf); //Send the character does not exist message to the client
		}
		else
		{
			player.login(characterName); //Login to the character for the player

			sendMenu(sendbuf); //Go to the sendMenu function with the senbuf
		}
	}
	else
	{
		char tempMessage[DEFAULT_BUFLEN];

		//Loop for the number of character in the recvbuf
		for (i = 0; i < (int)strlen(recvbuf) - 1; i++)
		{
			tempMessage[i] = recvbuf[i]; //Set tempMessage to be the same as the recvbuf except for the last character
		}

		tempMessage[i] = '\0'; //Add the terminating character to the tempMessage

		//Check if the message recieved was a y - means the user wants to create a new character
		if (strcmp(tempMessage, "y") == 0 || strcmp(tempMessage, "Y") == 0)
		{
			character.characterCreator(characterName); //Send the name of the new character to the characterCreator function which handles creating the character
		}

		strcpy(sendbuf, "Reset");

		sendMessage(sendbuf); //Send the message "Reset" to the client, which tells the client to loop back to the beginning of the tcp section
	}
}

//Handles sending the menu to the client
void ClassTCPServer::sendMenu(char* sendbuf)
{
	strcpy(sendbuf, "What would you like to do?\n1. PVE\n2. PVP\n3. Profile\n4. Leaderboard\n5. Exit\n4");
	sendMessage(sendbuf); //Send the menu to the client
}

//Handles the menu inputs from the client
bool ClassTCPServer::menu(char* sendbuf)
{
	int i;
	char tempMessage[DEFAULT_BUFLEN];
	bool leaveTCP = false; //Tells the server if it should exit the tcp server

	//Loop through for the number of characters in recvbuf
	for (i = 0; i < (int)strlen(recvbuf) - 1; i++)
	{
		tempMessage[i] = recvbuf[i]; //Copy over the recvbuf to the tempMessage except for the last character
	}

	tempMessage[i] = '\0'; //Add the terminating character to tempMessage

	int result = atoi(tempMessage); //Convert the char* to an integer

	//Check which option was recieved from the client
	switch (result)
	{
	case 1:
	{
		//Send a message to the client to tell it to stop running the tcp client and begin the udp client and game
		strcpy(sendbuf, "Start Game");
		sendMessage(sendbuf);

		leaveTCP = true; //Set leaveTCP to true as it now needs to switch over to udp

		break;
	}
	case 2:
	{
		//Send the client a message about the PVP option
		strcpy(sendbuf, "PVP - Will be implemented in the future");
		sendMessage(sendbuf);
		break;
	}
	case 3:
	{
		//Send the client a message about the character profile option
		strcpy(sendbuf, "Character Profile Will Go Here");
		sendMessage(sendbuf);
		break;
	}
	case 4:
	{
		sendbuf = game.getLeadboard();
		//Send the leaderboard to the client
		strcat(sendbuf, "\nSend anything to return to the menu\n");
		sendMessage(sendbuf);
		break;
	}
	case 5:
	{
		//Handles exiting the game
		strcpy(sendbuf, "Exit Game");
		sendMessage(sendbuf);
		break;
	}
	default:
		break;
	}

	return leaveTCP; //Leave tcp if true
}