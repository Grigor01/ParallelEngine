#pragma once
#include "camera.h"
#include <fstream>
// ������� �������� ��� �����(���� ����). ������ �� ������ ��������, ������ �� ������, ������ �� ������ ���������.
void readFromFile(string fname, Camera& camera) {
	fstream fin;
	sf::Vector3f pos;
	double a, b, c, maxit, epsilon;
	fin.open(fname);
	fin >> pos.x;
	fin >> pos.y;
	fin >> pos.z;
	fin >> a;
	fin >> b;
	fin >> c;
	while (!fin.eof()) {

	}
	fin.close();
}
