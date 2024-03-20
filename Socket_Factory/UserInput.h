#pragma once
#include <map>
#include <string>
#include <iostream>

class UserInput {
public:
	template< template <typename, typename, typename...> class Container, class t, class m, typename ... Ts >
	auto requestOption(const Container<t, m, Ts...>& options) {
		std::cout << "Select an option below:" << std::endl;
		std::map<int, t> intToVal;
		int i = 0;
		for (auto iter = options.begin(); iter != options.end(); ++iter, ++i) {
			intToVal[i] = iter->first;
			std::cout << i << ": " << iter->first << std::endl;
		}

		return options.at(intToVal[i]);
	};

	std::string requestString(std::string  question);
	int requestInt(std::string question);
};