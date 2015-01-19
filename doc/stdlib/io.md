#io

This document specifies the API exposed by the io library.

##Usage

The io library can be accessed with the following import statement.
```
import io
```


##Interfaces

**_Note: Interfaces have not been specified, the example code in this section is subject to change._**

###Descriptor

```
interface Descriptor
```

####close

Abstract function that frees any resources the Descriptor holds. Successive calls to this function can be safely ignored, closing an already closed Descriptor is considered a success.

```
def close (Descriptor descriptor) -> ()=0
```

####state

Abstract function that returns the current state of the Descriptor.

```
def state (Descriptor descriptor) -> (State state)=0
```

###IPipe

Extends 'Descriptor'

```
interface IPipe : Descriptor
```

####read

Abstract function that returns an array of up to 'length' bytes read from the underlying Descriptor.

```
def read (IPipe pipe, uint64 length) -> (uint8 [] data)=0
```

###IStream

Extends 'IPipe'

```
interface IStream : IPipe
```

####readUntil

Abstract function that returns all the bytes read from the underlying pipe until a terminating byte is reached. Terminating bytes are passed in via the 'bytes' argument.

```
def readUntilAny (IStream pipe, const ref uint8 [] bytes) -> (uint8 [] data)=0
```

####scan

_**Note: This function will be be templated when the feature is added.**_

Returns a the next token, the stream's new position is the first character after the token. A token is defined as a sequence of non white-space characters. Default implementation calls 'readUntilAny'.

```
def scan (IStream pipe) -> (string token)
```

###OPipe

Extends 'Descriptor'

```
interface OPipe : Descriptor
```

####write

Abstract function that writes the contents of the 'data' argument to the underlying descriptor. The number of successfully written bytes is returned.

```
def write (OPipe pipe, const ref uint8 [] data) -> (uint64 length)=0
```

###OStream

Extends 'OPipe'

```
interface OStream : OPipe
```

####print

Writes the formatted bytes in a string to the underlying descriptor in utf-8. The default implementation calls 'write'.

```
def print (IStream pipe, const ref string str) -> ()
```

###Pipe

Extends 'IPipe' and 'OPipe'

```
interface Pipe : IPipe, OPipe
```

###Stream

Extends 'IStream' and 'OStream'

```
interface Stream : IStream, OStream
```



##Data Types

###FileDescriptor

```
struct FileDescriptor : Descriptor
```

###IFile

```
struct IFile : IPipe
```

###BasicIStream

```
struct BasicIStream : IStream
```

###OFile

```
struct OFile : OPipe
```

###BasicOStream

```
struct BasicOStream : OStream
```

###File

```
struct File : Pipe
```

###BasicStream

```
struct BasicStream : Stream
```

##Functions

###print

Calls print on the stdout stream.

```
def print (const ref string str) -> ()

###scan

Calls 'scan' on the stdin stream.

```
def scan (const ref string str) -> ()
```