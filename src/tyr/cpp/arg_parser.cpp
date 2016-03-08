#include "../headers/arg_parser.hpp"
#include "../headers/arg_exception.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <locale>
#include <future>

using namespace tyr;

ArgumentParser::ArgumentParser(std::string exec_name) noexcept :
	args(),
	parsed()
{
	addBaseArgs(std::move(exec_name));
}

ArgumentParser::ArgumentParser(std::vector<Argument> &args_v, std::string exec_name) noexcept :
	args(args_v.begin(), args_v.end()),
	parsed()
{
	addBaseArgs(std::move(exec_name));

	args.insert(args.end(), args_v.begin(), args_v.end());

}

ArgumentParser::ArgumentParser(const ArgumentParser& orig) noexcept :
	args(orig.args),
	parsed(orig.parsed)
{
}

ArgumentParser::~ArgumentParser() 
{
	args.clear();
	parsed.clear();
}

void ArgumentParser::add(std::string s_arg, std::string l_arg, std::string cmd, std::string desc, std::string ex, ArgumentFlags flags, std::function<void(std::string)> func) noexcept
{
    Argument arg;
    arg.short_arg = s_arg;
    arg.long_arg = l_arg;
    arg.command = cmd;
    arg.description = desc;
    arg.example = ex;
	arg.func = func;
	arg.flags = flags;

	arg.flags &= (ArgumentFlags::SHORT_ARG | ArgumentFlags::LONG_ARG | ArgumentFlags::COMMAND);
	if(!arg.short_arg.empty())
		arg.flags |= ArgumentFlags::SHORT_ARG;
	if(!arg.short_arg.empty())
		arg.flags |= ArgumentFlags::LONG_ARG;
	if(!arg.short_arg.empty())
		arg.flags |= ArgumentFlags::COMMAND;

    args.push_back(arg);
}

void ArgumentParser::add(Argument &arg) noexcept
{
	arg.flags &= (ArgumentFlags::SHORT_ARG | ArgumentFlags::LONG_ARG | ArgumentFlags::COMMAND);
	if(!arg.short_arg.empty())
		arg.flags |= ArgumentFlags::SHORT_ARG;
	if(!arg.short_arg.empty())
		arg.flags |= ArgumentFlags::LONG_ARG;
	if(!arg.short_arg.empty())
		arg.flags |= ArgumentFlags::COMMAND;

    args.push_back(arg);
}
 
