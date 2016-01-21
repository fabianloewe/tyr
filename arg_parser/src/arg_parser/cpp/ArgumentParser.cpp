/* 
 * File:   ArgumentParser.cpp
 * Author: HERON
 * 
 * Created on 20. Januar 2016, 10:13
 */

#include "../headers/ArgumentParser.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>
#include <locale>

using namespace hybros;

ArgumentParser::ArgumentParser(std::string exec_name) :
	args(),
	parsed()
{
	addBaseArgs(std::move(exec_name));
}

ArgumentParser::ArgumentParser(std::vector<Argument> &args_v, std::string exec_name) :
	args(args_v.begin(), args_v.end()),
	parsed()
{
	addBaseArgs(std::move(exec_name));

	args.insert(args.end(), args_v.begin(), args_v.end());

}

ArgumentParser::ArgumentParser(const ArgumentParser& orig) : 
	args(orig.args),
	parsed(orig.parsed)
{
}

ArgumentParser::~ArgumentParser() 
{
	args.clear();
	parsed.clear();
}

void ArgumentParser::add(std::string s_arg, std::string l_arg, std::string cmd, std::string desc, std::string ex, std::function<void(std::string &)> func, ArgumentFlags flags)
{
    Argument arg;
    arg.short_arg = s_arg;
    arg.long_arg = l_arg;
    arg.command = cmd;
    arg.description = desc;
    arg.example = ex;
	arg.func = func;
	arg.flags = flags;

	// If one of the arguments is empty disable it (!ENABLE_*)
	arg.flags[0] = (arg.short_arg.empty()) ? false : true;
	arg.flags[1] = (arg.long_arg.empty()) ? false : true;
	arg.flags[2] = (arg.command.empty()) ? false : true;

    args.push_back(arg);
}

void ArgumentParser::add(Argument &arg)
{
	arg.flags[0] = (arg.short_arg.empty()) ? false : true;
	arg.flags[1] = (arg.long_arg.empty()) ? false : true;
	arg.flags[2] = (arg.command.empty()) ? false : true;

    args.push_back(arg);
}

void ArgumentParser::add(std::vector<Argument> &args_v)
{
    args.reserve(args_v.size());
    args.insert(args.end(), args_v.begin(), args_v.end());
}

void ArgumentParser::remove(std::string matching_str)
{
    std::remove_if(args.begin(), args.end(), [&](Argument &arg) -> bool
	{
		return compareArgs(arg, matching_str);
	});
}

void ArgumentParser::remove(Argument &arg)
{
	// If compare() returns true remove
	std::remove_if(args.begin(), args.end(), [&](Argument &current_arg) -> bool
	{
		return compareArgs(current_arg, arg);
	});
}

void ArgumentParser::remove(std::vector<Argument> &args_v)
{
	// Set iterators to first position
	auto other_iter = args_v.begin();
	auto iter = args.begin();

	// Loop through the private Argument vector
	while(iter != args.end())
	{
		iter = std::remove_if(args.begin(), args.end(), [&](Argument &arg) -> bool
		{
			// Compare the current element of 'args' with next element of 'args_v'
			if(compareArgs(arg, *other_iter))
			{
				// If both are equal remove the current and compare the next element of 'args_v' next time
				other_iter++;
				return true;
			}
			else
				return false;
		});
	}
}

auto ArgumentParser::get(std::string match_str) const -> std::string
{
	auto iter = std::find_if(parsed.begin(), parsed.end(), [&](const std::tuple<arg_iter, std::string> &current_res)
	{
		arg_iter arg = std::get<0>(current_res);
		if(arg->command == match_str)
			return true;
		else if(arg->long_arg == match_str)
			return true;
		else if(arg->short_arg == match_str)
			return true;
		else
			return false;
	});
	return std::get<1>(*iter);
}

void ArgumentParser::parse(int argc, char **argv)
{
	saveExecName(argv[0]);

    for(unsigned int i = 0; i < argc; i++)
    {
		std::string arg_str = argv[i];
		auto iter = std::find_if(args.begin(), args.end(), [&](Argument &arg)
		{
			if(arg.command == arg_str)
				return true;
			else if(arg.long_arg == arg_str)
				return true;
			else if(arg.short_arg == arg_str)
				return true;
			else
				return false;
		});

		if(iter != args.end())
		{
			if(iter->flags.hasUserData())
			{
				std::string user_data = argv[++i];
				parsed.push_back(std::tuple<arg_iter, std::string>(iter, user_data));
			}
			else
				parsed.push_back(std::tuple<arg_iter, std::string>(iter, ""));
		}
    }
}

