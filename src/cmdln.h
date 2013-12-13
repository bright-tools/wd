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

#if !defined CMDLN_H
#define      CMDLN_H

#include <sys/param.h>

typedef enum {
    WD_OPER_NONE,
    WD_OPER_ADD,
    WD_OPER_REMOVE,
    WD_OPER_DUMP,
    WD_OPER_LIST,
    WD_OPER_GET_BY_BM_NAME,
    WD_OPER_GET
} wd_oper_t;

typedef enum {
    WD_ENTITY_ANY,
    WD_ENTITY_DIR,
    WD_ENTITY_FILE,
    WD_ENTITY_UNKNOWN,
    WD_ENTITY_NONEXISTANT
} wd_entity_t;

typedef enum {
    WD_DIRFORM_NONE,
    WD_DIRFORM_CYGWIN,
    WD_DIRFORM_WINDOWS
} wd_dir_format_t;

typedef struct {
    /** Type of operation which the command line has instructed should be
        performed */
    wd_oper_t       wd_oper;
    /** Format in which file paths should be output */
    wd_dir_format_t wd_dir_form;
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
} config_container_t;

/** Initialise the specified config with default values

    \param[in] p_config The configuration to initialise
*/
void init_cmdln( config_container_t* const p_config );
int process_cmdln( config_container_t* const p_config, const int argc, char* const argv[] );
int process_env( config_container_t* const p_config );

#define _DEBUG
#if defined _DEBUG
#define DEBUG_OUT( ... ) do { fprintf(stdout,"wd: " __VA_ARGS__ ); fprintf(stdout,"\n"); fflush(stdout); } while( 0 )
#else
#define DEBUG_OUT( ... )
#endif

#endif
