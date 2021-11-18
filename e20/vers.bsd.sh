ver=e.r.c
#mv -f $ver ,$ver
if [ -e $ver ]; then
  mv -f $ver ,$ver
fi

whoami() {
	who am i | sed 's/[ 	].*//'
}

string0="E20 version of `/bin/date`"
string0=`echo $string0`
string1="made in `/bin/pwd` by `whoami`@`hostname`."

echo "short revision = -20;" >> $ver
#echo "static char whatstr[] = { '@', '(', '#', ')' } ;" >> $ver
echo "char verstr[] = \"$string0\\\\n$string1\\\\n\";" >> $ver
