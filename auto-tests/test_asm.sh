#!/bin/bash

clear
test=$1
compiler_path=../l22/l22
test_path=tests/*


if [ -z "$test" ]; then 
   for file in $test_path; do 
        if [[ -f $file && $file == *.l22 ]]  ; then 
            echo -e "\e[1;34m$file\e[0m"
            test_name=$(basename "$file" .l22)
            echo -en "\e[1;31m"

            ./$compiler_path --target asm $file
            yasm -felf32 -o ./tests/$test_name.o ./tests/$test_name.asm
            ld -melf_i386 -o ./tests/$test_name.x ./tests/$test_name.o ../librts5-202103031636/*.o
            ./tests/$test_name.x > results/$test_name.out
            wait

            diff <( tr -d ' \t\n' <expected/$test_name.out) <( tr -d ' \t\n' <results/$test_name.out)

            echo -en "\e[1;0m"
        fi 
    done
elif [ "$test" == "clean" ]; then
    rm ./tests/*.asm
    rm ./tests/*.o
    rm ./tests/*.x
    rm ./results/*.out

fi 
