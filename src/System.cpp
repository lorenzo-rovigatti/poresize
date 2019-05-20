/*
 * System.cpp
 *
 *  Created on: May 20, 2019
 *      Author: lorenzo
 */

#include "System.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>

System::System(char *filename) {
	_parse_input(filename);
	_init_cells();

	rcut = 1.0;
}

System::~System() {

}

void System::_parse_input(char *filename) {
	std::ifstream input(filename);

	if(!input.good()) {
		std::cerr << "Invalid file '" << filename << "'" << std::endl;
	}

	std::string line;
	std::vector<std::string> split;
	std::getline(input, line);
	std::getline(input, line);
	boost::split(split, line, boost::is_any_of(" "));
	try {
		box[0] = boost::lexical_cast<double>(boost::trim_copy(split[2]));
		box[1] = boost::lexical_cast<double>(boost::trim_copy(split[3]));
		box[2] = boost::lexical_cast<double>(boost::trim_copy(split[4]));
	}
	catch(boost::bad_lexical_cast &e) {
		std::cerr << "The box line '" << line << "' found in the oxDNA configuration is not valid";
		exit(1);
	}
	std::getline(input, line);

	bool end = false;
	while(input.good() && !end) {
		if(input.peek() == 't') {
			end = true;
		}
		else {
			getline(input, line);
			if(input.good() && line.size() > 0) {
				boost::split(split, line, boost::is_any_of(" "));
				vec3 position = vec3(boost::lexical_cast<double>(split[0]), boost::lexical_cast<double>(split[1]), boost::lexical_cast<double>(split[2]));
				particles.push_back(position);
			}
		}
	}

	std::cout << "Number of particles: " << particles.size() << ", box: <" << box[0] << ", " << box[1] << ", " << box[2] << ">" << std::endl;

	input.close();
}

void System::_init_cells() {
	N_cells_side = glm::floor(box / rcut);
	int N_cells = N_cells_side[0] * N_cells_side[1] * N_cells_side[3];
	next.resize(N_cells);
	head.resize(N_cells);
}
