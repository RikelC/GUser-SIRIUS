#!/bin/tcsh -f

if ($#argv < 1) then

  echo "Specify the depth for compiling the project\n 0 = GUSER alone \n 1 = UserLib alone \n > 1 = GUSER + UserLib"

  exit 0

endif

set depth=$1

echo "make depth specified $depth ."

set myPath = $PWD

if ( $depth == 0 ) then

    echo "compiling GUSER only.............."

    cd $myPath

    make

    echo "GUSER compiled at `date`"

else if ( $depth == 1 ) then

    cd $myPath/../GUser-sources/UserLib/build/

    make

    echo "user library compiled at `date`"

    cd $myPath

 else

    echo "building USER library.............."

    cd $myPath/../GUser-sources/UserLib/build/

    make

    echo "user library compiled at `date`"

    echo "compiling GUSER.............."

    cd $myPath

    make

    echo "GUSER compiled at `date`"

endif
