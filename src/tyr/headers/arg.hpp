#ifndef __ARG__
#define __ARG__

#include <functional>
#include <string>

#include "arg_flags.hpp"

namespace hyronx
{

class Argument
{
public:
	std::string short_arg;
	std::string long_arg;
	std::string command;
	std::string data_info;		// Info for the user what to input (myapp.exe [-t|--test|test <data_info>])
	std::string description;
	std::string long_description;
	std::string example;
	std::function<void(std::string)> func;
	ArgumentFlags flags;
};

}

#endif // !__ARG__

