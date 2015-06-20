#ifndef TEXTUTIL_HPP
#define TEXTUTIL_HPP


#include <string>


namespace text {


enum PrintMode {PLAIN=0, COLOUR=1};

const std::string normal="\x1B[0m",
	red="\x1B[31m",
	grn="\x1B[32m",
	blu="\x1B[33m",
	yel="\x1B[34m",
	mag="\x1B[35m",
	cyn="\x1B[36m",
	wht="\x1B[37m",
	reset="\033[0m";

};



#endif /*TEXTUTIL_HPP*/