#!/bin/bash

var="$1"
flag=
lock=0

find(){
if [[ "$1" == "" ]]; then
	exit 0
fi

if ! test -f "$1" && ! test -d "$1"; then
	echo "File '$1' doesn't exist"
	exit 2
fi
}

lower(){
	ext="${1##*.}"
	name=$(basename "${1%.*}")
	dir=$(dirname "$1")

	if [[ "$(basename $ext)" == "$name" ]]; then
		mv "$1" "$dir/${name,,}" > /dev/null 2>&1
	else
		mv "$1" "$dir/${name,,}.$ext" > /dev/null 2>&1
	fi
}

uper(){
	ext="${1##*.}"
	name=$(basename "${1%.*}")
	dir=$(dirname "$1")

	if [[ "$(basename $ext)" == "$name" ]]; then
                mv "$1" "$dir/${name^^}" > /dev/null 2>&1
        else
                mv "$1" "$dir/${name^^}.$ext" > /dev/null 2>&1
        fi

}

classify(){
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
if [[ "$flag" == "sed" ]]; then
	if [[ "$lock" == 0 ]]; then
		pattern=
		if [[ lock==0 ]];then
			pattern="$1"
			lock=1
		fi
	else
		if [ -d "$1" ];then
			for file in "$1"/*; do
				recursive "$file"
				sed "$pattern" "$file"
			done
		else
			sed "$pattern" "$1"
		fi
	fi
else
	for file in "$1"/*; do
                if [ -d "$file" ];then
			recursive "$file"
			classify "$file"
                else
                        classify "$file"
                fi
	done
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
			if [[ "$flag" != "sed" ]]; then
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
