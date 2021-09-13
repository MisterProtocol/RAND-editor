: System V
ver=e.r.c
mv -f $ver ,$ver

string0="E19 version of `/bin/date`\\\\n"
string1a=`pwd`
string1b=`id | sed -e 's/\([^(]*(\)\([^)]*\).*/\2/'`
string1c=`uuname -l | sed s"/ //"`
string1="made in $string1a by $string1b ($string1c!$LOGNAME)"

echo "short revision = -19;" >> $ver
echo "static char whatstr[] = { '@', '(', '\#', ')' } ;" >> $ver
echo 'char verstr[] = "\' >> $ver
echo "$string0\\" >> $ver
echo "$string1\\"'\n";' >> $ver
