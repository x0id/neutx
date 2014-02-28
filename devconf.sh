#!/bin/bash

opts[k++]="--prefix=`pwd`/install"
opts[k++]="--enable-debug"
opts[k++]="--enable-warnings"

./configure ${opts[@]}
