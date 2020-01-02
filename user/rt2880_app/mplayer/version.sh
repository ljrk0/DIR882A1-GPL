#!/bin/sh
echo "#define VERSION \"1.0rc2-$1\"" > version.h
echo "#define MP_TITLE \"MPlayer 1.0rc2-$1 (C) 2000-2007 MPlayer Team\"" >> version.h
