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
    sf::Texture qr_send_tex, qr_recv_tex;
    if (!qr_send_tex.loadFromMemory(qr_bmp_send.c_str(), qr_bmp_send.size())) {
        std::cerr << "Error: Could not load send qr BMP file!" << std::endl;
        return 1;
    }
    if (!qr_recv_tex.loadFromMemory(qr_bmp_recv.c_str(), qr_bmp_recv.size())) {
        std::cerr << "Error: Could not load recv qr BMP file!" << std::endl;
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
    sf::Sprite qr_spr_send, qr_spr_recv;
    qr_spr_send.setTexture(qr_send_tex);
    qr_spr_recv.setTexture(qr_recv_tex);

    //sf::RectangleShape qrBG(sf::Vector2f(325.f, 300.f));

    qr_spr_send.setScale(12.f, 12.f);
    qr_spr_recv.setScale(12.f, 12.f);
    //qrBG.setFillColor(sf::Color::Black);

    centerSprite(qr_spr_send, window);
    centerSprite(qr_spr_recv, window);
    centerSprite(url_text, window);
    centerSprite(fn_text, window);
    //centerSprite(qrBG, window);

    url_text.setPosition(url_text.getPosition().x, qr_spr_send.getGlobalBounds().top + qr_spr_send.getGlobalBounds().height + 6.f);
    fn_text.setPosition(fn_text.getPosition().x, qr_spr_send.getGlobalBounds().top - fn_text.getLocalBounds().height - 10.f);

    sf::Sprite *current_qr = &qr_spr_send;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
                if (event.key.scancode == sf::Keyboard::Scan::Space)
                {
                    if (current_qr == &qr_spr_send) current_qr = &qr_spr_recv; else current_qr = &qr_spr_send;
                    if (fn_text.getString() == filename) fn_text.setString("Receive"); else fn_text.setString(filename);
                    if (url_text.getString() == url_send) url_text.setString(url_recv); else url_text.setString(url_send);
                }
        }

        // Clear, draw, and display
        window.clear(sf::Color(SETTINGS.bgcolor));
        window.draw(fn_text);
        window.draw(url_text);
        //window.draw(qrBG);
        window.draw(*current_qr);
        window.display();
    }

    return 0;
}
