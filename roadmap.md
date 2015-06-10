#Roadmap#

##Version 0.1.1##
 - [x] Floating point constants
 - [x] Nested while loops

##Version 0.2##
 - [x] 'as' operator
 - [x] Enums
 - [x] Function auto return
 - [x] if/else statement
 - [x] ptrint casting
   - [x] Arrays
   - [x] References
 - [x] Type meta data

##Version 0.2.1##
 - [x] Clean-up output
 - [x] Fix bug preventing enums being used in standalone executables
 - [x] Print error nodes as errors and end compilation

##Version 0.2.2##
 - [x] Compilation problems should cause a gracious failure
 - [x] Debug symbol generation
 - [x] Formal documentation on the memory model including when destructors are called

##Version 0.2.3##
 - [x] Destructors
   - [x] Freeing arrays
   - [x] Overloading
   - [x] Recursive struct destructors
   - [x] Temporary variables
 - [x] Make errors display line and file data correctly

##Version 0.3##
 - [ ] Analysis pass
   - [ ] Side effect propagation
 - [ ] High level optimisation pass
   - [ ] Static code evaluation
 - [ ] Replace ASTnumbatInstruction with a more robust solution
 - [ ] Rewrite (and properly document) code generator
 - [ ] Rewrite parser
   - [x] Parser should be bottom up
   - [x] Parser should not handle semantics, only syntax
   - [x] Parser should still accept arbitrary operators
   - [ ] Testing, lots of testing
 - [ ] Semantic analysis pass
   - [ ] Transforms the parse tree into the abstract syntax tree
 - [ ] Verification pass
   - [ ] Define what situations result in error/warning in documentation

##Version 0.3.1##
 - [ ] Array of complex data types
   - [ ] Copying
   - [ ] Destruction
   - [ ] Initialisation
 - [ ] Smart pointers (reference counting)

##Version 0.3.2##
 - [ ] Enum parser
 - [ ] Enum printer
 - [ ] Flagged enums

##Version 0.3.3##
 - [ ] Partial file I/O
   - [ ] Open in different modes
   - [ ] Read string (up to n chars)
   - [ ] Seek
   - [ ] Write string

##Version 0.3.3##
 - [ ] Partial Socket I/O
   - [ ] Open in UDP or TCP ipv4 only
   - [ ] Read string (up to n chars)
   - [ ] Write string

##Version 0.4##
 - [ ] Templates
 - [ ] Interfaces
 - [ ] Objects

##Version TBD##
 - [ ] File I/O
   - [ ] Full implementation
     - Requires flagged enums, Interfaces and templates
 - [ ] Full I/O library
     - Requires file I/O, socket I/O, flagged enums, Interfaces and templates
 - [ ] Socket I/O (TCP and UDP only) basic
   - [ ] Document a specification
   - [ ] Reference implementation
 - [ ] Socket I/O
   - [ ] Full implementation
     - Requires flagged enums, Interfaces and templates
   - [ ] Protocols other than TCP/UDP
     - Must decide if this will be a low level api
