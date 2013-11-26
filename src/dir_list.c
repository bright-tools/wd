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
#include "cmdln.h"
#if defined WIN32
#include <windows.h>
#endif

#define FILE_HEADER_DESC_STRING "# WD directory list file"
#define FILE_HEADER_VER_STRING "# File format: version 1"

#define TIME_FORMAT_STRING "%Y/%m/%d %H:%M:%S"
#define TIME_SSCAN_STRING  "%04u/%02u/%02u %02u:%02u:%02u"
#define TIME_STRING_BUFFER_SIZE (21U)

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
#include <time.h>

#define MIN_DIR_SIZE 100

struct dir_list_item
{
    char*  dir_name;
    char*  bookmark_name;
    time_t time_added;
    /* TODO: Other data here?  Time added?  Meta-data such as whether it exists?
       Shortcut name? */
};

#define DLI_SIZE (sizeof( struct dir_list_item ))

struct dir_list_s
{
    size_t                dir_count;
    struct dir_list_item* dir_list;
    int                   dir_size;
};

static void increase_dir_alloc( dir_list_t p_list )
{
    struct dir_list_item* new_mem;
    p_list->dir_size += MIN_DIR_SIZE;

    new_mem = 
        (struct dir_list_item*) realloc( p_list->dir_list,
                                         p_list->dir_size * DLI_SIZE );
    if( new_mem != NULL )
    {
        p_list->dir_list = new_mem;
    }
}

int add_dir( dir_list_t p_list,
             const char* const p_dir,
             const char* const p_name,
             const time_t      p_t_added )
{
    int ret_val = 0;
    char* dest = NULL;
    const size_t idx = p_list->dir_count;

    if( idx == p_list->dir_size )
    {
        DEBUG_OUT("list does not have space, increasing allocation"); 
        increase_dir_alloc( p_list );
        DEBUG_OUT("increased allocation"); 
    }

    /* Check to see if there's space now - it's possible that an attempt to
       increase the allocation (above) actually failed */
    if( idx < p_list->dir_size )
    {
        dest = (char*)malloc( strlen( p_dir ) + 1);
        if( dest != NULL ) {
            strcpy( dest, p_dir );
            p_list->dir_list[ idx ].dir_name = dest;

            if( p_name != NULL )
            {
                dest = (char*)malloc( strlen( p_name ) + 1);
                if( dest != NULL ) {
                    strcpy( dest, p_name );
                    p_list->dir_list[ idx ].bookmark_name = dest;
                    ret_val = 1;
                } else {
                    p_list->dir_list[ idx ].bookmark_name = NULL;
                }
            } else {
                p_list->dir_list[ idx ].bookmark_name = NULL;
                ret_val = 1;
            }

            if( ret_val ) {
                p_list->dir_list[ idx ].time_added = p_t_added;
                p_list->dir_count++;
            }
        }
    }

    return( ret_val );
}

dir_list_t new_dir_list( void )
{
    dir_list_t ret_val = (dir_list_t)malloc( sizeof( struct dir_list_s ) );

    if( ret_val != NULL ) {
        ret_val->dir_count = 0;
        ret_val->dir_list = NULL;
        ret_val->dir_size = 0;

        /* Allocate some initial memory for the directory list - this saves us
           having to deal with dir_list being NULL in the general case */
        increase_dir_alloc( ret_val );
    }

    return( ret_val );
}

dir_list_t load_dir_list( const char* const p_fn )
{
    FILE* file = fopen( p_fn, "rt" );
    dir_list_t ret_val = NULL;

    if( file != NULL ) {
        DEBUG_OUT("opened bookmark file");
        ret_val = new_dir_list();

        if( ret_val != NULL ) {
            char path[ MAXPATHLEN ];
            char read[ MAXPATHLEN ];
            char name[ MAXPATHLEN ];
            time_t added;
            char* fstr;
            int one_last_go = 1;

            path[0] = 0;
            name[0] = 0;
            added = -1;
            DEBUG_OUT("generated empty bookmark list");

            while(( fstr = fgets( read, MAXPATHLEN, file ) ) ||
                  one_last_go ) {
                DEBUG_OUT("read from file: %s",read);
                /* Didn't get anything from the file?  Set the read string to
                   indicate that it's a new bookmark so that we flush out
                   any previous bookmark data and flag that we don't need to go
                   round again ( one_last_go ) */
                if( fstr == NULL ) {
                    read[0] = ':';
                    read[1] = 0;
                    one_last_go = 0;
                }

                /* Check that it wasn't a comment line */
                if( read[0] != '#' ) {
                    size_t len = strlen( read );

                    /* Trim off line endings */
                    size_t trimmer;
                    for( trimmer = len - 1;
                         trimmer > 0;
                         trimmer-- ) {
                        if(( read[ trimmer ] == '\r' ) ||
                           ( read[ trimmer ] == '\n' )) {
                            read[ trimmer ] = 0;
                        } else {
                            break;
                        }
                    }

                    /* Is this the start of a new bookmark? */
                    if( read[0] == ':' ) {
                        /* Already read some bookmark details? */
                        if ( path[0] != '\0' ) {

                            DEBUG_OUT("creating new bookmark: %s",path);

                            /* Create the new bookmark and reset attributes */
                            add_dir( ret_val, path, name, added );

                            DEBUG_OUT("created new bookmark");

                            path[0] = 0;
                            name[0] = 0;
                        }
                        strcpy( path, &(read[1]) );
                    } else if(( read[0] == 'N' ) &&
                              ( read[1] == ':' )) {
                        strcpy( name, &(read[2]) );
                    } else if(( read[0] == 'A' ) &&
                              ( read[1] == ':' )) {
                        struct tm tm;
                        tm.tm_isdst = -1;
                        sscanf(&(read[2]), TIME_SSCAN_STRING,
                                           &tm.tm_year,
                                           &tm.tm_mon,
                                           &tm.tm_mday,
                                           &tm.tm_hour,
                                           &tm.tm_min,
                                           &tm.tm_sec);

                        /* tm's year is baselined at 1900 */
                        tm.tm_year -= 1900;
                        /* tm's month is the count of months since Jan */
                        tm.tm_mon -= 1;

                        /* Convert to a UTC time */
                        added = mktime(&tm) - timezone;
                    } else {
                        fprintf(stderr,
                                "Unrecognised content in bookmarks file: %s\n",
                                read);
                    }
                }
            }
        }
        fclose( file );
    }

    return( ret_val );
}

