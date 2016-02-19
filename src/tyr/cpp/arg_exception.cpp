#include "../headers/arg_exception.hpp"

using namespace tyr;

ArgumentException::ArgumentException(ErrorCode error) :
	ex_code(error),
	ex_info()
{
}

ArgumentException::ArgumentException(ErrorCode error, std::string info) :
	ex_code(error),
	ex_info(std::move(info))
{
}

auto ArgumentException::what() const -> const char *
{
	return ex_info.c_str();
}

auto ArgumentException::code() const -> ErrorCode
{
	return ex_code;
}