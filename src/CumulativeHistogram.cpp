/*
 * CumulativeHistogram.cpp
 *
 *  Created on: 21 mag 2019
 *      Author: lorenzo
 */

#include "CumulativeHistogram.h"

#include <iostream>
#include <cmath>

CumulativeHistogram::CumulativeHistogram(double bin_size) :
				_min(0.),
				_max(-1e20),
				_bin_size(bin_size) {

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
	int n_bins = ceil((_max - _min) / _bin_size);
	_bin_size = (_max - _min) / n_bins;
	std::vector<int> histogram(n_bins, 0);

	for(auto point : _points) {
		int bin = (point - _min) / _bin_size;
		for(int i = 0; i <= bin; i++) {
			histogram[i]++;
		}
	}

	for(int i = 0; i < n_bins; i++) {
		double bin = _min + _bin_size * (i + 0.5);
		std::cout << bin << " " << histogram[i] / (double) histogram[0] << std::endl;
	}
}
