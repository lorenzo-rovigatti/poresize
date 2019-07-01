/*
 * CumulativeHistogram.h
 *
 *  Created on: 21 mag 2019
 *      Author: lorenzo
 */

#ifndef SRC_CUMULATIVEHISTOGRAM_H_
#define SRC_CUMULATIVEHISTOGRAM_H_

#include <vector>
#include <string>

class CumulativeHistogram {
public:
	CumulativeHistogram(double bin_size);
	virtual ~CumulativeHistogram();

	void add_point(double np);
	void print_out(std::string filename);

protected:
	std::vector<double> _points;
	double _min;
	double _max;
	double _bin_size;
};

#endif /* SRC_CUMULATIVEHISTOGRAM_H_ */
