#! /bin/bash

for i in `ls fonts/*.{bdf,pcf,ttf}`
do
    ./FT2dft -i $i -o /dev/null -d $i
done

