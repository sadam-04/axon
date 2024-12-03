#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <mutex>
#include "common/common.hpp"
#include "icon/icon.hpp"
#include "settings/settings.hpp"

#pragma comment(lib, "sfml-graphics.lib")
#pragma comment(lib, "sfml-window.lib")
#pragma comment(lib, "sfml-system.lib")

extern AXONSETTINGSCONF SETTINGS;

const char HORIZ = 0b00000001;
const char VERT =  0b00000010;

void centerObj(sf::Transformable &obj, sf::FloatRect &bounds, sf::RenderWindow &w, const unsigned char axis)
{
    sf::Vector2u wSize = w.getSize();

    sf::Vector2f center = obj.getPosition();

    if (axis & HORIZ) center.x = (wSize.x - bounds.width) / 2;
    if (axis & VERT) center.y = (wSize.y - bounds.height) / 2;

    obj.setPosition(center);
}

extern std::string ROOT_DIR;

int create_window(std::string qr_bmp, std::string url, std::queue<FileRC> &file_q, std::mutex &file_q_mutex, std::string filename = "") {
    const unsigned int W_WIDTH_SEND = 375;
    const unsigned int W_WIDTH_RECV = 675;
    const unsigned int W_HEIGHT = 380;

    const unsigned int QR_MARGIN = 15;

    bool MODE_SEND = false;
    bool MODE_RECV = false;
    if (filename.empty())
        MODE_RECV = true;
    else
        MODE_SEND = true;

    sf::RenderWindow window(sf::VideoMode((MODE_SEND ? W_WIDTH_SEND : W_WIDTH_RECV), W_HEIGHT), "Axon", sf::Style::Close);
    //window.setFramerateLimit(30);
    window.setVerticalSyncEnabled(true);
    window.setIcon(ICON_WIDTH, ICON_HEIGHT, ICON_RGBA);

    // Load the BMP image into a texture
    sf::Texture qr_tex;

    if (!qr_tex.loadFromMemory(qr_bmp.c_str(), qr_bmp.size())) {
        std::cerr << "Error: Could not load recv qr BMP file!" << std::endl;
        return 1;
    }

    sf::Font font;
    if (!font.loadFromFile(ROOT_DIR + "/static/font/GeistMono-SemiBold.ttf"))
    {
        std::cerr << "Unable to load font\n";
        exit(0);
    }
    
    //Header and footer/url text
    sf::Text fn_text, url_text, recv_file;
    fn_text.setFont(font);
    url_text.setFont(font);
    recv_file.setFont(font);
    fn_text.setString((MODE_SEND ? "Serving: " + filename : "Receiving"));
    url_text.setString(url);
    // set recv text later
    fn_text.setCharacterSize(16);
    url_text.setCharacterSize(14);
    recv_file.setCharacterSize(10);
    fn_text.setFillColor(sf::Color::White);
    url_text.setFillColor(sf::Color::White);
    recv_file.setFillColor(sf::Color::White);

    fn_text.setFillColor(sf::Color(SETTINGS.textcolor));
    url_text.setFillColor(sf::Color(SETTINGS.textcolor));
    recv_file.setFillColor(sf::Color(SETTINGS.textcolor));

    // QR Sprite
    sf::Sprite qr_spr;
    qr_spr.setTexture(qr_tex);
    
    sf::FloatRect qrbounds = qr_spr.getGlobalBounds();
    float qrScalar = 300.f/qrbounds.height;
    qr_spr.setScale(qrScalar, qrScalar);

    // Center elements
    centerObj(qr_spr, qr_spr.getGlobalBounds(), window, VERT | (HORIZ & MODE_SEND));
    if (MODE_RECV) qr_spr.move({QR_MARGIN, 0.f});
    centerObj(url_text, url_text.getGlobalBounds(), window, HORIZ);
    centerObj(fn_text, fn_text.getGlobalBounds(), window, HORIZ);

    recv_file.setPosition(W_WIDTH_RECV * 0.55f, W_HEIGHT * 0.1f);

    // Vertically shift header and footer text
    url_text.setPosition(url_text.getPosition().x, qr_spr.getGlobalBounds().top + qr_spr.getGlobalBounds().height + 6.f);
    fn_text.setPosition(fn_text.getPosition().x, qr_spr.getGlobalBounds().top - fn_text.getLocalBounds().height - 10.f);

    // file queue update timer
    sf::Clock clock;
    sf::Time interval = sf::milliseconds(500); //500ms
    sf::Time elapsedTime = interval; //init to interval so the first trigger occurs immediately

    while (window.isOpen()) {
        sf::Event event;
        elapsedTime += clock.restart();

        // Check if 500ms have passed
        if (elapsedTime >= interval) {
            
            elapsedTime -= interval; // Reset elapsed time, keep the remaining time for accuracy
            if (file_q.empty())
                recv_file.setString("Uploads will appear here");
            else
            {
                std::lock_guard<std::mutex> lock(file_q_mutex);
                recv_file.setString(file_q.front().getShortName() + "\n    (y) to accept\n    (n) to discard");
            }
        }
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return 1;
            }
            if (event.type == sf::Event::KeyPressed)
                switch (event.key.scancode)
                {
                    case sf::Keyboard::Scan::Escape:
                        window.close();
                        exit(0);
                    case sf::Keyboard::Scan::Space:
                        if (!SETTINGS.served_file.empty())
                        {
                            window.close();
                            return 0;
                        }
                        else
                            break;
                    case sf::Keyboard::Scan::Y:
                        if (!file_q.empty())
                        {
                            std::thread fileSave(pop_filerc, std::ref(file_q));
                            fileSave.detach();
                            elapsedTime = interval; // refresh file queue display
                        }
                        break;
                    case sf::Keyboard::Scan::N:
                        if (!file_q.empty())
                        {
                            file_q.pop();
                            elapsedTime = interval; // refresh file queue display
                        }
                }
        }

        // Clear, draw, and display
        window.clear(sf::Color(MODE_SEND ? SETTINGS.sendbgcolor : SETTINGS.recvbgcolor));
        window.draw(fn_text);
        window.draw(url_text);
        if (MODE_RECV)
        {
            window.draw(recv_file);
        }
        window.draw(qr_spr);

        window.display();
    }

    return 0;
}


