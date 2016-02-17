#ifndef __ARG_EXCEPTION__
#define __ARG_EXCEPTION__

#include <exception>
#include <string>

namespace hyronx
{
	
class ArgumentException : public std::exception
{
public:
	enum ErrorCode
	{
		ARG_NOT_FOUND_ERROR,
		NO_USER_DATA_ERROR,
		ALIAS_ERROR,
		TOO_MANY_ARGS_ERROR,
		UNKNOWN = 0xFFFFFFFF
	};

public:
	ArgumentException(ErrorCode error);
	ArgumentException(ErrorCode error, std::string info);

	virtual auto what() const -> const char *;
	auto code() const->ErrorCode;

private:
	ErrorCode ex_code;
	std::string ex_info;
};

}

#endif // !__ARG_EXCEPTION__
