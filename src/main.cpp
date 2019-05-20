/*
 * main.cpp
 *
 *  Created on: May 20, 2019
 *      Author: lorenzo
 */

#include "System.h"

#include <iostream>

int main(int argc, char *argv[]) {
	if(argc < 2) {
		std::cerr << "Usage is " << argv[0] << " input_file" << std::endl;
		exit(1);
	}

	System syst(argv[1]);

	return 0;
}
