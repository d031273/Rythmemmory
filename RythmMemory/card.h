#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class CardStatus { opened, closed, discarded };
enum class CardType { notisflower, sunflower, gentian, ghostpipe, waterlily };

extern std::vector<CardType> cardTypes;

class Card {
public:
    CardType type = CardType::notisflower;
    CardStatus status = CardStatus::closed;

    sf::RectangleShape shape;

    float timer = 0.f;

    Card();
    Card(CardType cardType, CardStatus cardStatus);

    void draw(sf::RenderWindow& window);
    static bool isPair(Card card1, Card card2);
};