#include <SFML/Graphics.hpp>
#include <iostream>
#include "icon/icon.hpp"
#include "settings/settings.hpp"

#pragma comment(lib, "sfml-graphics.lib")
#pragma comment(lib, "sfml-window.lib")
#pragma comment(lib, "sfml-system.lib")

extern AXONSETTINGSCONF SETTINGS;

void centerSprite(sf::Sprite &spr, sf::RenderWindow &w)
{
    sf::Vector2u wSize = w.getSize();
    sf::FloatRect sprBounds = spr.getGlobalBounds();

    float centerX = (wSize.x - sprBounds.width) / 2;
    float centerY = (wSize.y - sprBounds.height) / 2;

    spr.setPosition(centerX, centerY);
}

void centerSprite(sf::Text &spr, sf::RenderWindow &w)
{
    sf::Vector2u wSize = w.getSize();
    sf::FloatRect sprBounds = spr.getGlobalBounds();

    float centerX = (wSize.x - sprBounds.width) / 2;
    float centerY = (wSize.y - sprBounds.height) / 2;

    spr.setPosition(centerX, centerY);
}

void centerSprite(sf::RectangleShape &spr, sf::RenderWindow &w)
{
    sf::Vector2u wSize = w.getSize();
    sf::FloatRect sprBounds = spr.getGlobalBounds();

    float centerX = (wSize.x - sprBounds.width) / 2;
    float centerY = (wSize.y - sprBounds.height) / 2;

    spr.setPosition(centerX, centerY);
}

extern std::string ROOT_DIR;

int createWindow(std::string qr_bmp_send, std::string qr_bmp_recv, std::string filename, std::string url_send, std::string url_recv) {

    //////////
    std::cout << "url_recv: " << url_recv << '\n';
    //////////

    // Create a window
    sf::RenderWindow window(sf::VideoMode(330, 360), "Axon", sf::Style::Close);

    window.setFramerateLimit(30);
    window.setIcon(ICON_WIDTH, ICON_HEIGHT, ICON_RGBA);

    // Load the BMP image into a texture
    sf::Texture texture;
    if (!texture.loadFromMemory(qr_bmp_send.c_str(), qr_bmp_send.size())) {
        std::cerr << "Error: Could not load image file!" << std::endl;
        return 1;
    }

    sf::Font font;
    if (!font.loadFromFile(ROOT_DIR + "/static/font/GeistMono-SemiBold.ttf"))
    {
        std::cerr << "Unable to load font\n";
        exit(0);
    }
    
    sf::Text fn_text, url_text;
    fn_text.setFont(font);
    url_text.setFont(font);

    fn_text.setString(filename);
    url_text.setString(url_send);

    fn_text.setCharacterSize(18);
    url_text.setCharacterSize(14);

    fn_text.setFillColor(sf::Color::White);
    url_text.setFillColor(sf::Color::White);
    //text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    // Create a sprite to display the texture
    sf::Sprite qrSpr;
    qrSpr.setTexture(texture);

    //sf::RectangleShape qrBG(sf::Vector2f(325.f, 300.f));

    qrSpr.setScale(12.f, 12.f);
    //qrBG.setFillColor(sf::Color::Black);

    centerSprite(qrSpr, window);
    centerSprite(url_text, window);
    centerSprite(fn_text, window);
    //centerSprite(qrBG, window);

    url_text.setPosition(url_text.getPosition().x, qrSpr.getGlobalBounds().top + qrSpr.getGlobalBounds().height + 6.f);
    fn_text.setPosition(fn_text.getPosition().x, qrSpr.getGlobalBounds().top - fn_text.getLocalBounds().height - 10.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear, draw, and display
        window.clear(sf::Color(SETTINGS.bgcolor));
        window.draw(fn_text);
        window.draw(url_text);
        //window.draw(qrBG);
        window.draw(qrSpr);
        window.display();
    }

    return 0;
}
