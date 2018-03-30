/*
   Copyright 2013-2018 John Bailey

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

#include "os_if.h"

#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

char* get_home_dir( void )
{
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
    return homedir;
}

void release_home_dir( char* p_dir )
{

}

void  canonicalize_dir( const char* const p_dir, char* const p_target )
{
    realpath( p_dir, p_target );
}
