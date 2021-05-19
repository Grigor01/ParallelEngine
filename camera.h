#pragma once
#include "SFML/Graphics.hpp"
#include "Object.h"

class Camera {
	double epsilon = .01;

	sf::Vector2i mouse;
	sf::Vector3f dn = sf::Vector3f(0., 0., 1.);
	sf::Vector3f dt = sf::Vector3f(1., 0., 0.);
	sf::Vector3f dv = sf::Vector3f(0., 1., 0.);

public:
	int max_iters = 15;
	double alpha = 0;
	double beta = 0;
	double gamma = 0;

	sf::Vector3f pos = sf::Vector3f(0., 0., 0.);

	// camera normal, tangential and kurvic normalized direction components
	inline sf::Vector3f dir_normal() { return dn; }
	inline sf::Vector3f dir_tang() { return dt;	}
	inline sf::Vector3f dir_vec() { return  dv; }

	Camera(sf::Vector3f initpos, double a, double b, double c, int maxit, double eps) :pos(initpos), alpha(a), beta(b), gamma(c), max_iters(maxit), epsilon(eps) {}

	void update();

	int cast(sf::Vector3f, sf::Vector3f&, vector<Object>&, int&);
};

// update camera with current keyboard and mouse state
void Camera::update() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) pos += dir_normal() * (float).05;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) pos -= dir_normal() * (float).05;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) pos -= dir_tang() * (float).05;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) pos += dir_tang() * (float).05;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) pos -= dir_vec() * (float).05;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) pos += dir_vec() * (float).05;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		sf::Vector2i newmouse = sf::Mouse::getPosition();
		alpha -= (double)(newmouse - mouse).x / 800 / 4;
		beta += (double)(newmouse - mouse).y / 600 / 4;
		dn = rotate(sf::Vector3f(0., 0., 1.), alpha, beta, gamma);
		dt = rotate(sf::Vector3f(1., 0., 0.), alpha, beta, gamma);
		dv = vecProd(dn, dt);
		mouse = newmouse;
	}
	else mouse = sf::Mouse::getPosition();
}

// cast a ray with normalized direction in ray and scenery in objs, returns number of calculated iterations and collided object num in objnum 
int Camera::cast(sf::Vector3f ray, sf::Vector3f& from, vector<Object>& objs, int& objnum) {
	double step;
	int ctr = 0;
	while (ctr < max_iters) {
		step = 1e10;
		for (int i = 0; i < objs.size(); i++) {
			double stepn = objs[i].distance(from);
			if (stepn < step) {
				step = stepn;
				objnum = i;
			}
		}
		if (step <= epsilon) return ctr;
		from += sf::Vector3f(ray.x * step, ray.y * step, ray.z * step);
		ctr++;
	}
	return -1;
}