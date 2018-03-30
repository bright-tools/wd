/*
   Copyright 2013-15 John Bailey

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

#include "wd.h"
#include "cmdln.h"
#include "dir_list.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
/* Just for the define values */
#include <errno.h>

#if defined WIN32
#include <io.h>
#include <fcntl.h>
#endif

/** Handle removal of an item from the dir list interactively using stdin/stdout
 *
 *  @param p_cmd      String referencing the executing program (e.g. c:\something\wd.exe)
 *  @param p_dir_list Dirlist to operate on
 *  @return WD_SUCCESS in the case of successful removal
 *          WD_GENERIC_FAIL otherwise
 */
static int do_remove_interactive( const char* const               p_cmd, 
                                  dir_list_t                      p_dir_list )
{
    size_t index = 0;
    int ret_val = WD_GENERIC_FAIL;
    int success;

    /* Precondition check */
    assert( p_cmd != NULL );
    assert( p_dir_list != NULL );
    /* !Precondition check */

    dump_dir_list( p_dir_list );

    fprintf(stdout,"Enter number to remove: ");

    /* Flush the output to ensure that the user knows that we're waiting for
       some input.  This is necessary when, for example, running the Win32
       version of this app within MinTTy on Cygwin */
    (void)fflush(stdout);

    success = fscanf(stdin,"%u",&index);

    if( success )
    {
        success = remove_dir_by_index( p_dir_list, index );
        if( success )
        {
            ret_val = WD_SUCCESS;
        }
        else
        {
            fprintf(stderr, "%s: Error: Invalid index '%u'\n",
                    p_cmd, index);
        }
    }
    else
    {
        fprintf(stderr, "%s: Entry was not a positive integer\n",
                p_cmd);
    }

    return ret_val;
}

/** Handle removal of an item from the dir list, potentially interactively
 *   (dependent on p_config->wd_prompt) using stdin/stdout
 *
 *  @param p_config   User's preferences
 *                    If wd_prompt is set then the directory will be prompted
 *                    for interactively.  Otherwise wd_oper_dir must be set to
 *                    sepcify the directory to be removed
 *  @param p_cmd      String referencing the executing program (e.g. c:\something\wd.exe)
 *  @param p_dir_list Dirlist to operate on
 *  @return WD_SUCCESS in the case of successful removal
 *          WD_GENERIC_FAIL otherwise
 */
static int do_remove( const config_container_t* const p_config, 
                      const char* const               p_cmd, 
                      dir_list_t                      p_dir_list )
{
    int ret_val = 0;

    /* Precondition check */
    assert( p_cmd != NULL );
    assert( p_dir_list != NULL );
    assert( p_config != NULL );
    assert( p_config->wd_prompt || ( p_config->wd_oper_dir != NULL ));
    /* !Precondition check */

    /* Interactive? */
    if( p_config->wd_prompt )
    {
        ret_val = do_remove_interactive( p_cmd, p_dir_list ); 
    }
    else
    {
        /* Non-interactive - use the directory specified in the config */
        ret_val = remove_dir( p_dir_list, p_config->wd_oper_dir );

        if( !WD_SUCCEEDED( ret_val ))
        {
            fprintf(stderr, "%s: Warning: Directory not in list: '%s'\n",
                    p_cmd, p_config->wd_oper_dir);
        }
    }

    return ret_val;
}

/** Dump the dirlist entry with the specified index or name 
 *  (p_config->wd_bookmark_name) to the output stream.  
 *
 *  wd_bookmark_name is checked in the following order or priority:
 *    1) If it is numeric, it is treated as an index into the dirlist
 *    2) If a dirlist entry with a matching name exists, this is used
 *    3) If a directory with a matching name exists, this is used
 *
 *  If p_config->wd_store_access is set then the corresponding entry's timestamp
 *  will be updated appropriately
 *
 *  @param  p_config   Program settings.  
 *  @param  p_cmd      String referencing the executing program (e.g. c:\something\wd.exe)
 *  @param  p_dir_list The directory list to search                
 *  @return WD_SUCCESS in the case that the directory list has been modified
 *          WD_GENERIC_FAIL otherwise
 */
static int do_get( const config_container_t* const p_config, 
                   const char* cmd, dir_list_t p_dir_list )
{
    size_t idx;
    int dir_list_needs_save = 0;

    /* Precondition check */
    assert( cmd != NULL );
    assert( p_dir_list != NULL );
    assert( p_config != NULL );
    assert( p_config->wd_bookmark_name != NULL );
    /* !Precondition check */

    /* Try to convert the string representing the entry's index to integer */
    if( sscanf( p_config->wd_bookmark_name, "%u", &idx ) == 1 ) 
    {
        /* Bounds check */
        if( dir_list_get_count( p_dir_list ) > idx )
        {
            dir_list_needs_save = dump_dir_with_index( p_dir_list, idx ) && 
                                  p_config->wd_store_access;
        } 
        else 
        {
            fprintf(stderr, "%s: Error: Index %u doesn't exist\n",
                    cmd, idx);
        }
    }
    else if(( dump_dir_with_name( p_dir_list, p_config->wd_bookmark_name ) ||
              dump_dir_if_exists( p_dir_list, p_config->wd_bookmark_name )) &&
            p_config->wd_store_access ) 
    {
        /* We're updating access times, so flag that the list needs
           saving */
        dir_list_needs_save = 1;
    }
    else
    {
        /* Wasn't an index or an named entry or a directory */
        fprintf(stderr, "%s: Error: Couldn't find an appropriate entry for '%s'\n",
                cmd, p_config->wd_bookmark_name);
    }

    return dir_list_needs_save;
}

