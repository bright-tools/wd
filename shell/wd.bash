# wd support in BASH

function wd_run()
{
    local fmt=w
    # See if the parameter was a bookmark name?
    if [ "$3" = "cygwin" ]; then
        # Ensure paths are cygwin formatted
        frt=c
    else
        # Ensure paths are windows formatted
        fmt=w
    fi

    # Try and resolve the bookmark
    local dir=$(wd -g "$2" -s ${fmt})

    # Any useful result?
    if [ -d "$2" ]; then
        $1 "$2"
    else
        # Priority is given to a bookmark, but if there's a local directory
        #  with that name, we give that a whirl
        if [ -d "${dir}" ]; then
            $1 "${dir}"
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
    # If there's no current word, request index numbers in the output
    if [ "x${word}" = "x" ] || [ ${word} -ge 0 2>/dev/null ]; then
        extra="1"
    fi
    if [ "${OSTYPE}" = "cygwin" ]; then
        # Ensure paths are cygwin formatted
        local list=$(wd -l ${extra} -e d -C -s c)
    else
        local list=$(wd -l ${extra} -e d -C)
    fi

    COMPREPLY=($(compgen -W "${list}" -- "${word}"))
    unset IFS
}

EXPLORER=$(which explorer 2>/dev/null)
if [ -x ${EXPLORER} ]; then
    function wed()
    {
        wd_run explorer "$1"
    }
    complete -F _wd_complete wed
fi

# Function to change directory using wd for favourites
function wcd()
{
    wd_run cd "$1" "${OSTYPE}"
}
complete -F _wd_complete wcd
