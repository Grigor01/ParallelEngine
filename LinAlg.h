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

sf::Vector3f normalize(sf::Vector3f vec);
double scalProd(sf::Vector3f& vec1, sf::Vector3f& vec2);
sf::Vector3f vecProd(sf::Vector3f& vec1, sf::Vector3f& vec2);
Matrix composition(Matrix& mat1, Matrix& mat2);
sf::Vector3f composition(sf::Vector3f& vec, Matrix& mat);
sf::Vector3f composition(Matrix& mat, sf::Vector3f& vec);
sf::Vector3f rotate(sf::Vector3f& vec, double& a, double& b, double& c);

// произведение матриц размера 3х3.
Matrix composition(Matrix& mat1, Matrix& mat2) { 
	mat1.check();
	mat2.check();
	Matrix result(3);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			result[i] = result[i] + (mat1[i].x * mat2[j] + mat1[i].y * mat2[j] + mat1[i].z * mat2[j]);
	}
	return result;
}

// произведение вектора на матрицу.
sf::Vector3f composition(sf::Vector3f& vec, Matrix& mat) { 
	mat.check();
	sf::Vector3f res = vec.x * mat[0] + vec.y * mat[1] + vec.z * mat[2];
	return res;
}

// произведение матрицы на вектор.
sf::Vector3f composition(Matrix& mat, sf::Vector3f& vec) { 
	mat.check();
	sf::Vector3f res(scalProd(mat[0],vec), scalProd(mat[1], vec), scalProd(mat[2], vec));
	return res;
}

//поворот на углы эйлера(a,b,c)
sf::Vector3f rotate(sf::Vector3f& vec, double& a, double& b, double& c) { 
	sf::Vector3f res;
	Matrix mat(3); 
	mat[0].x = cos(a) * cos(c) - sin(a) * cos(b) * sin(c);
	mat[0].y = (-1) * cos(a) * sin(c) - sin(a) * cos(b) * cos(c);
	mat[0].z = sin(a) * sin(b);
	mat[1].x = sin(a) * cos(c) + cos(a) * cos(b) * sin(c);
	mat[1].y = (-1) * sin(a) * sin(c) + cos(a) * cos(b) * cos(c);
	mat[1].z = (-1) * cos(a) * sin(b);
	mat[2].x = sin(b)*sin(c);
	mat[2].y = sin(b)*cos(c);
	mat[2].z = cos(b);
	res = composition(vec, mat);
	return res;
}

//векторное произведение
sf::Vector3f vecProd(sf::Vector3f& vec1, sf::Vector3f& vec2) { 
	sf::Vector3f res;
	res.x = vec1.y * vec2.z - vec1.z * vec2.y;
	res.y = (-1)*(vec1.x * vec2.z - vec1.z * vec2.x);
	res.z = vec1.x * vec2.y - vec1.y * vec2.x;
	return res;
}

//скалярное произведение
double scalProd(sf::Vector3f& vec1, sf::Vector3f& vec2) { 
	double res;
	res = (double)vec1.x * vec2.x + (double)vec1.y * vec2.y + (double)vec1.z * vec2.z;
	return res;
}

//нормирование вектора
sf::Vector3f normalize(sf::Vector3f vec) { 
	double len = sqrt(scalProd(vec, vec));
	return sf::Vector3f(vec.x / len, vec.y / len, vec.z / len);
}