/** Dump the dirlist entry with the specified name (p_config->wd_bookmark_name) 
 *  to the output stream.  
 *
 *  If p_config->wd_store_access is set then the corresponding entry's timestamp
 *  will be updated appropriately
 *
 *  @param  p_config   Program settings.  
 *  @param  p_dir_list The directory list to search                
 *  @return WD_SUCCESS in the case that the directory list has been modified
 *          WD_GENERIC_FAIL otherwise
 */
static int do_get_by_name( const config_container_t* const p_config, 
                           dir_list_t p_dir_list )
{
    int dir_list_needs_save = 0;

    /* Precondition check */
    assert( p_dir_list != NULL );
    assert( p_config != NULL );
    assert( p_config->wd_bookmark_name != NULL );
    /* !Precondition check */

    if( dump_dir_with_name( p_dir_list, p_config->wd_bookmark_name ) && p_config->wd_store_access ) 
    {
        /* We're updating access times, so flag that the list needs
           saving */
        dir_list_needs_save = 1;
    }
    return dir_list_needs_save;
}

/** Add the specified (p_config->wd_oper_dir) directory to the dirlist.
 *  If a name is specified (p_config->wd_bookmark_name) then this will be
 *  associated with the bookmark.
 *  The function will check to collisions (either the directory or the bookmark
 *  name) before changing the dirlilst
 *
 *  @param  p_config   Program settings.  
 *  @param  p_cmd      String referencing the executing program (e.g. c:\something\wd.exe)
 *  @param  p_dir_list The directory list to which the directory should be added
 *  @return WD_SUCCESS in the case that the directory list has been modified
 *          WD_GENERIC_FAIL otherwise
 */
static int do_add( const config_container_t* const p_config, 
                   const char* cmd, dir_list_t p_dir_list )
{
    int dir_list_needs_save = 0;

    /* Precondition check */
    assert( p_dir_list != NULL );
    assert( p_config != NULL );
    assert( p_config->wd_oper_dir != NULL );
    /* !Precondition check */

    /* TODO: Consider allowing directory to be added twice with
       different bookmark name? */
    if( dir_in_list( p_dir_list, p_config->wd_oper_dir )) 
    {
        fprintf(stderr,
                "%s: Warning: Directory already in list: '%s'\n",
                cmd, p_config->wd_oper_dir);
    } 
    else if( (p_config->wd_bookmark_name != NULL) &&
             bookmark_in_list( p_dir_list, p_config->wd_bookmark_name )) 
    {
        fprintf(stderr,
                "%s: Warning: Bookmark name already in list: '%s'\n",
                cmd, p_config->wd_bookmark_name);
    } 
    else 
    {
        time_t a_time = -1;
        if( p_config->wd_store_access ) 
        {
            a_time = p_config->wd_now_time;
        }
        if( WD_SUCCEEDED( add_dir( p_dir_list,
                                   p_config->wd_oper_dir,
                                   p_config->wd_bookmark_name,
                                   p_config->wd_now_time,
                                   a_time,
                                   WD_ENTITY_UNKNOWN ) ))
        {
            dir_list_needs_save = 1;
        } 
        else 
        {
            fprintf(stderr,
                    "%s: Error: Failed to add bookmark to: '%s'\n",
                    cmd, p_config->wd_bookmark_name);
            /* TODO: Be a little bit more verbose regarding why? */
        }
    }
    return dir_list_needs_save;
}

/** Display an error based on the parameters
 *
 * @param p_err    Error code, using errno value representations
 * @param p_action Action that was being performed at the time the error was
 *                 encountered
 * @param p_loc    A textual representation of the location at which the error 
 *                 was found (e.g. function name)
 */
static void display_err( const int p_err, 
                         const char* const p_action, 
                         const char* const p_loc )
{
    /* Precondition check */
    assert( p_action != NULL );
    assert( p_loc != NULL );
    /* !Precondition check */

    fprintf(stderr,"Failed to %s during %s\n",p_action,p_loc);
    switch( p_err )
    {
        case ENOENT:
            fprintf(stderr,"Home directory could not be found\n");
            break;
        case ENOMEM:
            fprintf(stderr,"Out of memory\n");
            break;
        default:
            break;
    }
}

