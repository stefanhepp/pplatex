#!/bin/sh

check_exists() {
    local file=$1

    if [ ! -e "$releasedir/$file" ]; then
	echo "Required file $file does not exist!"
	exit 1
    fi
}

check_not_exists() {
    local file=$1

    if [ -e "$releasedir/$file" ]; then
	echo "File $file exists but should not!"
	exit 1
    fi
}

. RELEASE

rname=$NAME-$VERSION
releasedir=dist/$rname/

rm -rf $releasedir
mkdir -p $releasedir

# Create a clean copy
tar -c --exclude=.svn --exclude=obj --exclude=tmp --exclude=dist * | (cd $releasedir && tar -x)
mkdir $releasedir/dist
mkdir $releasedir/obj
mkdir $releasedir/tmp

check_exists "bin/pcre3.dll"
check_exists "bin/pcreposix3.dll"

# Create the dist files

tar -czf dist/$rname-src.tar.gz -C dist $rname
(cd dist && zip $rname-win32.zip $rname/*.txt $rname/bin/*.exe $rname/bin/*.dll)
(cd dist && zip $rname-linux.zip $rname/*.txt $rname/bin/pplatex $rname/bin/ppdflatex)

