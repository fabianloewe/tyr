/* 
 * File:   ArgumentParser.hpp
 * Author: HERON
 *
 * Created on 20. Januar 2016, 10:13
 */

#ifndef __ARG_PARSER__
#define	__ARG_PARSER__

#include <string>
#include <vector>
#include <functional>
#include <tuple>

#include "arg.hpp"
#include "arg_flags.hpp"
#include "arg_utility.hpp"

namespace tyr
{

class ArgumentParser 
{
public:
    ArgumentParser(std::string exec_name = "") noexcept;
    ArgumentParser(std::vector<Argument> &args_v, std::string exec_name = "") noexcept;
    ArgumentParser(const ArgumentParser& orig) noexcept;
    virtual ~ArgumentParser();
    
    void add(std::string s_arg, std::string l_arg, std::string cmd, std::string desc, std::string ex, ArgumentFlags flags, std::function<void(std::string)> func) noexcept;
    void add(Argument &arg) noexcept;
    void add(std::vector<Argument> &args_v) noexcept;

	template<typename Class>
	void add(std::string s_arg, std::string l_arg, std::string cmd, std::string desc, std::string ex, ArgumentFlags flags, void(Class::* func)(std::string), Class * const this_ptr) noexcept;
    
    void remove(std::string match_str);
    void remove(Argument &arg);
    void remove(std::vector<Argument> &args_v);

	auto getArgument(std::string match_str) const -> const Argument &;
	auto getArgument(std::string match_str) -> Argument &;

	auto getUserData(std::string match_str) const -> std::string;
	auto getUserData(Argument &arg) const -> std::string;

	void setAlias(std::string existing_arg, Argument &alias);
	void setAlias(Argument &existing_arg, Argument &alias);
    
	void parse(int argc, char **argv, bool execute_funcs = true);
    
    auto loop(int argc, char **argv, bool catch_except = true) -> int;
    
private:
	using ArgIter = std::vector<Argument>::iterator;

    std::vector<Argument> args;
	std::vector<std::tuple<ArgIter, std::string>> parsed;
	std::string exec_name;
	std::string exec_path;

private:
	inline auto compareArgs(const Argument &arg, std::string &str) const noexcept -> bool;
	inline auto compareArgs(Argument &arg, Argument &other_arg) const noexcept -> bool;

	void addBaseArgs(std::string &&exec_name) noexcept;
	void saveExecName(std::string name) noexcept;

	void parseAndRun(std::string cmdline);

	void help() const noexcept;
	void help(std::string) const noexcept;
};

template<typename Class>
void ArgumentParser::add(std::string s_arg, std::string l_arg, std::string cmd, std::string desc, std::string ex, ArgumentFlags flags, void(Class::* func)(std::string), Class * const this_ptr) noexcept
{
	Argument arg;
	arg.short_arg = s_arg;
	arg.long_arg = l_arg;
	arg.command = cmd;
	arg.description = desc;
	arg.example = ex;
	arg.func = func;
	arg.flags = easy_bind(func, *this_ptr);

	// If one of the arguments is empty disable it (!ENABLE_*)
	arg.flags[0] = (arg.short_arg.empty()) ? false : true;
	arg.flags[1] = (arg.long_arg.empty()) ? false : true;
	arg.flags[2] = (arg.command.empty()) ? false : true;

	args.push_back(arg);
}

}

#endif	/* __ARG_PARSER__ */