void ArgumentParser::add(std::vector<Argument> &args_v) noexcept
{
    args.reserve(args_v.size());
    args.insert(args.end(), args_v.begin(), args_v.end());

	for(auto &arg : args)
	{
		arg.flags &= (ArgumentFlags::SHORT_ARG | ArgumentFlags::LONG_ARG | ArgumentFlags::COMMAND);
		if(!arg.short_arg.empty())
			arg.flags |= ArgumentFlags::SHORT_ARG;
		if(!arg.short_arg.empty())
			arg.flags |= ArgumentFlags::LONG_ARG;
		if(!arg.short_arg.empty())
			arg.flags |= ArgumentFlags::COMMAND;
	}
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
	// If compareArgs() returns true remove
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

auto ArgumentParser::getArgument(std::string match_str) const -> const Argument &
{
	auto iter = std::find_if(args.begin(), args.end(), [&](const Argument &current_arg)
	{
		return compareArgs(current_arg, match_str);
	});
	
	if(iter == args.end())
		throw ArgumentException(ArgumentException::ARG_NOT_FOUND_ERROR, "The argument " + match_str + " could not be found");

	return *iter;
}

auto ArgumentParser::getArgument(std::string match_str) -> Argument &
{
	auto iter = std::find_if(args.begin(), args.end(), [&](Argument &current_arg)
	{
		return compareArgs(current_arg, match_str);
	});

	if(iter == args.end())
		throw ArgumentException(ArgumentException::ARG_NOT_FOUND_ERROR, "The argument " + match_str + " could not be found");

	return *iter;
}

auto ArgumentParser::getUserData(std::string match_str) const -> std::string
{
	auto iter = std::find_if(parsed.begin(), parsed.end(), [&](const std::tuple<ArgIter, std::string> &current_res)
	{
		ArgIter arg = std::get<0>(current_res);
		if(arg->command == match_str)
			return true;
		else if(arg->long_arg == match_str)
			return true;
		else if(arg->short_arg == match_str)
			return true;
		else
			return false;
	});

	if(iter == parsed.end())
		throw ArgumentException(ArgumentException::NO_USER_DATA_ERROR, "The user has no data supplied");

	return std::get<1>(*iter);
}

auto ArgumentParser::getUserData(Argument &arg) const -> std::string
{
	auto iter = std::find_if(parsed.begin(), parsed.end(), [&](const std::tuple<ArgIter, std::string> &current_res)
	{
		return compareArgs(*std::get<0>(current_res), arg);
	});

	if(iter == parsed.end())
		throw ArgumentException(ArgumentException::NO_USER_DATA_ERROR, "The user has no data supplied");

	return std::get<1>(*iter);
}

void ArgumentParser::setAlias(std::string existing_arg, Argument &alias)
{
	auto iter = std::find_if(args.begin(), args.end(), [&](Argument &arg)
	{
		if(arg.command == existing_arg)
			return true;
		else if(arg.long_arg == existing_arg)
			return true;
		else if(arg.short_arg == existing_arg)
			return true;
		else
			return false;
	});

	if(iter == args.end())
		throw ArgumentException(ArgumentException::ALIAS_ERROR, "The argument " + existing_arg + " does not exist");

	alias.flags = iter->flags;
	alias.flags &= (ArgumentFlags::SHORT_ARG | ArgumentFlags::LONG_ARG | ArgumentFlags::COMMAND);
	if(!alias.short_arg.empty())
		alias.flags |= ArgumentFlags::SHORT_ARG;
	if(!alias.short_arg.empty())
		alias.flags |= ArgumentFlags::LONG_ARG;
	if(!alias.short_arg.empty())
		alias.flags |= ArgumentFlags::COMMAND;

	alias.func = iter->func;
	alias.example = iter->example;
	alias.data_info = iter->data_info;

	if(alias.description.empty())
	{
		// Choose the most meaningful and non-empty argument		[A bit ugly but short]
		// At first check if COMMAND flag is set. If not no command is set
		std::string &arg_descript = (iter->flags & ArgumentFlags::COMMAND) ? iter->command : 
			// Then check if LONG_ARG flags is set. If not there is also no long argument
			// so use short argument as description
			((iter->flags & ArgumentFlags::LONG_ARG) ? iter->long_arg : iter->short_arg);
		alias.description = "This is an alias for " + arg_descript;
	}

	if(alias.long_description.empty())
	{
		std::string &arg_descript = (iter->flags & ArgumentFlags::COMMAND) ? iter->command : ((iter->flags & ArgumentFlags::LONG_ARG) ? iter->long_arg : iter->short_arg);
		alias.long_description = "This is an alias for " + arg_descript + ". See the help for " + arg_descript + " for more information.";
	}
}

void ArgumentParser::setAlias(Argument &existing_arg, Argument &alias)
{
	alias.flags = existing_arg.flags;
	alias.flags &= (ArgumentFlags::SHORT_ARG | ArgumentFlags::LONG_ARG | ArgumentFlags::COMMAND);
	if(!alias.short_arg.empty())
		alias.flags |= ArgumentFlags::SHORT_ARG;
	if(!alias.short_arg.empty())
		alias.flags |= ArgumentFlags::LONG_ARG;
	if(!alias.short_arg.empty())
		alias.flags |= ArgumentFlags::COMMAND;

	auto iter = std::find_if(args.begin(), args.end(), [&](Argument &arg)
	{
		return compareArgs(existing_arg, arg);
	});

	if(iter == args.end())
		throw ArgumentException(ArgumentException::ALIAS_ERROR, "The specified argument does not exist");

	alias.func = iter->func;
	alias.example = iter->example;
	alias.data_info = iter->data_info;

	if(alias.description.empty())
	{
		// Choose the most meaningful and non-empty argument		[A bit ugly but short]
		// At first check if COMMAND flag is set. If not no command is set
		std::string &arg_descript = (iter->flags & ArgumentFlags::COMMAND) ? iter->command :
			// Then check if LONG_ARG flags is set. If not there is also no long argument
			// so use short argument as description
			((iter->flags & ArgumentFlags::LONG_ARG) ? iter->long_arg : iter->short_arg);
		alias.description = "This is an alias for " + arg_descript;
	}

	if(alias.long_description.empty())
	{
		std::string &arg_descript = (iter->flags & ArgumentFlags::COMMAND) ? iter->command : ((iter->flags & ArgumentFlags::LONG_ARG) ? iter->long_arg : iter->short_arg);
		alias.long_description = "This is an alias for " + arg_descript + ". See the help for " + arg_descript + " for more information.";
	}

	args.push_back(alias);
}

void ArgumentParser::parse(int argc, char **argv, bool execute_funcs)
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
			if(iter->flags.isUserDataAllowed())
			{
				std::string user_data = argv[++i];
				if(compareArgs(*iter, user_data))
					argv--;	

				if(execute_funcs)
					iter->func(user_data);

				parsed.push_back(std::tuple<ArgIter, std::string>(iter, user_data));
			}
			else
			{
				if(execute_funcs)
					iter->func("");

				parsed.push_back(std::tuple<ArgIter, std::string>(iter, ""));
			}
		}
    }
}

