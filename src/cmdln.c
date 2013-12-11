/*
   Copyright 2013 John Bailey

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "cmdln.h"

#define VERSION_STRING "wd v1.1 by dev@brightsilence.com\n https://github.com/bright-tools/wd"
#define UNRECOGNISED_ARG_STRING "Unrecognised command line argument"
#define NEED_PARAMETER_STRING "No parameter specified for argument"
#define INCOMPATIBLE_OP_STRING "Parameter incompatible with other arguments"
#define UNRECOGNISED_PARAM_STRING "Parameter to argument not recognised"

/* Windows is quite happy with a forward slash in the path */
#define DEFAULT_LIST_FILE "/.wd_list"
#define ENV_VAR_NAME      "WD_OPTS"

#include <stdio.h>
#include <time.h>

/* Supporting get_home() */
#include <unistd.h>
#if defined _WIN32
#include <shlobj.h>
#else
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>
#endif
/* !Supporting get_home() */

static void get_home( config_container_t* const p_config )
{
    int success = 0;

#if defined _WIN32
    char homedir[MAX_PATH];
    success = SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, homedir));
#else
    /* Try and retrieve the home from the environment first */
    char *homedir = getenv("HOME");

    if( homedir == NULL ) {
        /* Couldn't get user's home directory from the environment so
           try the user database */
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        if (pw != NULL) {
            homedir = pw->pw_dir;
        }
    }
    if( homedir != NULL ) {
        success = 1;
    }
#endif

    if( success ) {
        /* Stitch the home directory and default filename together into list_fn
           */

        size_t home_size = strlen( homedir );
        size_t complete = home_size +
                          strlen( DEFAULT_LIST_FILE ) +
                          1U;
        p_config->list_fn = realloc( p_config->list_fn, complete );
        strcpy( p_config->list_fn, homedir );
        strcpy( &(p_config->list_fn[ home_size ]), DEFAULT_LIST_FILE);
        p_config->list_fn[ complete -1 ] = 0;
    } else {
        /* TODO: Deal with not having a home directory */
    }
}

void init_cmdln( config_container_t* const p_config ) {
    /* TODO: Consider only doing this if the file has not been specified on the
       command line */
    get_home( p_config );

    p_config->wd_oper = WD_OPER_NONE;
    p_config->wd_prompt = 0;
    p_config->wd_store_access = 0;
    p_config->wd_bookmark_name = NULL;
    p_config->wd_dir_form = WD_DIRFORM_NONE;
    p_config->wd_now_time = time(NULL);
}

static void show_help( const char* const p_cmd ) {
    fprintf(stdout,
            "%s [-v] [-h] [-t] [-f <fn>] [-r [dir] [-p]] [-a [dir]] [-d] [-l] [-s <c>]\n"
            " -v       : Show version information\n"
            " -h       : Show usage help\n"
            " -d       : Dump bookmark list\n"
            " -t       : Store access times for bookmarks\n"
            " -l       : List directories & bookmark names (generally for use in tab\n"
            "             expansion)\n"
            " -s <c>   : Format paths for cygwin\n"
            " -n <nam> : Get bookmark directory with specified shortcut name\n"
            " -p       : Prompt for input (can be used with -r instead of specifying\n"
            "             directory\n"
            " -f <fn>  : Use file <fn> for storing bookmarks\n"
            " -r [dir] : Remove specified directory or current directory if none\n"
            " -a [dir] : Add specified directory or current directory if none\n"
            "             specified\n",
            p_cmd );
    /* TODO: Complete the description */
    /* TODO: Add option to sort dump alphabetically by directory, by name,
             by date added */
}

