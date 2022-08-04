#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

float playerX = 5.0f;
float playerY = 5.0f;
float playerA = 0.0f;

int mapHeight = 16;
int mapWidth = 16;

float FOV = 3.14159 / 2.0;
float depth = 16.0f;

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Raymarcher", sf::Style::Close);
    // Image that we render our screen to
    sf::Image screen;
    screen.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Blue);
    sf::Texture screenTexture;
    sf::Sprite screenSprite;

    std::string map;
    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#...#..........#";
    map += "#...#..........#";
    map += "#...#..........#";
    map += "#...######.....#";
    map += "#...#..........#";
    map += "#...#..........#";
    map += "#...#..........#";
    map += "#..............#";
    map += "#..............#";
    map += "################";

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();


    while (window.isOpen())
    {
        // Calculates delta time
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float elapsedTimeF = elapsedTime.count();
        float Framerate = 1.0f / elapsedTime.count();
        std::cout << Framerate << std::endl;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Player input
        // CCW rotation
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            playerA -= 1.5f * elapsedTimeF;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            playerA += 1.5f * elapsedTimeF;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            playerX += sinf(playerA) * 5.0f * elapsedTimeF;
            playerY += cosf(playerA) * 5.0f * elapsedTimeF;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            playerX -= sinf(playerA) * 5.0f * elapsedTimeF;
            playerY -= cosf(playerA) * 5.0f * elapsedTimeF;
        }

        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            // For each column, calculate the projected ray angle into world space
            float rayAngle = (playerA - FOV / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * FOV;

            float distanceToWall = 0.0f;
            bool hitWall = false;

            float eyeX = sinf(rayAngle); // Unit vector for ray in player space
            float eyeY = cosf(rayAngle);

            while (!hitWall && distanceToWall < depth)
            {
                distanceToWall += 0.1f;

                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);

                // Test if ray is out of bounds
                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
                {
                    hitWall = true;
                    distanceToWall = depth;
                }
                else
                {
                    // Ray is inbounds so test to see if the ray cell is a wall block
                    if (map[testY * mapWidth + testX] == '#')
                    {
                        hitWall = true;
                    }
                }
            }

            // Calculate distance to ceiling and floor
            int ceiling = (float)(SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / ((float)distanceToWall);
            int floor = SCREEN_HEIGHT - ceiling;

            sf::Color shade;

            // Automate this eventually for supporting colored walls
            if (distanceToWall <= depth / 4.0f)         shade = sf::Color::White;           // Very Close
            else if (distanceToWall < depth / 3.5f)     shade = sf::Color(224, 224, 224);
            else if (distanceToWall < depth / 3.0f)     shade = sf::Color(192, 192, 192);
            else if (distanceToWall < depth / 2.5f)     shade = sf::Color(160, 160, 160);
            else if (distanceToWall < depth / 2.0f)     shade = sf::Color(128, 128, 128);
            else if (distanceToWall < depth / 1.5f)     shade = sf::Color(96, 96, 96);
            else if (distanceToWall < depth / 1.0f)     shade = sf::Color(64, 64, 64);
            else if (distanceToWall < depth / 0.5f)     shade = sf::Color(32, 32, 32);
            else                                        shade = sf::Color::Black;           // Too far

            for (int y = 0; y < SCREEN_HEIGHT; y++)
            {
                if (y < ceiling)
                    screen.setPixel(x, y, sf::Color::Black);
                else if (y > ceiling && y <= floor)
                    screen.setPixel(x, y, shade);
                else
                    screen.setPixel(x, y, sf::Color::Black);
            }
        }

        // Updates the screen image
        screenTexture.loadFromImage(screen);
        screenSprite.setTexture(screenTexture);

        window.clear();
        window.draw(screenSprite);
        window.display();
    }

    return 0;
}