// int create_window_recv(std::string qr_bmp, std::string url) {

//     sf::RenderWindow window(sf::VideoMode(530, 360), "Axon", sf::Style::Close);
//     //window.setFramerateLimit(30);
//     window.setVerticalSyncEnabled(true);
//     window.setIcon(ICON_WIDTH, ICON_HEIGHT, ICON_RGBA);

//     // Load the BMP image into a texture
//     sf::Texture qr_tex;

//     if (!qr_tex.loadFromMemory(qr_bmp.c_str(), qr_bmp.size())) {
//         std::cerr << "Error: Could not load recv qr BMP file!" << std::endl;
//         return 1;
//     }

//     sf::Font font;
//     if (!font.loadFromFile(ROOT_DIR + "/static/font/GeistMono-SemiBold.ttf"))
//     {
//         std::cerr << "Unable to load font\n";
//         exit(0);
//     }
    
//     sf::Text fn_text, url_text;
//     fn_text.setFont(font);
//     url_text.setFont(font);

//     fn_text.setString("Receive");
//     url_text.setString(url);

//     fn_text.setCharacterSize(18);
//     url_text.setCharacterSize(14);

//     fn_text.setFillColor(sf::Color::White);
//     url_text.setFillColor(sf::Color::White);
//     //text.setStyle(sf::Text::Bold | sf::Text::Underlined);

//     // Create a sprite to display the texture
//     sf::Sprite qr_spr;
//     qr_spr.setTexture(qr_tex);

//     //sf::RectangleShape qrBG(sf::Vector2f(325.f, 300.f));

//     qr_spr.setScale(12.f, 12.f);
//     //qrBG.setFillColor(sf::Color::Black);

//     centerSprite(qr_spr, window);
//     centerSprite(url_text, window);
//     centerSprite(fn_text, window);
//     //centerSprite(qrBG, window);

//     url_text.setPosition(url_text.getPosition().x, qr_spr.getGlobalBounds().top + qr_spr.getGlobalBounds().height + 6.f);
//     fn_text.setPosition(fn_text.getPosition().x, qr_spr.getGlobalBounds().top - fn_text.getLocalBounds().height - 10.f);

//     while (window.isOpen()) {
//         sf::Event event;
//         while (window.pollEvent(event)) {
//             if (event.type == sf::Event::Closed)
//             {
//                 window.close();
//                 return 1;
//             }
//             if (event.type == sf::Event::KeyPressed)
//                 if (event.key.scancode == sf::Keyboard::Scan::Space)
//                 {
//                     window.close();
//                     return 0;
//                     // if (current_qr == &qr_spr_send) current_qr = &qr_spr_recv; else current_qr = &qr_spr_send;
//                     // if (fn_text.getString() == filename) fn_text.setString("Receive"); else fn_text.setString(filename);
//                     // if (url_text.getString() == url_send) url_text.setString(url_recv); else url_text.setString(url_send);
//                     // if (currentBGColor == &SETTINGS.sendbgcolor) currentBGColor = &SETTINGS.recvbgcolor; else currentBGColor = &SETTINGS.sendbgcolor;
//                 }
//         }

//         // Clear, draw, and display
//         window.clear(sf::Color(SETTINGS.recvbgcolor));
//         window.draw(fn_text);
//         window.draw(url_text);
//         //window.draw(qrBG);
//         window.draw(qr_spr);
//         window.display();
//     }

//     return 0;
// }
