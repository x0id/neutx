#!/bin/bash

opts[k++]="--enable-optimize"
opts[k++]="--enable-warnings"

./configure ${opts[@]}
