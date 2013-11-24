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

#define VERSION_STRING "wd by John, v."
#define UNRECOGNISED_ARG_STRING "Unrecognised command line argument"
#define NEED_PARAMETER_STRING "No parameter specified for argument"
#define INCOMPATIBLE_OP_STRING "Parameter incompatible with other arguments"
#define UNRECOGNISED_PARAM_STRING "Parameter to argument not recognised"

#define DEFAULT_LIST_FILE "/.wd_list"

#include <stdio.h>

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

wd_oper_t wd_oper;
char*         list_fn = NULL;
char          wd_oper_dir[ MAXPATHLEN ];
char*         wd_bookmark_name;
int           wd_prompt;
wd_dir_format_t wd_dir_form;

void get_home( void )
{
    int success = 0;
#if defined _WIN32
    char homedir[MAX_PATH];
    success = SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, homedir));
#else
    char *homedir = getenv("HOME");
    if( homedir == NULL ) {
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        if (pw != NULL) {
            homedir = pw->pw_dir;
        }
    }
    if( homedir == NULL ) {
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
        list_fn = realloc( list_fn, complete );
        strcpy( list_fn, homedir );
        strcpy( &(list_fn[ home_size ]), DEFAULT_LIST_FILE);
        list_fn[ complete -1 ] = 0;
    } else {
        /* TODO */
    }
}

void init_cmdln( void ) {
    /* TODO: Consider only doing this if the file has not been specified on the
       command line */
    get_home();

    wd_oper = WD_OPER_NONE;
    wd_prompt = 0;
    wd_bookmark_name = NULL;
    wd_dir_form = WD_DIRFORM_NONE;
}

static void show_help( const char* const p_cmd ) {
    fprintf(stdout,
            "%s [-v] [-h] [-r] [-a [dir]]\n"
            " -v       : Show version information\n"
            " -h       : Show usage help\n"
            " -d       : Dump bookmark list\n"
            " -s <c>   : Format paths for cygwin\n"
            " -p       : Prompt for input (can be used with -r instead of specifying directory\n"
            " -f <fn>  : Use file <fn> for storing bookmarks\n"
            " -r [dir] : Remove directory\n"
            " -a [dir] : Add directory\n",
            p_cmd );
    /* TODO */
}

int process_cmdln( const int argc, char* const argv[] ) {
    int arg_loop;
    int ret_val = 1;

    /* Start loop at 1 - index 0 is name of executable */
    for( arg_loop = 1; arg_loop < argc; arg_loop++ )
    {
        char* this_arg = argv[ arg_loop ];
        if( 0 == strcmp( this_arg, "-v" ) ) {
            fprintf( stdout, "%s\n", VERSION_STRING );
        } else if( 0 == strcmp( this_arg, "-h" ) ) {
            show_help( argv[0] );
        } else if( 0 == strcmp( this_arg, "-p" ) ) {
            wd_prompt = 1;
        } else if( 0 == strcmp( this_arg, "-s" ) ) {
            if(( arg_loop + 1 ) < argc ) {
                arg_loop++;
                /* TODO: Validate length of argument? */
                switch( argv[ arg_loop ][0] ) {
                    case 'c':
                        wd_dir_form = WD_DIRFORM_CYGWIN;
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
        } else if( 0 == strcmp( this_arg, "-d" ) ) {
            wd_oper = WD_OPER_DUMP;
        } else if( 0 == strcmp( this_arg, "-l" ) ) {
            wd_oper = WD_OPER_LIST;
        } else if(( 0 == strcmp( this_arg, "-a" )) ||
                  ( 0 == strcmp( this_arg, "-r" )) ) {
            if( wd_oper == WD_OPER_NONE ) {
                switch( this_arg[ 1 ] ) {
                    case 'a':
                        wd_oper = WD_OPER_ADD;
                        break;
                    case 'r':
                        wd_oper = WD_OPER_REMOVE;
                        break;
                }

                /* Check to see if there's an argument to this command */
                if((( arg_loop + 1 ) < argc ) &&
                    ( argv[ arg_loop + 1 ][0] != '-' )) {
                    arg_loop++;
                    strcpy( wd_oper_dir, argv[ arg_loop ] );
                } else {
                    /* No argument .. use the CWD */
                    getcwd( wd_oper_dir, MAXPATHLEN );
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
                list_fn = realloc( list_fn, strlen( argv[ arg_loop ] ));
                strcpy( list_fn, argv[ arg_loop ] );
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
