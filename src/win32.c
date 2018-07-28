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

#if defined WIN32
#include <shlobj.h>
#include <sys/param.h>
#include <time.h>
#include <locale.h>

void platform_init( void )
{
    char* osType = getenv("OSTYPE");

    /* Set the locale to the user's locale */
    setlocale(LC_ALL, "");

    /* If we're running within Cygwin, unset the TZ environment variable
       as Windows doesn't understand the format.  When unset, Windows should
       fall back on the registry to get the machine's time settings */
    if(( osType != NULL ) && (strcmp( osType, "cygwin" ) == 0 ))
    {
        putenv( "TZ=" );
        tzset();
    }
}

char* get_home_dir( void )
{
    int success = 0;
    char* homedir = malloc( MAX_PATH );
    success = SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, homedir));
    if( ! success )
    {
        free(homedir);
        homedir = NULL;
    }
    return homedir;
}

void release_home_dir( char* p_dir )
{
    free( p_dir );
}

void  canonicalize_dir( const char* const p_dir, char* const p_target )
{
    GetFullPathName( p_dir,
                     MAXPATHLEN,
                     p_target,
                     NULL );
}

#endif