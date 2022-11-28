#!/bin/sh
# Shell file to run ctags on the editor source.
# Historically the standard way of commenting a function
# was to put a comment block at the top surrounded by #ifdef COMMENT / #endif.
# Inside the block is a function prototype documenting the purpose and
# arguments of the function.
#
# Unfortunately this format confuses "ctags", which was invented decades later.
# This shell file munges the entirety of the source into a form acceptable
# to ctags, runs ctags on the resulting tree, moves the tags file to the
# e20 directory, and then deletes the temporary tree.
#
# There are duplicate tags in this tree, so ctags is run with the
# "-n" option, opting for line numbers inside the tags
# file and not search strings.  This requires the ctags command to be
# run more often, since search strings allow the tags to move up and
# down in the file while line numbers do not.  However, the format
# with line numbers is the only way to allow duplicate tags, since with
# search strings, all the search strings for duplicates would look the same.
#
# Note that this script requires the "exuberant ctags" or "universal"
# version of ctags. The Mac Developer Tools version is ancient and
# does not support the "-n" option.

#ctags=/usr/local/Cellar/ctags/5.8_2/bin/ctags
#ctags=ctags

ctags=$(which ctags)

if [[ $ctags == "" ]]; then
    echo "set the variable 'ctags' to the binary on your system"
    exit
fi

if [ "$#" -eq 0  -o  "$1" = "string" ] ; then
    type=""
else
    type="-n"
    echo "ctags will include line numbers"
fi

# for a tag string search, add line no field to make sure duplicate
# matches of a tag are not omitted by the built-in "sort -u"
if [[ $type == "" ]] ; then
    type="--fields=+n"
fi

#debug:
#echo "ctags=$ctags, type=$type"
#exit

dirs="e20 ff3 la1 lib include"

if [ ! -d ctags.d ] ; then
    mkdir ctags.d
    for d in $dirs ; do
	mkdir ctags.d/$d
    done
fi

for d in $dirs; do
    cd $d

    # make sure "$d/tagfiles" exists
    if [[ ! -f tagfiles ]]; then
	echo "**error:  file $d/tagfiles not found"
	exit
    fi

    cp $(cat tagfiles) ../ctags.d/$d
    cd ..
done

cd ctags.d

for i in e20 ff3 la1; do
    cd $i
    for j in *.[ch]; do
	sed -e 's/^#ifdef.*COMMENT/#if 0 &/' \
	    -e 's/^#ifdef.*OUT/#if 0 &/' < $j > tmp.$$
	mv tmp.$$ $j
    done
    cd ..
done

cd e20

echo Processing in `pwd`

$ctags $type --exclude=',*' --exclude='*~' --C-kinds=+l *.[ch] ../ff3/*.[ch] \
      ../la1/*.[ch] ../lib/*.[ch] ../include/*.h

mv tags ../../e20
cd ../..
rm -rf ctags.d

# extract individual tag files
egrep '/lib/' e20/tags > lib/tags
egrep '/ff3/' e20/tags > ff3/tags
egrep '/ff3/|/la1/|/lib/' e20/tags > la1/tags


