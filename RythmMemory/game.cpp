#include "card.h"
#include "game.h"
#include <iostream>
#include <vector>
#include <random>

void dealCards(Card cards[])
{
    std::vector<CardType> shuffledTypes = cardTypes;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffledTypes.begin(), shuffledTypes.end(), g);

    for (unsigned int i = 0; i < allCardsCount; i++)
    {
        cards[i].status = CardStatus::closed;
        cards[i].type = shuffledTypes[i];
    }
}