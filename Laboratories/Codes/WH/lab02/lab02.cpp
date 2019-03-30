#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "font.h"

class Font {
	private:
		sf::Font font;
		sf::Text *text;

	public:
		Font() {
			font.loadFromMemory(font_data, font_data_size);
			text  = new sf::Text;
			text -> setFont(font);
			text -> setCharacterSize(12);
			text -> setFillColor(sf::Color::Black);
		}
		void outtextxy(sf::RenderTarget& target, float x, float y, const char *str) const {
			text -> setPosition(x, y);
			text -> setString(str);
			target.draw(*text);
		}
		~Font() {
			delete text;
		}
};

class Circle : public sf::CircleShape, public Font {
	private:
		const int colors_size_x  = 250;
		const int colors_size_y  = 250;
		sf::Uint8* pixels        = new sf::Uint8[colors_size_x * colors_size_y * 4];
		sf::Texture* texture     = new sf::Texture();
		sf::CircleShape* shape   = new sf::CircleShape(125.);
		sf::CircleShape* blender = new sf::CircleShape(125.);
		
		inline void draw_to_color_pixels(int x, int y, float r, float g, float b) {
			pixels[4 * (y * colors_size_x + x) + 0] = static_cast<sf::Uint8>(r);
			pixels[4 * (y * colors_size_x + x) + 1] = static_cast<sf::Uint8>(g);
			pixels[4 * (y * colors_size_x + x) + 2] = static_cast<sf::Uint8>(b);
			pixels[4 * (y * colors_size_x + x) + 3] = 255;
		}

		virtual void convert(float ARZ[3], float RGB[3]) = 0;

		virtual float blendFactor() = 0;

	public:
		float x, y, Z = 0.5f;
		char* zString = new char(8);
		sf::Color blendColor;
		sf::BlendMode blendMode;

		void createTexture(float z) {
			float r, a;
			float ARZ[3];
			float RGB[3];
			for (float x = -125; x < colors_size_x - 125; ++x) {
				for (float y = -125; y < colors_size_y - 125; ++y) {
					r = sqrtf(x * x + y * y);
					a = fmodf(atan2f(y, x) * 57.29577951f + 180, 360);
					if (r <= 125) {
						ARZ[0] = a;
						ARZ[1] = r * 0.008f;
						ARZ[2] = z;
						convert(ARZ, RGB);
						draw_to_color_pixels(125 - static_cast<int>(x), 125 + static_cast<int>(y), RGB[0], RGB[1], RGB[2]);
					}
				}
			}
			texture -> update(pixels);
		}

		void updateBlend(float z) {
			Z = z;
			blendColor.a = static_cast<sf::Uint8>(blendFactor() * 255);
			blender     -> setFillColor(blendColor);
		}

		void setBlend(sf::Color color) {
			blendColor = color;
			updateBlend(Z);
		}

		void drawBlend(sf::RenderTarget& target) const {
			target.draw(*blender, sf::RenderStates(blendMode));
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			target.draw(*shape);
			drawBlend(target);
		}

		Circle(float x, float y) : x(x), y(y) {
			texture -> create(250, 250);
			shape   -> setTexture(texture);
			shape   -> setPosition(x, y);
			blender -> setPosition(x, y);
		}

		~Circle() {
			delete pixels;
			delete texture;
			delete shape;
			delete blender;
		}
};

class CircleHSL : public Circle {
	private:
		void convert(float HSL[3], float RGB[3]) {
			float C = (1.0f - fabsf(2.0f * HSL[2] - 1.0f)) * HSL[1];
			float X = C * (1.0f - fabsf(fmodf(HSL[0] * 0.016666f, 2.0f) - 1.0f));
			float m = HSL[2] - C / 2.0f;
			if (HSL[0] < 60) {
				RGB[0] = (C + m) * 255;
				RGB[1] = (X + m) * 255;
				RGB[2] = (0 + m) * 255;
			}
			else if (HSL[0] < 120) {
				RGB[0] = (X + m) * 255;
				RGB[1] = (C + m) * 255;
				RGB[2] = (0 + m) * 255;
			}
			else if (HSL[0] < 180) {
				RGB[0] = (0 + m) * 255;
				RGB[1] = (C + m) * 255;
				RGB[2] = (X + m) * 255;
			}
			else if (HSL[0] < 240) {
				RGB[0] = (0 + m) * 255;
				RGB[1] = (X + m) * 255;
				RGB[2] = (C + m) * 255;
			}
			else if (HSL[0] < 300) {
				RGB[0] = (X + m) * 255;
				RGB[1] = (0 + m) * 255;
				RGB[2] = (C + m) * 255;
			}
			else {
				RGB[0] = (C + m) * 255;
				RGB[1] = (0 + m) * 255;
				RGB[2] = (X + m) * 255;
			}
		}

