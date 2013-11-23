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

typedef enum {
    WD_OPER_NONE,
    WD_OPER_ADD
} wd_oper_t;

int process_cmdln( const int argc, char* const argv[] );

extern wd_oper_t wd_oper;
extern char*     list_fn;
#endif