int bookmark_in_list( dir_list_t p_list, const char* const p_name )
{
    int ret_val = 0;
    size_t dir_loop;
    struct dir_list_item* current_item = p_list->dir_list;

    for( dir_loop = 0; dir_loop < p_list->dir_count; dir_loop++, current_item++ )
    {
        if((current_item->bookmark_name != NULL ) &&
           (0 == strcmp( p_name, current_item->bookmark_name ))) {
            ret_val = 1;
            break;
        }
    }

    return( ret_val );
}

static int find_dir_location( dir_list_t p_list, const char* const p_dir, size_t* p_loc )
{
    int ret_val = 0;
    size_t dir_loop;
    struct dir_list_item* current_item = p_list->dir_list;

    for( dir_loop = 0; dir_loop < p_list->dir_count; dir_loop++, current_item++ )
    {
        if( 0 == strcmp( p_dir, current_item->dir_name )) {
            ret_val = 1;
            if( p_loc != NULL ) {
                *p_loc = dir_loop;
            }
            break;
        }
    }

    return( ret_val );
}

int remove_dir_by_index( dir_list_t p_list, const size_t p_dir )
{
    int ret_val = 0;

    if( p_dir < p_list->dir_count ) {
        p_list->dir_count--;

        /* Must use memmove here not memcpy as regions overlap */
        memmove(&(p_list->dir_list[p_dir]), 
                &(p_list->dir_list[p_dir+1]),
                (p_list->dir_count - p_dir) * DLI_SIZE );

        ret_val = 1;
    }

    return( ret_val );
}

int remove_dir( dir_list_t p_list, const char* const p_dir )
{
    int ret_val = 0;
    size_t location;

    if( find_dir_location( p_list, p_dir, &location )) {
        ret_val = remove_dir_by_index( p_list, location );
    }

    return( ret_val );
}

int dir_in_list( dir_list_t p_list, const char* const p_dir )
{
    return( find_dir_location( p_list, p_dir, NULL ) );
}

char* format_dir( char* p_dir ) {
    char* ret_val = NULL;

    switch( wd_dir_form ) {
        case WD_DIRFORM_NONE:
            ret_val = p_dir;
            break;
        case WD_DIRFORM_CYGWIN: {
            char* dest;
            char* src;
            ret_val = (char*)malloc( strlen( p_dir ) + 1 );
            for( dest = ret_val, src = p_dir;
                 *src != '\0';
                 dest++, src++ ) {
                if( *src == '\\' ) {
                    *dest = '/';
                } else {
                    *dest = *src;
                }
            }
            *dest = 0;
                                }
            break;
        default:
            fprintf(stderr,"Unhandled directory format\n");
            ret_val = p_dir;
            break;
    }

    return( ret_val );
}

void dump_dir_with_name( const dir_list_t p_list, const char* const p_name )
{
    size_t dir_loop;
    struct dir_list_item* current_item;

    for( dir_loop = 0, current_item = p_list->dir_list;
         dir_loop < p_list->dir_count;
         dir_loop++, current_item++ )
    {
        if(( current_item->bookmark_name != NULL ) &&
           ( 0 == strcmp( p_name, current_item->bookmark_name ))) {

            char* dir_formatted = format_dir( current_item->dir_name );
            fprintf( stdout, "%s", dir_formatted );

            if( current_item->dir_name != dir_formatted ) {
                free( dir_formatted );
            }

            break;
        }
    }
}

