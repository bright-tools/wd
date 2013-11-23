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

#include "dir_list.h"

#define FILE_HEADER_DESC_STRING "# WD directory list file"
#define FILE_HEADER_VER_STRING "# File format: version 1"

#define ANSI_COLOUR_RED     "\x1b[31m"
#define ANSI_COLOUR_GREEN   "\x1b[32m"
#define ANSI_COLOUR_GREY    "\x1b[37;2m"
#define ANSI_COLOUR_RESET   "\x1b[0m"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>

#define MIN_DIR_SIZE 100

struct dir_list_s
{
    size_t dir_count;
    char** dir_list;
    int    dir_size;
};

static void increase_dir_alloc( dir_list_t p_list )
{
    if( p_list->dir_size == 0 ) {
        p_list->dir_size += MIN_DIR_SIZE;
    }

    p_list->dir_list = (char**) realloc( p_list->dir_list,
                                         p_list->dir_size * sizeof( char* ) );
    /* TODO: deal with re-alloc failure */
}

int add_dir( dir_list_t p_list, const char* const p_dir )
{
    int ret_val = 0;

    if( p_list->dir_count == p_list->dir_size )
    {
        increase_dir_alloc( p_list );
        /* TODO: deal with failure */
    }

    p_list->dir_list[ p_list->dir_count ] = malloc( strlen( p_dir ) + 1);
    strcpy( p_list->dir_list[ p_list->dir_count ], p_dir );

    p_list->dir_count++;
    ret_val = 1;


    return( ret_val );
}

dir_list_t new_dir_list( void )
{
    dir_list_t ret_val = (dir_list_t)malloc( sizeof( dir_list_t ) );

    if( ret_val != NULL ) {
        ret_val->dir_count = 0;
        ret_val->dir_list = NULL;
        ret_val->dir_size = 0;
    }

    return( ret_val );
}

dir_list_t load_dir_list( const char* const p_fn )
{
    FILE* file = fopen( p_fn, "rt" );
    dir_list_t ret_val = NULL;

    if( file != NULL ) {
        ret_val = new_dir_list();

        if( ret_val != NULL ) {
            char path[ MAXPATHLEN ];

            while( fgets( path, MAXPATHLEN, file )) {
                if( path[0] != '#' ) {
                    size_t trimmer;
                    for( trimmer = (strlen( path ) - 1);
                         trimmer > 0;
                         trimmer-- ) {
                        if(( path[ trimmer ] == '\r' ) ||
                           ( path[ trimmer ] == '\n' )) {
                            path[ trimmer ] = 0;
                        } else {
                            break;
                        }
                    }
                    add_dir( ret_val, path );
                }
            }
        }
        fclose( file );
    }

    return( ret_val );
}

int        dir_in_list( dir_list_t p_list, const char* const p_dir )
{
    int ret_val = 0;
    size_t dir_loop;

    for( dir_loop = 0; dir_loop < p_list->dir_count; dir_loop++ )
    {
        if( 0 == strcmp( p_dir, p_list->dir_list[ dir_loop ] )) {
            ret_val = 1;
            break;
        }
    }

    return( ret_val );
}
void dump_dir_list( const dir_list_t p_list )
{
    if( p_list == NULL )
    {
        fprintf( stdout, "Empty dirlist structure\n" );
    } else {
        size_t dir_loop;
        fprintf( stdout, "Dirlist has %d entries of %d used\n", 
                 p_list->dir_count, p_list->dir_size );

        for( dir_loop = 0; dir_loop < p_list->dir_count; dir_loop++ )
        {
            char* col = ANSI_COLOUR_RESET;
            char* dir = p_list->dir_list[ dir_loop ];
            struct stat s;
            int err = stat( dir , &s);
            if(-1 == err) {
                if(ENOENT == errno) {
                    col = ANSI_COLOUR_RED;
                }
            } else {
                if(S_ISDIR(s.st_mode)) {
                    col = ANSI_COLOUR_GREEN;
                } else {
                    /* Not a directory */
                    col = ANSI_COLOUR_GREY;
                }
            }

            fprintf( stdout, "[%d] %s%s%s\n", dir_loop,
                                              col,
                                              dir,
                                              ANSI_COLOUR_RESET );
        }
    }
}

int save_dir_list( const dir_list_t p_list, const char* p_fn ) {
    int ret_val = 0;
    FILE* file;

    assert( p_fn != NULL );
    assert( p_list != NULL );

    file = fopen( p_fn, "wt" );

    if( file != NULL ) {
        size_t dir_loop;
        fprintf( file, "%s\n%s\n", FILE_HEADER_DESC_STRING,
                                   FILE_HEADER_VER_STRING );

        for( dir_loop = 0; dir_loop < p_list->dir_count; dir_loop++ )
        {
            fprintf( file, "%s\n", p_list->dir_list[ dir_loop ] );
        }

        fclose( file );
        ret_val = 1;
    }

    return( ret_val );
}
