#pragma once
#include <SFML/Graphics.hpp>
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

	size_t size() {
		return matrix.size();
	}

	void check() {
		if (matrix.size() != 3) {
			throw runtime_error("Error! Size of Matrix isn't equal to 3.");
		}
	}
};

sf::Vector3f normalize(sf::Vector3f);
double scalProd(sf::Vector3f, sf::Vector3f);
sf::Vector3f vecProd(sf::Vector3f, sf::Vector3f);
Matrix composition(Matrix&, Matrix&);
sf::Vector3f composition(sf::Vector3f&, Matrix&);
sf::Vector3f composition(Matrix&, sf::Vector3f&);
sf::Vector3f rotate(sf::Vector3f, double, double, double);

// 3х3 matrix composition
Matrix composition(Matrix& mat1, Matrix& mat2) {
	mat1.check();
	mat2.check();
	Matrix result(3);
	#pragma omp parallel for
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			result[i] = result[i] + (mat1[i].x * mat2[j] + mat1[i].y * mat2[j] + mat1[i].z * mat2[j]);
	}
	return result;
}

// product of vector x matrix
sf::Vector3f composition(sf::Vector3f& vec, Matrix& mat) {
	mat.check();
	sf::Vector3f res = vec.x * mat[0] + vec.y * mat[1] + vec.z * mat[2];
	return res;
}

// product of matrix x vector
sf::Vector3f composition(Matrix& mat, sf::Vector3f& vec) {
	mat.check();
	sf::Vector3f res(scalProd(mat[0], vec), scalProd(mat[1], vec), scalProd(mat[2], vec));
	return res;
}

// euler vector rotation (a-alpha, b-beta, c-gamma)
sf::Vector3f rotate(sf::Vector3f vec, double a, double b, double c) {
	sf::Vector3f res;
	Matrix mat(3);
	mat[0].x = cos(a) * cos(c) - sin(a) * cos(b) * sin(c);
	mat[0].y = (-1) * cos(c) * sin(a) - sin(c) * cos(a) * cos(b);
	mat[0].z = sin(b) * sin(c);
	mat[1].x = sin(c) * cos(a) + cos(c) * cos(b) * sin(a);
	mat[1].y = (-1) * sin(a) * sin(c) + cos(a) * cos(b) * cos(c);
	mat[1].z = (-1) * cos(c) * sin(b);
	mat[2].x = sin(b) * sin(a);
	mat[2].y = sin(b) * cos(a);
	mat[2].z = cos(b);
	res = composition(vec, mat);
	return res;
}

// cross product
sf::Vector3f vecProd(sf::Vector3f vec1, sf::Vector3f vec2) {
	sf::Vector3f res;
	res.x = vec1.y * vec2.z - vec1.z * vec2.y;
	res.y = (-1) * (vec1.x * vec2.z - vec1.z * vec2.x);
	res.z = vec1.x * vec2.y - vec1.y * vec2.x;
	return res;
}

// scalar product
double scalProd(sf::Vector3f vec1, sf::Vector3f vec2) {
	return (double)vec1.x * vec2.x + (double)vec1.y * vec2.y + (double)vec1.z * vec2.z;
}

// normalize vector
sf::Vector3f normalize(sf::Vector3f vec) {
	double len = sqrt(scalProd(vec, vec));
	return sf::Vector3f(vec.x / len, vec.y / len, vec.z / len);
}

// crop double with two boundaries
inline double crop(double a, double min, double max) {
	return (a < min) ? min : (a > max) ? max : a;
}

// modulus scaled on double numbers
double mod(double a, double factor) {
	while (a > factor) a -= factor;
	while (a < 0) a += factor;
	return a;
}

double distance(sf::Vector3f vec1, sf::Vector3f vec2) {
	return sqrt((vec1.x-vec2.x)*(vec1.x-vec2.x) + (vec1.y - vec2.y) * (vec1.y - vec2.y) + (vec1.z - vec2.z) * (vec1.z - vec2.z));
}