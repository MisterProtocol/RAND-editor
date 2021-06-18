ver=e.r.c
touch $ver
mv -f $ver ,$ver
whoami() {
	who am i | sed 's/[ 	].*//'
}

string0="E19+ version of `/bin/date`"
string0=`echo $string0`
string1="made in `/bin/pwd` by `whoami`@`hostname`."

echo "short revision = -19;" >> $ver
#echo "static char whatstr[] = { '@', '(', '#', ')' } ;" >> $ver
echo "char verstr[] = \"$string0\\n$string1\\n\";" >> $ver