void list_dirs( const dir_list_t p_list )
{
    if( p_list == NULL )
    {
        fprintf( stdout, "Empty dirlist structure\n" );
    } else {
        size_t dir_loop;
        struct dir_list_item* current_item;
        int valid = 1;

        for( dir_loop = 0, current_item = p_list->dir_list;
             dir_loop < p_list->dir_count;
             dir_loop++, current_item++ )
        {
            char* dir = current_item->dir_name;
            struct stat s;
            int err = stat( dir , &s);
            if(-1 == err) {
                valid = 0;
            } else {
                if(!S_ISDIR(s.st_mode)) {
                    /* Not a directory */
                    valid = 0;
                }
            }

            if( valid ) {
                char* dir_formatted = format_dir( dir );
                fprintf( stdout, "%s\n", dir_formatted );
                if( current_item->bookmark_name != NULL ) {
                    fprintf( stdout, "%s\n", current_item->bookmark_name );
                }
                if( dir != dir_formatted ) {
                    free( dir_formatted );
                }
            }
        }
    }
}

int determine_if_term_is_ansi()
{
    int ret_val = 0;
    char* term = getenv("TERM");

    if( term != NULL ) {
        if(( strcmp( term, "vt100" ) == 0 ) ||
           ( strcmp( term, "xterm" ) == 0 )) {
            ret_val = 1;
        }
    }

    return ret_val;
}

#if defined WIN32
WORD TextColour(WORD fontcolor)
{
    HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    WORD wOldColorAttrs;

    GetConsoleScreenBufferInfo(h, &csbiInfo);
    wOldColorAttrs = csbiInfo.wAttributes; 

    /* Seems to return 0 (i.e. fail) from Mintty, but otherwise no adverse
       side-effects */
    SetConsoleTextAttribute(h,fontcolor);

    return wOldColorAttrs;
}
#endif

void dump_dir_list( const dir_list_t p_list )
{
    if( p_list == NULL )
    {
        fprintf( stdout, "Empty dirlist structure\n" );
    } else {
        size_t dir_loop;
        struct dir_list_item* current_item;
        int term_is_ansi = determine_if_term_is_ansi();

        fprintf( stdout, "Dirlist has %d entries of %d used\n",
                 p_list->dir_count, p_list->dir_size );

        for( dir_loop = 0, current_item = p_list->dir_list;
             dir_loop < p_list->dir_count;
             dir_loop++, current_item++ )
        {
#if defined WIN32
            int wcol = -1;
            WORD wOldColorAttrs;
#endif
            char* col = ANSI_COLOUR_RESET;
            char* dir = current_item->dir_name;
            struct stat s;
            int err = stat( dir , &s);
            if(-1 == err) {
                if(ENOENT == errno) {
#if defined WIN32
                    wcol = FOREGROUND_INTENSITY;
#endif
                    col = ANSI_COLOUR_GREY;
                }
            } else {
                if(S_ISDIR(s.st_mode)) {
#if defined WIN32
                    wcol = FOREGROUND_GREEN;
#endif
                    col = ANSI_COLOUR_GREEN;
                } else {
                    /* Not a directory */
#if defined WIN32
                    wcol = FOREGROUND_RED;
#endif
                    col = ANSI_COLOUR_RED;
                }
            }

            fprintf( stdout, "[%3d] ", dir_loop);
#if defined WIN32
            if( wcol != -1 ) {
                wOldColorAttrs = TextColour(wcol);
            }
#endif
            if( term_is_ansi ) {
                fprintf( stdout, "%s", col );
            }
            fprintf( stdout, "%s", dir );
            if(( current_item->bookmark_name != NULL ) &&
               ( current_item->bookmark_name[0] != 0 )) {
                fprintf( stdout, "\n      - Shorthand: %s",
                         current_item->bookmark_name);
            }
            if( current_item->time_added != -1 ) {
                char buffer[ TIME_STRING_BUFFER_SIZE ];

                strftime( buffer, sizeof( buffer ), "%c %Z",
                          gmtime( &( current_item->time_added )));

                fprintf( stdout, "\n      - Added: %s",
                         buffer );
            }
#if defined WIN32
            if( wcol != -1 ) {
                TextColour(wOldColorAttrs);
            }
#endif
            if( term_is_ansi ) {
                fprintf( stdout, "%s", ANSI_COLOUR_RESET );
            }
            printf("\n");
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
            struct dir_list_item* this_item = &(p_list->dir_list[ dir_loop ]);
            fprintf( file, ":%s\n",
                           this_item->dir_name );
            if( this_item->bookmark_name != NULL ) {
                fprintf( file, "N:%s\n",
                               this_item->bookmark_name );
            }
            if( this_item->time_added != -1 ) {
                char buff[ TIME_STRING_BUFFER_SIZE ];

                if (strftime(buff, sizeof( buff ), TIME_FORMAT_STRING, 
                             gmtime( &(this_item->time_added) ))) {
                    fprintf( file, "A:%s\n",buff);
                }
            }
        }

        fclose( file );
        ret_val = 1;
    }

    return( ret_val );
}
