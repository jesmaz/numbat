#Numbat compiler static analysis

This document details the current and planned methods of analysis in the numbat compiler. These are used to detect problems and optimise code.

##Alias analysis (planned)

Most aliasable code is generated by the compiler. In the following snippet the compiler will create duplicate pointer calculations.
```
foo.bar = a
foo.bar = b
```
By recognising that both a and b are being written to the same memory location the compiler can eliminate the first assignment.

##Conditional validation (planned)

Depends: Alias analysis

A conditional expression is considered suspicious if it:
 - Always evaluates to true
 - Always evaluates to false
 - Has any sub expressions that do not contribute to the result

```
#Simple example of always evaluating to true
if (true) {
	...
}

#Always evaluates to false as a is an alias of itself
while (a != a) {
	...
}

#The sub expression 'a == 5' has no effect on the result (if a is 5 then it can't also be 10)
if (a == 5 or a != 10) {
	...
}

#Special case, ignored
while (true) {
	...
}
```

##Redundant code detection (planned)

Depends: Alias analysis

```
if (foo.bar) {
	print (foo.bar)
}
```
In the above example the code generator would normally generate code that retrieves the value of 'foo.bar' twice. However that value will not change between the conditional and the function call (assuming foo.bar is not volatile).

```
if (foo.bar) {
	foo.bar = 10
}
```
In this example the memory address calculated for the conditional can be reused in the assignment operation.