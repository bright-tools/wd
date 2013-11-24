function _wd_complete()
{
    # Make separator a newline for directories with spaces in their names
    IFS=$'\n';
    local cmd="${1##*/}"
    local word=${COMP_WORDS[COMP_CWORD]}
    local line=${COMP_LINE}
    local list=$(wd -l)

    COMPREPLY=($(compgen -W "${list}" -- "${word}"))
    unset IFS
}
function wcd()
{
    if [ -d $1 ]; then
        cd $1
    else
        local dir=$(wd -n $1)
        if [ -d "${dir}" ]; then
            cd ${dir}
        else
            echo "wcd: Couldn't find a directory or a bookmark for '$1'";
        fi
    fi
}
complete -F _wd_complete wcd
