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
#define NEED_PARAMETER_STRING "No parametere specified for argument"

#define DEFAULT_LIST_FILE "/.wd_list"

#include <stdio.h>

/* Supporting get_home() */
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>
/* !Supporting get_home() */


wd_oper_t wd_oper;
char*     list_fn = NULL;
char      wd_oper_dir[ MAXPATHLEN ];

void get_home( void )
{
    char *homedir = getenv("HOME");
    if( homedir == NULL ) {
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        if (pw != NULL) {
            homedir = pw->pw_dir;
        }
    }

    if( homedir != NULL ) {
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
        } else if( 0 == strcmp( this_arg, "-d" ) ) {
            wd_oper = WD_OPER_DUMP;
        } else if( 0 == strcmp( this_arg, "-a" ) ) {
            wd_oper = WD_OPER_ADD;

            /* Check to see if there's an argument to this command */
            if((( arg_loop + 1 ) < argc ) &&
                ( argv[ arg_loop + 1 ][0] != '-' )) {
                arg_loop++;
                strcpy( wd_oper_dir, argv[ arg_loop ] );
            } else {
                /* No argument .. use the CWD */
                getcwd( wd_oper_dir, MAXPATHLEN );
            }
        } else if( 0 == strcmp( this_arg, "-l" ) ) {
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
