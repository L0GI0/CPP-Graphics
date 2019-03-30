// Calosc mozna dowolnie edytowac wedle uznania. 

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "font.h"

class Menu : public sf::Drawable {
	private:
		sf::Font font;
		sf::Text *text;
		sf::RectangleShape *rectangle;
  
		sf::Texture *colors_texture;
		sf::Sprite *colors_sprite;
		sf::Uint8 *colors_pixels;
		const unsigned int colors_size_x = 765;
		const unsigned int colors_size_y = 60;
		inline void draw_to_color_pixels(unsigned int x, unsigned int y,unsigned char r, unsigned char g, unsigned char b) {
			colors_pixels[4 * (y * colors_size_x + x) + 0] = r;
			colors_pixels[4 * (y * colors_size_x + x) + 1] = g;
			colors_pixels[4 * (y * colors_size_x + x) + 2] = b;
			colors_pixels[4 * (y * colors_size_x + x) + 3] = 255;
		}
	public:
		Menu() {
			font.loadFromMemory(font_data, font_data_size);
			text = new sf::Text;
			text->setFont(font);
			text->setCharacterSize(12);
			text->setFillColor(sf::Color::White);

			rectangle = new sf::RectangleShape(sf::Vector2f(796, 536));
			rectangle->setFillColor(sf::Color::Transparent);
			rectangle->setOutlineColor(sf::Color::White);
			rectangle->setOutlineThickness(1.0f);
			rectangle->setPosition(2, 62);

			unsigned int x, y;
			colors_pixels = new sf::Uint8[colors_size_x * colors_size_y * 4];
			for (x = 0; x<255; x++)
				for (y = 0; y < 30; y++) {
					draw_to_color_pixels(x,       y,      x,       255,     0);
					draw_to_color_pixels(x + 255, y,      255,     255 - x, 0);
					draw_to_color_pixels(x + 510, y,      255,     0,       x);
					draw_to_color_pixels(254 - x, y + 30, 0,       255,     255-x);
					draw_to_color_pixels(509 - x, y + 30, 0,       x,       255 );
					draw_to_color_pixels(764 - x, y + 30, 255 - x, 0,       255);
				}

			colors_texture = new sf::Texture();
			colors_texture->create(colors_size_x, colors_size_y);
			colors_texture->update(colors_pixels);

			colors_sprite = new sf::Sprite();
			colors_sprite->setTexture(*colors_texture);
			colors_sprite->setPosition(1, 1);
		}

		void outtextxy(sf::RenderTarget& target, float x, float y, const wchar_t *str) const {
			text->setPosition(x, y); 
			text->setString(str); 
			target.draw(*text);
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			outtextxy(target,5, 600, L"f - wybór koloru rysowania");
			outtextxy(target, 5, 615, L"b - wybór koloru wype³niania");
			outtextxy(target, 5, 630, L"l - rysowanie linii");

			outtextxy(target, 200, 600, L"r - rysowanie prostok¹ta");
			outtextxy(target, 200, 615, L"a - rysowanie wype³nionego prostok¹ta");
			outtextxy(target, 200, 630, L"c - rysowanie okrêgu");

			outtextxy(target, 470, 600, L"w - zapis do pliku");
			outtextxy(target, 470, 615, L"o - odczyt z pliku");
			outtextxy(target, 470, 630, L"esc - wyjœcie");

			outtextxy(target, 650, 615, L"Aktualne:");

			target.draw(*rectangle);
			target.draw(*colors_sprite);
		}

		~Menu() {
			delete colors_sprite;
			delete colors_texture;
			delete colors_pixels;
			delete rectangle;
			delete text;
		}
};

class App : public sf::Drawable {
	public:
		App() {
			foregroundColorRect.setPosition( 767, 1);
			foregroundColorRect.setSize(sf::Vector2f(32, 29));
			foregroundColorRect.setOutlineColor(sf::Color::Transparent);
			foregroundColorRect.setOutlineThickness(0);
			foregroundColorRect.setFillColor(foregroundColor);

			backgroundColorRect.setPosition(767, 31);
			backgroundColorRect.setSize(sf::Vector2f(32, 29));
			backgroundColorRect.setOutlineColor(sf::Color::Transparent);
			backgroundColorRect.setOutlineThickness(0);
			backgroundColorRect.setFillColor(backgroundColor);

			resetVars();

			buffer.create(800, 600);
			buffer.clear();
			renderBuffer.setTexture(buffer.getTexture());

		}
		sf::Color foregroundColor = sf::Color(0, 0, 0, 0);
		sf::Color backgroundColor = sf::Color(0, 0, 0, 0);
		sf::RectangleShape foregroundColorRect, backgroundColorRect;
		char modes[6] = { 'f', 'b', 'l', 'r', 'a', 'c' };
		char mode     = 'f';
		sf::Vector2f corner1;
		sf::Vector2f corner2;
		sf::RenderTexture buffer;
		sf::Sprite renderBuffer;

