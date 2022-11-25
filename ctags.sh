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
# Note that this script requires the "exuberant ctags" version of ctags.
# This is probably the default version on Linux systems, but the Mac
# Developer Tools version is ancient and does not support the "-n" option.
# If you are on a Mac you must install the "exuberant" version of "ctags",
# from either Homebrew or MacPorts.  The example below uses the Homebrew
# version.
#
# The second version may be used on Linux, where the version of "ctags"
# is already new enough to work here.

#ctags=/usr/local/Cellar/ctags/5.8_2/bin/ctags
#ctags=/usr/bin/ctags
#ctags=/Library/Developer/CommandLineTools/usr/bin/ctags

ctags=$(which ctags)
if [[ $ctags == "" ]]; then
    echo "Set the variable 'ctags' in 'ctags.sh' to"
    echo "the location of the 'ctags' binary on your system."
fi

$($ctags -n e20/e.c > /dev/null)
if [[ "$?" == 1 ]]; then
    echo "This version of ctags is too old."
    echo "You must install exuberant ctags."
    echo "See the comments in 'ctags.sh' for details."
    exit;
fi

if [ ! -d ctags.d ] ; then
    mkdir ctags.d
fi

for i in e20 ff3 la1 lib; do
    cp -r $i ctags.d
done

cd ctags.d

for i in e20 ff3 la1; do
    cd $i
    for j in *.[ch]; do
	sed -e 's/^#ifdef.*COMMENT/#if 0 &/' < $j > tmp.$$
	mv tmp.$$ $j
    done
    cd ..
done

cd e20

echo Processing in `pwd`

if [ "$#" -eq 0  -o  "$1" = "string" ] ; then
    type=" "
else
    type="-n"
fi

#if [ $# = 0 ] ; then
#type=" "
#elif [ $1 == "string" ] ; then
#type=" "
#else
#type="-n"
#fi

# Actual ctags command.  Exclude "e" and "vi/vim" backup files, include
# local variables, and include line numbers at the end.  The latter is to
# force duplicates to be included in the tags file. Each "duplicate" represents
# an occurrance of a tag that we might want to search for.

$ctags $type --exclude=',*' --exclude='*~' --C-kinds=+l --fields=+n *.[ch] ../ff3/*.[ch] ../la1/*.[ch] ../lib/*.[ch]

mv tags ../../e20
cd ../..
rm -rf ctags.d

# Now, from the master tags file created in e20/, generate library-specific
# tags files in each of ff3/, la1/, and lib/.

grep "..\/ff3" e20/tags > ff3/tags
grep "..\/la1" e20/tags > la1/tags
grep "..\/lib" e20/tags > lib/tags
