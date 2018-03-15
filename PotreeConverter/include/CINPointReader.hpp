

#ifndef CINPOINTREADER_H
#define CINPOINTREADER_H

#include <string>
#include <iostream>
#include <vector>

#include "Point.h"
#include "PointReader.h"
#include "PointAttributes.hpp"

using std::string;

using std::ifstream;
using std::cout;
using std::endl;
using std::vector;

namespace Potree{

class CINPointReader : public PointReader{
private:
	AABB aabb;
	double scale;
	string path;
	vector<string> files;
	vector<string>::iterator currentFile;
	ifstream *reader;
	PointAttributes attributes;
	Point point;

	int numPts;
	int index;
	float bbox[6];
	std::vector<float> positions;
	std::vector<unsigned char> colors;
	std::vector<unsigned short> intensities;

	int bytesPerPoint;
	bool hasIntensity;

public:

	CINPointReader(string path, AABB aabb, double scale, PointAttributes pointAttributes);

	~CINPointReader();

	bool readNextPoint();

	Point getPoint();

	AABB getAABB();

	long numPoints();

	void close();

	Vector3<double> getScale();
};

}

#endif