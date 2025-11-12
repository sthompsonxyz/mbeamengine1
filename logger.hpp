#pragma once

#include <string>
#include <iostream>
#include <fstream>


namespace {

	class logger {
		std::ofstream outfile;
	public:

		logger() {
			outfile.open("testout.txt");
		}
		~logger() {
			outfile.close();
		}

		void log(std::string s) {
			outfile << s << std::endl;
		}


	};


}