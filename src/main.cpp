/*
 * main.cpp
 *
 *  Created on: May 20, 2019
 *      Author: lorenzo
 */

#include "System.h"
#include "CumulativeHistogram.h"

#include <nlopt.hpp>

#include <algorithm>
#include <random>
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
				// if the point is inside one of the spheres we just return
				if(distance_sqr < syst.particle_radius_sqr) {
					return 0.;
				}
				if(R_sqr < 0. || distance_sqr < R_sqr) {
					R_sqr = distance_sqr;
				}

				current = syst.next[current];
			}
		}
	}

	return sqrt(R_sqr) - syst.particle_radius;
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

	double R = radius(*(d->syst), centre);
	vec3 distance = centre - *(d->position);
	// periodic boundary conditions
	distance -= glm::round(distance / d->syst->box) * d->syst->box;

	// nlopt requires constraints to have the form f(x) <= 0. Here we set d - R <= 0, where d is the
	// distance between the sphere centre and the position
	return sqrt(glm::dot(distance, distance)) - R;
}

double find_maximum_radius(System &syst, nlopt::opt &opt, const vec3 &position) {
	constraint_data data;
	data.syst = &syst;
	data.position = (vec3 *) &position;

	opt.remove_inequality_constraints();
	opt.add_inequality_constraint(constraint, &data, 1e-6);

	std::vector<double> starting_position({position[0], position[1], position[2]});
	double maximum_radius;

	try {
	    nlopt::result result = opt.optimize(starting_position, maximum_radius);
	    if(result == nlopt::MAXTIME_REACHED) {
	    	std::cerr << "WARNING: maxtime reached, skipping this attempt" << std::endl;
	    	return 0.;
	    }
	}
	catch(std::exception &e) {
	    std::cerr << "nlopt failed: " << e.what() << std::endl;
	    exit(1);
	}

	return maximum_radius;
}

int main(int argc, char *argv[]) {
	if(argc < 6) {
		std::cerr << "Usage is " << argv[0] << " input_file input_file_type=oxDNA|LAMMPS r_cut histogram_bin_size steps" << std::endl;
		exit(1);
	}

	std::random_device dev;
	// deterministic runs for debugging purposes, use dev() to "randomly" initialise the rng
	std::mt19937 rng(12345);
	std::uniform_real_distribution<double> uniform(0., 1.);

	std::string input_type(argv[2]);
	std::transform(input_type.begin(), input_type.end(), input_type.begin(), ::toupper);

	// initialise the system
	System syst(atof(argv[3]));
	if(input_type == "LAMMPS") {
		syst.init_from_LAMMPS(argv[1]);
	}
	else if(input_type == "OXDNA") {
		syst.init_from_oxDNA(argv[1]);
	}
	else {
		std::cerr << "Invalid input_file_type '" << argv[2] << "', only LAMMPS and oxDNA supported" << std::endl;
		exit(1);
	}

	// initialise the optimisation machinery
	nlopt::opt opt(nlopt::LN_COBYLA, 3);
	// we restrict the solver to the region of space comprised between 0 and the box size
	std::vector<double> lower_bounds({0., 0., 0.});
	std::vector<double> upper_bounds({syst.box[0], syst.box[1], syst.box[2]});
	opt.set_lower_bounds(lower_bounds);
	opt.set_upper_bounds(upper_bounds);
	opt.set_max_objective(function_to_maximise, (void *) &syst);
	opt.set_xtol_rel(1e-4);
	opt.set_maxtime(1.);

	CumulativeHistogram result(atof(argv[4]));

	long long int steps = atol(argv[5]);
	for(int i = 0; i < steps; i++) {
		if(i > 0 && (i % (steps / 10) == 0)) {
			std::cerr << i << " steps completed" << std::endl;
		}
		vec3 random_position(uniform(rng) * syst.box[0], uniform(rng) * syst.box[1], uniform(rng) * syst.box[2]);

		// if the random position is inside one of the particles we disregard it
		if(radius(syst, random_position) > 0.) {
			double maximum_R = find_maximum_radius(syst, opt, random_position);
			if(maximum_R > 0.) {
				result.add_point(maximum_R);
			}
		}
	}

	result.print_out();

	return 0;
}
