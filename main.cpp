#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>

const int winw = 800;
const int winh = 600;
const int max_iters = 10;
const double epsilon = .01;

sf::Uint8* pixels = new sf::Uint8[winw * winh * 4];
sf::Uint8* offscreen = new sf::Uint8[winw * winh * 4];
sf::Vector3f camera(0, 0, -10);

inline void setPixel(int x, int y, sf::Color c, sf::Uint8* buffer) {
	buffer[(y * winw + x)*4] = c.r;
	buffer[(y * winw + x)*4 + 1] = c.g;
	buffer[(y * winw + x)*4 + 2] = c.b;
	buffer[(y * winw + x)*4 + 3] = c.a;
}

inline double crop(double a, double min, double max) {
	return (a < min) ? min : (a > max) ? max : a;
}

inline sf::Vector3f normalize(sf::Vector3f a) {
	double len = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	return sf::Vector3f(a.x / len, a.y / len, a.z / len);
}

double mod(double a, double factor) {
	while (a > factor) a -= factor;
	return a;
}

inline double distance(sf::Vector3f dot, sf::Vector3f sphere, double radius) {
	return sqrt((dot.x - sphere.x) * (dot.x - sphere.x) + (dot.y - sphere.y) * (dot.y - sphere.y) + (dot.z - sphere.z) * (dot.z - sphere.z)) - radius;
}

inline sf::Vector3f normal(sf::Vector3f dot, sf::Vector3f sphere, double radius) {
	return normalize(sphere - dot);
}

inline double scalar(sf::Vector3f a, sf::Vector3f b) {
	return (double)a.x * b.x + (double)a.y * b.y + (double)a.z * b.z;
}

int cast(sf::Vector3f ray, sf::Vector3f& from, sf::Vector3f sphere, double radius) {
	double step = 1e10;
	int ctr = 0;
	while (ctr < max_iters) {
		step = distance(from, sphere, radius);
		if (step <= epsilon) return true;
		from += sf::Vector3f(ray.x*step, ray.y * step, ray.z * step);
		ctr++;
	}
	return -1;
}

void executingThread(sf::RenderWindow* window) {
	while (window->isOpen()) {
		sf::Vector3f oldcam = camera;
		sf::Vector3f dot;
		double light = 0;
		#pragma omp parallel for private(dot, light)
		for (int i = 0; i < winw; i++)
			for (int j = 0; j < winh; j++) {
				dot = oldcam;
				if (cast(normalize(sf::Vector3f((i - winw / 2.) / winw, (winh / 2. - j) / winw, 1)), dot, sf::Vector3f(0, 0, 0), 2.) >= 0)
					light = scalar(normalize(sf::Vector3f(5, -5, 5)), normal(dot, sf::Vector3f(0, 0, 0), 2.)) * .5 + .5;
				else light = 0;
				setPixel(i, j, sf::Color(light * 255, light * 255, light * 255), offscreen);
			}
		sf::Uint8* tmp = offscreen;
		offscreen = pixels;
		pixels = tmp;
	}
}

int main() {
	sf::RenderWindow window(sf::VideoMode(winw, winh), "RayMarching");
	sf::Texture renderTexture;
	renderTexture.create(winw, winh);
	renderTexture.setRepeated(false);
	for (int i = 0; i < winw; i++) for (int j = 0; j < winh; j++) setPixel(i, j, sf::Color::Black, pixels);
	for (int i = 0; i < winw; i++) for (int j = 0; j < winh; j++) setPixel(i, j, sf::Color::Black, offscreen);
	sf::Sprite render;
	render.setTexture(renderTexture);
	sf::Thread thread(&executingThread, &window);
	thread.launch();
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed) {
				/*switch (event.key.code) {
				case sf::Keyboard::Up:
					camera.z += .2;
					break;
				case sf::Keyboard::Down:
					camera.z -= .2;
					break;
				case sf::Keyboard::Left:
					camera.x -= .2;
					break;
				case sf::Keyboard::Right:
					camera.x += .2;
					break;
				case sf::Keyboard::LShift:
					camera.y += .2;
					break;
				case sf::Keyboard::Space:
					camera.y -= .2;
					break;
				}*/
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) camera.z += .2;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) camera.z -= .2;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) camera.x -= .2;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) camera.x += .2;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) camera.y -= .2;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) camera.y += .2;
		renderTexture.update(pixels);
		window.draw(render);
		window.display();
		Sleep(5);
	}
}