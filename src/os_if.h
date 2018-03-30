/*
   Copyright 2018 John Bailey

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

#if !defined OS_IF_H
#define      OS_IF_H

char* get_home_dir( void );
void  release_home_dir( char* p_dir );
void  canonicalize_dir( const char* const p_dir, char* const p_target );

#endif