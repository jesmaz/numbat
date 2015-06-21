#ifndef TEXTUTIL_HPP
#define TEXTUTIL_HPP


#include <string>


namespace text {


enum PrintMode {PLAIN=0, COLOUR=1};

const std::string normal="\x1B[0m",
	red="\x1B[31m",
	grn="\x1B[32m",
	yel="\x1B[33m",
	blu="\x1B[34m",
	mag="\x1B[35m",
	cyn="\x1B[36m",
	wht="\x1B[37m",
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