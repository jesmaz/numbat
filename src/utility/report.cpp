#include <algorithm>
#include <iostream>
#include <utility/report.hpp>
#include <utility/array.hpp>
 


namespace report {


struct Message {
	Severity severity;
	std::string file;
	uint64_t line;
	uint64_t pos;
	std::string message;
	bool operator < (const Message & rhs) const {
		int cmp = file.compare (rhs.file);
		if (cmp) return cmp < 0;
		if (line != rhs.line) return line < rhs.line;
		if (pos != rhs.pos) return pos < rhs.pos;
		cmp = message.compare (rhs.message);
		if (cmp) return cmp < 0;
		return false;
	}
	Message (Severity s, const std::string & f, uint64_t l, uint64_t p, const std::string & m) : severity (s), file (f), line(l), pos (p), message (m) {}
	Message () {}
};

struct EstrangedMessage {
	Severity severity;
	std::string message;
	EstrangedMessage (Severity s, const std::string & m) : severity (s), message (m) {}
	EstrangedMessage () {}
};


bool errorReported=false;


DynArray <EstrangedMessage> estrangedMessages;
DynArray <Message> messages;


bool compilationFailed () {
	return errorReported;
}

bool empty () {
	return estrangedMessages.empty () and messages.empty ();
}


void clearLogs () {
	estrangedMessages.clear ();
	messages.clear ();
}


void dumpLogs () {
	printLogs ();
	clearLogs ();
}

void dumpLogs (std::ostream & ostream) {
	printLogs (ostream);
	clearLogs ();
}


void logMessage (Severity severity, const std::string & file, uint64_t line, uint64_t pos, const std::string & message) {
	messages.push_back (Message (severity, file, line, pos, message));
	std::sort (messages.begin (), messages.end ());
	if (not errorReported or severity == ERROR) errorReported = true;
}

void logMessage (Severity severity, const std::string & message) {
	estrangedMessages.push_back (EstrangedMessage (severity, message));
	if (not errorReported or severity == ERROR) errorReported = true;
}


void printLogs () {
	printLogs (std::cerr);
}

void printLogs (std::ostream & ostream) {
	
	for (const Message & msg : messages) {
		ostream << msg.severity << ": " << msg.file << ":" << msg.line << ":" << msg.pos << ": " << msg.message << "\n";
	}
	for (const EstrangedMessage & msg : estrangedMessages) {
		ostream << msg.severity << ": " << msg.message << "\n";
	}
	ostream << std::flush;
	
}


void reset () {
	clearLogs ();
	errorReported=false;
}


}