		float blendFactor() {
			if (Z <= 0.5f) {
				blendMode = sf::BlendMode(sf::BlendMode::Factor::SrcAlpha, sf::BlendMode::Factor::DstAlpha, sf::BlendMode::ReverseSubtract);
			}
			else {
				blendMode = sf::BlendAdd;
			}
			return fabsf(Z - 0.5f) * 2.0f;
		}

	public:
		CircleHSL(float x, float y) : Circle(x, y) {
			createTexture(0.5f);
			setBlend(sf::Color::White);
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			Circle::draw(target, states);
			sprintf_s(zString, 8, "L=%.3f", Z);
			outtextxy(target, x, y + 6, "HSL");
			outtextxy(target, x + 205, y + 240, zString);
		}
};

class CircleHSV : public Circle {
	private:
		void convert(float HSV[3], float RGB[3]) {
			float C = HSV[2] * HSV[1];
			float X = C * (1.0f - fabsf(fmodf(HSV[0] * 0.016666f, 2.0f) - 1.0f));
			float m = HSV[2] - C;
			if (HSV[0] < 60) {
				RGB[0] = (C + m) * 255;
				RGB[1] = (X + m) * 255;
				RGB[2] = (0 + m) * 255;
			}
			else if (HSV[0] < 120) {
				RGB[0] = (X + m) * 255;
				RGB[1] = (C + m) * 255;
				RGB[2] = (0 + m) * 255;
			}
			else if (HSV[0] < 180) {
				RGB[0] = (0 + m) * 255;
				RGB[1] = (C + m) * 255;
				RGB[2] = (X + m) * 255;
			}
			else if (HSV[0] < 240) {
				RGB[0] = (0 + m) * 255;
				RGB[1] = (X + m) * 255;
				RGB[2] = (C + m) * 255;
			}
			else if (HSV[0] < 300) {
				RGB[0] = (X + m) * 255;
				RGB[1] = (0 + m) * 255;
				RGB[2] = (C + m) * 255;
			}
			else {
				RGB[0] = (C + m) * 255;
				RGB[1] = (0 + m) * 255;
				RGB[2] = (X + m) * 255;
			}
		}

		float blendFactor() {
			return 1.0f - Z;
		}

	public:
		CircleHSV(float x, float y) : Circle(x, y) {
			blendMode = sf::BlendAlpha;
			createTexture(1.0f);
			setBlend(sf::Color::Black);
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			Circle::draw(target, states);
			sprintf_s(zString, 8, "V=%.3f", Z);
			outtextxy(target, x, y + 6, "HSV");
			outtextxy(target, x + 205, y + 240, zString);
		}
};

class CircleCMY : public Circle {
	private:
		void convert(float CMY[3], float RGB[3]) {
			RGB[0] = (1 - CMY[1]) * 255;
			RGB[1] = (1 - CMY[0] * 0.002777f) * 255 ;
			RGB[2] = (1 - CMY[2]) * 255;
		}

		float blendFactor() {
			return Z;
		}

	public:
		CircleCMY(float x, float y) : Circle(x, y) {
			blendMode = sf::BlendMode(sf::BlendMode::Factor::SrcAlpha, sf::BlendMode::Factor::DstAlpha, sf::BlendMode::ReverseSubtract);
			createTexture(0.0f);
			setBlend(sf::Color::Blue);
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			Circle::draw(target, states);
			sprintf_s(zString, 8, "Y=%.0f%%", Z * 100);
			outtextxy(target, x, y + 6, "CMY");
			outtextxy(target, x + 205, y + 240, zString);
		}
};

class CircleGRB : public Circle {
	private:
		void convert(float GRB[3], float RGB[3]) {
			RGB[0] = GRB[1] * 255;
			RGB[1] = GRB[0] * 0.708333f;
			RGB[2] = GRB[2] * 255;
		}

		float blendFactor() {
			return Z;
		}

	public:
		CircleGRB(float x, float y) : Circle(x, y) {
			blendMode = sf::BlendAdd;
			createTexture(0.0f);
			setBlend(sf::Color::Blue);
		}
		
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			Circle::draw(target, states);
			sprintf_s(zString, 8, "B=%.0f", Z * 255);
			outtextxy(target, x, y + 6, "RGB");
			outtextxy(target, x + 205, y + 240, zString);
		}
};