/** Invoke the approporiate operation on the specified dirlist based on the 
 *  value of cfg->wd_oper
 *
 *  @param  p_config   Program settings.  
 *  @param  p_dir_list The directory list which the operation is to be applied
 *                      to
 *  @param  argc       Command line argument count
 *  @param  argv       Command line argument strings
 */
static void perform_op( const config_container_t* cfg, 
                        dir_list_t dir_list,
                        /*@unused@*/ int argc, char* argv[] )
{
    int dir_list_needs_save = 0;

    /* Precondition check */
    assert( cfg != NULL );
    assert( dir_list != NULL );
    assert( argv != NULL );
    /* !Precondition check */

    switch( cfg->wd_oper ) {
        case WD_OPER_REMOVE:
            DEBUG_OUT("WD_OPER_REMOVE: %s",cfg->wd_bookmark_name);
            dir_list_needs_save = do_remove( cfg, argv[0], dir_list );
            break;
        case WD_OPER_GET:
            DEBUG_OUT("WD_OPER_GET: %s",cfg->wd_bookmark_name);
            dir_list_needs_save = do_get( cfg, argv[0], dir_list );
            break;
        case WD_OPER_GET_BY_BM_NAME:
            DEBUG_OUT("WD_OPER_GET_BY_BM_NAME: %s",cfg->wd_bookmark_name);
            dir_list_needs_save = do_get_by_name( cfg, dir_list );
            break;
        case WD_OPER_ADD:
            DEBUG_OUT("WD_OPER_ADD: %s",cfg->wd_bookmark_name);
            dir_list_needs_save = do_add( cfg, argv[0], dir_list );
            break;
        case WD_OPER_DUMP:
            dump_dir_list( dir_list );
            break;
        case WD_OPER_LIST:
#if defined WIN32
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
        if( !WD_SUCCEEDED( save_dir_list( dir_list, cfg->list_fn ) ) )
        {
            fprintf(stderr,"Error saving dir list\n");
            /* TODO: Be a little bit more verbose regarding why? */
        }
    }
}

/** Check to see if a dirlist operation is required and in the case that it is,
 *  load the dirlist and invoke the operation.  If the dirlist is changed as a
 *  result of the operation, it will then be saved
 *
 *  @param  p_config   Program settings.  
 *  @param  argc       Command line argument count
 *  @param  argv       Command line argument strings
 */
static void handle_op( const config_container_t* p_config, 
                       /*@unused@*/ int argc, char* argv[] )
{
    /* Precondition check */
    assert( p_config != NULL );
    assert( argv != NULL );
    /* !Precondition check */

    /* Anything to actually do?  Might not be in the case, for
     * example, that command line was invalid or just requesting the
     * 'help' output */
    if( p_config->wd_oper != WD_OPER_NONE ) 
    {
        dir_list_t dir_list = NULL;

        DEBUG_OUT("loading bookmark file %s", p_config->list_fn);

        dir_list = load_dir_list( p_config, p_config->list_fn );

        if( dir_list == NULL ) 
        {
            fprintf(stderr,"%s: Warning: Unable to load list file '%s'\nCreating empty list\n",
                    argv[0], p_config->list_fn);
            dir_list = new_dir_list();
        }

        DEBUG_OUT("loaded bookmark file");

        perform_op( p_config, dir_list, argc, argv );

        free( dir_list );
    }
}

/** main() function for wd.  Reads the parameters from the command line &
 *  environment and then performs any required operation on the specified dirlist
 *  see cmdln.c::show_help() for a list of possible options/operations
 *
 *  @param  argc       Command line argument count
 *  @param  argv       Command line argument strings
 */
int main( int argc, char* argv[] )
{
    int ret_code = 0;
    int fn_result = 0;

    config_container_t cfg;

    /* Initialise configuration */
    fn_result = init_cmdln( &cfg );

    if( WD_SUCCEEDED( fn_result )) 
    {
        /* Process settings from environment variable(s) */
        fn_result = process_env( &cfg );

        if( WD_SUCCEEDED( fn_result )) 
        {
            /* Process settings from the command-line, potentially overriding
             * those from the environment */
            fn_result = process_cmdln( &cfg, argc, argv );

            if( WD_SUCCEEDED( fn_result )) 
            {
                DEBUG_OUT("command line processed");

                handle_op( &cfg, argc, argv );

                /* TODO: be more selective about setting this - it's possible
                 * that the processing of the operation will fail */
                ret_code = EXIT_SUCCESS;
            } 
            else 
            {
                /* No call to display_err here - cmdln takes care of displaying
                 * its own errors */
                ret_code = EXIT_FAILURE;
            }
        } 
        else 
        {
            display_err( fn_result, "process environment", "process_env" );
            ret_code = EXIT_FAILURE;
        }
    } 
    else 
    {
        display_err( fn_result, "initialise", "init_cmdln" );
        ret_code = EXIT_FAILURE;
    }

    DEBUG_OUT("all done");
    return ret_code;
}
