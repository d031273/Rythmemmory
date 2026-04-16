#include "card.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <map>
#include <chrono>
#include <ctime>


std::ostream& operator<<(std::ostream& os, CardStatus status) {
    switch (status) {
    case CardStatus::opened: return os << "open";
    case CardStatus::closed: return os << "close";
    case CardStatus::discarded: return os << "discarded";
    }
    return os;
}

std::string toString(CardType type) {
    switch (type) {
    case CardType::sunflower: return "sunflower";
    case CardType::gentian: return "gentian";
    case CardType::ghostpipe: return "ghostpipe";
    case CardType::waterlily: return "waterlily";
    }
    return "unknown";
}

sf::Texture* getTextureByType(CardType type, std::map<CardType, sf::Texture>& textures) //???
{
    auto it = textures.find(type);
    if (it != textures.end())
        return &it->second;
    return nullptr;
}