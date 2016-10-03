#pragma once


#include <ostream>
#include <string>


namespace report {

enum Severity {ERROR, NOTICE, WARNING};


// true if any errors are reported
bool compilationFailed ();

// true if nothing has been logged.
bool empty ();

// Clears all logged messages. Compilation status is preserved.
void clearLogs ();

// Prints all logs to the stream then clears them. Compilation status is preserved.
// If no stream is specified std::cerr is used.
void dumpLogs ();
void dumpLogs (std::ostream & ostream);

// Logs a new message. Messages are ordered by file, line, and position.
void logMessage (Severity, const std::string & file, uint64_t line, uint64_t pos, const std::string & message);
void logMessage (Severity, const std::string & message);

// Prints all logs to the stream.
// If no stream is specified std::cerr is used.
void printLogs ();
void printLogs (std::ostream & ostream);

// Clears all logged messages and compilation status.
void reset ();


}