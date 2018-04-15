Feature: help

  Scenario: User invokes -v parameter to get the version information
    When I run `src/wd -v`
    Then the exit status should be 0
    And the output should match:
"""
wd v1.2 by dev@brightsilence.com\r*
 https://github.com/bright-tools/wd\r*
 Built for: 
"""

  Scenario: User invokes -h parameter to get the help information
    When I run `src/wd -h`
    Then the exit status should be 0
    And the output should match:
"""
wd(.exe)? \[-v\] \[-h\] \[-t\] \[-f <fn>\] \[-r \[dir\] \[-p\]\] \[-a \[dir\]\] \[-d\] \[-l\] \[-s <c>\]\r*
 -v       : Show version information\r*
 -h       : Show usage help\r*
 -d       : Dump bookmark list\r*
 -c       : Escape output\r*
 -C       : Double escape output\r*
 -t       : Store access times for bookmarks\r*
 -l <f>   : List paths & bookmark names \(generally for use in tab\r*
             expansion\)\r*
             f=l : Output paths and bookmarks each on separate lines\r*
             f=p : Output paths only\r*
             f=b : Output bookmarks only\r*
 -e <t>   : Filter output by entity type\r*
             t=a : All types\r*
             t=f : Files only\r*
             t=d : Directories only\r*
             t=F : Files and unknowns\r*
             t=D : Directories and unknowns\r*
 -s <c>   : Format paths for cygwin\r*
 -g <id>  : Get bookmark path.  ID can be index, name or path\r*
 -n <nam> : Get bookmark path with specified shortcut name\r*
 -p       : Prompt for input \(can be used with -r instead of specifying\r*
             path\r*
 -f <fn>  : Use file <fn> for storing bookmarks\r*
 -r \[dir\] : Remove specified path or current directory if none\r*
 -a \[dir\] : Add specified path or current directory if none\r*
             specified
"""