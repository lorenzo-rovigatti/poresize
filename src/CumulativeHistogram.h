/*
 * CumulativeHistogram.h
 *
 *  Created on: 21 mag 2019
 *      Author: lorenzo
 */

#ifndef SRC_CUMULATIVEHISTOGRAM_H_
#define SRC_CUMULATIVEHISTOGRAM_H_

#include <vector>

class CumulativeHistogram {
public:
	CumulativeHistogram(int n_bins);
	virtual ~CumulativeHistogram();

	void add_point(double np);
	void print_out();

protected:
	std::vector<double> _points;
	double _min;
	double _max;
	int _n_bins;
};

#endif /* SRC_CUMULATIVEHISTOGRAM_H_ */
