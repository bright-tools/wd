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

int main( int argc, char* argv[] )
{
    int ret_code = 0;

    init_cmdln();

    if( process_cmdln( argc, argv ) ) {
        if( wd_oper != WD_OPER_NONE ) {
            dir_list_t dir_list = load_dir_list( list_fn );

            if( dir_list == NULL ) {
                fprintf(stderr,"%s: Warning: Unable to load list file '%s'\n",
                        argv[0], list_fn);
                dir_list = new_dir_list();
            }

            switch( wd_oper ) {
                case WD_OPER_ADD:
                    if( !dir_in_list( dir_list, wd_oper_dir )) {
                        add_dir( dir_list, wd_oper_dir );
                        save_dir_list( dir_list, list_fn );
                    }
                    break;
                case WD_OPER_DUMP:
                    dump_dir_list( dir_list );
                    break;
                default:
                    fprintf(stderr,"Unhandled operation type\n");
            }
        }
    } else {
        ret_code = EXIT_FAILURE;
    }
    return ret_code;
}
