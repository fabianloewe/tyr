/* 
 * File:   ArgumentParser.hpp
 * Author: HERON
 *
 * Created on 20. Januar 2016, 10:13
 */

#ifndef ARGUMENTPARSER_HPP
#define	ARGUMENTPARSER_HPP

#include <string>
#include <vector>
#include <functional>
#include <tuple>

namespace hybros
{

class ArgumentFlags
{
public:
	ArgumentFlags() : flags(4, false) {}

	ArgumentFlags(bool short_arg, bool long_arg, bool cmd, bool user_data) : flags(4)
	{
		flags[0] = short_arg;
		flags[1] = long_arg;
		flags[2] = cmd;
		flags[3] = user_data;
	}

	virtual ~ArgumentFlags() { flags.clear(); }

	auto hasShortArg() const	{ return flags[0]; }
	auto hasLongArg() const		{ return flags[1]; }
	auto hasCommand() const		{ return flags[2]; }
	auto hasUserData() const	{ return flags[3]; }

	void enableShortArg(bool b) { flags[0] = b; }
	void enableLongArg(bool b)	{ flags[1] = b; }
	void enableCommand(bool b)	{ flags[2] = b; }
	void needsUserData(bool b)	{ flags[3] = b; }

	auto operator [](int position) { return flags[position]; }

	auto operator ==(ArgumentFlags &other) const
	{
		for(int i = 0; i < flags.size(); i++)
		{
			if(flags[i] != other[i])
				return false;
		}

		return true;
	}

	auto operator !=(ArgumentFlags &other) const { return (*this == other) ? false : true; }

protected:
	std::vector<bool> flags;
};

class Argument
{
public:
    std::string short_arg;
    std::string long_arg;
    std::string command;
    std::string description;
    std::string example;
    std::function<void(std::string &)> func;
	ArgumentFlags flags;
};

class ArgumentParser 
{
public:
    ArgumentParser(std::string exec_name = "");
    ArgumentParser(std::vector<Argument> &args_v, std::string exec_name = "");
    ArgumentParser(const ArgumentParser& orig);
    virtual ~ArgumentParser();
    
    void add(std::string s_arg, std::string l_arg, std::string cmd, std::string desc, std::string ex, std::function<void(std::string &)> func, ArgumentFlags flags);
    void add(Argument &arg);
    void add(std::vector<Argument> &args_v);
    
    void remove(std::string match_str);
    void remove(Argument &arg);
    void remove(std::vector<Argument> &args_v);

	auto get(std::string match_str) const -> std::string;
    
	void parse(int argc, char **argv);
    
    auto loop(int argc, char **argv) -> int;
    
private:
	using arg_iter = std::vector<Argument>::iterator;

    std::vector<Argument> args;
	std::vector<std::tuple<arg_iter, std::string>> parsed;
	std::string exec_name;
	std::string exec_path;

private:
	inline auto compareArgs(Argument &arg, std::string &str) const -> bool;
	inline auto compareArgs(Argument &arg, Argument &other_arg) const -> bool;

	void addBaseArgs(std::string &&exec_name);
	void saveExecName(std::string name);

	void parseAndRun(std::string cmdline);
};

}

#endif	/* ARGUMENTPARSER_HPP */

