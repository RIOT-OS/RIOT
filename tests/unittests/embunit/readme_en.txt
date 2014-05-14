

                        == Embedded Unit ==
               https://sourceforge.net/projects/embunit/

------------------------------------------------------------------------------

Embedded Unit is unit testing framework for Embedded C System. It's design was
copied from JUnit and CUnit and more, and then adapted somewhat for Embedded C
System. Embedded Unit does not require std C libs. All objects are allocated
to const area.

------------------------------------------------------------------------------

1.Release Contetns
    [embUnit]
    +- COPYING          : Copyright Notice
    +- makefile         :
    +- readme.txt       : japanese
    +- readme_en.txt    : this file
    +- [embUnit]        : Embedded Unit Source
    +- [msvc]           : MSVC++6.0 Project
    +- [samples]        : Embedded Unit Samples
    +- [tests]          : Embedded Unit Self Tests
    +- [tools]          : test template generation tools


2.Development environment and Execution environment

    2.1.Required execution environment
        - The ROM more than 2KB
        - The Stack more than 128b

    2.2.Required development environment
        - C Compiler

    2.3.Development of Embedded Unit is performed in the following environment
        - Microsoft Windows XP Professional
        - VC++.NET or cygwin 1.3.22 + gcc 3.2
        - Microsoft Windows 98
        - VC++6.0
        - Apple Computer MacOS X 10.1.5
        - Project Builder 1.1.1 (gcc 2.95.2)


3.Compile
    Embedded Unit is using stdio print function for the output of a test
    result message. Implement the following function, if you do not want
    to use stdio print function.

        void stdimpl_print(const char *string)
        * this function does not output a new-line in the end of a string.

    And then add compile-option '-DNO_STDIO_PRINTF', or release the following
    comments of a embUnit/config.h.

        /*#define NO_STDIO_PRINTF*/

    3.1.GNU building tools
        - edit embUnit/config.h if needed and execute the following commands.

            $ make

        - running self tests

            $ make test

        - running samples

            $ make samples

    3.2.MSVC++6.0
        - open msvc/embUnit.dsw and build each project.

    3.3.MacOSX
        - edit embUnit/config.h if needed and execute the following commands.

            $ make CC=cc


4.License

    MIT/X Consortium License


------------------------------------------------------------------------------
$Id: readme_en.txt,v 1.6 2003/09/16 11:08:45 arms22 Exp $
