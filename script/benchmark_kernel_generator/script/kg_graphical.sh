#!/bin/bash
#export PATH=`dirname $0`:$PATH


if [[ $# -ne 2 ]]
then
    echo "Usage: ./script data_file size_of_rectangle"
    exit
fi

FILE_DATA_ORIGINAL=$1
SIZE_RECTANGLE=$2


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

FILE_DATA_CYCLE=$FILE_DATA_ORIGINAL\_cycle


cat $FILE_DATA_ORIGINAL | awk '{printf ("%d ", $1 )  }' > $FILE_DATA_CYCLE

python $DIR/kg_distribution.py $FILE_DATA_CYCLE $SIZE_RECTANGLE



