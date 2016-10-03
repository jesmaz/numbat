#!/bin/bash

pass=0
total=0
score=0

cd `dirname "${BASH_SOURCE[0]}"`

for i in $( find interpreter/*.nbt ); do
	
	lines=`diff -EZby --suppress-common-lines "$i.out" <(cat "$i" | ../build/interpreter | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g")`
	lcount=`echo -n "$lines" | grep '^' | wc -l`
	expcount=`cat "$i.out" | grep '^' | wc -l`
	
	pc="`bc -l <<< \"1 - ($lcount / $expcount)\"`"
	echo "$i"
	echo "Score: `bc -l <<< \"(100 * $pc)\"`%"
	
	if [ $lcount -gt 0 ]
	then
		echo "$lines"
	else
		pass=$(($pass+1))
	fi
	
	total=$(($total+1))
	score="`bc -l <<< \"($score + $pc)\"`"
	
done

if [ $total -gt 0 ]
then
	echo "Results"
	echo "Total: $total"
	echo "Pass:  $pass"
	echo "Fail:  $(($total-$pass))"
	echo "Rate:  `bc -l <<< \"100 * ($pass / $total)\"`%"
	echo "Average Score:  `bc -l <<< \"100 * ($score / $total)\"`%"
	if [ $total -eq $pass ]
	then
		exit 0
	fi
else
	echo "Error: No tests found"
fi
exit 1