auto ArgumentParser::loop(int argc, char **argv, bool catch_except) -> int
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

		if(!catch_except)	
			parseAndRun(cmdline);
		else
		{
			try
			{
				parseAndRun(cmdline);
			}
			catch(const ArgumentException &e)
			{
				if(e.code() == ArgumentException::TOO_MANY_ARGS_ERROR)
					std::cout << "ERROR: Please specify multiple arguments as following: arg1 && arg2" << std::endl;
				else
					std::cout << e.what();
			}
		}
	}

	return 0;
}

auto ArgumentParser::compareArgs(const Argument &arg, std::string &str) const noexcept -> bool
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

auto ArgumentParser::compareArgs(Argument &arg, Argument &other_arg) const noexcept -> bool
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

void ArgumentParser::addBaseArgs(std::string &&prog_name) noexcept
{
	exec_path = prog_name;
	exec_name = exec_path.substr(exec_path.find_last_of("\\") + 1, exec_path.size());

	Argument help_arg;
	help_arg.short_arg = "-h";
	help_arg.long_arg = "--help";
	help_arg.command = "help";
	help_arg.data_info = "command";
	help_arg.description = "Shows information for registered commands";
	help_arg.example = exec_name + " help";
	help_arg.flags |= (ArgumentFlags::SHORT_ARG | 
		ArgumentFlags::LONG_ARG |
		ArgumentFlags::COMMAND	| 
		ArgumentFlags::OPTIONAL | 
		ArgumentFlags::USER_DATA_ALLOWED);
	help_arg.func = [&](std::string string)
	{
		if(string.empty())
			help();
		else
			help(string);
	};
	args.push_back(help_arg);

	Argument exit_arg;
	exit_arg.command = "exit";
	exit_arg.description = "Exits this application";
	exit_arg.data_info = "exit_code";
	exit_arg.example = exec_name + " > exit";
	exit_arg.flags |= (ArgumentFlags::COMMAND |
		ArgumentFlags::OPTIONAL |
		ArgumentFlags::USER_DATA_ALLOWED);
	exit_arg.func = [&](std::string string)
	{
		int exit_code = 0;
		if(!string.empty())
			exit_code = std::stoi(string);

		exit(exit_code);
	};
	args.push_back(exit_arg);

	Argument close_alias;
	close_alias.command = "close";
	close_alias.example = exec_name + " > close";
	setAlias(exit_arg, close_alias);
}

void ArgumentParser::saveExecName(std::string name) noexcept
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

	bool found_cmd = false;
	for(auto iter = commands.begin(); iter != commands.end(); iter++)
	{
		if(found_cmd && *iter == "&&")
			found_cmd = false;
		else if(found_cmd)
			throw ArgumentException(ArgumentException::TOO_MANY_ARGS_ERROR, "There were too many arguments specified");

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
			if(found_iter->flags.isUserDataAllowed() && (iter + 1) != commands.end())
			{
				if(!compareArgs(*found_iter, *++iter))
					user_data = *iter;
				else
					throw ArgumentException(ArgumentException::TOO_MANY_ARGS_ERROR, "There were too many arguments specified");
			}

			found_cmd = true;
			found_iter->func(user_data);
		}
	}
}

