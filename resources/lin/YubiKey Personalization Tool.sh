#!/bin/sh
appname="YKPersonalization"

dirname=`dirname "$0"`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH
exec $dirname/$appname 2>/dev/null >/dev/null $*

exit
