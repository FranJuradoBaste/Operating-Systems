#!/bin/bash
ls
../mygrep ../scripter.c lib
../mygrep ../scripter.c lib > mygrep1-medium-bash.out
sleep 5 & 
ls /nonexistent_directory
ls /nonexistent_directory 2> error1-medium-bash.err
sort < foo.txt > sort-medium-bash.out
ls > a.txt
wc -l < file1.txt
md5sum file3.txt
ls -a dir1
cat foo.txt | grep a | grep 1