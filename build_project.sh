#!/usr/bin/env bash


#OPTION
GEN_DOC=    #Generate the doxygen documentation (true or false)

#DIR_TOP is the root folder of the project, where this script should be located
DIR_TOP="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DIR_BUILD="$DIR_TOP/build"



#Cleaning
if [ -d "$DIR_BUILD" ]; then
  rm -rf $DIR_BUILD
fi

#Building
mkdir $DIR_BUILD
cd $DIR_BUILD
cmake ..

echo "Building the project... "
make

if [ "$GEN_DOC" = true ] ; then
    echo "Building the documentation... "
    make doc
fi




