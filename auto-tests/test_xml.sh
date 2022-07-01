#!/bin/bash

clear
test=$1
compiler_path=../l22/l22
test_path=tests/*


if [ -z "$test" ]; then 
   for file in $test_path; do 
        if [[ -f $file && $file == *.l22 ]]  ; then 
            echo -e "\e[1;34m$file\e[0m"
            test_name=$(basename $file)
            echo -en "\e[1;31m"
            ./$compiler_path $file -o tests/$test_name.xml.xml 2> tests/$test_name.log
            cat tests/$test_name.log | grep "** Syntax"
            wait
            echo -en "\e[1;0m"
        fi 
    done
elif [ "$test" == "clean" ]; then
    rm tests/*.xml
    rm tests/*.log
else
    echo -e "\e[1;35m$1\e[0m"
   ./$compiler_path $test -o target.xml.xml 
fi 