		bool mousePressed;

		void resetVars() {
			corner1 = sf::Vector2f(0, 0);
			corner2 = sf::Vector2f(0, 0);
			mousePressed = false;
		}

		void updateColor(unsigned int mouseX, unsigned int mouseY) {
			char  tab        = mouseX / 255;
			sf::Uint8 x      = mouseX % 255;
			sf::Uint8 a      = 255;
			sf::Uint8 rgb[3] = { x, 255, 0 };
			int pos[3]       = { 0, 1  , 2 };
			if (tab == 1)
				for (int i = 0; i < 3; ++i)
					rgb[i] = 255 - rgb[i];
			if (mouseY > 30)
				std::swap(pos[0], pos[2]);
			for (int i = 0; i < 3; ++i)
				pos[i] = (pos[i] - tab < 0 ? pos[i] - tab + 3 : pos[i] - tab) % 3;
			sf::Color color = sf::Color(rgb[pos[0]], rgb[pos[1]], rgb[pos[2]], a);
			if (mode == 'f') {
				foregroundColor = color;
				foregroundColorRect.setFillColor(foregroundColor);
			}
			else if (mode == 'b') {
				backgroundColor = color;
				backgroundColorRect.setFillColor(backgroundColor);
			}
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
			if (mode == 'l') {
				sf::VertexArray line(sf::LinesStrip, 2);
				line[0].position = corner1;
				line[0].color = foregroundColor;
				line[1].position = corner2;
				line[1].color = backgroundColor;
				target.draw(line);
			}
			else if (mode == 'r' or mode == 'a') {
				sf::RectangleShape rectangle(sf::Vector2f(corner2.x - corner1.x, corner2.y - corner1.y));
				rectangle.setOutlineColor(foregroundColor);
				rectangle.setOutlineThickness(-1.0f);
				rectangle.setPosition(corner1);
				rectangle.setFillColor(mode == 'r' ? sf::Color::Transparent : backgroundColor);
				target.draw(rectangle);
			}
			else if (mode == 'c') {
				float radius = sqrt(pow(corner2.x - corner1.x, 2) + pow(corner2.y - corner1.y, 2)) / 2;
				sf::CircleShape circle(radius);
				circle.setOutlineColor(foregroundColor);
				circle.setOutlineThickness(1.0f);
				circle.setFillColor(sf::Color::Transparent);
				circle.setPosition(corner1.x + (corner2.x - corner1.x) / 2 - radius, corner1.y + (corner2.y - corner1.y) / 2 - radius);
				circle.setRotation(atan2f(corner2.x - corner1.x, corner2.y - corner1.y));
				target.draw(circle);
			}
		}
};

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 650), "GFK Lab 01", sf::Style::Titlebar | sf::Style::Close);
	sf::Event event;
	Menu menu;
	App app;
 
	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		window.clear(sf::Color::Black);
   
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					if (app.mode == 'f' or app.mode == 'b') {
						if (event.mouseButton.x <= 765 and event.mouseButton.y <= 60) {
							app.updateColor(event.mouseButton.x, event.mouseButton.y);
						}
					}
					else if (event.mouseButton.y > 60 and event.mouseButton.y <= 600) {
						app.mousePressed = true;
						app.corner1 = sf::Vector2f(static_cast<float> (event.mouseButton.x), static_cast<float> (event.mouseButton.y));
						app.corner2 = app.corner1;

					}
				}
			}
			else if (event.type == sf::Event::MouseMoved) {
				if (event.mouseMove.x > 1 and event.mouseMove.x < 799 and event.mouseMove.y > 60 and event.mouseMove.y < 600 and app.mousePressed) {
					app.corner2 = sf::Vector2f(static_cast<float> (event.mouseMove.x), static_cast<float> (event.mouseMove.y));
				}
			}

			else if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left and app.mousePressed) {
					app.mousePressed = false;
					app.buffer.draw(app);
					app.buffer.display();
					app.resetVars();
				}
			}
	
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
				else if (event.key.code + 'a' == 'w' ) {
					app.renderBuffer.getTexture()->copyToImage().saveToFile("image.bmp");
				}
				else if (event.key.code + 'a' == 'o') {
					sf::Texture fromFile;
					fromFile.loadFromFile("image.bmp");
					app.buffer.draw(sf::Sprite(fromFile));
				}
				else {
					for (int i = 0; i < sizeof(app.modes); ++i) {
						if (event.key.code + 'a' == app.modes[i]) {
							app.mode = event.key.code + 'a';
							app.resetVars();
							break;
						}
					}
				}
			}
		}
		wchar_t modeStr[2] = { static_cast<wchar_t> (app.mode), 0 };
		
		window.draw(app.renderBuffer);
		window.draw(app);
		window.draw(app.foregroundColorRect);
		window.draw(app.backgroundColorRect);
		menu.outtextxy(window, 720, 615, modeStr);
		window.draw(menu);
		window.display();
	}
	return 0;
}