#include <SFML/Graphics.hpp>
#include <iostream>
#include <omp.h>
#include <Windows.h>
#include "camera.h"

const int winw = 800;
const int winh = 600;

// fog coefficient from 0 to 1, where 0 - no fog
const double fog_coefficient = 0.3;

// camera lighting coefficient from 0 to 1, where 0 - no lighting
const double camlight_coefficient = 0.1;

//N.B. fog and lighting quality depends on maximum number of iterations

sf::Uint8* pixels = new sf::Uint8[winw * winh * 4];
sf::Uint8* offscreen = new sf::Uint8[winw * winh * 4];
Camera camera(sf::Vector3f(3.7, 5.5, -9.), 0.625, -0.5, -0.75, 15, 0.01);
vector<Object> objs;

inline void setPixel(int x, int y, sf::Color c, sf::Uint8* buffer) {
	buffer[(y * winw + x)*4] = c.r;
	buffer[(y * winw + x)*4 + 1] = c.g;
	buffer[(y * winw + x)*4 + 2] = c.b;
	buffer[(y * winw + x)*4 + 3] = c.a;
}

void executingThread(sf::RenderWindow* window) {
	while (window->isOpen()) {
		Camera oldcam = camera;
		sf::Vector3f dot;
		double light = 0;
		#pragma omp parallel for private(dot, light)
		for (int i = 0; i < winw; i++)
			for (int j = 0; j < winh; j++) {
				dot = oldcam.pos;
				sf::Vector3f camd = normalize(oldcam.dir_normal() + oldcam.dir_tang() * (float)((i - winw / 2.) / winw) + oldcam.dir_vec() * (float)((winh / 2. - j) / winw));
				int n;
				int camlight;
				if ((camlight = camera.cast(camd, dot, objs, n)) >= 0)
					light = scalProd(normalize(sf::Vector3f(4, -5, 3)), objs[n].normal(dot)) * (.5 * (1. - (double)camlight_coefficient) + camlight_coefficient * (double)camlight / camera.max_iters) + .5;
				else light = (100. * fog_coefficient - distance(oldcam.pos, dot)) / 100.;
				light = crop(light, 0., 1.);
				setPixel(i, j, sf::Color((int)(light * 255), (int)(light * 255), (int)(light * 255)), offscreen);
			}
		// rendering is conducted on offscreen, drawing from pixels, when rendered they get swapped
		sf::Uint8* tmp = offscreen;
		offscreen = pixels;
		pixels = tmp;
	}
}

int main() {
	// window settings and initial preparations
	std::cout.precision(3);
	sf::RenderWindow window(sf::VideoMode(winw, winh), "RayMarching");
	sf::Texture renderTexture;
	renderTexture.create(winw, winh);
	renderTexture.setRepeated(false);
	for (int i = 0; i < winw; i++) for (int j = 0; j < winh; j++) setPixel(i, j, sf::Color::Black, pixels);
	for (int i = 0; i < winw; i++) for (int j = 0; j < winh; j++) setPixel(i, j, sf::Color::Black, offscreen);
	sf::Sprite render;
	render.setTexture(renderTexture);

	// thread for rendering scenery
	sf::Thread thread(&executingThread, &window);
	thread.launch();
	sf::Uint8* screensaver = offscreen;

	objs.push_back(Object(Type::SPHERE, sf::Vector3f(2., 0., 2.), 1.));
	objs.push_back(Object(Type::CUBE, sf::Vector3f(-2., 0., 2.), 1.));
	objs.push_back(Object(Type::PLANE, sf::Vector3f(0., -2., 0.), 1.));

	// cycle of updating camera and screen
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			// Enter - print current camera state
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Enter) {
					std::cout << "Current camera x:" << camera.pos.x << " y:" << camera.pos.y << " z:" << camera.pos.z << " a:" << camera.alpha << " b:" << camera.beta << " g:" << camera.gamma << std::endl;
				}
			}
		}
		camera.update();
		if (screensaver != pixels) {
			renderTexture.update(pixels);
			screensaver = pixels;
		}
		window.draw(render);
		window.display();
		Sleep(10);
	}
}