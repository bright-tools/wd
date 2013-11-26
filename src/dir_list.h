/**
   \file
   \brief The dir_list module provides functions to manipulate and
           search the list of bookmarks

   \copyright Copyright 2013 John Bailey

   \section LICENSE

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

#if !defined( DIR_LIST_H )

#include <stddef.h>
#include <time.h>

/**
    Structure to represent a list of directory bookmarks.
*/
typedef struct dir_list_s* dir_list_t;

/**
    Load a set of bookmarks from the specified file

    Note that in the case where invalid data is found in the file or a problem
    is encountered while reading from the file, the function will return
    a list of the bookmarks it was able to successfully read

    \param[in] p_fn The filename to load the bookmarks from
    \returns Pointer to a list of bookmarks loaded from the file or
              NULL in the case of a problem (failure to allocate memory,
              failure to open file)
*/
dir_list_t load_dir_list( const char* const p_fn );

/**
    Create a new directory list structure.

    Allocates the required memory and initialises the structure members

    \returns Pointer to the newly created structure or NULL if allocation
              failed
*/
extern dir_list_t new_dir_list( void );
int        add_dir( dir_list_t p_list,
                    const char* const p_dir,
                    const char* const p_name,
                    const time_t      p_t_added );
int        remove_dir( dir_list_t p_list, const char* const p_dir );
void       dump_dir_with_name( const dir_list_t p_list, const char* const p_name );
int        remove_dir_by_index( dir_list_t p_list, const size_t p_dir );
int        dir_in_list( dir_list_t p_list, const char* const p_dir );
int        bookmark_in_list( dir_list_t p_list, const char* const p_name );
int        save_dir_list( const dir_list_t p_list, const char* p_fn );
void       dump_dir_list( const dir_list_t p_list );
void       list_dirs( const dir_list_t p_list );


#endif
