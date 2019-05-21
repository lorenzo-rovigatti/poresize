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
using ivec3 = glm::ivec3;
using vector_vec3 = std::vector<vec3>;

class System {
public:
	System(char *filename);
	virtual ~System();

	int get_cell_index(const vec3 &pos) const;
	glm::ivec3 get_cell(const vec3 &pos) const;

	vector_vec3 particles;
	vec3 box;

	double r_cut;
	glm::ivec3 N_cells_side;
	std::vector<int> next;
	std::vector<int> heads;
//	std::vector<glm::ivec3> cell_shifts;
	std::vector<std::vector<ivec3>> cell_shifts;

protected:
	void _parse_input(char *filename);
	void _init_cells();
};

#endif /* SRC_SYSTEM_H_ */
