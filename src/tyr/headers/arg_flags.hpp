#ifndef __ARG_FLAGS__
#define __ARG_FLAGS__

#include <vector>

namespace hyronx
{

class ArgumentFlags
{
public:
	ArgumentFlags() :
		flags(7, false) 
	{
	}

	ArgumentFlags(bool short_arg, bool long_arg, bool cmd, bool optional, bool loop_only, bool has_user_data, bool needs_user_data) :
		flags(7)
	{
		flags[0] = short_arg;
		flags[1] = long_arg;
		flags[2] = cmd;
		flags[3] = optional;
		flags[4] = loop_only;
		flags[5] = has_user_data;
		flags[6] = needs_user_data;
	}

	virtual ~ArgumentFlags() {
		flags.clear();
	}

	auto hasShortArg() const 
	{
		return flags[0];
	}

	auto hasLongArg() const 
	{
		return flags[1];
	}

	auto hasCommand() const 
	{
		return flags[2];
	}

	auto isOptional() const 
	{
		return flags[3];
	}

	auto isLoopOnly() const
	{
		return flags[4];
	}

	auto isUserDataAllowed() const
	{
		return flags[5];
	}

	auto isUserDataRequired() const
	{
		return flags[6];
	}


	void enableShortArg(bool b) 
	{
		flags[0] = b;
	}

	void enableLongArg(bool b) 
	{
		flags[1] = b;
	}

	void enableCommand(bool b)
	{
		flags[2] = b;
	}

	void setOptional(bool b)
	{
		flags[3] = b;
	}

	void setLoopOnly(bool b)
	{
		flags[4] = b;
	}

	void allowUserData(bool b)
	{
		flags[5] = b;
		flags[6] = false;
	}

	void requiresUserData(bool b)
	{
		flags[5] = b;
		flags[6] = b;
	}

	auto operator =(ArgumentFlags &other)
	{
		flags.clear();
		flags = other.flags;
		return *this;
	}

	auto operator [](int position) {
		return flags[position];
	}

	auto operator ==(ArgumentFlags &other) const
	{
		for(int i = 0; i < flags.size(); i++)
		{
			if(flags[i] != other[i])
				return false;
		}

		return true;
	}

	auto operator !=(ArgumentFlags &other) const {
		return (*this == other) ? false : true;
	}

protected:
	std::vector<bool> flags;
};


}

#endif // !__ARG_FLAGS__

