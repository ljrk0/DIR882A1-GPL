#! /bin/sh
#source code packet download from ftp://xmlsoft.org/libxml2/
currentDIR=`pwd`

libxml2SrcPkt=$currentDIR/libxml2-sources-2.9.2.tar.gz
libxml2CompileDir=$currentDIR/libxml2-2.9.2
libxml2RetDir=$currentDIR/results/

kernelPath=$1
L1version=$2
DUType=$3
CFLAGS=''
LDFLAGS=''


if [ "$4" != "" ]
then
export CROSS_COMPILE=$4
mipsPath=$4
export PATH=$PATH:${mipsPath%/*}
echo cross-compile libxml2...
echo $PATH
echo $CROSS_COMPILE
compileCmd=$4"gcc"
copyLibPath=$currentDIR/mips_lib/
else
export CROSS_COMPILE=
echo gcc-compile libxml2...
compileCmd="gcc"
copyLibPath=$currentDIR/gcc_lib/
fi

copyHdrPath=$currentDIR/include


if [ -e $libxml2CompileDir ]
then
	rm -rf $libxml2CompileDir
fi

mkdir -p $libxml2CompileDir

if [ -e $libxml2RetDir ]
then
	rm -rf $libxml2RetDir
fi

mkdir -p $libxml2RetDir

if [ ! -e $libxml2SrcPkt ]
then
	echo $libxml2SrcPkt do not exist
	exit -1
fi

echo "extract libxml2 source..."
cd $currentDIR
tar zxf $libxml2SrcPkt
if [ ! -e $libxml2CompileDir ]
then
	echo $libxml2CompileDir do not exist
	exit -1
fi

cd $libxml2CompileDir

if [ "$4" != "" ]
then
echo "cross-compile libxml2 source..."
./configure --host=mipsel-linux --prefix=$libxml2RetDir --without-python --without-zlib
else
echo "gcc compile libxml2 source..."
./configure --prefix=$libxml2RetDir --without-python --without-zlib
fi

make clean;make;make install
rm -rf $copyHdrPath/libxml
cp -rf $libxml2RetDir"include/libxml2/libxml" $copyHdrPath
cp -rf $libxml2RetDir"lib/libxml2.a" $copyLibPath

rm -rf $libxml2CompileDir $libxml2RetDir


