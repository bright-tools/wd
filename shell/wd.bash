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

# Function to change directory using wd for favourites
function wcd()
{
    # If the parameter's a directory, change to it
    if [ -d "$1" ]; then
        cd "$1"
    else
        # See if the parameter was a bookmark name?
        local dir=$(wd -n $1)
        if [ -d "${dir}" ]; then
            cd "${dir}"
        else
            echo "wcd: Couldn't find a directory or a bookmark for '$1'";
        fi
    fi
}
complete -F _wd_complete wcd
