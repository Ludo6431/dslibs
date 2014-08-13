#! /bin/bash

for i in `find ./fonts/ -regextype posix-extended -regex '.*\.(ttf|pcf|bdf)'`
do
    ./FT2dft -i $i -o /dev/null -d $i $@
done

