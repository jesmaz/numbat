#include <algorithm>
#include <iostream>
#include <utility/array.hpp>
#include <utility/config.hpp>
#include <utility/report.hpp>
 


namespace report {


std::ostream & operator << (std::ostream & os, Severity sev) {
	switch (sev) {
		case ERROR:
			return os << "ERROR";
		case NOTICE:
			return os << "NOTICE";
		case WARNING:
			return os << "WARNING";
	}
}

struct Message {
	Severity severity;
	std::string file;
	numbat::lexer::position pos;
	std::string message;
	bool operator < (const Message & rhs) const {
		int cmp = file.compare (rhs.file);
		if (cmp) return cmp < 0;
		if (pos.line != rhs.pos.line) return pos.line < rhs.pos.line;
		if (pos.col != rhs.pos.col) return pos.col < rhs.pos.col;
		cmp = message.compare (rhs.message);
		if (cmp) return cmp < 0;
		return false;
	}
	Message (Severity s, const std::string & f, numbat::lexer::position p, const std::string & m) : severity (s), file (f), pos (p), message (m) {}
	Message () {}
};


bool errorReported=false;

DynArray <Message> messages;


bool compilationFailed () {
	return errorReported;
}

bool empty () {
	return messages.empty ();
}


const string & getPrintMode (Severity sev) {
	switch (sev) {
		case Severity::ERROR:
			return text::red;
		case Severity::NOTICE:
			return text::cyn;
		case Severity::WARNING:
			return text::yel;
	}
}

void clearLogs () {
	messages.clear ();
}


void dumpLogs () {
	printLogs ();
	clearLogs ();
}

void dumpLogs (std::ostream & ostream, text::PrintMode mode) {
	printLogs (ostream, mode);
	clearLogs ();
}


void logMessage (Severity severity, const numbat::File * file, numbat::lexer::position pos, const std::string & message) {
	messages.push_back (Message (severity, file->getFileName (), pos, message));
	std::sort (messages.begin (), messages.end ());
	if (not errorReported or severity == ERROR) errorReported = true;
}


void printLogs () {
	text::PrintMode mode = Config::globalConfig ().printModeSTDERR;
	printLogs (std::cerr, mode);
}

void printLogs (std::ostream & ostream, text::PrintMode mode) {
	
	for (const Message & msg : messages) {
		if (mode == text::COLOUR) ostream << getPrintMode (msg.severity);
		ostream << msg.severity << ": " << msg.file << ":" << msg.pos.line << ":" << msg.pos.col << ": " << msg.message << "\n";
		if (mode == text::COLOUR) ostream << text::reset;
	}
	ostream << std::flush;
	
}


void reset () {
	clearLogs ();
	errorReported=false;
}


}
