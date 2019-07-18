#pragma once
#include "pch.h"

class Login
{
private:

public:
	//Handles checking if the character exists
	bool loginChecker(char*);

	//Handles creating the character
	void characterCreator(char*);

	Login();
	~Login();
};

