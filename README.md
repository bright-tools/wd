wd
==

Utility to store and set the current working directory (cwd) in a command-line
shell.

wd could be considered to be a system for bookmarking favourite directories 
and then quickly jumping back to those bookmarks

wd is intended to be very light-weight with a minimum of dependencies - that's
why it's implemented in C as opposed to (for example) Python.  It's also
intended to be independent of the shell (so, for example, it could be used on
the Windows command prompt as well as BASH or ZSH).

Usage
=====

Manipulating The Directory List
-------------------------------

### Adding Directories

Add the current working directory:

    wd -a

Add a specific directory:

    wd -a /path/here

Add a directory with an alias:
 
    wd -a /path/here fav_dir

### Removing Directories

Remove the current working directory:

    wd -r

Remove a specific directory:

    wd -r /path/here

Show a list of directories and prompt for which one to remove:

    wd -r -p

Listing The Bookmarks
---------------------

    wd -d

will output something in the format:

    Dirlist has 3 entries of 100 used
    [  0] C:\Users\fred\Documents\wd\working\git
          - Shorthand: git_wd
    [  1] c:\Program Files\FOS
          - Shorthand: FOS
    [  2] c:\install.exe
          - Shorthand: invalid

If the console supports it the items in the list should be coloured

  * Red : Item is in the file-system but is not a directory (e.g. is a file)
  * Grey : Item does not exist in the filesystem
  * Green : Item exists and is a directory

Using Within BASH
-----------------

Source the support file:

    source wd.bash

To ensure that this is done automatically you could add it to `.bash_rc`, e.g.

    echo "source /path/to/wd.bash" >> ~/.bash_rc

Then use the 'wcd' command to change directory with tab completion support, e.g.:

    wcd /pa[TAB][TAB]

Tab complete should work for both directory paths and aliases.

If you have "[Pick](https://github.com/calleerlandsson/pick)" installed and would like to use this, set the environment variable `WD_USE_PICK`, e.g. by adding:

    export WD_USE_PICK=1

to your `.bash_rc`

Using Within ZSH
----------------

See instructions for using with BASH, except include `wd.zsh` into your `.zshrc`

Using Within Windows CMD
------------------------

Ensure that wcd.bat (and wd) is on the path.

Then use the 'wcd' command to change directory using bookmark aliases, e.g.:

    C:\Users\fred\Documents> wcd wd_git_repo
    C:\Users\fred\Documents\wd\working\git> 

In The Release Package
======================

  * README.md : This file
  * wcd.bat : Support for using wd in Windows CMD
  * wd.zsh : Support for using wd in Z Shell
  * wd.bash : Support for using wd in BASH
  * mingw/wd.exe : WD build using mingw.  No Cygwin dependency, uses "C:\xxx\yyy" style directories
  * cygwin/wd.exe : WD build using Cygwin.  Requires Cygwin to be installed, uses "/cygpath/c/xxx/yyy" style directories

Currently binary distribution of wd is for Windows platforms only, however it
should be relatively simple to build on other platforms due to the minimal
dependencies.

I'd suggest using the "mingw" version of md unless you hae a preference for
Cygwin style paths.

Project Doxygen
===============

Can be found here: http://bright-tools.github.io/wd

Alternatives
============

  * Autojump - https://github.com/joelthelion/autojump - A fine looking utility but requires Python
  * http://stackoverflow.com/questions/7374534/directory-bookmarking-for-bash
  * DirB - http://www.linuxjournal.com/article/10585
  * Bookmarks for the Bash - http://lug.fh-swf.de/shell/#bookmarks
  * CDargs - http://www.skamphausen.de/cgi-bin/ska/CDargs

References
==========

  * [1] Tab expansion in BASH - http://tldp.org/LDP/abs/html/tabexpansion.html
  * [2] Dr. Memory - http://code.google.com/p/drmemory/ - Helpful for tracking down those pesky memory access bugs
