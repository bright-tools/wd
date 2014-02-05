# wd support in zsh (Z shell)

_wd_complete()
{
    reply=(`wd -l -e d -C`)
}

wcd()
{
    # TODO: Same as bash - needs to be de-duped
    # If the parameter's a directory, change to it
    if [ -d "$1" ]; then
        cd "$1"
    else
        # See if the parameter was a bookmark name?
        if [ "${OSTYPE}" = "cygwin" ]; then
            # Ensure paths are cygwin formatted
            local dir=$(wd -n $1 -s c)
        else
            local dir=$(wd -n $1)
        fi
        if [ -d "${dir}" ]; then
            cd "${dir}"
        else
            echo "wcd: Couldn't find a directory or a bookmark for '$1'";
        fi
    fi

}

compctl -K _wd_complete wcd
