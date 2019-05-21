/*
 * main.cpp
 *
 *  Created on: May 20, 2019
 *      Author: lorenzo
 */

#include "System.h"

#include <cstdlib>
#include <iostream>
#include <cmath>

double maximum_radius(System &syst, const vec3 &centre) {
	ivec3 centre_cell = syst.get_cell(centre);

	double R_sqr = -1;
	for(auto &shifts : syst.cell_shifts) {
		for(auto &cell_shift : shifts) {
			ivec3 cell = centre_cell + cell_shift;
			cell[0] = (cell[0] + syst.N_cells_side[0]) % syst.N_cells_side[0];
			cell[1] = (cell[1] + syst.N_cells_side[1]) % syst.N_cells_side[1];
			cell[2] = (cell[2] + syst.N_cells_side[2]) % syst.N_cells_side[2];
			int cell_idx = cell[0] + syst.N_cells_side[0] * (cell[1] + cell[2] * syst.N_cells_side[1]);
			int centre_cell_idx = centre_cell[0] + syst.N_cells_side[0] * (centre_cell[1] + centre_cell[2] * syst.N_cells_side[1]);

			int current = syst.heads[cell_idx];
			while(current != -1) {
				vec3 distance = centre - syst.particles[current];
				// periodic boundary conditions
				distance -= glm::round(distance / syst.box) * syst.box;
				double distance_sqr = glm::dot(distance, distance);
				if(R_sqr < 0. || distance_sqr < R_sqr) {
					R_sqr = distance_sqr;
				}

				current = syst.next[current];
			}
		}
		if(R_sqr > 0.) {
			break;
		}
	}

	return sqrt(R_sqr);
}

int main(int argc, char *argv[]) {
	if(argc < 2) {
		std::cerr << "Usage is " << argv[0] << " input_file" << std::endl;
		exit(1);
	}

	System syst(argv[1]);
	std::cout << maximum_radius(syst, vec3(10., 0., 0.)) << std::endl;

	return 0;
}
