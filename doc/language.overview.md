This document is an overview of the current state of the language. Its scope is limited to language features, excluding both the compiler and standard library.


##Arrays

###Implemented
```
#Constant length allocation
int8 [10] array1

#Complex type allocation
string [4] array2

#Variable length allocation
int64 [n] array3

#Querying array length
uint64 len = array3.length

#Array concatenation
int8 [10] arr1
int8 [10] arr2
arr1 + arr2
```

###Planned
```
#Unnamed arrays
array = int8 [n]

#Array literals
array = {45, 7, 189}
```

###Known issues
 - Empty arrays `int8 [] arr` are not parsed correctly
   - use `int8 [0] arr` as a workaround
 - Array lengths are not read-only
   - `arr.length = n` can cause memory corruption or a crash where `n` is sufficiently large

##Comments

###Implemented
```
# Single line comments
# work as advertised
```

###Planned
```
#| Multi-line comments are
 | not yet working
 | Neither are #| nested |# comments
 |#
```

##Functions

###Implemented
```
#Simple functions
def func (int32 i) -> (int32 ret) {
	#function body
}

#Named return values are optional (although useful)
def func (int32 i) -> (int32) {
	#function body
}

#No return values or parameters
def func () -> () {
	#function body
}

#Multiple return values
def func (int8 param1, int64 param2) -> (int16 ret1, int16 ret2) {
	#function body
}

#Constructors
def string (int64 param2) -> (string) {
	#function body
}

#Operator overloading
def ' + ' (const ref string lhs, const ref string rhs) -> (string) {
	#function body
}
```

###Planned
```
#Destuctors
def ~string (ref string self) -> () {
	#function body
}

#Foreign constructors
def io.stream (int64 param2) -> (io.stream) {
	#function body
}

#Implied returning
def func () -> () {
	#no return statement in body
}

def func () -> (int64) {
	40
}

#Default values
def func (int32 i = 50) -> () {
	#function body
}
```

##If statements

###Planned
```
#Basic if
if (cond) {
	#body
}

#Basic if else
if (cond) {
	#body
} else {
	#body
}

#Chain if else
if (cond) {
	#body
} else if (cond2) {
	#body
} else if (cond3) {
	#body
} else {
	#body
}

#Implied return values
int32 = if (ultimateQuestion) {42} else {0}
```

##Interfaces

###Planned
```
#Declaring interfaces
interface Less

#Abstract functions
def ' < ' (Less lhs, Less rhs) -> (bool)=0
#proposed alternative syntax
def ' < ' (Less lhs, Less rhs) -> (bool)

#Virtual functions
def less (Less lhs, Less rhs) -> (bool) {
	return lhs < rhs
}

#Dynamic casting
Less l
More m => l

#as keyword
m as Less

#Null testing
if (m != nil) {
	#Body
}
```

##Lambdas

###Planned
```
#Basic syntax
(int32 a, int32 b) -> (int32) {return a + b}

#Type inference
(a, b) -> (a + b)
```

##Loops

###Implemented
```
#while loops
while (cond) {
	#body
}
```

###Planned
```
#For each loop
for (e in arr) {
	#body
}

#Iterative for loop
for (i in [0:10]) {
	#body
}
```

###Known issues
 - Nesting while loops is currently broken
   - Put the inner loop in a function as a workaround

##Ranges

###Planned
```
#0, 1, 2, ..., 8, 9
[0:10]

#10, 9, 8, ..., 2, 1
[10:0]

#0, 2, 4, 6, 8
[0:10:2]

#9, 8, 7, ..., 2, 1, 0
[0:10:-1]

#a, a+c, a+2c, ..., b-2c, b-c
[a:b:c]
```

##Slices

###Planned
```
#Array slices
int8 [10] arr
arr [0:2]

#Assignment
arr = arr2 [10:20]

#Concatenation
arr = arr2 [0:3] + arr3[3:70]
```

##Templates

###Planned
```
#Template functions
def [type T] func () -> (T) {
	#body
}

#Template structs
struct [type T] bar {T foo}
```

##Tuples

###Implemented
```
#Binary operations
a, b = 10, c + d, 50

#Function return values
a, b = func ()
```

###Planned
```
#Iteration over tuples, syntax yet to be determined
#One candidate:
for (var e in tuple) {
	#body
}

#Element extraction, syntax not finalised
tuple.1 = 10
```

###Known issues
 - Binary operations that are not overloaded will fail, no workaround available

##Types

###Implemented
```
#Declaring types
struct bar {
	int32 foo
}

#Static casting
int32 i = 3.0f
string s = 10

#Typedef
typedef os.filehandle : fd
```

###Planned
```
#Type inference
var foo = bar

#Function types
(int32, int32) -> (int32) foo
```