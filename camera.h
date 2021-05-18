#pragma once
#include "SFML/Graphics.hpp"
#include "linalg.h"

class Camera {
	sf::Vector2i mouse;
public:
	sf::Vector3f pos = sf::Vector3f(0., 0., 0.);
	sf::Vector3f dir_normal = sf::Vector3f(0., 0., 1.);
	sf::Vector3f dir_tang = sf::Vector3f(1., 0., 0.);
	inline sf::Vector3f dir_vec() { return vecProd(dir_normal, dir_tang); }

	void update() {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) pos.z += .05;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) pos.z -= .05;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) pos.x -= .05;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) pos.x += .05;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) pos.y -= .05;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) pos.y += .05;
		double alpha = 0;
		double beta = 0;
		double gamma = 0;
		if (sf::Mouse::isButtonPressed(sf::Mouse::XButton1)) {
			sf::Vector2i newmouse = sf::Mouse::getPosition();
			alpha = (newmouse - mouse).x / 800;
			beta = (newmouse - mouse).y / 600;
			mouse = newmouse;
			//dir_normal = rotate(dir_normal, alpha, beta, gamma);
			//dir_tang = rotate(dir_tang, alpha, beta, gamma);
		}
	}
};
