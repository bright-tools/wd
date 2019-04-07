# wd support in BASH

function wd_run()
{
    local fmt=w
    # See if the parameter was a bookmark name?
    if [ "$3" = "cygwin" ] || [ "$3" = "linux-gnu" ]; then
        # Ensure paths are cygwin formatted
        fmt=c
    else
        # Ensure paths are windows formatted
        fmt=w
    fi

    # Try and resolve the bookmark
    local dir=$(wd -g "$2" -s ${fmt})

    # Any useful result?
    if [ -d "${dir}" ]; then
        $1 "${dir}"
    else
        # Priority is given to a bookmark, but if there's a local directory
        #  with that name, we give that a whirl
        if [ -d "$2" ]; then
            $1 "$2"
        else
            echo "wcd: Couldn't find a directory or a bookmark for '$2' and there did not seem to be a local directory either";
        fi
    fi
}

function _wd_complete()
{
    # Make separator a newline for directories with spaces in their names
    IFS=$'\n';
    local cmd="${1##*/}"
    local word=${COMP_WORDS[COMP_CWORD]}
    local line=${COMP_LINE}
    local extra=""
    local PICK=""
    local WD_PICK_CMD=""
    local WD_PICK=""
    local ESCAPE="-C"

    # Is WD_USE_PICK set?
    if [ ! -z ${WD_USE_PICK+x} ];
    then
        # Check that Pick is installed
        WD_PICK=$(command -v pick)
        if [ ! -z ${WD_PICK+x} ];
        then 
            WD_PICK_CMD="${WD_PICK}"
            ESCAPE="-c"
        fi
    fi

    # Not using Pick?
    if [ "x${WD_PICK_CMD}" = "x" ];
    then
        # If there's no current word, or there is one, but it's numberic, 
        #  request index numbers in the output
        if [ "x${word}" = "x" ] || [ ${word} -ge 0 2>/dev/null ]; then
            extra="1"
        fi
    fi

    # -l b : Output bookmark names
    # -e d : Only list directories, not files
    # -C   : Double-escape paths
    if [ "${OSTYPE}" = "cygwin" ]; 
    then
        # -s c : Cygwin formatted paths
        local list=$(wd -l b${extra} -e d ${ESCAPE} -s c)
    else
        local list=$(wd -l b${extra} -e d ${ESCAPE})
    fi

    # Are we using pick?
    if [ "x${WD_PICK_CMD}" != "x" ];
    then
        COMPREPLY=($(echo "${list}" | pick -q "${word}"))
    else
        COMPREPLY=($(compgen -W "${list}" -- "${word}"))
    fi
    unset IFS
}

WD_EXPLORER=$(command -v explorer)
if [ -x "${WD_EXPLORER}" ]; then
    function wed()
    {
        wd_run explorer "$1"
    }
    complete -F _wd_complete wed
fi
unset WD_EXPLORER

# Function to change directory using wd for favourites
function wcd()
{
    wd_run cd "$1" "${OSTYPE}"
}
complete -F _wd_complete wcd
