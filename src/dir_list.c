/*
   Copyright 2013-2014 John Bailey

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

#define USE_FAVOURITES_FILE_STR "USE_FAVOURITES"

#define TIME_FORMAT_STRING "%Y/%m/%d %H:%M:%S"
#define TIME_SSCAN_STRING  "%04u/%02u/%02u %02u:%02u:%02u"
#define TIME_STRING_BUFFER_SIZE (30U)

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

/* TODO: Since chage #6, we support files as well as directories, so all of the
   "dir" references in this file are a little misleading */

struct dir_list_item
{
    char*       dir_name;
    char*       bookmark_name;
    time_t      time_added;
    time_t      time_accessed;
    wd_entity_t type;
    /* TODO: Other data here?  Time last usedc
       Meta-data such as whether it exists?  Shortcut name? */
};

#define DLI_SIZE (sizeof( struct dir_list_item ))

struct dir_list_s
{
    size_t                dir_count;
    struct dir_list_item* dir_list;
    int                   dir_size;

    /* TODO: Is it the best thing to store the config here?  config contains
       things that this class doesn't care about */
    const config_container_t* cfg;
};

static wd_entity_t get_type( const char* const p_path );
static dir_list_t load_dir_list_from_file( const config_container_t* const p_config,
                                           const char* const p_fn );
#if defined WIN32
static dir_list_t load_dir_list_from_favourites( const config_container_t* const p_config,
                                                 const char* const p_fn );
#endif


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

size_t     dir_list_get_count( const dir_list_t p_list )
{
    return( p_list->dir_count );
}

