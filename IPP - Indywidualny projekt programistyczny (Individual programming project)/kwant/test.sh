#!/bin/bash

if (($# > 2))
then
    echo Too many arguments.
elif (($# < 2))
then
    echo Too few arguments.
else
    prog="$1"
  dir="$2"

  if [[ ! -f $prog ]]
  then
    echo The first argument is not a program.

  else
    if [[ ${prog##*/} == $prog ]]
    then
      prog="./$prog"
    fi

    if [[ ! -d $dir ]]
    then
      echo The second argument is not a directory.
    else
      if [[ ${dir##*/} == $dir ]]
      then
        dir="./$dir"
      fi

      if [[ ${dir%/} == $dir ]]
      then
        dir="$dir/"
      fi

      mkdir ./tmpFiles >/dev/null 2>&1

      for f in "$dir"*.in
      do
        out=${f%in}out
        err=${f%in}err

        valgrind --log-file=./tmpFiles/kwantvalgrind.err \
        --error-exitcode=1 --leak-check=full \
        --show-leak-kinds=all --errors-for-leak-kinds=all \
        "$prog" <"$f" >./tmpFiles/tmpquant.out \
        2>./tmpFiles/tmpquant.err
        exitcode=$?

        echo -n "The program has "
          if diff ./tmpFiles/tmpquant.out "$out" >/dev/null \
            2>&1 && diff ./tmpFiles/tmpquant.err "$err" \
            >/dev/null 2>&1 && (($exitcode == 0))
        then
          echo "passed the test ${f##*/}"
        else
          echo -n "failed the test ${f##*/} "

          if (($exitcode != 0))
          then
            echo "due to memory issues"
          else
            echo "because their outputs differ"
          fi
        fi
      done

      rm ./tmpFiles/kwantvalgrind.err ./tmpFiles/tmpquant.out \
      ./tmpFiles/tmpquant.err >/dev/null 2>&1
      rm -r ./tmpFiles >/dev/null 2>&1
    fi
  fi
fi
