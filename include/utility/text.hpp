#ifndef TEXTUTIL_HPP
#define TEXTUTIL_HPP


#include <string>


namespace text {


enum PrintMode {PLAIN=0, COLOUR=1};

const std::string normal="\x1B[0m",
	black="\x1B[30m",
	red="\x1B[31m",
	grn="\x1B[32m",
	yel="\x1B[33m",
	blu="\x1B[34m",
	mag="\x1B[35m",
	cyn="\x1B[36m",
	wht="\x1B[37m",
	def="\x1B[39m",
	bgRed="\x1B[41m",
	bgGrn="\x1B[42m",
	bgYel="\x1B[43m",
	bgBlu="\x1B[44m",
	bgMag="\x1B[45m",
	bgCyn="\x1B[46m",
	bgWht="\x1B[47m",
	bgDef="\x1B[49m",
	reset="\033[0m";


template <typename ForwardItt>
std::string listPtrToStr (ForwardItt beg, ForwardItt end) {
	if (beg == end) return "";
	std::string s = (*beg)->toString ();
	while (++beg != end) s += ", " + (*beg)->toString ();
	return s;
}

template <typename ForwardItt, typename ...Args>
std::string listPtrToStr (ForwardItt beg, ForwardItt end, Args ... args) {
	if (beg == end) return "";
	std::string s = (*beg)->toString (args...);
	while (++beg != end) s += ", " + (*beg)->toString (args...);
	return s;
}

};


#endif /*TEXTUTIL_HPP*/