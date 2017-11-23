#include <cstring>
#include <iostream>
#include <unistd.h>
#include <utility/config.hpp>


Config config;


const Config & Config::globalConfig () {
	return config;
}

void Config::initalise (int argl, char ** args) {
	
	if (isatty (stdout->_fileno)) {
		config.printMode = text::COLOUR;
	}
	if (isatty (stderr->_fileno)) {
		config.printModeSTDERR = text::COLOUR;
	}
	
	for (int i=1; i<argl; ++i) {
		char * str = args [i];
		//int len = strlen (str);
		if (str [0] == '-') {
			if (str [1] == '-') {
				//TODO: handle options
			} else {
				if (std::strcmp (str, "-emit-llvm") == 0) {
					config.emitLLVM = true;
					continue;
				} else if (std::strcmp (str, "-no-const-folding") == 0) {
					config.const_folding = false;
					continue;
				} else if (std::strcmp (str, "-interpret") == 0) {
					config.interpret = true;
					continue;
				} else if (std::strcmp (str, "-jit") == 0) {
					config.jit = true;
					continue;
				} else if (std::strcmp (str, "-debug-compiler") == 0) {
					std::cerr << str << " is currently ignored" << std::endl;
					continue;
				} else if (std::strncmp (str, "-core-lib-dir=", 14) == 0) {
					config.coreLibDir = str+14;
					continue;
				}
				for (int j=1; str [j]; ++j) {
					switch (str[j]) {
						case 'c':
							config.link = false;
							break;
						case 'o':
							if (i+1<argl) {
								config.outfile = args [i+1];
								++i;
							} else {
								//TODO: handle invalid options
							}
							break;
						case 'S':
							config.emitAssembly = true;
							break;
						default:
							//TODO: handle invalid options
							break;
					}
				}
			}
		} else {
			config.files.insert (str);
		}
	}
	
}

int numbatMain (const Config & cfg);

int main (int argl, char ** args) {
	Config::initalise (argl, args);
	return numbatMain (config);
}
