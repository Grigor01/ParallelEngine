#pragma once
#include <SFML/Graphics.hpp>
#include "Linalg.h"

enum class Type {
	CUBE,
	CUBOID,
	SPHERE,
	PLANE,
	COMBINED,
};

//The class implements shapes such as cuboid, cube, and sphere.
class Object {
private:
	Type shape;
	sf::Vector3f position;
	double length;
	double width;
	double height;
	sf::Vector3f angles;
public:
	Object(Type T, sf::Vector3f pos, sf::Vector3f rote, double side, ...) {
		shape = T;
		position = pos;
		angles = rote;
		double* a = &side;
		switch (T) {
		case Type::CUBOID:
			length = *a;
			a++;
			width = *a;
			a++;
			height = *a;
			break;
		case Type::PLANE:
			length = 0;
			width = 0;
			height = 0;
			break;
		default:
			length = *a;
			width = 0;
			height = 0;
			break;
		}
	}

	//Calculates the distance from the observer to the shape.
	double distance(sf::Vector3f dot) {
		switch (shape) {
		case Type::SPHERE: {
			bool rotation = ((angles.x != 0) || (angles.y != 0) || (angles.z != 0));
			if (rotation) {
				sf::Vector3f rote = rotate(dot - position, angles.x, angles.y, angles.z);
				return sqrt(rote.x * rote.x + rote.y * rote.y + rote.z * rote.z);
			}
			return sqrt((dot.x - position.x) * (dot.x - position.x) + (dot.y - position.y) * (dot.y - position.y) + (dot.z - position.z) * (dot.z - position.z)) - length;
			break;
		}
		case Type::CUBE: {
			bool rotation = ((angles.x != 0) || (angles.y != 0) || (angles.z != 0));
			sf::Vector3f rote;
			if (rotation) rote = rotate(dot - position, angles.x, angles.y, angles.z);
			else rote = dot - position;
			sf::Vector3f dist(abs(rote.x) - length, abs(rote.y) - length, abs(rote.z) - length);
			bool revert = ((dist.x < 0) && (dist.y < 0) && (dist.z < 0));
			dist = sf::Vector3f(max(dist.x, (float)0.), max(dist.y, (float)0.), max(dist.z, (float)0.));
			double d = sqrt(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);
			if (revert) return -d;
			return d;
			break;
		}
		case Type::CUBOID: {
			bool rotation = ((angles.x != 0) || (angles.y != 0) || (angles.z != 0));
			sf::Vector3f rote;
			if (rotation) rote = rotate(dot - position, angles.x, angles.y, angles.z);
			else rote = dot - position;
			sf::Vector3f dist(abs(rote.x) - length, abs(rote.y) - height, abs(rote.z) - width);
			bool revert = ((dist.x < 0) && (dist.y < 0) && (dist.z < 0));
			dist = sf::Vector3f(max(dist.x, (float)0.), max(dist.y, (float)0.), max(dist.z, (float)0.));
			double d = sqrt(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);
			if (revert) return -d;
			return d;
			break;
		}
		case Type::PLANE: {
			bool rotation = ((angles.x != 0) || (angles.y != 0) || (angles.z != 0));
			if (rotation) {
				sf::Vector3f rote = rotate(dot - position, angles.x, angles.y, angles.z);
				return scalProd(sf::Vector3f(0., 1., 0.), rote);
			}
			return scalProd(sf::Vector3f(0., 1., 0.), dot - position);
			break;
		}
		}
		return 0;
	}

	//Unit normal to the shape.
	sf::Vector3f normal(sf::Vector3f dot) {
		switch (shape) {
		case Type::SPHERE: {
			bool rotation = ((angles.x != 0) || (angles.y != 0) || (angles.z != 0));
			if (rotation) return normalize(rotate(position - dot, angles.x, angles.y, angles.z));
			return normalize(position - dot);
			break;
		}
		case Type::CUBE: {
			bool rotation = ((angles.x != 0) || (angles.y != 0) || (angles.z != 0));
			sf::Vector3f rote;
			if (rotation) rote = rotate(position - dot, angles.x, angles.y, angles.z);
			else rote = dot - position;
			sf::Vector3f dist(abs(abs(rote.x) - length), abs(abs(rote.y) - length), abs(abs(rote.z) - length));
			if ((dist.x < dist.y) && (dist.x < dist.z)) {
				dist.x = 1;
				dist.y = 0;
				dist.z = 0;
				if (rote.x > 0) dist.x *= (-1);
				return dist;
			}
			else if (dist.y < dist.z) {
				dist.x = 0;
				dist.y = 1;
				dist.z = 0;
				if (rote.y > 0) dist.y *= (-1);
				return dist;
			}
			else {
				dist.x = 0;
				dist.y = 0;
				dist.z = 1;
				if (rote.z > 0) dist.z *= (-1);
				return dist;
			}
			break;
		}
		case Type::CUBOID: {
			bool rotation = ((angles.x != 0) || (angles.y != 0) || (angles.z != 0));
			sf::Vector3f rote;
			if (rotation) rote = rotate(position - dot, angles.x, angles.y, angles.z);
			else rote = position - dot;
			sf::Vector3f dist(abs(abs(rote.x) - length), abs(abs(rote.y) - height), abs(abs(rote.z) - width));
			if ((dist.x < dist.y) && (dist.x < dist.z)) {
				dist.x = 1;
				dist.y = 0;
				dist.z = 0;
				if (rote.x < 0) dist.x *= (-1);
				return dist;
			}
			else if (dist.y < dist.z) {
				dist.x = 0;
				dist.y = 1;
				dist.z = 0;
				if (rote.y < 0) dist.y *= (-1);
				return dist;
			}
			else {
				dist.x = 0;
				dist.y = 0;
				dist.z = 1;
				if (rote.z < 0) dist.z *= (-1);
				return dist;
			}
			break;
		}
		case Type::PLANE: {
			bool rotation = ((angles.x != 0) || (angles.y != 0) || (angles.z != 0));
			if (rotation) return rotate(sf::Vector3f(0., 1., 0.), angles.x, angles.y, angles.z);
			return sf::Vector3f(0., 1., 0.);
			break;
		}
		}
		return sf::Vector3f();
	}
};

/*
1. Add a rotation of the shapes.
2. Add interaction with objects (addition, subtraction, and so on (specify)).
*/