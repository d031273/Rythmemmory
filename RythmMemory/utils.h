#pragma once
#include "card.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <map>

std::ostream& operator<<(std::ostream& os, CardStatus status);

std::string toString(CardType type);

sf::Texture* getTextureByType(CardType type, std::map<CardType, sf::Texture>& textures);