static int process_opts( config_container_t* const p_config, const int argc, char* const argv[], const int p_cmd_line ) {
    int arg_loop;
    int ret_val = 1;

    /* Start loop at 1 - index 0 is name of executable */
    for( arg_loop = 1; arg_loop < argc; arg_loop++ )
    {
        char* this_arg = argv[ arg_loop ];
        DEBUG_OUT("process_opts: %s",this_arg); 
        if( p_cmd_line && ( 0 == strcmp( this_arg, "-v" ) )) {
            fprintf( stdout, "%s\n", VERSION_STRING );
        } else if( p_cmd_line && ( 0 == strcmp( this_arg, "-h" )) ) {
            show_help( argv[0] );
        } else if( p_cmd_line && ( 0 == strcmp( this_arg, "-p" )) ) {
            p_config->wd_prompt = 1;
        } else if( 0 == strcmp( this_arg, "-t" ) ) {
            p_config->wd_store_access = 1;
        } else if( 0 == strcmp( this_arg, "-z" ) ) {
            if(( arg_loop + 1 ) < argc ) {
                arg_loop++;
                sscanf(argv[arg_loop],"%ld",(long int*)(&p_config->wd_now_time));
            } else {
                fprintf( stdout, "%s: %s\n", NEED_PARAMETER_STRING, this_arg );
                ret_val = 0;
            }
        } else if( 0 == strcmp( this_arg, "-s" ) ) {
            if(( arg_loop + 1 ) < argc ) {
                arg_loop++;
                /* TODO: Validate length of argument? */
                switch( argv[ arg_loop ][0] ) {
                    case 'c':
                        p_config->wd_dir_form = WD_DIRFORM_CYGWIN;
                        break;
                    case 'w':
                        p_config->wd_dir_form = WD_DIRFORM_WINDOWS;
                        break;
                    default:
                        fprintf( stdout, "%s: %s\n", UNRECOGNISED_PARAM_STRING, this_arg );
                        ret_val = 0;
                        break;
                }
            } else {
                fprintf( stdout, "%s: %s\n", NEED_PARAMETER_STRING, this_arg );
                ret_val = 0;
            }
        } else if( p_cmd_line && ( 0 == strcmp( this_arg, "-d" )) ) {
            p_config->wd_oper = WD_OPER_DUMP;
        } else if( p_cmd_line && ( 0 == strcmp( this_arg, "-l" )) ) {
            p_config->wd_oper = WD_OPER_LIST;
        } else if( p_cmd_line && (( 0 == strcmp( this_arg, "-n" )) ||
                                  ( 0 == strcmp( this_arg, "-g" )))) {
            if((( arg_loop + 1 ) < argc ) &&
                ( argv[ arg_loop + 1 ][0] != '-' )) {
                arg_loop++;
                if( 0 == strcmp( this_arg, "-n" ) ) {
                    p_config->wd_oper = WD_OPER_GET_BY_BM_NAME;
                } else {
                    p_config->wd_oper = WD_OPER_GET;
                }
                p_config->wd_bookmark_name = argv[ arg_loop ];
            } else {
                fprintf( stdout, "%s: %s\n", NEED_PARAMETER_STRING, this_arg );
                ret_val = 0;
            }
        } else if( p_cmd_line && 
                  (( 0 == strcmp( this_arg, "-a" )) ||
                   ( 0 == strcmp( this_arg, "-r" ))) ) {
            if( p_config->wd_oper == WD_OPER_NONE ) {
                switch( this_arg[ 1 ] ) {
                    case 'a':
                        p_config->wd_oper = WD_OPER_ADD;
                        break;
                    case 'r':
                        p_config->wd_oper = WD_OPER_REMOVE;
                        break;
                }

                /* Check to see if there's an argument to this command */
                if((( arg_loop + 1 ) < argc ) &&
                    ( argv[ arg_loop + 1 ][0] != '-' )) {
                    arg_loop++;

                    /* TODO: Add a switch to prevent the path being made
                       absolute? */
#if defined _WIN32
                    GetFullPathName( argv[ arg_loop ],
                                     MAXPATHLEN,
                                     p_config->wd_oper_dir,
                                     NULL );
#else
                    realpath( argv[ arg_loop ], p_config->wd_oper_dir );
#endif

                    /* TODO: This only really makes sense for an add operation
                    */
                    if((( arg_loop + 1 ) < argc ) &&
                        ( argv[ arg_loop + 1 ][0] != '-' )) {
                        arg_loop++;
                        p_config->wd_bookmark_name = argv[ arg_loop ];
                    }
                } else {
                    /* No argument .. use the CWD */
                    getcwd( p_config->wd_oper_dir, MAXPATHLEN );
                }
            } else {
                fprintf( stdout, "%s: %s\n", INCOMPATIBLE_OP_STRING, this_arg );
                
                /* Check to see if there's an argument to this command */
                if((( arg_loop + 1 ) < argc ) &&
                    ( argv[ arg_loop + 1 ][0] != '-' )) {
                    arg_loop++;
                }
            }
        } else if( 0 == strcmp( this_arg, "-f" ) ) {
            arg_loop++;
            if( arg_loop < argc ) {
                /* To be consistent, list_fn always points to malloc'd memory
                   rather than just pointing it to the parameter string */
                p_config->list_fn = realloc( p_config->list_fn, strlen( argv[ arg_loop ] ));
                strcpy( p_config->list_fn, argv[ arg_loop ] );
            } else {
                fprintf( stdout, "%s: %s\n", NEED_PARAMETER_STRING, this_arg );
                ret_val = 0;
            }
        } else {
            fprintf( stdout, "%s:\n  %s\n", UNRECOGNISED_ARG_STRING, this_arg );
            ret_val = 0;
        }
    }

    return ret_val;
}

