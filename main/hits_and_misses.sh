#!/bin/bash

mkdir -p ../output/hits_and_misses
cd ../output/

for f in *.raw
do
    echo "hits and misses: processing $f"

    # get filename without extention
    filename="${f%.*}"
    
    # if _M.raw file is not in hits_and_misses, create it
    if [ `ls hits_and_misses | grep "${filename}_M.raw" | wc -l` == 0 ]; then
	cat "$f" | grep "M" | awk '{ print $2 }' > "hits_and_misses/${filename}_M.raw"
    fi

    # if _H.raw file is not in hits_and_misses, create it
    if [ `ls hits_and_misses | grep "${filename}_H.raw" | wc -l` == 0 ]; then
	cat "$f" | grep "H" | awk '{ print $2 }' > "hits_and_misses/${filename}_H.raw"
    fi
done
