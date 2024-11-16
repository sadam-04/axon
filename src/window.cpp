#include <SFML/Graphics.hpp>
#include <iostream>

int showBMP(std::string bmpdata) {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(500, 500), "Axon");

    // Load the BMP image into a texture
    sf::Texture texture;
    if (!texture.loadFromMemory(bmpdata.c_str(), bmpdata.size())) {
        std::cerr << "Error: Could not load image file!" << std::endl;
        return 1;
    }

    // Create a sprite to display the texture
    sf::Sprite sprite;
    sprite.setTexture(texture);

    sprite.setScale(12.f, 12.f);

    sf::Vector2u windowSize = window.getSize();
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();

    float centerX = (windowSize.x - spriteBounds.width) / 2;
    float centerY = (windowSize.y - spriteBounds.height) / 2;
    sprite.setPosition(centerX, centerY);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear, draw, and display
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }

    return 0;
}
