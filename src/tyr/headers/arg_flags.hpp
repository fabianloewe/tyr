#ifndef __ARG_FLAGS__
#define __ARG_FLAGS__

#include <vector>

namespace CPM_TYR_CN
{

class ArgumentFlags
{
public:
	enum Flags
	{
		SHORT_ARG			= 0x1,
		LONG_ARG			= 0x2,
		COMMAND				= 0x4,
		OPTIONAL			= 0x8,
		LOOP_ONLY			= 0x10,
		USER_DATA_ALLOWED	= 0x20,
		USER_DATA_REQUIRED	= 0x40
	};

public:
	ArgumentFlags(unsigned int flags = 0) :
		af_flags(flags)
	{
	}

	auto hasShortArg() const
	{
		return (af_flags & SHORT_ARG) ? true : false;
	}

	auto hasLongArg() const
	{
		return (af_flags & LONG_ARG) ? true : false;
	}

	auto hasCommand() const
	{
		return (af_flags & COMMAND) ? true : false;
	}

	auto isOptional() const
	{
		return (af_flags & OPTIONAL) ? true : false;
	}

	auto isLoopOnly() const
	{
		return (af_flags & LOOP_ONLY) ? true : false;
	}

	auto isUserDataAllowed() const
	{
		return (af_flags & USER_DATA_ALLOWED) ? true : false;
	}

	auto isUserDataRequired() const
	{
		return (af_flags & USER_DATA_REQUIRED) ? true : false;
	}

	auto operator ==(const ArgumentFlags &other) const
	{
		return (af_flags & other.af_flags) ? true : false;
	}

	auto operator !=(const ArgumentFlags &other) const
	{
		return (af_flags & other.af_flags) ? false : true;
	}

	auto operator &(const ArgumentFlags &other) const -> bool
	{
		return (af_flags & other.af_flags) ? true : false;
	}

	auto operator &=(const ArgumentFlags &other) -> ArgumentFlags &
	{
		af_flags &= other.af_flags;
		return *this;
	}

	auto operator |(const ArgumentFlags &other) const -> bool
	{
		return (af_flags | other.af_flags) ? true : false;
	}

	auto operator |=(const ArgumentFlags &other) -> ArgumentFlags &
	{
		af_flags |= other.af_flags;
		return *this;
	}

private:
	unsigned int af_flags;
};

}

#endif // !__ARG_FLAGS__

