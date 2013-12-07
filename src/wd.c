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
#include "dir_list.h"

#include <stdlib.h>
#include <stdio.h>

#if defined _WIN32
#include <io.h>
#include <fcntl.h>
#endif

int do_remove( const char* cmd, dir_list_t p_dir_list )
{
    int success = 0;
    size_t index = 0;
    int ret_val = 0;

    if( wd_prompt )
    {
        dump_dir_list( p_dir_list );
        fprintf(stdout,"Enter number to remove: ");

        /* Flush the output to ensure that the user knows that we're waiting for
           some input.  This is necessary when, for example, running the Win32
           version of this app within MinTTy on Cygwin */
        fflush(stdout);

        success = fscanf(stdin,"%u",&index);
        if( success )
        {
            success = remove_dir_by_index( p_dir_list, index );
        }
    }
    else
    {
        success = remove_dir( p_dir_list, wd_oper_dir );
    }

    if( success )
    {
        ret_val = 1;
    }
    else
    {
        if( wd_prompt )
        {
            fprintf(stderr,
                    "%s: Error: Invalid index '%d'\n",
                    cmd, index);
        }
        else
        {
            fprintf(stderr,
                    "%s: Warning: Directory not in list: '%s'\n",
                    cmd, wd_oper_dir);
        }
    }

    return ret_val;
}

int main( int argc, char* argv[] )
{
    int ret_code = 0;

    init_cmdln();

    if( process_env() &&
        process_cmdln( argc, argv ) ) {
        DEBUG_OUT("command line processed");
        if( wd_oper != WD_OPER_NONE ) {

            int dir_list_needs_save = 0;

            dir_list_t dir_list = NULL;

            DEBUG_OUT("loading bookmark file %s",list_fn);

            dir_list = load_dir_list( list_fn );

            if( dir_list == NULL ) {
                fprintf(stderr,"%s: Warning: Unable to load list file '%s'\n",
                        argv[0], list_fn);
                dir_list = new_dir_list();
            }

            DEBUG_OUT("loaded bookmark file");

            switch( wd_oper ) {
                case WD_OPER_REMOVE:
                    dir_list_needs_save = do_remove( argv[0], dir_list );
                    break;
                case WD_OPER_GET:
                    DEBUG_OUT("WD_OPER_GET: %s",wd_bookmark_name);

                    if(( dump_dir_with_name( dir_list, wd_bookmark_name ) ||
                         dump_dir_if_exists( dir_list, wd_bookmark_name )) && wd_store_access ) {
                        /* We're updating access times, so flag that the list needs
                           saving */
                        dir_list_needs_save = 1;
                    }
                    break;
                case WD_OPER_GET_BY_BM_NAME:
                    DEBUG_OUT("WD_OPER_GET_BY_BM_NAME: %s",wd_bookmark_name);

                    if( dump_dir_with_name( dir_list, wd_bookmark_name ) && wd_store_access ) {
                        /* We're updating access times, so flag that the list needs
                           saving */
                        dir_list_needs_save = 1;
                    }
                    break;
                case WD_OPER_ADD:
                    /* TODO: Consider allowing directory to be added twice with
                             different bookmark name? */
                    if( dir_in_list( dir_list, wd_oper_dir )) {
                        fprintf(stderr,
                                "%s: Warning: Directory already in list: '%s'\n",
                                argv[0], wd_oper_dir);
                    } else if( (wd_bookmark_name != NULL) &&
                               bookmark_in_list( dir_list, wd_bookmark_name )) {
                        fprintf(stderr,
                                "%s: Warning: Bookmark name already in list: '%s'\n",
                                argv[0], wd_bookmark_name);
                    } else {
                        time_t a_time = -1;
                        if( wd_store_access ) {
                            a_time = wd_now_time;
                        }
                        add_dir( dir_list,
                                 wd_oper_dir,
                                 wd_bookmark_name,
                                 wd_now_time,
                                 a_time );
                        dir_list_needs_save = 1;
                    }
                    break;
                case WD_OPER_DUMP:
                    dump_dir_list( dir_list );
                    break;
                case WD_OPER_LIST:
#if defined _WIN32
                    _setmode(1,_O_BINARY);
#endif
                    /* TODO: This won't update the access time - don't
                       necessarily want to because we might be listing out a lot
                       of directories.  Probably want a call-back from the shell
                       script */
                    list_dirs( dir_list );
                    break;
                default:
                    fprintf(stderr,"Unhandled operation type\n");
                    break;
            }
            if( dir_list_needs_save ) {
                save_dir_list( dir_list, list_fn );
            }
        }
        /* TODO: be more selective about setting this */
        ret_code = EXIT_SUCCESS;
    } else {
        ret_code = EXIT_FAILURE;
    }

    DEBUG_OUT("all done");
    return ret_code;
}
