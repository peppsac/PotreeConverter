
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

	std::vector<float> positions;
	std::vector<unsigned char> colors;

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

				positions.push_back(x);
				positions.push_back(y);
				positions.push_back(z);

			}else if(attribute == PointAttribute::COLOR_PACKED){
				// std::cout << point.intensity << '/' << (int)((unsigned char)(point.intensity)) << '/' << (int)point.color.x << std::endl;
				// unsigned char intensity = (unsigned char)(point.intensity);// / 255);
				unsigned char rgba[4] = {point.color.x, point.color.y, point.color.z, 255};
				colors.push_back(rgba[0]);
				colors.push_back(rgba[1]);
				colors.push_back(rgba[2]);
				colors.push_back(rgba[3]);
			}else if(attribute == PointAttribute::INTENSITY){
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

	void write2(const Point &point){
		for(int i = 0; i < attributes.size(); i++){
			PointAttribute attribute = attributes[i];
			if(attribute == PointAttribute::POSITION_CARTESIAN){
				//float pos[3] = {(float) point.x,(float)  point.y,(float)  point.z};
				int x = (int)((point.position.x - aabb.min.x) / scale);
				int y = (int)((point.position.y - aabb.min.y) / scale);
				int z = (int)((point.position.z - aabb.min.z) / scale);
				int pos[3] = {x, y, z};
				writer->write((const char*)pos, 3*sizeof(int));
			}else if(attribute == PointAttribute::COLOR_PACKED){
				unsigned char rgba[4] = {point.color.x, point.color.y, point.color.z, 255};
				writer->write((const char*)rgba, 4*sizeof(unsigned char));
			}else if(attribute == PointAttribute::INTENSITY){
				writer->write((const char*)&point.intensity, sizeof(unsigned short));
			}else if(attribute == PointAttribute::CLASSIFICATION){
				writer->write((const char*)&point.classification, sizeof(unsigned char));
			}else if(attribute == PointAttribute::NORMAL_SPHEREMAPPED){
				// see http://aras-p.info/texts/CompactNormalStorage.html
				float nx = point.normal.x;
				float ny = point.normal.y;
				float nz = point.normal.z;
				float lengthxy = sqrt(nx * nx + ny * ny);

				float ex = 0.5f * (nx / lengthxy) * sqrt(-nz * 0.5f + 0.5f) + 0.5f;
				float ey = 0.5f * (ny / lengthxy) * sqrt(-nz * 0.5f + 0.5f) + 0.5f;

				unsigned char bx = (unsigned char)(ex * 255);
				unsigned char by = (unsigned char)(ey * 255);

				writer->write((const char*)&bx, 1);
				writer->write((const char*)&by, 1);
			}else if(attribute == PointAttribute::NORMAL_OCT16){
				// see http://lgdv.cs.fau.de/get/1602

				float nx = point.normal.x;
				float ny = point.normal.y;
				float nz = point.normal.z;

				float norm1 = abs(nx) + abs(ny) + abs(nz);

				nx = nx / norm1;
				ny = ny / norm1;
				nz = nz / norm1;

				float u = 0;
				float v = 0;

				if(nz >= 0){
					u = nx;
					v = ny;
				}else{
					u = psign(nx)*(1-psign(ny)*ny);
					v = psign(ny)*(1-psign(nx)*nx);
				}

				unsigned char bx = (unsigned char)(min((u + 1) * 128, 255.0f));
				unsigned char by = (unsigned char)(min((v + 1) * 128, 255.0f));

				writer->write((const char*)&bx, 1);
				writer->write((const char*)&by, 1);
			}else if(attribute == PointAttribute::NORMAL){
				writer->write((const char*)&point.normal.x, sizeof(float));
				writer->write((const char*)&point.normal.y, sizeof(float));
				writer->write((const char*)&point.normal.z, sizeof(float));
			}
		}

		numPoints++;
	}

	void close(){
		if(writer != NULL){
			// std::cout << "WRITING FOR REAL " << file << ':' << numPoints<<  ',' << (positions.size() / 3) << ',' << (colors.size() / 4) << std::endl;

			writer->write((const char*)&_min, 3 * sizeof(int));
			writer->write((const char*)&_max, 3 * sizeof(int));
			writer->write((const char*)positions.data(), positions.size() * sizeof(int));
			writer->write((const char*)colors.data(), colors.size() * sizeof(unsigned char));


			writer->close();
			delete writer;
			writer = NULL;
		}
	}

};

}

#endif


