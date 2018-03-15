
#ifndef CINPOINTWRITER_H
#define CINPOINTWRITER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "AABB.h"
#include "PointAttributes.hpp"
#include "PointWriter.hpp"
#include "stuff.h"


using std::string;
using std::vector;
using std::ofstream;
using std::ios;

namespace Potree{

class CINPointWriter : public PointWriter{

public:
	string file;
	int numPoints;
	PointAttributes attributes;
	ofstream *writer;
	AABB aabb;
	double scale;

	struct Position {
		float xyz[3];
		Position(float x, float y, float z) { xyz[0] = x; xyz[1] = y; xyz[2] = z;}
	};
	struct Color {
		unsigned char rgba[4];
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { rgba[0] = r; rgba[1] = g; rgba[2] = b; rgba[3] = a;}
	};

	std::vector<Position> positions;
	std::vector<Color> colors;
	std::vector<unsigned short> intensities;

	struct {
		float x, y, z;
	} _min, _max;

	CINPointWriter(string file, AABB aabb, double scale, PointAttributes pointAttributes) {
		this->file = file;
		this->aabb = aabb;
		this->scale = scale;
		numPoints = 0;

		attributes = pointAttributes;

		writer = new ofstream(file, ios::out | ios::binary);


	}

	CINPointWriter(string file, PointAttributes attributes) {
		this->file = file;
		numPoints = 0;
		this->attributes = attributes;

		writer = new ofstream(file, ios::out | ios::binary);
	}

	~CINPointWriter(){
		close();
	}


	void write(const Point &point){
		for(int i = 0; i < attributes.size(); i++){
			PointAttribute attribute = attributes[i];
			if(attribute == PointAttribute::POSITION_CARTESIAN){
				//float pos[3] = {(float) point.x,(float)  point.y,(float)  point.z};
				// int x = (int)((point.position.x - aabb.min.x) / scale);
				// int y = (int)((point.position.y - aabb.min.y) / scale);
				// int z = (int)((point.position.z - aabb.min.z) / scale);
				float x = ((point.position.x - aabb.min.x) / scale);
				float y = ((point.position.y - aabb.min.y) / scale);
				float z = ((point.position.z - aabb.min.z) / scale);

				if (positions.size() == 0) {
					_min.x = x; _min.y = y; _min.z = z;
					_max.x = x; _max.y = y; _max.z = z;
				} else {
					_min.x = std::min(_min.x, x); _min.y = std::min(_min.y, y); _min.z = std::min(_min.z, z);
					_max.x = std::max(_max.x, x); _max.y = std::max(_max.y, y); _max.z = std::max(_max.z, z);
				}

				positions.push_back(Position(x, y, z));

			}else if(attribute == PointAttribute::COLOR_PACKED){
				colors.push_back(Color(point.color.x, point.color.y, point.color.z, 255));
			}else if(attribute == PointAttribute::INTENSITY){
				intensities.push_back(point.intensity);
				// unsigned char i = (unsigned char)(255 * (point.intensity / 65535.0));
				// colors.push_back(i);
				// colors.push_back(i);
				// colors.push_back(i);
				// colors.push_back(i);
			}else if(attribute == PointAttribute::CLASSIFICATION){
				// throw;
			}else if(attribute == PointAttribute::NORMAL_SPHEREMAPPED){
				// throw;
			}else if(attribute == PointAttribute::NORMAL_OCT16){
				// throw;
			}else if(attribute == PointAttribute::NORMAL){
				// throw;
			}
		}

		numPoints++;
	}


	void close(){
		if(writer != NULL){
			writer->write((const char*)&_min, 3 * sizeof(int));
			writer->write((const char*)&_max, 3 * sizeof(int));

			// randomize
			if (true) {
				// ...
				std::vector<size_t> indices(positions.size());
				const size_t count = positions.size();
				for (size_t i=0; i<count; i++) {
					indices[i] = i;
				}
				std::random_shuffle(indices.begin(), indices.end());

				for (size_t i=0; i<count; i++) {
					writer->write((const char*)&(positions[indices[i]].xyz), 3 * sizeof(float));
				}
				for (size_t i=0; i<count; i++) {
					writer->write((const char*)&(colors[indices[i]].rgba), 4);
				}
				if (intensities.size() > 0) {
					for (size_t i=0; i<count; i++) {
						writer->write((const char*)&(intensities[indices[i]]), 2);
					}
				}
			} else {
				writer->write((const char*)positions.data(), positions.size() * 3 * sizeof(int));
            	writer->write((const char*)colors.data(), colors.size() * 4 * sizeof(unsigned char));
			}



			writer->close();
			delete writer;
			writer = NULL;
		}
	}

};

}

#endif


