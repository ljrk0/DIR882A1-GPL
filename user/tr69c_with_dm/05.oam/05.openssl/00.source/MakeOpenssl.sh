#!/bin/sh

currentDIR=`pwd`
opensslPkt=$currentDIR/openssl-1.0.2d.tar.gz
opensslRsltDIR=$currentDIR/results
opensslDIR=$currentDIR/openssl-1.0.2d
opensslDstHdrDir=$currentDIR/../01.include

kernelPath=$1
L1version=$2
duType=$3
if [ "$4" != "" ]
then
export CROSS_COMPILE=$4
mipsPath=$4
export PATH=$PATH:${mipsPath%/*}
echo cross-compile...
opensslDstLibDir=$currentDIR/../02.mips_libs
else
export CROSS_COMPILE=
echo gcc-compile...
opensslDstLibDir=$currentDIR/../03.gcc_libs
fi

rm -rf $opensslDIR
tar zxf $opensslPkt -C $currentDIR

if [ -d $opensslRsltDIR/ ]
then
rm -rf $opensslRsltDIR/*
else
mkdir $opensslRsltDIR
fi

cd $opensslDIR
./config no-asm shared --prefix=$opensslRsltDIR compiler:gcc

cp -rf $currentDIR/Makefile_crypto $opensslDIR/crypto/Makefile
cp -rf $currentDIR/Makefile_engines $opensslDIR/engines/Makefile
cp -rf $currentDIR/Makefile_engines_ccgost $opensslDIR/engines/ccgost/Makefile
cp -rf $currentDIR/Makefile_ssl $opensslDIR/ssl/Makefile

make;make install
rm -rf $opensslDstLibDir/*
cp -rf $opensslRsltDIR/lib/*.a $opensslDstLibDir
rm -rf $opensslDstHdrDir/*
cp -rf $opensslRsltDIR/include/* $opensslDstHdrDir/
cp -rf $opensslDIR/apps/apps.h $opensslDstHdrDir/
cp -rf $opensslDIR/e_os.h $opensslDstHdrDir/