class App : public sf::Drawable, public Font {
	private:
		Circle* circle1;
		Circle* circle2;
		Circle* circle3;
		Circle* circle4;
		const int sliderX          = 37;
		const int sliderY          = 256;
		float Z                    = 0.5f;
		sf::Uint8* sliderPixels    = new sf::Uint8[sliderX * sliderY * 4];
		sf::Texture* sliderTexture = new sf::Texture();
		sf::RectangleShape* slider = new sf::RectangleShape(sf::Vector2f(static_cast<float>(sliderX), static_cast<float>(sliderY)));
		sf::VertexArray* sliderPos = new sf::VertexArray(sf::Lines, 2);
		
		inline void draw_to_color_pixels(unsigned int x, unsigned int y, unsigned int z) {
			sliderPixels[4 * (y * sliderX + x) + 0] = z;
			sliderPixels[4 * (y * sliderX + x) + 1] = z;
			sliderPixels[4 * (y * sliderX + x) + 2] = z;
			sliderPixels[4 * (y * sliderX + x) + 3] = 255;
		}

	public:
		bool mousePressed = false;
		App() {
			circle1 = new CircleHSL(25, 25);
			circle2 = new CircleHSV(325, 25);
			circle3 = new CircleCMY(25, 325);
			circle4 = new CircleGRB(325, 325);

			for (int x = 2; x < sliderX; ++x) {
				for (int y = 1; y < sliderY; ++y) {
					draw_to_color_pixels(x, y, y);
				}
			}
			sliderTexture -> create(sliderX, sliderY);
			sliderTexture -> update(sliderPixels);

			slider -> setTexture(sliderTexture);
			slider -> setPosition(611, 10);
			slider -> setOutlineColor(sf::Color::Black);
			slider -> setOutlineThickness(1.0f);

			(*sliderPos)[0].color = (*sliderPos)[1].color = sf::Color::Black;
		}

		void updateZ(float z) {
			Z = z;
			circle1 -> updateBlend(z);
			circle2 -> updateBlend(z);
			circle3 -> updateBlend(z);
			circle4 -> updateBlend(z);
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			target.draw(*circle1);
			target.draw(*circle2);
			target.draw(*circle3);
			target.draw(*circle4);
			target.draw(*slider);
			(*sliderPos)[0].position = sf::Vector2f(600, 11 + Z * 255);
			(*sliderPos)[1].position = sf::Vector2f(608, 11 + Z * 255);
			target.draw(*sliderPos);
			(*sliderPos)[0].position = sf::Vector2f(651, 11 + Z * 255);
			(*sliderPos)[1].position = sf::Vector2f(659, 11 + Z * 255);
			target.draw(*sliderPos);
		}

		~App() {
			delete circle1;
			delete circle2;
			delete circle3;
			delete circle4;
			delete sliderPixels;
			delete sliderTexture;
			delete slider;
			delete sliderPos;
		}
};

int main() {
	sf::RenderWindow window(sf::VideoMode(750, 600), "GFK Lab 02 Wiktor Hladki", sf::Style::Titlebar | sf::Style::Close);
	sf::Event event;
	sf::Clock clock;
	sf::Time time = sf::Time::Zero;
	App app;
	char fpsString[10] = "0 FPS";
	unsigned int FPS = 0, frame_counter = 0;
 
	clock.restart().asMilliseconds();

	while (window.isOpen()) {
		window.clear(sf::Color::White);

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					if (event.mouseButton.x >= 613 and event.mouseButton.x <= 648 and event.mouseButton.y >= 11 and event.mouseButton.y <= 266) {
						app.updateZ((event.mouseButton.y - 11) / 255.0f);
						app.mousePressed = true;
					}
				}
			}
			else if (event.type == sf::Event::MouseMoved) {
				if (app.mousePressed) {
					if (event.mouseMove.x >= 613 and event.mouseMove.x <= 648 and event.mouseMove.y >= 11 and event.mouseMove.y <= 266) {
						app.updateZ((event.mouseMove.y - 11) / 255.0f);
					}
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					if (app.mousePressed) {
						app.mousePressed = false;
					}
				}
			}
		}
  
		if (clock.getElapsedTime().asSeconds() >= 1.0f) {
			FPS = static_cast<unsigned int>(static_cast<float>(frame_counter) / clock.getElapsedTime().asSeconds());
			clock.restart();
			frame_counter = 0;
			sprintf_s(fpsString, 10, "%d FPS", FPS);
		}
		++frame_counter;

		app.outtextxy(window, 610, 317, fpsString);
		window.draw(app);
		window.display();
	}	
}