int add_dir( dir_list_t p_list,
             const char* const p_dir,
             const char* const p_name,
             const time_t      p_t_added,
             const time_t      p_t_accessed,
             const wd_entity_t p_type )
{
    /* TODO: Check that item is of type p_list->cfg->wd_entity_type? */
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
                p_list->dir_list[ idx ].time_accessed = p_t_accessed;
                if( p_type == WD_ENTITY_UNKNOWN ) {
                    p_list->dir_list[ idx ].type = get_type( dest );
                } else {
                    p_list->dir_list[ idx ].type = p_type;
                }

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

static time_t sscan_time( const char* const p_str )
{
    time_t ret_val;
    struct tm tm;
    tm.tm_isdst = -1;
    tm.tm_yday = -1;
    tm.tm_wday = -1;
    sscanf(p_str, TIME_SSCAN_STRING,
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

    /* This call will ensure that tm_yday is calculated - we need that for the
       conversion below */
    mktime( &tm );

    /* The time in the file is already UTC, so using
       mktime is the wrong thing to do, as this assumes that
       the contents of tm is in local time
       see http://pubs.opengroup.org/onlinepubs/007904975/basedefs/xbd_chap04.html#tag_04_14 */
    ret_val = tm.tm_sec +
              tm.tm_min*60 +
              tm.tm_hour*3600 +
              tm.tm_yday*86400 +
              (tm.tm_year-70)*31536000 +
              ((tm.tm_year-69)/4)*86400 -
              ((tm.tm_year-1)/100)*86400 +
              ((tm.tm_year+299)/400)*86400;

    return ret_val;
}

dir_list_t load_dir_list( const config_container_t* const p_config, const char* const p_fn )
{
    dir_list_t ret_val = NULL;
#if defined _WIN32
    if( 0 == strcmp( p_fn, USE_FAVOURITES_FILE_STR )) {
        ret_val = load_dir_list_from_favourites( p_config, p_fn );
    } else {
#endif
        ret_val = load_dir_list_from_file( p_config, p_fn );
#if defined _WIN32
    }
#endif
    return ret_val;
}

#if defined _WIN32
#include "shrtcut.h"
#include <shlobj.h>
#include <dirent.h>
static dir_list_t load_dir_list_from_favourites( const config_container_t* const p_config, const char* const p_fn )
{
    dir_list_t ret_val = NULL;
    TCHAR path[MAX_PATH];
    HRESULT hr = SHGetFolderPath(0, CSIDL_FAVORITES, 0, 0, path);
 
    if (SUCCEEDED(hr)) {
        DIR* FD;
        DEBUG_OUT("got favourites drectory: %s", path);
        /* TODO: recurse sub-directories */

        CoInitialize( NULL );

        if (NULL != (FD = opendir (path))) 
        {
            struct dirent* in_file;
            DEBUG_OUT("opened favourites drectory: %s", path);
            ret_val = new_dir_list();
            ret_val->cfg = p_config;
            while ((in_file = readdir(FD))) 
            {
                if((0 != strcmp (in_file->d_name, ".")) &&
                   (0 != strcmp (in_file->d_name, "..")))
                {
                    size_t len = strlen( in_file->d_name );
                    if(( len > 4 ) &&
                       ( 0 == stricmp( &(in_file->d_name[len-4]), ".lnk" ))) {
                        char shortcut_full[ MAXPATHLEN ];
                        char dest_path[ MAXPATHLEN ];
                        char name[ MAXPATHLEN ];
                        time_t added;
                        time_t accessed;
                        wd_entity_t ent_type;
                        struct stat s;
                        int err;

                        DEBUG_OUT("found file: %s",in_file->d_name);

                        /* TODO */
                        dest_path[0] = 0;
                        accessed = -1;
                        added = -1;
                        ent_type = WD_ENTITY_UNKNOWN;


                        strcpy( shortcut_full, path );
                        strcat( shortcut_full, "\\" );
                        strcat( shortcut_full, in_file->d_name );

                        err = stat( shortcut_full, &s);
                        if( err == -1 ) {
                            /* TODO */
                        } else {
                            /* TODO: Are these really the appropriate attributes
                               to use? */
                            added = s.st_ctime;
                            accessed = s.st_atime;
                            ResolveIt( NULL, shortcut_full, dest_path,
                                    MAXPATHLEN );
                        }

                        strncpy( name, in_file->d_name, len-4 );
                        name[len-4] = 0;


 
                        add_dir( ret_val, dest_path, name, added, accessed,
                                 ent_type );

                        DEBUG_OUT("created new bookmark");
                    }
                }
            }
        }
        
        CoUninitialize();
    }
    return ret_val;
}
#endif

static dir_list_t load_dir_list_from_file( const config_container_t* const p_config, const char* const p_fn )
{
    FILE* file = fopen( p_fn, "rt" );
    dir_list_t ret_val = NULL;

    if( file != NULL ) {
        DEBUG_OUT("opened bookmark file");
        ret_val = new_dir_list();
        ret_val->cfg = p_config;

        if( ret_val != NULL ) {
            char path[ MAXPATHLEN ];
            char read[ MAXPATHLEN ];
            char name[ MAXPATHLEN ];
            time_t added;
            time_t accessed;
            char* fstr;
            int one_last_go = 1;
            wd_entity_t ent_type;

            path[0] = 0;
            name[0] = 0;
            added = -1;
            accessed = -1;
            ent_type = WD_ENTITY_UNKNOWN;
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
                            add_dir( ret_val, path, name, added, accessed,
                                     ent_type );

                            DEBUG_OUT("created new bookmark");

                            path[0] = 0;
                            name[0] = 0;
                            added = -1;
                            accessed = -1;
                            ent_type = WD_ENTITY_UNKNOWN;
                        }
                        strcpy( path, &(read[1]) );
                    } else if(( read[0] == 'N' ) &&
                              ( read[1] == ':' )) {
                        strcpy( name, &(read[2]) );
                    } else if(( read[0] == 'A' ) &&
                              ( read[1] == ':' )) {
                        added = sscan_time(&(read[2]));
                    } else if(( read[0] == 'C' ) &&
                              ( read[1] == ':' )) {
                        accessed = sscan_time(&(read[2]));
                    } else if(( read[0] == 'T' ) &&
                              ( read[1] == ':' )) {
                        switch(read[2]) {
                            case 'D':
                                ent_type = WD_ENTITY_DIR;
                                break;
                            case 'F':
                                ent_type = WD_ENTITY_FILE;
                                break;
                            default:
                                ent_type = WD_ENTITY_UNKNOWN;
                                break;
                        }
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

#define CYGDRIVE_PREFIX "/cygdrive/"

/** String length of CYGDRIVE_PREFIX, here as a constant to save repeated strlen
    calls */
#define CYGDRIVE_PREFIX_LEN 10U

char* escape_string( int p_escape, char* p_str )
{
    char* ret_val;
    char* dest;
    char* src = p_str;

    if( p_escape == 0 ) {
        ret_val = p_str;
    } else {
        ret_val = (char*)malloc( (strlen( p_str )*3) + 1 );
        for( dest = ret_val;
             *src != '\0';
             dest++, src++ ) {
            if(( *src == ' ' ) || 
               ( *src == '\\' )) {
                *dest = '\\';
                dest++;
                if( p_escape > 1 ) {
                    *dest = '\\';
                    dest++;
                }
            }
            *dest = *src;
        }
        *dest = 0;
    }

    return ret_val;
}

char* format_dir( wd_dir_format_t p_fmt, int p_escape, char* p_dir ) {
    char* ret_val = NULL;

    switch( p_fmt ) {
        case WD_DIRFORM_NONE:
            ret_val = p_dir;
            break;
        case WD_DIRFORM_CYGWIN: {
            char* dest;
            char* src = p_dir;
            ret_val = (char*)malloc( (strlen( p_dir )*2) + CYGDRIVE_PREFIX_LEN + 4 );
            dest = ret_val;
            if(((( src[0] >= 'a' ) && (src[0] <= 'z' )) ||
                (( src[0] >= 'A' ) && (src[0] <= 'Z' ))) &&
               ( src[1] == ':' ) &&
               (( src[2] == '\\') ||
                ( src[2] == '/' )))
            {
                strcpy( ret_val, CYGDRIVE_PREFIX );
                dest += CYGDRIVE_PREFIX_LEN;
                *dest = *src;
                dest++;
                src += 2;
            }
            for( ;
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
       case WD_DIRFORM_WINDOWS: {
            char* dest;
            char* src;
            ret_val = (char*)malloc( (strlen( p_dir )*2) + 1 );
            src = p_dir;
            dest = ret_val;
            if( strncmp( src, CYGDRIVE_PREFIX, CYGDRIVE_PREFIX_LEN ) == 0 )
            {
                src += CYGDRIVE_PREFIX_LEN;
                *dest = *src;
                dest++;
                *dest = ':';
                dest++;
                src += 1;
            }
            for( ;
                 *src != '\0';
                 dest++, src++ ) {
                if( *src == '/' ) {
                    *dest = '\\';
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
    
    if( p_escape ) {
        char* src;
        char* f = NULL;
        if( ret_val == p_dir ) {
            src = p_dir;
        } else {
            src = ret_val;
            f = src;
        }
        ret_val = escape_string( p_escape, src );
        if( f != NULL ) {
            free( f );
        }

    }


    return( ret_val );
}

static void dump_dir( const dir_list_t p_list, struct dir_list_item* p_item )
{
    char* dir_formatted = format_dir( p_list->cfg->wd_dir_form,
                                      p_list->cfg->wd_escape_output,
                                      p_item->dir_name );
    fprintf( stdout, "%s", dir_formatted );

    if( p_item->dir_name != dir_formatted ) {
        free( dir_formatted );
    }

    if( p_list->cfg->wd_store_access ) {
        p_item->time_accessed = p_list->cfg->wd_now_time;
    }
}

int        dump_dir_if_exists( const dir_list_t p_list, const char* const p_dir )
{
    size_t dir_loop;
    struct dir_list_item* current_item;
    int found = 0;

    for( dir_loop = 0, current_item = p_list->dir_list;
         dir_loop < p_list->dir_count;
         dir_loop++, current_item++ )
    {
        /* TODO: Case insensitive on Windows?  Case insensitive switch? */
        if(( current_item->bookmark_name != NULL ) &&
           ( 0 == strcmp( p_dir, current_item->dir_name ))) {

            dump_dir( p_list, current_item );
            found = 1;
            break;
        }
    }
    return( found );
}

int dump_dir_with_index( const dir_list_t p_list, const unsigned p_idx )
{
    int found = 0;

    if( p_idx < p_list->dir_count ) {
        struct dir_list_item* current_item = &( p_list->dir_list[ p_idx ] );
        dump_dir( p_list, current_item );
        found = 1;
    }

    return( found );
}

int dump_dir_with_name( const dir_list_t p_list, const char* const p_name )
{
    size_t dir_loop;
    struct dir_list_item* current_item;
    int found = 0;

    for( dir_loop = 0, current_item = p_list->dir_list;
         dir_loop < p_list->dir_count;
         dir_loop++, current_item++ )
    {
        if(( current_item->bookmark_name != NULL ) &&
           ( 0 == strcmp( p_name, current_item->bookmark_name ))) {

            dump_dir( p_list, current_item );
            found = 1;
            break;
        }
    }

    return( found );
}

static wd_entity_t get_type( const char* const p_path )
{
    wd_entity_t ret_val = WD_ENTITY_UNKNOWN;
    struct stat s;
    int err = stat( p_path , &s);

    if( err == -1 )
    {
        if(ENOENT == errno) {
            ret_val = WD_ENTITY_NONEXISTANT;
        }
    } else {
        if( S_ISDIR(s.st_mode) ) {
            ret_val = WD_ENTITY_DIR;
        } else if( S_ISREG( s.st_mode )) {
            ret_val = WD_ENTITY_FILE;
        }
    }

    return ret_val;
}

void list_dirs( const dir_list_t p_list )
{
    if( p_list == NULL )
    {
        fprintf( stdout, "Empty dirlist structure\n" );
    } else {
        size_t dir_loop;
        struct dir_list_item* current_item;
        int valid;

        for( dir_loop = 0, current_item = p_list->dir_list;
             dir_loop < p_list->dir_count;
             dir_loop++, current_item++ )
        {
            char* dir = current_item->dir_name;
            valid = 1;
            /* TODO: Update current_item->type here? */
            wd_entity_t type = get_type( dir );

            /* Check to see if this item matches the filter */
            if((p_list->cfg->wd_entity_type != WD_ENTITY_ANY) &&
               (p_list->cfg->wd_entity_type != type )) {
                /* No, don't output */
                valid = 0;
            } else if( 0 == p_list->cfg->wd_output_all ) {

                /* Check to see if it's other than a file or directory (ie. an
                   invalid entity */
                if(( type != WD_ENTITY_DIR ) &&
                   ( type != WD_ENTITY_FILE ))
                {
                    valid = 0;
                }
            }

            if( valid ) {
                char* dir_formatted = format_dir( p_list->cfg->wd_dir_form,
                                                  p_list->cfg->wd_escape_output,
                                                  dir );
                if( p_list->cfg->wd_dir_list_opt == WD_DIRLIST_NUMBERED ) {
                    /* Using dir_loop here may mean that we get non-contiguous
                       numbers on the output, however this is preferable to
                       having to iterate the list to check for validity of each
                       item when looking up the index on a subsequent operation
                       */
                    fprintf( stdout, "%u ", dir_loop );
                }
                fprintf( stdout, "%s\n", dir_formatted );
                if( current_item->bookmark_name != NULL ) {
                    char* name_escaped = escape_string( p_list->cfg->wd_escape_output,
                                                        current_item->bookmark_name );
                    if( p_list->cfg->wd_dir_list_opt == WD_DIRLIST_NUMBERED ) {
                        fprintf( stdout, "%u ", dir_loop );
                    }
                    fprintf( stdout, "%s\n", name_escaped );
                    if( name_escaped != current_item->bookmark_name ) {
                        free( name_escaped );
                    }
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

static void dump_time( const char* const p_header, const time_t* const p_time )
{
    char buffer[ TIME_STRING_BUFFER_SIZE ];

    strftime( buffer, sizeof( buffer ), "%c %Z",
              gmtime( p_time ));

    fprintf( stdout, "\n      - %s: %s",
                     p_header,
                     buffer );
}

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
            char* dir_formatted;

            current_item->type = get_type( dir );

            if( current_item->type == WD_ENTITY_NONEXISTANT ) {
#if defined WIN32
                wcol = FOREGROUND_INTENSITY;
#endif
                col = ANSI_COLOUR_GREY;
            } else if((p_list->cfg->wd_entity_type != WD_ENTITY_ANY) &&
                      (p_list->cfg->wd_entity_type != current_item->type )) {
#if defined WIN32
                wcol = FOREGROUND_RED;
#endif
                col = ANSI_COLOUR_RED;
            } else {
#if defined WIN32
                wcol = FOREGROUND_GREEN;
#endif
                col = ANSI_COLOUR_GREEN;
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

            dir_formatted = format_dir( p_list->cfg->wd_dir_form,
                                        p_list->cfg->wd_escape_output,
                                        dir );
            fprintf( stdout, "%s", dir_formatted );

            if( current_item->dir_name != dir_formatted ) {
                fprintf( stdout, "\n      - Unconverted: %s",
                         dir);
                free( dir_formatted );
            }

            if(( current_item->bookmark_name != NULL ) &&
               ( current_item->bookmark_name[0] != 0 )) {
                fprintf( stdout, "\n      - Shorthand: %s",
                         current_item->bookmark_name);
            }
            if( current_item->time_added != -1 ) {
                dump_time( "Added", &( current_item->time_added ) );
            }
            if( current_item->time_accessed != -1 ) {
                dump_time( "Accessed", &( current_item->time_accessed ) );
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
    /* TODO: File locking here?  flock?  lockf? */

    if( file != NULL ) {
        size_t dir_loop;
        fprintf( file, "%s\n%s\n", FILE_HEADER_DESC_STRING,
                                   FILE_HEADER_VER_STRING );

        for( dir_loop = 0; dir_loop < p_list->dir_count; dir_loop++ )
        {
            struct dir_list_item* this_item = &(p_list->dir_list[ dir_loop ]);
            char*  type_string;
            fprintf( file, ":%s\n",
                           this_item->dir_name );
            if(( this_item->bookmark_name != NULL ) &&
               ( this_item->bookmark_name[0] != 0 )) {
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
            if( this_item->time_accessed != -1 ) {
                char buff[ TIME_STRING_BUFFER_SIZE ];

                if (strftime(buff, sizeof( buff ), TIME_FORMAT_STRING, 
                             gmtime( &(this_item->time_accessed) ))) {
                    fprintf( file, "C:%s\n",buff);
                }
            }
            
            /* Refresh the type.
               TODO: This may be over-zealous if it has already been done */
            this_item->type = get_type( this_item->dir_name );

            switch( this_item->type )
            {
                case WD_ENTITY_DIR:
                    type_string = "D";
                    break;
                case WD_ENTITY_FILE:
                    type_string = "F";
                    break;
                default:
                    /* Unknown */
                    type_string = "U";
                    break;
            }
            fprintf( file, "T:%s\n",type_string);
        }

        fclose( file );
        ret_val = 1;
    }

    return( ret_val );
}
