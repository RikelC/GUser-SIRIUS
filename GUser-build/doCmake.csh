#!/bin/tcsh -f

if ($#argv < 1) then

  echo "Specify the depth for building the project\n 0 = GUSER alone \n 1 = UserLib alone \n > 1 GUSER + UserLib"

  exit 0

endif

set depth=$1

echo "cmake depth specified $depth ."

set myPath = $PWD

if ( $depth == 0 ) then

    echo "building GUSER only.............."

    cd $myPath

    rm -f CMakeCache.txt  GUser_C.so cmake_install.cmake GUserDict.C GUserDict.h  Makefile

    rm -rf CMakeFiles

    rm -rf UserLib

    sleep 1

    cmake ../GUser-sources/

    sleep 1

    make

    echo "GUSER built at `date`"


else  if ( $depth == 1 ) then

    echo "building USER library only.............."

    cd $myPath/../GUser-sources/UserLib/build/

    source doCmake.csh

    echo "User library bult at `date`"

    cd $myPath

 else

    echo "building USER library.............."

    cd $myPath/../GUser-sources/UserLib/build/

    source doCmake.sh

    echo "User library bult at `date`"

    echo "building GUSER .............."

    cd $myPath

    rm -f CMakeCache.txt  GUser_C.so cmake_install.cmake GUserDict.C GUserDict.h  Makefile

    rm -rf CMakeFiles

    sleep 1

    cmake ../GUser-sources/

    sleep 1

    make

    echo "GUSER built at `date`"
endif
