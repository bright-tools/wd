# wd support in BASH

function wd_run()
{
    # See if the parameter was a bookmark name?
    if [ "$3" = "cygwin" ]; then
        # Ensure paths are cygwin formatted
        local dir=$(wd -g "$2" -s c)
    else
        # Ensure paths are windows formatted
        local dir=$(wd -g "$2" -s w)
    fi
    if [ -d "${dir}" ]; then
        $1 "${dir}"
    else
        echo "wcd: Couldn't find a directory or a bookmark for '$2'";
    fi
}

function _wd_complete()
{
    # Make separator a newline for directories with spaces in their names
    IFS=$'\n';
    local cmd="${1##*/}"
    local word=${COMP_WORDS[COMP_CWORD]}
    local line=${COMP_LINE}
    if [ "${OSTYPE}" = "cygwin" ]; then
        # Ensure paths are cygwin formatted
        local list=$(wd -l -s c)
    else
        local list=$(wd -l)
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
