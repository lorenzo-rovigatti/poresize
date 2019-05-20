/*
 * System.h
 *
 *  Created on: May 20, 2019
 *      Author: lorenzo
 */

#ifndef SRC_SYSTEM_H_
#define SRC_SYSTEM_H_

#include <glm/vec3.hpp>
#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include <vector>

using vec3 = glm::dvec3;
using vector_vec3 = std::vector<vec3>;

class System {
public:
	System(char *filename);
	virtual ~System();

	vector_vec3 particles;
	vec3 box;

	double rcut;
	glm::ivec3 N_cells_side;
	std::vector<int> next;
	std::vector<int> head;

protected:
	void _parse_input(char *filename);
	void _init_cells();
};

#endif /* SRC_SYSTEM_H_ */
