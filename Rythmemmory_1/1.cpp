#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <map>


const unsigned int count_all_cards = 8;

enum class CardStatus {open,close, discarded};
enum class CardType {sunflower, gentian, ghost_pipe, waterlily};
std::vector<CardType> types = {
    CardType::sunflower,
    CardType::sunflower,
    CardType::gentian,
    CardType::gentian,
    CardType::ghost_pipe,
    CardType::ghost_pipe,
    CardType::waterlily,
    CardType::waterlily
};


std::ostream& operator<<(std::ostream& os, CardStatus status) {
    switch (status) {
    case CardStatus::open: return os << "open";
    case CardStatus::close: return os << "close";
    case CardStatus::discarded: return os << "discarded";
    }
    return os;
}
std::string toString(CardType type) {
    switch (type) {
    case CardType::sunflower: return "sunflower";
    case CardType::gentian: return "gentian";
    case CardType::ghost_pipe: return "ghost_pipe";
    case CardType::waterlily: return "waterlily";
    }
    return "unknown";
}



class Card {
public:
    CardType type; 
    CardStatus status; 
    sf::RectangleShape shape;

    Card() 
    {
        shape.setSize({ 250.f, 350.f });
        shape.setFillColor(sf::Color::Green);
    }

    Card(CardType p_type, CardStatus p_status)
    {
        type = p_type;
        status = p_status;
        shape.setSize({ 250.f, 350.f });
        shape.setFillColor(sf::Color::Green);
    }

    float timer = 0.f; // время после изменения состояния


    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

   
};

void full_table(Card Cards[])
{
    // 1. Копируем types (чтобы не портить оригинал)
    std::vector<CardType> shuffledTypes = types;

    // 2. Перемешиваем
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffledTypes.begin(), shuffledTypes.end(), g);

    // 3. Заполняем карты
    for (unsigned int i = 0; i < count_all_cards; i++)
    {
        Cards[i].status = CardStatus::close;
        Cards[i].type = shuffledTypes[i];
    }
}

bool is_pair(Card open_card1, Card open_card2)
{
    if (open_card1.type == open_card2.type)
        return 1;
    return 0;
}

sf::Texture* getTextureByType(CardType type, std::map<CardType, sf::Texture>& textures)
{
    auto it = textures.find(type);
    if (it != textures.end())
        return &it->second;
    return nullptr;
}


int main()
{
    
    sf::Texture sunflowerTexture;
    sf::Texture gentianTexture;
    sf::Texture ghost_pipeTexture;
    sf::Texture waterlilyTexture;
    std::map<CardType, sf::Texture> textures;
    textures[CardType::sunflower].loadFromFile("../Flowers/sunflower.png");
    textures[CardType::gentian].loadFromFile("../Flowers/gentian.png");
    textures[CardType::ghost_pipe].loadFromFile("../Flowers/ghost_pipe.png");
    textures[CardType::waterlily].loadFromFile("../Flowers/waterlily.png");
    


    sf::Font font;
    font.openFromFile("../fonts/BRLNSR.TTF");

    sf::Text points_txt(font);
    points_txt.setCharacterSize(100);
    points_txt.setFillColor(sf::Color::White);

    sf::Text timer_txt(font);
    timer_txt.setCharacterSize(100);
    timer_txt.setFillColor(sf::Color::White);
    


    Card Cards[count_all_cards];
   
    for (unsigned int i = 0; i < count_all_cards; i++)
    {
        Cards[i].status = CardStatus::discarded;
    }

    

    Cards[0].shape.setPosition({ 100.f, 100.f });



    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "My window");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    



    unsigned int count_open_cards = 0;
    unsigned int open_card1, open_card2;
    unsigned int points = 0;
    
    sf::Clock clock;
    float gameTime = 0.f;

    bool waiting = false;
    float waitTimer = 0.f;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (waiting)
        {
            waitTimer += dt;

            if (waitTimer >= 1.f)
            {
                Cards[open_card1].status = CardStatus::close;
                Cards[open_card2].status = CardStatus::close;

                count_open_cards = 0;
                waiting = false;
            }
        }
        
        gameTime += dt;

        while (const std::optional event = window.pollEvent())
        {

            if (event->is<sf::Event::Closed>()) //закрытие окна
                window.close();

            if ((event->is<sf::Event::MouseButtonPressed>())&& (!waiting)) // если обнаружено нажатие мыши
            {
                const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) // если обнаружено левое нажатие мыши
                {
                    sf::Vector2f mousePos = window.mapPixelToCoords(
                    { mouseEvent->position.x, mouseEvent->position.y });//  координаты нажатия
                    for (int i = 0; i < count_all_cards; i++)
                    {
                        if (Cards[i].shape.getGlobalBounds().contains(mousePos) && (Cards[i].status == CardStatus::close)) //координаты нажатия совпадают с областью карты
                        {
                            Cards[i].status = CardStatus::open;
                            count_open_cards += 1;
                            if (count_open_cards == 1)
                            {
                                open_card1 = i;
                            }
                            else if (count_open_cards == 2)
                            {
                                open_card2 = i;
                                if (is_pair(Cards[open_card1], Cards[open_card2]))
                                {
                                    dt = clock.restart().asSeconds();
                                    
                                    points += 100;
                                    Cards[open_card1].status = CardStatus::discarded;
                                    Cards[open_card2].status = CardStatus::discarded;

                                    count_open_cards = 0;
                                }
                                else
                                {
                                    waiting = true;
                                    waitTimer = 0.f;

                                }

                            }
                        }
                    }

                }
            }
        }

        unsigned int count_discarded_cards = 0;
        for (int i = 0; i < count_all_cards; i++)
        {
            if (Cards[i].status == CardStatus::open)
            {
                sf::Texture* texture = getTextureByType(Cards[i].type, textures); // получить текстуру для этого типа карты

                if (texture) {
                    Cards[i].shape.setTexture(texture);
                    Cards[i].shape.setFillColor(sf::Color::White);
                }
            }
            else if (Cards[i].status == CardStatus::close)
            {

                Cards[i].shape.setTexture(nullptr);
                Cards[i].shape.setFillColor(sf::Color::Green);

            }       

            else
            {
                count_discarded_cards += 1;
                sf::Texture* texture = getTextureByType(Cards[i].type, textures); // получить текстуру для этого типа карты

                if (texture) {
                    Cards[i].shape.setTexture(texture);
                    Cards[i].shape.setFillColor(sf::Color::White);
                }
                Cards[i].timer += dt;

                if (Cards[i].timer >= 2.f)
                {
                    Cards[i].shape.setTexture(nullptr);
                    Cards[i].shape.setFillColor(sf::Color::Black);
                }

            }

            if (count_discarded_cards == count_all_cards)
            {
                full_table(Cards);
            }
        }

        int i;
        window.clear();
        for (i = 0; i < (count_all_cards/2); i++)
        {
            Cards[i].shape.setPosition({ 100.f + i * 400, 100.f  });
            Cards[i].draw(window);
        }

        points_txt.setPosition({ 100.f + i * 400, 100.f });
        points_txt.setString(std::to_string(points));
        window.draw(points_txt);

        for (i = count_all_cards / 2;i < count_all_cards; i++)
        {
            Cards[i].shape.setPosition({ 100.f + (i - count_all_cards / 2) * 400, 600.f });
            Cards[i].draw(window);
        }

        timer_txt.setPosition({ 100.f + (i - count_all_cards / 2) * 400, 600.f });
        timer_txt.setString(std::to_string(gameTime));
        window.draw(timer_txt);

        
        window.display();

        if (gameTime >= 180.f)
        {
            window.close();
        }
    }
}