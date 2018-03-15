

#include <fstream>
#include <iostream>
#include <vector>

#include "boost/filesystem.hpp"
#include <boost/algorithm/string.hpp>

#include "CINPointReader.hpp"
#include "stuff.h"

namespace fs = boost::filesystem;

using std::ifstream;
using std::cout;
using std::endl;
using std::vector;
using boost::iequals;
using std::ios;

namespace Potree{

CINPointReader::CINPointReader(string path,  AABB aabb, double scale, PointAttributes pointAttributes){
	this->path = path;
	this->aabb = aabb;
	this->scale = scale;
	this->attributes = pointAttributes;

	if(fs::is_directory(path)){
		// if directory is specified, find all las and laz files inside directory

		for(fs::directory_iterator it(path); it != fs::directory_iterator(); it++){
			fs::path filepath = it->path();
			if(fs::is_regular_file(filepath)){
				files.push_back(filepath.string());
			}
		}
	}else{
		files.push_back(path);
	}

	currentFile = files.begin();
	reader = new ifstream(*currentFile, ios::in | ios::binary);
	// std::cout << "new " << *currentFile << std::endl;
	numPts = 0;
	index = -1;

	this->hasIntensity = false;
	this->bytesPerPoint = 0;
	for(int i = 0; i < attributes.size(); i++){
		PointAttribute attribute = attributes[i];
		if(attribute == PointAttribute::POSITION_CARTESIAN) {
			this->bytesPerPoint += 3 * 4;
		} else if (attribute == PointAttribute::COLOR_PACKED) {
			this->bytesPerPoint += 4;
		} else if (attribute == PointAttribute::INTENSITY) {
			this->bytesPerPoint += 2;
			this->hasIntensity = true;
		}
	}
}

CINPointReader::~CINPointReader(){
	close();
	numPts = 0;
	index = -1;
}

void CINPointReader::close(){
	if(reader != NULL){
		reader->close();
		delete reader;
		reader = NULL;
	}
}

long CINPointReader::numPoints(){
	//TODO

	return 0;
}

bool CINPointReader::readNextPoint(){
	if(!numPts || index == numPts){
		if (numPts) {
			// try to open next file, if available
			if (reader) {
				reader->close();
				delete reader;
			}
			reader = NULL;
			currentFile++;
			if(currentFile != files.end()){
				reader = new ifstream(*currentFile, ios::in | ios::binary);
			}
		}

		if(reader && reader->good()){
    		reader->seekg (0, reader->end);
    		int length = reader->tellg();
    		// std::cout << *currentFile << "length " << length << '.' << std::endl;
    		reader->seekg (0, reader->beg);

    		this->numPts = (length - 6 * 4) / this->bytesPerPoint;

    		if (this->numPts > 0) {
	    		reader->read((char*)this->bbox, sizeof(float) * 6);
	    		this->positions.resize(3 * numPts);
				reader->read((char*)this->positions.data(), numPts * sizeof(float) * 3);
				this->colors.resize(4 * numPts);
				reader->read((char*)this->colors.data(), numPts * 4);
				if (this->hasIntensity) {
					this->intensities.resize(numPts);
					reader->read((char*)this->intensities.data(), numPts * 2);
				}
			}
			index = 0;
		} else {
			return false;
		}
	}

	if(numPts && index < numPts){
		point = Point();

		point.position.x = (positions[3 * index] * scale) + aabb.min.x;
		point.position.y = (positions[3 * index + 1] * scale) + aabb.min.y;
		point.position.z = (positions[3 * index + 2] * scale) + aabb.min.z;

		point.color.x = colors[4 * index + 0];
		point.color.y = colors[4 * index + 1];
		point.color.z = colors[4 * index + 2];

		point.intensity = intensities[index];

		index++;
		return true;
	}

	return false;
}

Point CINPointReader::getPoint(){
	return point;
}

AABB CINPointReader::getAABB(){
	AABB aabb;
	//TODO

	return aabb;
}

}






