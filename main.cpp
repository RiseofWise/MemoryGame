#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

int WIDTH = 600;
int HEIGHT = 600;
int size = 4;
int bWIDTH = WIDTH / size;
int bHEIGHT = WIDTH / size;

string int_to_string(int i){
    stringstream ss;
    ss<<i;
    return ss.str();
}

int randomRange(int min, int max){
    return min + (rand() % static_cast<int>(max - min + 1));
}

class Tile{
public:
    int x, y, id, currentWidth;
    bool isFlipping = false;
    bool flipped = true;
    bool allowFlipping = true;
    RectangleShape body;
    Texture* texture;
    Tile(int _x, int _y, int _id, Texture* _texture, IntRect textureArea){
        x = _x;
        y = _y;
        id = _id;
        texture = _texture;
        body = RectangleShape(Vector2f(-bWIDTH, bHEIGHT));
        FloatRect textRect = body.getLocalBounds();
        body.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
        body.setPosition(Vector2f(x * bWIDTH + bWIDTH / 2.0f, y * bHEIGHT + bHEIGHT / 2.0f));
        body.setTextureRect(textureArea);
        body.setOutlineColor(Color::Black);
        body.setOutlineThickness(-1);
        currentWidth = -bWIDTH;
    }

    void draw(RenderWindow* window){
        window->draw(body);
    }

    void setPos(int _x, int _y){
        x = _x;
        y = _y;
        body.setPosition(x * bWIDTH, y * bHEIGHT);
    }

    void flip(){
        currentWidth += flipped ? 10 : -10;
        body.setSize(Vector2f(currentWidth, bHEIGHT));
        FloatRect textRect = body.getLocalBounds();
        body.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
        body.setPosition(Vector2f(x * bWIDTH + bWIDTH / 2.0f, y * bHEIGHT + bHEIGHT / 2.0f));
        if(currentWidth < 0 && flipped == false){
            body.setTexture(NULL);
        }else if(currentWidth > 0 && flipped == true){
            body.setTexture(texture);
        }
        if(currentWidth < -bWIDTH + 10){
            isFlipping = false;
            flipped = true;
            return;
        }
        if(currentWidth > bWIDTH - 10){
            isFlipping = false;
            flipped = false;
        }
    }
};

int main()
{
    srand(time(0));
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Memory Game");
    Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
       cout << "Failed to load font!" << endl;
    }
    Texture runes;
    if (!runes.loadFromFile("runes.png"))
    {
       cout << "Failed to load texture!" << endl;
    }
    vector<Tile> tiles;
    vector<vector<int>> positions = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {3, 0}, {3, 1}, {3, 2}, {3, 3} };
    int row = 0;
    for(int i = 0; i < size * size; ++i){
        if(i % 4 && i != 0) row++;
        int pos = randomRange(0, positions.size() - 1);
        vector<int> position = positions.at(pos);
        positions.erase(positions.begin() + pos);
        tiles.push_back(Tile(position.at(0), position.at(1), i % 8, &runes, IntRect(185 * (i % 4), 185 * (row % 2), 185, 185)));
    }
    Clock animationClock;
    animationClock.restart();
    Tile* firstTile = NULL;
    Tile* secondTile = NULL;
    Clock cdClock;
    Clock startClock;
    int stage = 0;
    startClock.restart();
    bool cd = false;
    cdClock.restart();
    int tries = 0;
    int right = 0;

    RectangleShape winPanel(Vector2f(400, 150));
    winPanel.setOutlineColor(Color::Black);
    winPanel.setOutlineThickness(4);
    FloatRect textRect = winPanel.getLocalBounds();
    winPanel.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
    winPanel.setPosition(Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f));

    Text winText;
    winText.setFont(font);
    winText.setColor(Color::Black);
    winText.setCharacterSize(40);

    Text restartText;
    restartText.setFont(font);
    restartText.setColor(Color::Black);
    restartText.setString("Press 'R' to restart!");
    restartText.setCharacterSize(40);
    textRect = restartText.getLocalBounds();
    restartText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
    restartText.setPosition(Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f + 40));

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

             if (event.type == sf::Event::MouseButtonPressed){
                if (event.mouseButton.button == sf::Mouse::Left && stage == 2){
                    int x = event.mouseButton.x / bWIDTH;
                    int y = event.mouseButton.y / bHEIGHT;
                    for(int i = 0; i < tiles.size(); ++i){
                        Tile t = tiles.at(i);
                        if(t.x == x && t.y == y && t.isFlipping == false){
                            if(firstTile == NULL && tiles.at(i).allowFlipping){
                                firstTile = &tiles.at(i);
                                firstTile->isFlipping = true;
                                firstTile->allowFlipping = false;
                            }else if(firstTile && secondTile == NULL && firstTile != &tiles.at(i) && tiles.at(i).allowFlipping){
                                secondTile = &tiles.at(i);
                                secondTile->isFlipping = true;
                                secondTile->allowFlipping = false;
                                if(firstTile->id != secondTile->id){
                                    secondTile->allowFlipping = true;
                                    firstTile->allowFlipping = true;
                                    cd = true;
                                    cdClock.restart();
                                }else{
                                    firstTile->allowFlipping = false;
                                    secondTile->allowFlipping = false;
                                    firstTile = NULL;
                                    secondTile = NULL;
                                    right++;
                                }
                                tries++;
                                if(right == 8){
                                    stage = 3;
                                    winText.setString("Finished with " + int_to_string(tries) + " tries");
                                    textRect = winText.getLocalBounds();
                                    winText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top  + textRect.height / 2.0f);
                                    winText.setPosition(Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f - 40));
                                }
                            }
                        }
                    }
                }
             }

             if(event.type == Event::KeyPressed && Keyboard::isKeyPressed(Keyboard::R) && stage == 3){
                stage = 0;
                right = 0;
                tries = 0;
                tiles.clear();
                positions = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {3, 0}, {3, 1}, {3, 2}, {3, 3} };
                row = 0;
                for(int i = 0; i < size * size; ++i){
                    if(i % 4 && i != 0) row++;
                    int pos = randomRange(0, positions.size() - 1);
                    vector<int> position = positions.at(pos);
                    positions.erase(positions.begin() + pos);
                    tiles.push_back(Tile(position.at(0), position.at(1), i % 8, &runes, IntRect(185 * (i % 4), 185 * (row % 2), 185, 185)));
                }
                startClock.restart();
            }
        }

        if(animationClock.getElapsedTime().asMilliseconds() >= 10){
            for(int i = 0; i < tiles.size(); ++i){
                if(tiles.at(i).isFlipping){
                    tiles.at(i).flip();
                }
            }
            animationClock.restart();
        }

        if(startClock.getElapsedTime().asMilliseconds() >= 500 && stage == 0){
            stage = 1;
            for(int i = 0; i < tiles.size(); ++i){
                tiles.at(i).isFlipping = true;
            }
        }

        if(startClock.getElapsedTime().asMilliseconds() >= 3500 && stage == 1){
            stage = 2;
            for(int i = 0; i < tiles.size(); ++i){
                tiles.at(i).isFlipping = true;
            }
        }

        if(cd && cdClock.getElapsedTime().asMilliseconds() >= 1000){
            cd = false;
            firstTile->isFlipping = true;
            secondTile->isFlipping = true;
            firstTile = NULL;
            secondTile = NULL;
        }

        window.clear(Color::White);
        for(int i = 0; i < tiles.size(); ++i){
            tiles.at(i).draw(&window);
        }
        if(stage == 3){
            window.draw(winPanel);
            window.draw(winText);
            window.draw(restartText);
        }
        window.display();
    }

    return 0;
}
