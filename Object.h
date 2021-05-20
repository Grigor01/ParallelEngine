#pragma once
#include <SFML/Graphics.hpp>
#include "Linalg.h"

enum class Type {
	CUBE,
	CUBOID,
	SPHERE,
	PLANE,
	COMBINED
};

enum class Comb {
	ADD,
	SMOOTHADD,
	MULTIPLY,
	SUBTRL,
	SUBTRR,
	NEGATIVE
};

//The class implements shapes such as cuboid, cube, and sphere.
class Object {
private:
	Type shape;
	Comb type = Comb::ADD;
	sf::Vector3f position;
	Object* left, *right = nullptr;
	sf::Color col;

	double length = 0;
	double width = 0;
	double height = 0;
	sf::Vector3f angles;

public:
	// constructors for others / combined / negative
	Object(Type T, sf::Vector3f pos, sf::Vector3f rote, sf::Color col_, double side, ...) {
		shape = T;
		col = col_;
		position = pos;
		angles = rote;
		double* a = &side;
		left = nullptr;
		right = nullptr;
		switch (T) {
		case Type::CUBOID:
			length = *a;
			a++;
			width = *a;
			a++;
			height = *a;
			break;
		case Type::PLANE:
			length = *a;
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

	Object(Type T, Comb t, Object* l, Object* r) {
		shape = T;
		if (shape != Type::COMBINED) throw runtime_error("object with two references should be combined");
		type = t;
		left = l;
		if (type == Comb::NEGATIVE) throw runtime_error("extra right object for negative combination");
		right = r;
	}

	Object(Type T, Comb t, Object* l) {
		shape = T;
		if (shape != Type::COMBINED) throw runtime_error("object with two references should be combined");
		type = t;
		left = l;
		if (type != Comb::NEGATIVE) throw runtime_error("right object expected for non-negative combination");
	}

	sf::Color color(sf::Vector3f);

	double distance(sf::Vector3f);

	sf::Vector3f normal(sf::Vector3f);
};

// gets object color in the dot
sf::Color Object::color(sf::Vector3f dot) {
	switch (shape) {
	case Type::COMBINED: {
		switch (type) {
		case Comb::ADD: {
			double d1 = left->distance(dot);
			double d2 = right->distance(dot);
			if (d1 < d2) return left->color(dot);
			else return right->color(dot);
			break;
		}
		case Comb::SMOOTHADD: {
			double d1 = left->distance(dot);
			double d2 = right->distance(dot);
			d1 = d1 / (d2 + d1);
			d2 = 1 - d1;
			return sf::Color(left->color(dot).r * d1 + right->color(dot).r * d2, left->color(dot).g * d1 + right->color(dot).g * d2, left->color(dot).b * d1 + right->color(dot).b * d2);
			break;
		}
		case Comb::MULTIPLY:
		case Comb::SUBTRL:
		case Comb::SUBTRR: {
			double d1 = left->distance(dot);
			double d2 = right->distance(dot);
			if (d1 > d2) return left->color(dot);
			else return right->color(dot);
			break;
		}
		case Comb::NEGATIVE: {
			return left->color(dot);
			break;
		}
		}
		break;
	}
	default: return col;
	}
}

//Calculates the distance from the observer to the shape.
double Object::distance(sf::Vector3f dot) {
	switch (shape) {
	case Type::COMBINED: {
		switch (type) {
		case Comb::ADD:
			return min(left->distance(dot), right->distance(dot));
			break;
		case Comb::SMOOTHADD:
			return smoothmin(left->distance(dot), right->distance(dot));
			break;
		case Comb::MULTIPLY:
			return max(left->distance(dot), right->distance(dot));
			break;
		case Comb::SUBTRL:
			return max(-left->distance(dot), right->distance(dot));
			break;
		case Comb::SUBTRR:
			return max(left->distance(dot), -right->distance(dot));
			break;
		case Comb::NEGATIVE:
			return -left->distance(dot);
			break;
		}
		break;
	}
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
				return abs(scalProd(sf::Vector3f(0., 1., 0.), rote));
			}
			return abs(scalProd(sf::Vector3f(0., 1., 0.), dot - position));
      break;
    }
	}
	return 0;
}

//Unit normal to the shape.
sf::Vector3f Object::normal(sf::Vector3f dot) {
	switch (shape) {
	case Type::COMBINED: {
		switch (type) {
		case Comb::ADD: {
			double d1 = left->distance(dot);
			double d2 = right->distance(dot);
			if (d1 < d2) return left->normal(dot);
			else return right->normal(dot);
			break;
		}
		case Comb::SMOOTHADD: {
			double d1 = left->distance(dot);
			double d2 = right->distance(dot);
			d1 = d1 / (d2 + d1);
			d2 = 1 - d1;
			return sf::Vector3f(left->normal(dot).x * d1 + right->normal(dot).x * d2, left->normal(dot).y * d1 + right->normal(dot).y * d2, left->normal(dot).z * d1 + right->normal(dot).z * d2);
			break;
		}
		case Comb::MULTIPLY: {
			double d1 = left->distance(dot);
			double d2 = right->distance(dot);
			if (d1 > d2) return left->normal(dot);
			else return right->normal(dot);
			break;
		}
		case Comb::SUBTRL: {
			double d1 = -left->distance(dot);
			double d2 = right->distance(dot);
			if (d1 > d2) return -left->normal(dot);
			else return right->normal(dot);
			break;
		}
		case Comb::SUBTRR: {
			double d1 = left->distance(dot);
			double d2 = -right->distance(dot);
			if (d1 > d2) return left->normal(dot);
			else return -right->normal(dot);
			break;
		}
		case Comb::NEGATIVE: {
			return -left->normal(dot);
			break;
		}
		}
		break;
	}
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
    if (rotation) return rotate(sf::Vector3f(0., -1., 0.), angles.x, angles.y, angles.z);
    return sf::Vector3f(0., -1., 0.);
    break;
  }
  }
	return sf::Vector3f();
}
/*
1. Add a rotation of the shapes.
2. Add interaction with objects (addition, subtraction, and so on (specify)).
*/