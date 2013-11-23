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

int main( int argc, char* argv[] )
{
    int ret_code = 0;

    init_cmdln();

    if( process_cmdln( argc, argv ) ) {
        if( wd_oper != WD_OPER_NONE ) {
            dir_list_t dir_list = load_dir_list( list_fn );

            if( dir_list != NULL ) {
            }
        }
    } else {
        ret_code = EXIT_FAILURE;
    }
    return 0;
}
