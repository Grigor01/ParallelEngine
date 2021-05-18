#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
enum Type {
	CUBE,
	PARALLELEPIPED,
	SPHERE,
};
class Object {
private:
	Type figure;
	sf::Vector3f position;
	double length;
	double width;
	double height;
public:
	Object(Type T, sf::Vector3f pos, double side, ...) {
		figure = T;
		position = pos;
		double* a = &side;
		switch (T) {
		case PARALLELEPIPED:
			length = *a;
			a++;
			width = *a;
			a++;
			height = *a;
			break;
		default:
			length = *a;
			width = 0;
			height = 0;
			break;
		}
	}
	double distance(sf::Vector3f dot) {
		switch (figure) {
		case SPHERE:
			return sqrt((dot.x - position.x) * (dot.x - position.x) + (dot.y - position.y) * (dot.y - position.y) + (dot.z - position.z) * (dot.z - position.z)) - length;
			break;
		case CUBE:
			sf::Vector3f dist(abs(dot.x - position.x) - length, abs(dot.y - position.y) - length, abs(dot.z - position.z) - length);
			double d = sqrt(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);
			if (dist.x < 0 && dist.y < 0 && dist.z < 0) return -d;
			return d;
			break;
		case PARALLELEPIPED:
			sf::Vector3f dist(abs(dot.x - position.x) - length, abs(dot.y - position.y) - height, abs(dot.z - position.z) - width);
			double d = sqrt(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);
			if (dist.x < 0 && dist.y < 0 && dist.z < 0) return -d;
			return d;
			break;
		default:
			break;
		}
	}

};


/*Куб: корень модуля - сторона
Параллелепипед: формула*/