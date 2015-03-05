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

##Version 0.3##
 - [x] Destructors
   - [x] Freeing arrays
   - [x] Overloading
   - [x] Recursive struct destructors
   - [x] Temporary variables
 - [ ] File I/O basic
   - [x] Document a specification
   - [ ] Reference implementation
 - [x] Make errors display line and file data correctly
 - [ ] Smart pointers (reference counting)

##Version 0.3.1##
 - [ ] Array of complex data types
   - [ ] Copying
   - [ ] Destruction
   - [ ] Initialisation

##Version 0.4##
 - [ ] High level optimisation pass
 - [ ] Replace ASTnumbatInstruction with a more robust solution
 - [ ] Rewrite parser
   - [ ] Parser should be bottom up
   - [ ] Parser should not handle semantics, only syntax
   - [ ] Parser should still accept arbitrary operators
 - [ ] Separate the Abstract syntax tree from the concrete syntax tree
 - [ ] Verification pass
   - [ ] Define what situations result in error/warning in documentation

##Version TBD##
 - [ ] File I/O
   - [ ] Full implementation
     - Requires flagged enums, Interfaces and templates
 - [ ] Flagged enums
 - [ ] Full I/O library
     - Requires file I/O, socket I/O, flagged enums, Interfaces and templates
 - [ ] Interfaces
 - [ ] Socket I/O (TCP and UDP only) basic
   - [ ] Document a specification
   - [ ] Reference implementation
 - [ ] Socket I/O
   - [ ] Full implementation
     - Requires flagged enums, Interfaces and templates
   - [ ] Protocols other than TCP/UDP
     - Must decide if this will be a low level api
 - [ ] Templates
   - Requires V0.4 rewrite
