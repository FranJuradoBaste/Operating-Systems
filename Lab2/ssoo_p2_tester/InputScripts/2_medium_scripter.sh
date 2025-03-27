## Script de SSOO
ls
../mygrep ../scripter.c lib
../mygrep ../scripter.c lib > mygrep1-medium-scripter.out
sleep 5 & 
ls /nonexistent_directory
ls /nonexistent_directory !> error1-medium-scripter.err
sort < foo.txt > sort-medium-scripter.out
ls > a.txt
wc -l < file1.txt
md5sum file3.txt
ls -a dir1
cat foo.txt | grep a | grep 1