#pragma once


#include <chrono>
#include <iostream>
#include <set>
#include <string>
#include <sstream>


struct StopWatch {
	
	public:
		
		struct Lap {
			public:
				Lap (StopWatch & ref) : ref (ref), start (std::chrono::steady_clock::now ()) {}
				Lap (const Lap &)=delete;
				~Lap () {
					ref.totalTime += std::chrono::steady_clock::now () - start;
					++ref.laps;
				}
				const Lap & operator = (const Lap &)=delete;
			private:
				StopWatch & ref;
				std::chrono::steady_clock::time_point start;
		};
		
		StopWatch (const StopWatch &)=delete;
		const StopWatch & operator = (const StopWatch &)=delete;
		
		StopWatch (const std::string & name) : name (name), totalTime (0), laps (0) {++SW_Count;}
		~StopWatch () {
			auto micro = std::chrono::duration_cast <std::chrono::microseconds> (totalTime).count ();
			double msecs =  micro / 1000.0;
			std::ostringstream ostr;
			if (micro > 10000) {
				double secs =  micro / 1000000.0;
				ostr << "Report for '" << name << "': " << laps << " laps in " << secs << "s (avg: " << (msecs/laps) << "ms)\n";
			} else {
				ostr << "Report for '" << name << "': " << laps << " laps in " << msecs << "ms (avg: " << (msecs/laps) << "ms)\n";
			}
			SW_Report.insert (ostr.str ());
			if (--SW_Count == 0) {
				for (auto & s : SW_Report) {
					std::cerr << s;
				}
			}
		}
		
		static StopWatch test;
		
	protected:
	private:
		
		std::string name;
		std::chrono::steady_clock::duration totalTime;
		size_t laps;
		
		static std::set <string> SW_Report;
		static size_t SW_Count;
		
};

std::set <string> StopWatch::SW_Report= {};
size_t StopWatch::SW_Count = 0;

#define PROFILE(str) \
static StopWatch __PROFIER_SW__ (str); \
StopWatch::Lap __PROFIER_SW__LAP__ (__PROFIER_SW__);
