wd
==

Utility to store and set the current working directory (cwd) in a command-line
shell.

wd could be considered to be a system for bookmarking favourite directories 
and then quickly jumping back to those bookmarks

wd is intended to be very light-weight with a minimum of dependencies - that's
why it's implemented in C as opposed to (for example) Python.  It's also
intended to be independent of the shell (so, for example, it could be used on
the Windows command prompt as well as BASH).

Usage
=====

Manipulating The Directory List
-------------------------------

### Adding Directories

Add the current working directory:

    wd -a

Add a specific directory:

    wd -a /path/here

Using Within BASH
-----------------

Source the support file:

    source md.bash

Then use the 'wcd' command to change directory with tab completion support, e.g.:

    wcd /pa[TAB][TAB]

TODO: named bookmark support

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
