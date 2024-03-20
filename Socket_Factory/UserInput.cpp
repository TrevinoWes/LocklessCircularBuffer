#include "UserInput.h"
#include <limits>

std::string UserInput::requestString(std::string question) {
	std::string response;
	bool valid = false;
	while (!valid) {
		std::cout << question << std::endl;
		std::cin >> response;
		if (std::cin.fail()) {
			std::cout << "Invalid response" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else {
			valid = true;
		}
	}

	return response;

	return response;
};
int UserInput::requestInt(std::string question) {
	int response;
	bool valid = false;
	while (!valid) {
		std::cout << question << std::endl;
		std::cin >> response;
		if (std::cin.fail()) {
			std::cout << "Invalid response" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else {
			valid = true;
		}
	}

	return response;
};