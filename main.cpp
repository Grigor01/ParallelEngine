#include <SFML/Graphics.hpp>
const int winw = 1920*5;
const int winh = 1080*4;

sf::Uint8* pixels = new sf::Uint8[winw * winh * 4];

void setPixel(int x, int y, sf::Color c) {
	pixels[y * winw + x] = c.r;
	pixels[y * winw + x + 1] = c.g;
	pixels[y * winw + x + 2] = c.b;
	pixels[y * winw + x + 3] = c.a;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(winw/3, winh/3), "RayMarching", sf::Style::Fullscreen);
	sf::Texture renderTexture;
	renderTexture.create(winw, winh);
	renderTexture.setRepeated(false);
	for (int i = 0; i < winw; i++) for (int j = 0; j < winh; j++) setPixel(i, j, sf::Color::Black);
	sf::Sprite render;
	render.setTexture(renderTexture);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed) window.close();
		}
		for (int i = 0; i < winh; i++) {
			setPixel(0, i, sf::Color::White);
			setPixel(winw, i, sf::Color::White);
		}
		for (int i = 0; i < winw; i++) {
			setPixel(i, 0, sf::Color::White);
			setPixel(i, winh, sf::Color::White);
		}
		renderTexture.update(pixels);
		window.draw(render);
		window.display();
	}
}