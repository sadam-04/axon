#include <SFML/Graphics.hpp>
#include <iostream>

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

extern std::string ROOT_DIR;

int createWindow(std::string bmpdata, std::string filename, std::string url) {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(500, 500), "Axon");

    // Load the BMP image into a texture
    sf::Texture texture;
    if (!texture.loadFromMemory(bmpdata.c_str(), bmpdata.size())) {
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
    url_text.setString(url);

    fn_text.setCharacterSize(20);
    url_text.setCharacterSize(20);

    fn_text.setFillColor(sf::Color::White);
    url_text.setFillColor(sf::Color::White);
    //text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    // Create a sprite to display the texture
    sf::Sprite qrSpr;
    qrSpr.setTexture(texture);

    qrSpr.setScale(12.f, 12.f);

    centerSprite(qrSpr, window);
    centerSprite(url_text, window);

    url_text.setPosition(url_text.getPosition().x, qrSpr.getGlobalBounds().top + qrSpr.getGlobalBounds().height + 10.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear, draw, and display
        window.clear(sf::Color::Black);
        window.draw(fn_text);
        window.draw(url_text);
        window.draw(qrSpr);
        window.display();
    }

    return 0;
}
