Feature: help

  Scenario: User invokes -v parameter to get the version information
    When I run `src/wd -v`
    Then the exit status should be 0
    And the output should match:
    """
wd v1.2 by dev@brightsilence.com\r*
\shttps://github.com/bright-tools/wd\r*
\sBuilt for: 
    """

  Scenario: User invokes -h parameter to get the help information
    When I run `src/wd -h`
    Then the exit status should be 0
    And the output should match:
    """
wd(.exe)? \[-v\] \[-h\] \[-t\] \[-f <fn>\] \[-r \[dir\] \[-p\]\] \[-a \[dir\]\] \[-d\] \[-l\] \[-s <c>\]\r*
\s-v       : Show version information\r*
\s-h       : Show usage help\r*
\s-d       : Dump bookmark list\r*
\s-c       : Escape output\r*
\s-C       : Double escape output\r*
\s-t       : Store access times for bookmarks\r*
\s-l <f>   : List paths & bookmark names \(generally for use in tab\r*
\s            expansion\)\r*
\s            f=l : Output paths and bookmarks each on separate lines\r*
\s            f=p : Output paths only\r*
\s            f=b : Output bookmarks only\r*
\s-e <t>   : Filter output by entity type\r*
\s            t=a : All types\r*
\s            t=f : Files only\r*
\s            t=d : Directories only\r*
\s            t=F : Files and unknowns\r*
\s            t=D : Directories and unknowns\r*
\s-s <c>   : Format paths for cygwin\r*
\s-g <id>  : Get bookmark path.  ID can be index, name or path\r*
\s-n <nam> : Get bookmark path with specified shortcut name\r*
\s-p       : Prompt for input \(can be used with -r instead of specifying\r*
\s            path\r*
\s-f <fn>  : Use file <fn> for storing bookmarks\r*
\s-r \[dir\] : Remove specified path or current directory if none\r*
\s-a \[dir\] : Add specified path or current directory if none\r*
\s            specified
    """