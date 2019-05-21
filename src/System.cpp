/*
 * System.cpp
 *
 *  Created on: May 20, 2019
 *      Author: lorenzo
 */

#include "System.h"

#include <glm/gtx/component_wise.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>

System::System(char *filename, double rc) :
				r_cut(rc) {
	_parse_input(filename);
	_init_cells();
}

System::~System() {

}

int System::get_cell_index(const vec3 &pos) const {
	int res = (int) ((pos[0] / box[0] - floor(pos[0] / box[0])) * (1.0 - DBL_EPSILON) * N_cells_side[0]);
	res += N_cells_side[0] * ((int) ((pos[1] / box[1] - floor(pos[1] / box[1])) * (1.0 - DBL_EPSILON) * N_cells_side[1]));
	res += N_cells_side[0] * N_cells_side[1] * ((int) ((pos[2] / box[2] - floor(pos[2] / box[2])) * (1.0 - DBL_EPSILON) * N_cells_side[2]));
	return res;
}

ivec3 System::get_cell(const vec3 &pos) const {
	int cell_index = get_cell_index(pos);
	return ivec3(
			cell_index % N_cells_side[0],
			(cell_index / N_cells_side[0]) % N_cells_side[1],
			cell_index / (N_cells_side[0] * N_cells_side[1])
	);
}

void System::_parse_input(char *filename) {
	std::ifstream input(filename);

	if(!input.good()) {
		std::cerr << "Invalid file '" << filename << "'" << std::endl;
		exit(1);
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
		std::cerr << "The box line '" << line << "' found in the oxDNA configuration is not valid" << std::endl;
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

	std::cerr << "Number of particles: " << particles.size() << ", box: <" << box[0] << ", " << box[1] << ", " << box[2] << ">" << std::endl;

	input.close();
}

void System::_init_cells() {
	N_cells_side = glm::floor(box / r_cut);
	// everything becomes easier if the number of cells per side is odd
	for(int i = 0; i < 3; i++) {
		if((N_cells_side[i] % 2) == 0) {
			N_cells_side[i]--;
		}
		if(N_cells_side[i] < 3) {
			N_cells_side[i] = 3;
		}
	}
	int N_cells = N_cells_side[0] * N_cells_side[1] * N_cells_side[3];
	next.resize(particles.size(), -1);
	heads.resize(N_cells, -1);

	std::cerr << "Number of cells: " << N_cells << ", cells per box side: <" << N_cells_side[0] << ", " << N_cells_side[1] << ", " << N_cells_side[2] << ">" << std::endl;

	for(unsigned int i = 0; i < particles.size(); i++) {
		int cell_idx = get_cell_index(particles[i]);
		next[i] = heads[cell_idx];
		heads[cell_idx] = i;
	}

	// here we generate lists of all possible shifts according to their order (i.e. distance from the central cell)
	int max_shift = (glm::compMax(N_cells_side) - 1) / 2;
	cell_shifts.resize(max_shift);
	ivec3 shift;
	for(shift[0] = -N_cells_side[0] / 2; shift[0] <= N_cells_side[0] / 2; shift[0]++) {
		for(shift[1] = -N_cells_side[1] / 2; shift[1] <= N_cells_side[1] / 2; shift[1]++) {
			for(shift[2] = -N_cells_side[2] / 2; shift[2] <= N_cells_side[2] / 2; shift[2]++) {
				auto shift_abs = glm::abs(shift);
				int shift_order = glm::compMax(shift_abs) - 1;
				if(shift_order == -1) {
					shift_order = 0;
				}
				cell_shifts[shift_order].push_back(shift);
			}
		}
	}

	std::cerr << "Maximum shift order: " << max_shift << ", number of shifts per order:";
	for(auto &shift: cell_shifts) {
		std::cerr << " " << shift.size();
	}
	std::cerr << std::endl;
}
