if [ -z "$NDK" ]; then
    THISFILE=""
    if [ "x$BASH_VERSION" != "x" ]; then
        THISFILE=${BASH_SOURCE[0]}
    elif [ "x$ZSH_VERSION" != "x" ]; then
        THISFILE=${(%):-%x}
    else
        echo "we're running in unknown shell (only BASH or ZSH are supported)!" 1>&2
        return 1
    fi
    if [ "x$THISFILE" = "x" ]; then
        echo "Can't detect path of sourced file! Please check you're running in BASH or ZSH!" 1>&2
        return 1
    fi
    NDK=$(cd $(dirname $THISFILE)/../.. && pwd)
    unset THISFILE
fi
source $NDK/build/tools/dev-defaults.sh
