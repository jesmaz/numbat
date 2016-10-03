#!/bin/bash

fail=0
total=0

cd `dirname "${BASH_SOURCE[0]}"`

runTest () {
	./"$1"
	fail=$(($fail+$?))
	total=$(($total+1))
}

runTest "interpreter.sh"
runTest "parser.sh"

echo ""
echo "Results"
echo "Total: $total"
echo "Pass:  $(($total-$fail))"
echo "Fail:  $fail"
echo "Rate:  `bc -l <<< \"100 * ($(($total-$fail)) / $total)\"`%"