#include <SFML/Graphics.hpp>
#include <iostream>
#include <omp.h>
#include <Windows.h>
#include "camera.h"
#include "file.h"

const int winw = 800;
const int winh = 600;

// fog coefficient from 0 to 1, where 0 - no fog
double fog_coefficient = 0.5;

// camera lighting coefficient from 0 to 1, where 0 - no lighting
double camlight_coefficient = 0.5;

// ambient lighting coefficient from 0 to 1, where 1 - no shadows
double shadow_coefficient = 0.5;

//N.B. fog and lighting quality depends on maximum number of iterations

sf::Uint8* pixels = new sf::Uint8[winw * winh * 4];
sf::Uint8* offscreen = new sf::Uint8[winw * winh * 4];
Camera camera(sf::Vector3f(3.7, 5.5, -9.), 0.625, -0.5, -0.75, 50, 0.01);
sf::Vector3f lightvec = normalize(sf::Vector3f(4, -5, 3));
vector<Object> objs;
vector<Object> buf;

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
				int n = -1;
				int camlight;
				if ((camlight = camera.cast(camd, dot, objs, n)) >= 0) {
					light = scalProd(lightvec, objs[n].normal(dot)) * (.5 * (1. - (double)camlight_coefficient) + camlight_coefficient * (double)camlight / camera.max_iters) + .5;
					if (shadow_coefficient < 1) {
						int k = n;
						if (camera.cast(-lightvec, dot, objs, k) > -1) light *= shadow_coefficient;
					}
				}
				else {
					light = (100. * fog_coefficient - dist(oldcam.pos, dot)) / 100.;
					n = -1;
				}
				light = crop(light, 0., 1.);
				sf::Color col = sf::Color::White;
				if (n > -1) col = objs[n].color(dot);
				setPixel(i, j, sf::Color((int)(light * col.r), (int)(light * col.g), (int)(light * col.b)), offscreen);
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

	buf.reserve(100);
	readFromFile("data.txt", camera, objs, buf, fog_coefficient, camlight_coefficient, shadow_coefficient, lightvec);
	// thread for rendering scenery
	sf::Thread thread(&executingThread, &window);
	thread.launch();
	sf::Uint8* screensaver = offscreen;
	int framecount = 0;
	// cycle of updating camera and screen
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			// Enter - print current camera state
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Enter) {
					std::cout << "Current camera x:" << camera.pos.x << " y:" << camera.pos.y << " z:" << camera.pos.z << std::endl;
				}
			}
		}
		camera.update();
		if (screensaver != pixels) {
			renderTexture.update(pixels);
			screensaver = pixels;
			std::cout << framecount*10 << "ms/frame" << std::endl;
			framecount = 0;
		}
		else framecount++;
		window.draw(render);
		window.display();
		Sleep(10);
	}
}