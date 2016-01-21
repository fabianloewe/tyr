#include <iostream>

#include "../../arg_parser/headers/ArgumentParser.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    using namespace hybros;

	ArgumentParser parser(argv[0]);
	
	Argument test;
	test.short_arg = "-t";
	test.long_arg = "--test";
	test.command = "test";
	test.description = "This is just a example";
	test.example = "myapp.exe test";
	test.func = [](std::string &) { cout << "My test" << endl; };
	parser.add(test);

    return parser.loop(argc, argv);
}
