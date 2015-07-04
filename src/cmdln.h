/*
   \file
   \brief The cmdln module deals with reading parameters and options from the
          command line and environment - coverting these from strings into
          a data structure

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

#if !defined CMDLN_H
#define      CMDLN_H

#include <sys/param.h>

/** Indicate what type of operation the user has requested */
typedef enum {
    WD_OPER_NONE,            /**< No operation requested */
    WD_OPER_ADD,             /**< Add a bookmark */
    WD_OPER_REMOVE,          /**< Remove a bookmark */
    WD_OPER_DUMP,            /**< Dump a report on all the current bookmarks */
    WD_OPER_LIST,            /**< List of the bookmark names and destinations */
    WD_OPER_GET_BY_BM_NAME,  /**< Get a bookmark based on the name */
    WD_OPER_GET              /**< Get a bookmark based on either name or
                                  destination */
} wd_oper_t;

/** Status/type of a bookmark destination */
typedef enum {
    WD_ENTITY_ANY,         /**< When filtering, match any type of entity */
    WD_ENTITY_DIR,         /**< Bookmark pointing to a directory */
    WD_ENTITY_FILE,        /**< Bookmark pointing to a file */
    WD_ENTITY_UNKNOWN,     /**< Bookmark pointing to an unknown entity type */
    WD_ENTITY_NONEXISTANT  /**< Bookmark pointing to an entity which doesn't
                                exist in the filesystem */
} wd_entity_t;

/** Specify the format of strings to be output */
typedef enum {
    WD_DIRFORM_NONE,       /**< No specifier - use the default */
    WD_DIRFORM_CYGWIN,     /**< Use /cydrive/c/cygwin/type/paths */
    WD_DIRFORM_WINDOWS     /**< Use C:\windows\style\paths */
} wd_dir_format_t;

/** Specify the formatting for the list */
typedef enum {
    WD_DIRLIST_PLAIN,    /**< No formatting requirements */
    WD_DIRLIST_NUMBERED  /**< Number each list item */
} wd_dir_list_opt_t;

/** Structure to wrap up all of the options/parameters read by this module.
    Should be initialised using init_cmdln() before use */
typedef struct {
    /** Type of operation which the command line has instructed should be
        performed */
    wd_oper_t       wd_oper;
    /** Format in which file paths should be output */
    wd_dir_format_t wd_dir_form;
    /** Options to control the format when displaying a list */
    wd_dir_list_opt_t wd_dir_list_opt;
    /** Directory containing list of bookmarks */
    char*           list_fn;
    /** Directory read from the command line upon which operations should be
        performed */
    char            wd_oper_dir[ MAXPATHLEN ];
    /** Name of a bookmark read from the command line on which operations should
        be performed */
    char*           wd_bookmark_name;
    /** Indicate whether or not to run in "interactive" mode */
    int             wd_prompt;
    /** Indicate whether or not access times should be stored in the bookmarks
    */
    int             wd_store_access;
    /** Time to use as the current time when manipulating datestamps.  Saves
        calls to time() and also allows time to be manipulated for testing
        purposes */
    time_t          wd_now_time;
    /** Control which types of entity should be included in the output */
    wd_entity_t     wd_entity_type;
    /** Control whether or not all items should be output regardless of whether
        or not they seem to point to a valid entry in the current filesystem */
    int             wd_output_all;
    /** Control whether or not the string contents of a list should be escaped.
        May have the value 0 (no escape), 1 (single escape) or 2 (double-escape) */
    int             wd_escape_output;
} config_container_t;

/** Initialise the specified config with default values

    \param[out] p_config The configuration to initialise
*/
int init_cmdln( /*@out@*/ config_container_t* const p_config );

/** Process command line options

    \param[in,out] p_config The structure to add the options to.  Existing
                            options will be overridden if new values are derived
                            based on the contents of the command line.  Other
                            values will be left as-is.
    \param[in] argc Count of command line options
    \param[in] argv String array of command line options to process
*/
int process_cmdln( config_container_t* const p_config, const int argc, char* const argv[] );

/** Process options from the environment (a subset of those that can be used on
    the command line).

    \param[in,out] p_config The structure to add the options to.  Existing
                            options will be overridden if new values are derived
                            based on the contents of the environment.  Other
                            values will be left as-is.
*/
int process_env( config_container_t* const p_config );

#if defined _DEBUG
#define DEBUG_OUT( ... ) do { fprintf(stdout,"wd: " __VA_ARGS__ ); fprintf(stdout,"\n"); fflush(stdout); } while( 0 )
#else
#define DEBUG_OUT( ... )
#endif

#endif
