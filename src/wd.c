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

void do_remove( const char* cmd, dir_list_t p_dir_list )
{
    int success;
    size_t index = 0;

    if( wd_prompt )
    {
        dump_dir_list( p_dir_list );
        fprintf(stdout,"Enter number to remove: ");
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
        save_dir_list( p_dir_list, list_fn );
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
}

int main( int argc, char* argv[] )
{
    int ret_code = 0;

    init_cmdln();

    if( process_cmdln( argc, argv ) ) {
        if( wd_oper != WD_OPER_NONE ) {
#if 0
            printf("FILE: %s\n",list_fn);
#endif
            dir_list_t dir_list = load_dir_list( list_fn );

            if( dir_list == NULL ) {
                fprintf(stderr,"%s: Warning: Unable to load list file '%s'\n",
                        argv[0], list_fn);
                dir_list = new_dir_list();
            }

            switch( wd_oper ) {
                case WD_OPER_REMOVE:
                    do_remove( argv[0], dir_list );
                    break;
                case WD_OPER_GET_BY_BM_NAME:
                    dump_dir_with_name( dir_list, wd_bookmark_name );
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
                        add_dir( dir_list, wd_oper_dir, wd_bookmark_name );
                        save_dir_list( dir_list, list_fn );
                    }
                    break;
                case WD_OPER_DUMP:
                    dump_dir_list( dir_list );
                    break;
                case WD_OPER_LIST:
#if defined _WIN32
                    _setmode(1,_O_BINARY);
#endif
                    list_dirs( dir_list );
                    break;
                default:
                    fprintf(stderr,"Unhandled operation type\n");
                    break;
            }
        }
    } else {
        ret_code = EXIT_FAILURE;
    }
    return ret_code;
}
