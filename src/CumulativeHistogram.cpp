/*
 * CumulativeHistogram.cpp
 *
 *  Created on: 21 mag 2019
 *      Author: lorenzo
 */

#include "CumulativeHistogram.h"

#include <iostream>

CumulativeHistogram::CumulativeHistogram(int n_bins) :
				_min(0.),
				_max(-1e20),
				_n_bins(n_bins) {

}

CumulativeHistogram::~CumulativeHistogram() {

}

void CumulativeHistogram::add_point(double np) {
	_points.push_back(np);
	if(np > _max) {
		_max = np;
	}
}

void CumulativeHistogram::print_out() {
	double bin_size = (_max - _min) / _n_bins;
	std::vector<int> histogram(_n_bins, 0);

	for(auto point : _points) {
		int bin = (point - _min) / bin_size;
		for(int i = 0; i <= bin; i++) {
			histogram[i]++;
		}
	}

	for(int i = 0; i < _n_bins; i++) {
		double bin = _min + bin_size * (i + 0.5);
		std::cout << bin << " " << histogram[i] / (double) histogram[0] << std::endl;
	}
}
