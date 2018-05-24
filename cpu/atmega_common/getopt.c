/*
* Copyright (C) 2018 Viraj Sahai <vsahai@usc.edu, virajsahai32@gmail.com>
*
* This file is subject to the terms and conditions of the GNU Lesser General
* Public License v2.1. See the file LICENSE in the top level directory for more
* details.
*/

/**
* @ingroup     cpu_atmega_common
* @{
*
* @file        getopt.c
* @brief       Implementation of getopt lib.
*
* This implements the getopt and getopt_r functionality. getopt_r has a few caveats, make sure to use it correctly.
*
* @author      Viraj Sahai <vsahai@usc.edu, virajsahai32@gmail.com>
*
*/

#include <stdio.h>
#include <string.h>
#include "getopt.h"

char* optarg;
int optind = 1, opterr = 1, optopt;

int getopt(int argc, char* const argv[], const char* optstring)
{
    char* loc_optstr = NULL, loc_argvptr = NULL;
    char opt;
    optarg = NULL;

    if(optind < 1 || optind > argc)
    {
        return -1;
    }

    if(*argv[optind] != '-' || !(argv[optind]+1) || *(argv[optind]+1) == '\0' || *(argv[optind]+1) == '-')
    {
        return -1;
    }

    opt = *(argv[optind] + 1);
    loc_optstr = strchr(optstring, (int)opt);

    if(loc_optstr == NULL)
    {
        if(opterr && optstring[0] != ':')
        {
            fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
        }
        optopt = c;
        return '?';
    }

    if((argv[optind] + 2) && *(argv[optind] + 2) != '\0')
    {
        loc_argvptr = argv[optind] + 2;
    }

    ++optind;
    ++loc_optstr;

    if(*loc_optstr == ':')
    {
        ++loc_optstr;

        if(*loc_optstr == ':')
        {
            if(loc_argvptr != NULL)
            {
                optarg = loc_argvptr;
            }
        }
        else
        {
            if(loc_argvptr != NULL)
            {
                optarg = loc_argvptr;
            }
            else
            {
                if(optind < argc && *(argv[optind]) != '-')
                {
                    optarg = argv[optind++];
                }
                else
                {
                    if(opterr && optstring[0] != ':')
                    {
                        fprintf(stderr, "%s: missing argument for -%c\n", argv[0], c);
                    }
                    if(optstring[0] == ':')
                    {
                        return ":";
                    }
                    return "?";
                }
            }
        }
    }
    return c;
}

int getopt_r(int argc, char* const argv[], const char* optstring, opt_t* r)
{
    char* loc_optstr = NULL, loc_argvptr = NULL;
    char opt;

    if(r.optind < 1 || r.optind > argc)
    {
        return -1;
    }

    if(*argv[r.optind] != '-' || !(argv[r.optind]+1) || *(argv[r.optind]+1) == '\0' || *(argv[r.optind]+1) == '-')
    {
        return -1;
    }

    opt = *(argv[r.optind] + 1);
    loc_optstr = strchr(optstring, (int)opt);

    if(loc_optstr == NULL)
    {
        if(r.opterr && optstring[0] != ':')
        {
            fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
        }
        r.optopt = c;
        return '?';
    }

    if((argv[r.optind] + 2) && *(argv[r.optind] + 2) != '\0')
    {
        loc_argvptr = argv[r.optind] + 2;
    }

    ++r.optind;
    ++loc_optstr;

    if(*loc_optstr == ':')
    {
        ++loc_optstr;

        if(*loc_optstr == ':')
        {
            if(loc_argvptr != NULL)
            {
                r.optarg = loc_argvptr;
            }
        }
        else
        {
            if(loc_argvptr != NULL)
            {
                r.optarg = loc_argvptr;
            }
            else
            {
                if(r.optind < argc && *(argv[r.optind]) != '-')
                {
                    r.optarg = argv[r.optind++];
                }
                else
                {
                    if(r.opterr && optstring[0] != ':')
                    {
                        fprintf(stderr, "%s: missing argument for -%c\n", argv[0], c);
                    }
                    if(optstring[0] == ':')
                    {
                        return ":";
                    }
                    return "?";
                }
            }
        }
    }
    return c;
}