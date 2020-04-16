#!/bin/bash

mkdir test
cd test
mkdir test1
mkdir test2
touch file1.txt
touch FILE2
cd test1
touch file3.txt
touch file4.txt
cd ../test2
mkdir test3
touch file5
cd test3
touch FILE6
cd ../../..

echo "------------------------------------------------"
echo "--------------- Correct usages -----------------"

echo "---------------------Help-----------------------"
echo "Command: ./modify.sh -h"
./modify.sh -h
echo "------------------Uppercase---------------------"
echo "Command: ./modify.sh -u test/file1.txt test/test2/file5"
./modify.sh -u test/file1.txt test/test2/file5
ls -l test test/test2
echo "------------------Lowerize----------------------"
echo "Command: ./modify.sh -l test/FILE2"
./modify.sh -l test/FILE2
ls -l test
echo "-------------Uppercase recursive----------------"
echo "Command: ./modify.sh -r -u test/test1"
./modify.sh -r -u test/test1
ls -l test/TEST1
echo "-------------Lowerize recursive-----------------"
echo "Command: ./modify.sh -r -l test/TEST1"
./modify.sh -r -l test/TEST1
ls -l test/test1
echo "----------------Sed pattern---------------------"
echo "Command: ./modify.sh -r s/FI/fi/ test/"
./modify.sh -r s/FI/fi/ test/
ls -l test/ test/test2

echo "-----------------Incorrect usages---------------"
echo "Commnad: ./modify.sh -t -l test"
./modify.sh -t -l test
echo "Command: ./modify.sh -u file"
./modify.sh -u file
echo "Command: ./modify.sh -r s/fi/fi test/test1/"
./modify.sh -r s/fi/fi test/test1/
echo "Command: ./modify.sh -r -p test/"
./modify.sh -r -p test/ 
echo "-----End of the test, removing test dir---------"
rm -r test/

