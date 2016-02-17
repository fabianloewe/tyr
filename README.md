# Tyr

This is a simple argument parser in C++11 which can parse arguments and commands in the following format:

          short  long   command  additional arg
          
    myapp  -o   --open   open     file1


It also provides a loop function which executes a user-defined (lambda) function every turn.

E.g.

    myapp -o file1                      (--open=file1 or open file1 would also be possible)

    {... opens file1 ...}

    myapp > {... waits for user input ...}


    myapp > save

    {... saves file1 ...}

    myapp > open file2

    {... opens file2 ...}

    myapp > exit

    {... exits app and goes back to shell ...}


Already included is the exit (-e, --exit) and help (-h, --help) argument.

You can find a source code example under 'main'.

**This is not a release version and currently meant for my own use.**
