#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct Matrix {
	vector<sf::Vector3f> matrix;
	Matrix() {}
	Matrix(int _size) {
		sf::Vector3f tmp(0, 0, 0);
		for (int i = 0; i < _size; i++) {
			matrix.push_back(tmp);
		}
	}
	sf::Vector3f& operator[](int i) {
		return matrix[i];
	}
	int size() {
		return matrix.size();
	}
	void check() {
		if (matrix.size() != 3) {
			cout << "Error!Size of Matrix isn't equal to 3.";
		}
	}
};

Matrix composition1(Matrix& mat1, Matrix& mat2) { // произведение матриц размера 3х3.
	mat1.check();
	mat2.check();
	Matrix result(3);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			result[i] = result[i] + (mat1[i].x * mat2[j] + mat1[i].y * mat2[j] + mat1[i].z * mat2[j]);
	}
	return result;
}

sf::Vector3f composition0(sf::Vector3f& vec, Matrix& mat) { // произведение вектора на матрицу.
	mat.check();
	sf::Vector3f res=vec.x*mat[0]+vec.y*mat[1]+vec.z*mat[2];
	return res;
}

sf::Vector3f rotate(sf::Vector3f& vec, double& a, double& b, double& c) { //поворот на углы эйлера(a,b,c)
	sf::Vector3f res;
	Matrix mat(3);
	mat[0].x = cos(a) * cos(c) - sin(a) * cos(b) * sin(c);
	mat[0].y = (-1) * cos(a) * sin(c) - sin(a) * cos(b) * cos(c);
	mat[0].z = sin(a) * sin(b);
	mat[1].x = sin(a) * cos(c) + cos(a) * cos(b) * sin(c);
	mat[1].y = (-1) * sin(a) * sin(c) + cos(a) * cos(b) * cos(c);
	mat[1].z = (-1)*
	return res;
}

sf::Vector3f vecProd(sf::Vector3f& vec1, sf::Vector3f& vec2) {

}