int process_env( config_container_t* const p_config )
{
    char *opts = getenv(ENV_VAR_NAME);
    int ret_val = 1;

    /* Retrieved environment string OK? */
    if( opts != NULL ) {
        /* +1 to take the NULL terminator into account - when we're looping
           through the string below we want to include this, hence we factor
           it in at this point */
        const size_t opts_len = strlen( opts ) + 1U;
        char*        opt_copy = (char*) malloc( opts_len );
        size_t breaks = 0;
        size_t loop = 0;
        char** argv;
        int waiting;
        char *src, *dest;

        DEBUG_OUT(ENV_VAR_NAME ": %s",opts);

        /* TODO: Deal with quotes in strings */

        /* Copy the environment string to a fresh memory area so that we can
           maniupulate it.  Replace spaces with null terminators to break string
           into individual arguments and keep a count of the amount we've done
           */
        for( loop = 0, src = opts, dest = opt_copy ;
             loop < opts_len;
             loop++, src++, dest++ ) 
        {
            if( *src == ' ' )
            {
                *dest = 0;
                breaks++;
            } else {
                *dest = *src;
            }
        }

        /* Allocate memory for pointers to the arguments
           +1 to take into account the fact that the first argument has special
           meaning
           +1 to take into account the final, un-counted element in the loop
           above */
        argv = malloc( sizeof( char*[ breaks + 2 ] ));
        /* First argument is the name of the program */
        argv[0] = ENV_VAR_NAME;
        breaks = 1;
        waiting = 1;

        /* Loop through the broken up string, populating argv with pointers to
           the start of each argument */
        for( loop = 0;
             loop < opts_len;
             loop++ )
        {
            /* Are we waiting to find the start of a new string and this is a
               non-null character?
               Using this mechanism deals with multiple consecutive spaces/NULLs
               in the string and prevents argv having entries pointing to NULL
               strings */
            if( waiting && ( opt_copy[ loop ] != '0' )) {
                argv[breaks++] = &(opt_copy[loop]);
                DEBUG_OUT(ENV_VAR_NAME ": opt - %s",argv[breaks-1]);
                waiting = 0;
            } else if( opt_copy[loop ] == 0 ) {
                waiting = 1;
            }
        }

        DEBUG_OUT(ENV_VAR_NAME ": opt count - %d",breaks);
        /* Finally, process the array of options */
        ret_val = process_opts( p_config, breaks, argv, 0 );

        free( opt_copy );
        free( argv );
    }

    return ret_val;
}

int process_cmdln( config_container_t* const p_config, const int argc, char* const argv[] ) {
    return process_opts( p_config, argc, argv, 1 );
}
