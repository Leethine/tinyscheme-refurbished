#!/bin/bash

#sed '/;/d' init.scm | sed 's/"/\\"/g' | sed '/^$/d' | awk '{print "\""$0" \""}' > initblob
sed '/;/d' init.scm | sed 's/"/\\"/g' | awk -v RS= '{print > ("initblob-" NR ".txt")}'

for n in $(seq 9); do mv initblob-$n.txt initblob-0$n.txt; done;

cp scheme_bin_template.c temp_main.c

#LINENUMBER1=$(grep -n "#define INIT_BLOB_CONST_CHAR_DECLARED_HERE" scheme_bin_template.c | cut -d ':' -f1)
#LINENUMBER2=$(grep -n "#define INIT_BLOB_CONST_CHAR_LOADED_HERE" scheme_bin_template.c | cut -d ':' -f1)

COUNTER=0
for F in initblob-*.txt; do
  VARDECLARE="const char * INITBLOB${COUNTER} = "
  FILECONTENT="$(cat $F | tr '\n' ' '  | tr -s ' ' | sed 's/"/\\"/g')"
  INSERTTEXT1="${VARDECLARE} \" ${FILECONTENT}\" ;"
  INSERTTEXT2="scheme_load_string(&sc, INITBLOB${COUNTER});"
  sed -i "/#define INIT_BLOB_CONST_CHAR_DECLARED_HERE/i ${INSERTTEXT1}" temp_main.c
  sed -i "/#define INIT_BLOB_CONST_CHAR_LOADED_HERE/i ${INSERTTEXT2}" temp_main.c
  #sed -i "/#define INIT_BLOB_CONST_CHAR_DECLARED_HERE/a ${INSERTTEXT}" main_temp.c
  let COUNTER+=1
done

rm -f initblob-*
mv temp_main.c scheme_bin.c