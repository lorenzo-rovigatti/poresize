/*
 * main.cpp
 *
 *  Created on: May 20, 2019
 *      Author: lorenzo
 */

#include "System.h"

#include <nlopt.hpp>

#include <cstdlib>
#include <iostream>
#include <cmath>

// this is the function that calculates the radius of the largest sphere centred in "centre" that does not overlap with any of the particles
double radius(System &syst, const vec3 &centre) {
	ivec3 centre_cell = syst.get_cell(centre);

	bool done = false;
	double R_sqr = -1;
	// here we loop on the various cells, shell after shell
	for(unsigned int i = 0; i < syst.cell_shifts.size() && !done; i++) {
		// the current shell might contain a particle that is closer than one found in the previous shell. We thus tell the code
		// to analyse also this shell and then quit the loop
		if(R_sqr > 0.) {
			done = true;
		}

		auto &shifts = syst.cell_shifts[i];
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
	}

	std::cout << centre[0] << " " << centre[1] << " " << centre[2] << " " << sqrt(R_sqr) << std::endl;

	return sqrt(R_sqr);
}

// this is the function we want to maximise
double function_to_maximise(unsigned n, const double *x, double *grad, void *f_data) {
	System *syst = (System *) f_data;
	vec3 centre(x[0], x[1], x[2]);
	return radius(*syst, centre);
}

struct constraint_data {
	System *syst;
	vec3 *position;
};

double constraint(unsigned n, const double *x, double *grad, void *data) {
	constraint_data *d = (constraint_data *) data;
	vec3 centre(x[0], x[1], x[2]);

	double R = radius(*(d->syst), *(d->position));
	vec3 distance = centre - *(d->position);
	// periodic boundary conditions
	distance -= glm::round(distance / d->syst->box) * d->syst->box;

	// nlopt requires constraints to have the form f(x) <= 0. Here we set d - R <= 0, where d is the
	// distance between the sphere centre and the position
	return sqrt(glm::dot(distance, distance)) - R;
}

double find_maximum_radius(System &syst, const vec3 &position) {
	nlopt::opt opt(nlopt::LN_COBYLA, 3);
	std::vector<double> lower_bounds({0., 0., 0.});
	std::vector<double> upper_bounds({syst.box[0], syst.box[1], syst.box[2]});
	opt.set_lower_bounds(lower_bounds);
	opt.set_upper_bounds(upper_bounds);
	opt.set_max_objective(function_to_maximise, (void *) &syst);
	opt.set_xtol_rel(1e-8);

	constraint_data data;
	data.syst = &syst;
	data.position = (vec3 *) &position;

	opt.add_inequality_constraint(constraint, &data, 1e-8);

	std::vector<double> starting_position({position[0], position[1], position[2]});
	double maximum_radius;

	try {
	    nlopt::result result = opt.optimize(starting_position, maximum_radius);
	}
	catch(std::exception &e) {
	    std::cerr << "nlopt failed: " << e.what() << std::endl;
	    exit(1);
	}

	return maximum_radius;
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " input_file r_cut" << std::endl;
		exit(1);
	}

	System syst(argv[1], atof(argv[2]));
	std::cout << find_maximum_radius(syst, vec3(5., 5., 0.)) << std::endl;

	return 0;
}
