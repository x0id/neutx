#!/bin/bash
# ex: ts=4 sw=4 et ft=sh

opts[k++]="--prefix=`pwd`/install"
opts[k++]="--enable-demo"
opts[k++]="--enable-debug"
opts[k++]="--enable-warnings"
[[ -n $BOOST ]] && opts[k++]="--with-boost=$BOOST"

./configure ${opts[@]}
