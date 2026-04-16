#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class CardStatus { opened, closed, discarded };
enum class CardType { notIsFlower, sunflower, gentian, ghostpipe, waterlily };

extern std::vector<CardType> cardTypes;

class Card {
public:
    CardType type = CardType::notIsFlower;
    CardStatus status = CardStatus::discarded;
    
    const sf::Texture* frontSide;
    const sf::Texture* backSide;

    sf::RectangleShape shape;

    float timer = 0.f;

    Card();
    Card(CardType cardType, CardStatus cardStatus);

    void draw(sf::RenderWindow& window);
    static bool isPair(Card card1, Card card2);
};