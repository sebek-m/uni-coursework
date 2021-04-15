#!/bin/bash

exitCode=0

if (($# < 2))
then
	exitCode=1
else
  file="$1"
  backup="$file"

  if [[ "${file##*/}" == "$file" ]]
  then
  	dir="$(pwd)"
    avoid=""
    found=""
    # Przy zalozeniu, ze szukany plik z opisami drog znajduje sie
    # "najdalej" w katalogu /home/ lub pewnym jego podfolderze.
    # Wywolanie find na katalogu / poskutkowaloby odmowa dostepu
    # np. do katalogu /sys/.
    while [[ "$found" == "" && "$dir" != "/" ]]
    do
      found="$(find "$dir" -type f -name "$file" -print -quit -not -path "$avoid")"
    		
      avoid="$dir"
      dir="$(cd "$dir/"../ && pwd)"
    done
            
    file="$found"
  fi
        
  if [[ ! -f "$file" ]]
  then
  	exitCode=1
  else
        
  	OIFS=$IFS
    IFS=';'

    for arg in "$@"
    do
      if [ "$arg" != "$backup" ]
      then
      	noSpaces="${arg// }"
      	# Sprawdzamy, czy argument jest liczba, czy nie zawiera
      	# spacji ani wiodacych zer oraz czy miesci sie w zakresie
      	# potencjalnych id drogi krajowej.
      	[ -n "$arg" ] && [ "$arg" -eq "$arg" ] 2>/dev/null
        if [ $? -eq 0 ] && [[ "$noSpaces" == "$arg" ]] &&
        [[ "$((10#$arg))" == "$arg" ]] && (("$arg" <= 999 && "$arg" >= 1))
        then
        	while read -ra line || [ -n "$line" ]
    			do
    				id=${line[0]}
    										
    				if (("$id" == "$arg"))
    				then
    					lineLength=${#line[@]}
    					routeLength=0
    												
    					for (( i=2; i<$lineLength; i += 3 ))
    					do
    						(( routeLength += ${line[$i]} ))
    					done
    												
    					echo "$arg;$routeLength"
    				fi
    			done < "$file"
				else
    			exitCode=1
        fi
      fi
    done
        
    IFS=$OIFS
  fi
fi

exit $exitCode
