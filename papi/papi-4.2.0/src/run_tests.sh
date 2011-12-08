#!/bin/sh

# File:    papi.c
# CVS:     $Id: run_tests.sh,v 1.49 2011/10/12 00:26:51 vweaver1 Exp $
# Author:  Philip Mucci
#          mucci@cs.utk.edu
# Mods:    Kevin London
#          london@cs.utk.edu
#          Philip Mucci
#          mucci@cs.utk.edu

AIXTHREAD_SCOPE=S
export AIXTHREAD_SCOPE
if [ "X$1" = "X-v" ]; then
  shift ; TESTS_QUIET=""
else
# This should never have been an argument, but an environment variable!
  TESTS_QUIET="TESTS_QUIET"
  export TESTS_QUIET
fi

chmod -x ctests/*.[ch]
chmod -x ftests/*.[Fch]

if [ "x$VALGRIND" = "x" ]; then
# Uncomment the following line to run tests using Valgrind
# VALGRIND="valgrind --leak-check=full";
    VALGRIND="";
fi

#CTESTS=`find ctests -maxdepth 1 -perm -u+x -type f`;
CTESTS=`find ctests/* -prune -perm -u+x -type f`;
FTESTS=`find ftests -perm -u+x -type f`;
COMPTESTS=`find components/*/tests -perm -u+x -type f`;
#EXCLUDE=`grep --regexp=^# --invert-match run_tests_exclude.txt`
EXCLUDE=`grep -v -e '^#' run_tests_exclude.txt`

ALLTESTS="$CTESTS $FTESTS $COMPTESTS";
x=0;
CWD=`pwd`

PATH=./ctests:$PATH
export PATH

echo "Platform:"
uname -a

echo "Date:"
date

echo ""
if [ -r /proc/cpuinfo ]; then
   echo "Cpuinfo:"
   # only print info on first processor on x86
   sed '/^$/q' /proc/cpuinfo
fi

echo ""
if ["$VALGRIND" = ""]; then
  echo "The following test cases will be run:";
else
  echo "The following test cases will be run using valgrind:";
fi
echo ""

MATCH=0
LIST=""
for i in $ALLTESTS;
do
  for xtest in $EXCLUDE;
  do
    if [ "$i" = "$xtest" ]; then
      MATCH=1
      break
    fi;
  done
  if [ `basename $i` = "Makefile" ]; then
    MATCH=1
  fi;
  if [ $MATCH -ne 1 ]; then
	LIST="$LIST $i"
  fi;
  MATCH=0
done
echo $LIST
echo ""

CUDA=`find Makefile | xargs grep cuda`;
if [ "$CUDA" != "" ]; then
  EXCLUDE=`grep -v -e '^#' run_tests_exclude_cuda.txt`
fi

echo ""
echo "The following test cases will NOT be run:";
echo $EXCLUDE;

echo "";
echo "Running C Tests";
echo ""

if [ "$LD_LIBRARY_PATH" = "" ]; then
  LD_LIBRARY_PATH=.:./libpfm-3.y/lib
else
  LD_LIBRARY_PATH=.:./libpfm-3.y/lib:"$LD_LIBRARY_PATH"
fi
export LD_LIBRARY_PATH
if [ "$LIBPATH" = "" ]; then
  LIBPATH=.:./libpfm-3.y/lib
else
  LIBPATH=.:./libpfm-3.y/lib:"$LIBPATH"
fi
export LIBPATH

for i in $CTESTS;
do
  for xtest in $EXCLUDE;
  do
    if [ "$i" = "$xtest" ]; then
      MATCH=1
      break
    fi;
  done
  if [ `basename $i` = "Makefile" ]; then
    MATCH=1
  fi;
  if [ $MATCH -ne 1 ]; then
    if [ -x $i ]; then
      if [ "$i" = "ctests/timer_overflow" ]; then
        echo Skipping test $i, it takes too long...
      else
	  RAN="$i $RAN"
      printf "Running $i:";
      $VALGRIND ./$i $TESTS_QUIET
      fi;
    fi;
  fi;
  MATCH=0
done

echo ""
echo "Running Fortran Tests";
echo ""

for i in $FTESTS;
do
  for xtest in $EXCLUDE;
  do
    if [ "$i" = "$xtest" ]; then
      MATCH=1
      break
    fi;
  done
  if [ `basename $i` = "Makefile" ]; then
    MATCH=1
  fi;
  if [ $MATCH -ne 1 ]; then
    if [ -x $i ]; then
	RAN="$i $RAN"
    printf "Running $i:";
    $VALGRIND ./$i $TESTS_QUIET
    fi;
  fi;
  MATCH=0
done

echo "";
echo "Running Component Tests";
echo ""

for i in $COMPTESTS;
do
  for xtest in $EXCLUDE;
  do
    if [ "$i" = "$xtest" ]; then
      MATCH=1
      break
    fi;
  done
  if [ `basename $i` = "Makefile" ]; then
    MATCH=1
  fi;
  if [ $MATCH -ne 1 ]; then
    if [ -x $i ]; then
	RAN="$i $RAN"
    printf "Running $i:";
    $VALGRIND ./$i $TESTS_QUIET
    fi;
  fi;
  MATCH=0
done

if [ "$RAN" = "" ]; then 
	echo "FAILED to run any tests. (you can safely ignore this if this was expected behavior)"
fi;
