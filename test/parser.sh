#!/bin/bash

pass=0
total=0
# passLine=0
# totalLine=0

cd `dirname "${BASH_SOURCE[0]}"`

for i in $( find parser/*.nbt ); do
	
	lines=`diff -EZby --suppress-common-lines "$i.out" <(cat "$i" | ../build/parsertest | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g")`
	fail=`echo -n "$lines" | grep '^' | wc -l`
	lcount=`cat "$i.out" | grep '^' | wc -l`
	
	if [ $fail -gt 0 ]
	then
		echo "$i"
		echo "$lines"
	else
		pass=$(($pass+1))
	fi
	
	total=$(($total+1))
	
done

if [ $total -gt 0 ]
then
	echo "Results"
	echo "Total: $total"
	echo "Pass:  $pass"
	echo "Fail:  $(($total-$pass))"
	echo "Rate:  `bc -l <<< \"100 * ($pass / $total)\"`%"
	if [ $total -eq $pass ]
	then
		exit 0
	fi
else
	echo "Error: No tests found"
fi
exit 1