void ArgumentParser::help() const noexcept
{
	std::future<std::string> cmdline_str = std::async([this]()
	{
		std::stringstream cmdline;
		cmdline << exec_name << " ";
		for(auto &arg : args)
		{
			if(!arg.flags.isLoopOnly())
			{
				if(arg.flags.hasShortArg())
					cmdline << "[" << arg.short_arg;

				if(arg.flags.hasLongArg())
					cmdline << (arg.flags.hasShortArg() ? '|' : '[') << arg.long_arg << ((arg.flags.isUserDataAllowed()) ? "=" : "");

				if(arg.flags.hasCommand())
					cmdline << (arg.flags.hasLongArg() ? '|' : '[') << arg.command;

				if(!arg.data_info.empty())
					cmdline << " <" << arg.data_info << ">";

				cmdline << "] ";
			}
		}

		return cmdline.str();
	});

	std::future<std::string> req_info_str = std::async([this]()
	{
		std::stringstream info;
		info << "    Required:\n";

		for(auto &arg : args)
		{
			if(!arg.flags.isOptional())
			{
				if(arg.flags.hasShortArg())
					info << "      " << arg.short_arg;

				if(arg.flags.hasLongArg())
					info << (arg.flags.hasShortArg() ? ", " : "      ") << arg.long_arg << ((arg.flags.isUserDataAllowed()) ? "=" : "");

				if(arg.flags.hasCommand())
					info << (arg.flags.hasLongArg() ? ", " : "      ") << arg.command;

				if(!arg.data_info.empty())
					info << " <" << arg.data_info << "> ";

				if(!arg.description.empty())
				{
					info << std::setw(45) << arg.description << std::endl;
				}

				info << std::endl;
			}
		}

		return info.str();
	});

	std::future<std::string> opt_info_str = std::async([this]()
	{
		std::stringstream info;
		info << "    Optional:\n";

		for(auto &arg : args)
		{
			if(arg.flags.isOptional())
			{
				if(arg.flags.hasShortArg())
					info << "      " << arg.short_arg;

				if(arg.flags.hasLongArg())
					info << (arg.flags.hasShortArg() ? ", " : "      ") << arg.long_arg;

				if(arg.flags.hasCommand())
					info << (arg.flags.hasLongArg() ? ", " : "      ") << arg.command;

				if(!arg.data_info.empty())
					info << " <" << arg.data_info << "> ";

				if(!arg.description.empty())
				{
					info << std::setw(45) << arg.description << std::endl;
				}

				info << std::endl;
			}
		}

		return info.str();
	});

	std::future<std::string> examples_str = std::async([this]()
	{
		std::stringstream examples;
		examples << "    Examples:\n";

		for(auto &arg : args)
		{
			if(!arg.example.empty())
				examples << "      " << arg.example << std::endl;
		}

		return examples.str();
	});

	std::cout << "Help:\n\n" 
		<< cmdline_str.get() 
		<< "\n\n" 
		<< req_info_str.get()
		<< opt_info_str.get()
		<< examples_str.get();
}

void ArgumentParser::help(std::string arg_help) const noexcept
{
	std::future<std::string> help_str = std::async([this, &arg_help]()
	{
		std::stringstream help;
		auto arg = getArgument(arg_help);

		help << "Extended help:\n\n"
			<< "    Arguments/Command:\n";
		
		if(arg.flags.hasShortArg())
			help << "      Short argument: " << arg.short_arg << (!arg.data_info.empty() ? (" <" + arg.data_info + ">\n") : "\n");
			
		if(arg.flags.hasLongArg())
			help << "      Long argument: " << arg.long_arg << (!arg.data_info.empty() ? ("=<" + arg.data_info + ">\n") : "\n");
			
		if(arg.flags.hasCommand())
			help << "      Command: " << arg.command << (!arg.data_info.empty() ? (" <" + arg.data_info + ">\n") : "\n");
		
		help << std::endl
			<< "    Full description:\n";
		if(!arg.long_description.empty())
			help << "      " << arg.long_description << std::endl;
		else if(!arg.description.empty())
			help << "      " << arg.description << std::endl;

		help << std::endl
			<< "    Flags:\n"
			<< "      Has short arg: " << (arg.flags.hasShortArg() ? "true" : "false") << std::endl
			<< "      Has long arg: " << (arg.flags.hasLongArg() ? "true" : "false") << std::endl
			<< "      Has command: " << (arg.flags.hasCommand() ? "true" : "false") << std::endl
			<< "      Is optional: " << (arg.flags.isOptional() ? "true" : "false") << std::endl
			<< "      Is loop only: " << (arg.flags.isLoopOnly() ? "true" : "false") << std::endl
			<< "      Is user data allowed: " << (arg.flags.isUserDataAllowed() ? "true" : "false") << std::endl
			<< "      Is user data required: " << (arg.flags.isUserDataRequired() ? "true" : "false") << std::endl
			<< std::endl;	

		return help.str();
	});

	std::cout << help_str.get();
}

