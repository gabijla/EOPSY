#!/bin/bash

var="$1"	#The first option argument is stored here
flag=		#In case the first option is '-r' then the second option is stored here
lock=0		#Lock used to know the first time recursive() is called 

find(){
#Find is used to know if the file exists
if [[ "$1" == "" ]]; then
	#In case find() is called with the last argument ""
	exit 0
fi

if ! test -f "$1" && ! test -d "$1"; then
	#If the file doesn't exist then the script terminates with exit code 2
	echo "File '$1' doesn't exist"
	exit 2
fi
}

lower(){
#Used to lowerize the names of the file, extracting extension, name and directory
	ext="${1##*.}"
	name=$(basename "${1%.*}")
	dir=$(dirname "$1")

	if [[ "$(basename $ext)" == "$name" ]]; then
		#In case the file has no extension
		mv "$1" "$dir/${name,,}" > /dev/null 2>&1
	else
		mv "$1" "$dir/${name,,}.$ext" > /dev/null 2>&1
	fi
}

uper(){
#Used to uppercase the name of the file, extracting extension, name and directory
	ext="${1##*.}"
	name=$(basename "${1%.*}")
	dir=$(dirname "$1")

	if [[ "$(basename $ext)" == "$name" ]]; then
		#In case the file has no extension
                mv "$1" "$dir/${name^^}" > /dev/null 2>&1
        else
                mv "$1" "$dir/${name^^}.$ext" > /dev/null 2>&1
        fi

}

classify(){
#Declared as this will be called in recursive several times, just classify and call the repective functions
	case "$flag" in
        	-l)
        	lower "$1"
        	;;
        	-u)
        	uper "$1"
        	;;
        	* | "")
        	echo "Error: -h for help"
        	exit 1
       		;;
        esac
}

recursive(){
#This function is called in a recursive way as it has to change all files in a given directory
#If the script is called with sed then it enters the first conditional estatement
if [[ "$flag" != \-* ]]; then
	pattern="$flag"
	if [ -d "$1" ];then
	#Check if the file is a directory, calls recursive() on this file and execute the sed pattern
		for file in "$1"/*; do
			recursive "$file"
			mv "$file" $(echo "$file" | sed "$pattern") > /dev/null 2>&1
		done
	else
		mv "$1" $(echo "$1" | sed "$pattern") > /dev/null 2>&1
	fi
else
	for file in "$1"/*; do
                if [ -d "$file" ];then
		#Check if the file is a directory, calls recusrive() on this file and classify() on the dir
			recursive "$file"
			classify "$file"
                else
                        classify "$file"
                fi
	done
	#Can't call classify because need to be called with the $1 value
	case "$flag" in
                -l)
                lower "$1"
                ;;
                -u)
                uper "$1"
                ;;
                *)
                echo "Error: -h for help"
                exit 1
                ;;
        esac
fi


}
for arg in "$@"
#Goes trhough the argument list, and call the respective functions according the $var value
do
	case "$var" in
		-h | --help)
		echo "	*** HELP ***
modify.sh [-r] [-l|-u] <dir/file names...>
modify.sh [-r] <sed pattern> <dir/file names...>
modify.sh [-h]

The script is dedicated to lowerizing (-l)
file names, uppercasing (-u) file names or internally calling sed
command with the given sed pattern which will operate on file names.
Changes may be done either with recursion (-r) or without it.

modify.sh -u example example1
modify.sh -r -l dir_exmaple/ example
modify.sh -r sed 'sed_pattern' dir_example/ example example1
modify.sh -h
"
		exit
		;;
		-r)
		shift
		flag="$1"
		shift
		for recur in "$@";do
			if [[ "$flag" != \-* ]]; then
				find "$1"
			fi
			recursive "$1"
			shift
		done
		;;
		-l | --lowercase)
		shift
		find "$1" 
		lower "$1"
		;;
		-u | --upercase)
		shift
		find "$1"
		uper "$1"
		;;
		*)
		echo "Error: -h for help."
		exit 1
		;;
	esac
done
