#pragma once
#include "camera.h"
#include <fstream>


Object getline(fstream& fin, vector<Object>& buff) {
	string str;
	fin >> str;
	if (str == "combined") {
		fin >> str;
		buff.push_back(getline(fin, buff));
		if(str!="invert")
			buff.push_back(getline(fin, buff));
		if(str == "add")
			return Object(Type::COMBINED, Comb::ADD, &buff[buff.size() - 2], &buff[buff.size() - 1]);
		if (str == "invert")
			return Object(Type::COMBINED, Comb::NEGATIVE, &buff[buff.size() - 1]);
		if (str == "multiply")
			return Object(Type::COMBINED, Comb::MULTIPLY, &buff[buff.size() - 2], &buff[buff.size() - 1]);
		if (str == "smoothadd")
			return Object(Type::COMBINED, Comb::SMOOTHADD, &buff[buff.size() - 2], &buff[buff.size() - 1]);
		if (str == "subtrl")
			return Object(Type::COMBINED, Comb::SUBTRL, &buff[buff.size() - 2], &buff[buff.size() - 1]);
		if (str == "subtrr")
			return Object(Type::COMBINED, Comb::SUBTRR, &buff[buff.size() - 2], &buff[buff.size() - 1]);
	}
	else {
		sf::Vector3f pos, angle;
		int r, g, b, hmm;
		double length, width, height;

		fin >> pos.x >> pos.y >> pos.z;
		fin >> angle.x >> angle.y >> angle.z;
		fin >> r >> g >> b;
		
		if (str == "sphere") {
			fin >> length;
			return Object(Type::SPHERE, pos, angle, sf::Color(r, g, b), length);
		}
		if (str == "cube") {
			fin >> length;
			return Object(Type::CUBE, pos, angle, sf::Color(r, g, b), length);
		}
		if (str == "plane") {
			fin >> length;
			return Object(Type::PLANE, pos, angle, sf::Color(r, g, b), length);
		}
		if (str == "cuboid") {
			fin >> length >> width >> height;
			return Object(Type::CUBOID, pos, angle, sf::Color(r, g, b), length, width, height);
		}
	}
}

// функция получает имя файла(типо путь). ссылку на вектор обьектов, ссылку на камеру, ссылку на вектор освещения.
void readFromFile(string fname, Camera& camera, vector<Object>& objects, 
	vector<Object>& buff, double& fog, double& cam, double& shadow, sf::Vector3f& light) {
	fstream fin;
	sf::Vector3f pos;
	double a, b, c, epsilon;
	int maxiter;
	fin.open(fname);
	fin >> pos.x >> pos.y >> pos.z;
	fin >> a >> b >> c;
	fin >> maxiter >> epsilon;
	camera = Camera(pos, a, b, c, maxiter, epsilon);

	fin >> fog >> cam >> shadow;
	fin >> light.x >> light.y >> light.z;
	light = normalize(light);

	while (!fin.eof()) {
		objects.push_back(getline(fin, buff));
	}
	fin.close();
}
