#!/bin/bash
# ex: ts=4 sw=4 et ft=sh

opts[k++]="--enable-optimize"
opts[k++]="--enable-warnings"
# opts[k++]="--enable-demo"
[[ -n $BOOST ]] && opts[k++]="--with-boost=$BOOST"

./configure ${opts[@]}
