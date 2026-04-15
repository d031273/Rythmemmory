#include "card.h"

std::vector<CardType> cardTypes = {
    CardType::sunflower,
    CardType::sunflower,
    CardType::gentian,
    CardType::gentian,
    CardType::ghostpipe,
    CardType::ghostpipe,
    CardType::waterlily,
    CardType::waterlily
};

Card::Card() {
	shape.setSize({ 250.f, 350.f });
	shape.setFillColor(sf::Color::Green);
}

Card::Card(CardType cardType, CardStatus cardStatus) {
	type = cardType;
	status = cardStatus;
	shape.setSize({ 250.f, 350.f });
	shape.setFillColor(sf::Color::Green);
}

void Card::draw(sf::RenderWindow& window) {
	window.draw(shape);
}

bool Card::isPair(Card card1, Card card2) {
    return card1.type == card2.type;
}
