#include <utility/config.hpp>

extern Config config;

int numbatMain (const Config & cfg);

int main (int argl, char ** args) {
	Config::initalise (argl, args);
	return numbatMain (config);
}
