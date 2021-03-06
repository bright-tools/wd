/*
   Copyright 2015 John Bailey

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

#if !defined WD_H
#define      WD_H

#define WD_SUCCESS -1
#define WD_GENERIC_FAIL 0
#define WD_SUCCEEDED( _x ) (( _x ) == WD_SUCCESS )

#if defined WIN32
/** Printf format string for size_t */
#define PFFST "%Iu"
#define PFF3ST "%3Iu"
#else
#define PFFST "%zu"
#define PFF3ST "%3zu"
#endif

#endif