auto ArgumentParser::loop(int argc, char **argv) -> int
{
	parse(argc, argv);

	std::string output = exec_name;
	output.append(" > ");

	std::string input;	

	bool exit = false;
	while(!exit)
	{
		std::cout << output;

		std::string cmdline;
		std::getline(std::cin, cmdline);

		parseAndRun(cmdline);
	}

	return 0;
}

auto ArgumentParser::compareArgs(Argument &arg, std::string &str) const -> bool
{
	if(arg.command == str)
		return true;
	else if(arg.short_arg == str)
		return true;
	else if(arg.long_arg == str)
		return true;
	else if(arg.description == str)
		return true;
	else if(arg.example == str)
		return true;
	else 
		return false;
}

auto ArgumentParser::compareArgs(Argument &arg, Argument &other_arg) const -> bool
{
	if(arg.command != other_arg.command)
		return false;
	else if(arg.short_arg != other_arg.short_arg)
		return false;
	else if(arg.long_arg != other_arg.long_arg)
		return false;
	else if(arg.description != other_arg.description)
		return false;
	else if(arg.example != other_arg.example)
		return false;
	else if(arg.flags != other_arg.flags)
		return false;
	else
		return true;
}

void ArgumentParser::addBaseArgs(std::string &&prog_name)
{
	exec_path = prog_name;
	exec_name = exec_path.substr(exec_path.find_last_of("\\") + 1, exec_path.size());

	Argument help_arg;
	help_arg.short_arg = "-h";
	help_arg.long_arg = "--help";
	help_arg.command = "help";
	help_arg.description = "Shows information for registered commands";
	help_arg.example = exec_name + " help";
	help_arg.flags.enableShortArg(true);
	help_arg.flags.enableLongArg(true);
	help_arg.flags.enableCommand(true);
	help_arg.flags.needsUserData(false);
	help_arg.func = [&](std::string &)
	{
		std::cout << "Help:\n";
		for(Argument arg : args)
		{
			std::string info = "  ";
			bool first = true;

			if(arg.flags.hasShortArg())
			{
				info += arg.short_arg;
				first = false;
			}

			if(!first)
				info += ", ";

			if(arg.flags.hasLongArg())
			{
				info += arg.long_arg; 
				first = false;
			}

			if(!first)
				info += ", ";

			if(arg.flags.hasCommand())
			{
				info += arg.command;
				first = false;
			}

			if(!arg.description.empty())
				info += "         " + arg.description + "\n";

			std::cout << info << "    " << arg.example << std::endl;
		}
	};
	args.push_back(help_arg);

	Argument exit_arg;
	exit_arg.command = "exit";
	exit_arg.description = "Exits this application";
	exit_arg.example = exec_name + " > exit";
	exit_arg.flags.enableCommand(true);
	exit_arg.func = [&](std::string &)
	{
		exit(0);
	};
	args.push_back(exit_arg);
}

void ArgumentParser::saveExecName(std::string name)
{
	if(exec_path.empty())
		exec_path = name;

	if(exec_name.empty())
		exec_name = exec_path.substr(exec_path.find_last_of("\\") + 1, exec_path.size());

	auto &help_arg = args.front();
	help_arg.example = exec_name + " " + help_arg.command;
}

void ArgumentParser::parseAndRun(std::string cmdline)
{
	std::vector<std::string> commands;
	std::copy(std::istream_iterator<std::string>(std::stringstream(cmdline)), std::istream_iterator<std::string>(), std::back_inserter(commands));

	for(auto iter = commands.begin(); iter != commands.end(); iter++)
	{
		auto found_iter = std::find_if(args.begin(), args.end(), [&](Argument &current_arg)
		{
			if(current_arg.command == *iter)
				return true;
			else if(current_arg.long_arg == *iter)
				return true;
			else if(current_arg.short_arg == *iter)
				return true;
			else
				return false;
		});

		if(found_iter != args.end())
		{
			std::string user_data;
			if(found_iter->flags.hasUserData())
				user_data = *++iter;

			found_iter->func(user_data);
		}
	}
}
