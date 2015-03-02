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

Implements 'Descriptor'

Provides low level file access.

```
struct FileDescriptor
```

####close

Closes the file, returning any freed resources to the operating system. Subsequent calls to either read or write will fail regardless of the mode the file was initially opened with. Attempting to close a FileDescriptor that has already been closed has no effect.

```
def close (ref FileDescriptor fd) -> ()
```

####read

Reads up to 'length' bytes from the file and returns them in an array.

```
def read (ref FileDescriptor fd, uint64 length) -> (uint8 [] data)
```

####state

Returns the current state of the Descriptor.

```
def state (const ref FileDescriptor fd) -> (State state)
```

####write

Writes the contents of the 'data' argument to the file. The number of successfully written bytes is returned. Writing to a file in read-only mode will automatically fail.

```
def write (ref FileDescriptor fd, const ref uint8 [] data) -> (uint64 length)
```



###IFile

Implements 'IPipe'

Provides low level access to files in input mode.

```
struct IFile
```


###BasicIStream

Implements 'IStream'

Provides high level support for input operations on character streams.

```
struct BasicIStream
```


###OFile

Implements 'OPipe'

Provides low level access to files in output mode.

```
struct OFile
```


###BasicOStream

Implements 'OStream'

Provides high level support for output operations on character streams.

```
struct BasicOStream
```


###File

Implements 'Pipe'

Provides low level access to files in input and output mode.

```
struct File
```


###FileMode

Describes the mode for opening file descriptors

```
enum @flags FileMode
```

####Values
 - Append
 - Nonblock
 - Read
 - Sync
 - Truncate
 - Write


###BasicStream

Implements 'Stream'

Provides high level support for input and output operations on character streams.

```
struct BasicStream
```


###Socket###

Implements 'Descriptor'

Provides low level socket access.

```
struct Socket
```

####close

Closes the socket, returning any freed resources to the operating system. Subsequent calls to either read or write will fail. Attempting to close a Socket that has already been closed has no effect.

```
def close (ref Socket fd) -> ()
```

####read

Reads up to 'length' bytes from the socket and returns them in an array.

```
def read (ref Socket fd, uint64 length) -> (uint8 [] data)
```

####state

Returns the current state of the Socket.

```
def state (const ref Socket fd) -> (State state)
```

####write

Writes the contents of the 'data' argument to the socket. The number of successfully written bytes is returned.

```
def write (ref Socket fd, const ref uint8 [] data) -> (uint64 length)
```




##Functions


###open

```
def open (string path, FileMode mode=Append|Read|Write) -> (FileDescriptor fd)
```


###print

Calls print on the stdout stream.

```
def print (const ref string str) -> ()
```


###scan

Calls 'scan' on the stdin stream.

```
def scan (const ref string str) -> ()
```