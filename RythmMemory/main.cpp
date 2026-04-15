#include <SFML/Graphics.hpp> // ???
#include <optional>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <map>


const unsigned int allCardsCount = 8;

enum class CardStatus { opened, closed, discarded };
enum class CardType { sunflower, gentian, ghostpipe, waterlily };
std::vector<CardType> types = {
    CardType::sunflower,
    CardType::sunflower,
    CardType::gentian,
    CardType::gentian,
    CardType::ghostpipe,
    CardType::ghostpipe,
    CardType::waterlily,
    CardType::waterlily
};


std::ostream& operator<<(std::ostream& os, CardStatus status) { //две функции для вывода в cout новых типов
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



class Card {
public:
    CardType type; // растения
    CardStatus status; // открыто, закрыто, сброшено
    sf::RectangleShape shape; // визуальное представление карты, которое будет отриовано в окне
    float timer = 0.f; // время после изменения состояния

    Card() //конструктор по умолчанию - если не было передано параметров, нужен
        // чтобы создать массив карт без заполнения данных
    {
        shape.setSize({ 250.f, 350.f }); // задать размер карты
        shape.setFillColor(sf::Color::Green); //задать цвет заливки карты
    }

    Card(CardType cardType, CardStatus cardStatus) // конструктор с параметрами
    {
        type = cardType;
        status = cardStatus;
        shape.setSize({ 250.f, 350.f });
        shape.setFillColor(sf::Color::Green);
    }

    


    void draw(sf::RenderWindow& window) {//функция отрисовки карты добавлена в класс для удобства
        window.draw(shape);
    }


};

void dealCards(Card Cards[]) //фунция разложения стола
{
    //Копируем types 
    std::vector<CardType> shuffledTypes = types;

    // Перемешиваем
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffledTypes.begin(), shuffledTypes.end(), g);

    // Заполняем карты
    for (unsigned int i = 0; i < allCardsCount; i++)
    {
        Cards[i].status = CardStatus::closed;
        Cards[i].type = shuffledTypes[i];
    }
}

bool isPair(Card card1, Card card2)// проверка карт на парность
{
    if (card1.type == card2.type)
        return 1;
    return 0;
}

sf::Texture* getTextureByType(CardType type, std::map<CardType, sf::Texture>& textures)
// функция, которая проверяет получиось ли подгрузить текстуру, если получилось вернет текстуру
{
    auto it = textures.find(type);
    if (it != textures.end())
        return &it->second;
    return nullptr;
}


int main()
{

    sf::Texture sunflowerTexture; // создание переменных, где будут хранится текстуры
    sf::Texture gentianTexture;
    sf::Texture ghostpipeTexture;
    sf::Texture waterlilyTexture;
    std::map<CardType, sf::Texture> textures; // создание и заполнение словоря (тип карты -> текстура)
    textures[CardType::sunflower].loadFromFile("../Assets/sunflower.png");
    textures[CardType::gentian].loadFromFile("../Assets/gentian.png");
    textures[CardType::ghostpipe].loadFromFile("../Assets/ghostpipe.png");
    textures[CardType::waterlily].loadFromFile("../Assets/waterlily.png");



    sf::Font font;
    font.openFromFile("../Fonts/BRLNSR.TTF");//подгрузка шрифтов

    sf::Text txtPoints(font); // создание объекта текст для вывода очков
    txtPoints.setCharacterSize(100);
    txtPoints.setFillColor(sf::Color::White);

    sf::Text txtTimer(font); // создание объекта текст для вывода таймера
    txtTimer.setCharacterSize(100);
    txtTimer.setFillColor(sf::Color::White);



    Card cards[allCardsCount]; // создание массива с картами, все карты созданы без параметров

    for (unsigned int i = 0; i < allCardsCount; i++)
    {
        cards[i].status = CardStatus::discarded; // присвоение всем картам статуса сброшено
        // это происходит до создания окна и зацикливания программы.
        // это необходимо, т.к. условие раскладывания нового стола - это все карты сброшены.
        // при первом проходе программа пройдёт почти до конца ничего не выполнив, так с сброшенными картами
        // ничего делать нельзя. В конце программы будет совершен расклад стола и программа начнёт нормальное выполнение.
    }



    cards[0].shape.setPosition({ 100.f, 100.f }); // лишняя строка, забыл удалить



    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "My window"); //создание объекта окно под именем window
    //задаётся разрешение окна и его название
    window.setVerticalSyncEnabled(true); // включение вертикальной синхронизации
    window.setFramerateLimit(60); // установлен фреймрейт




    unsigned int openedCardsCount = 0; // подсчёт кол-ва открытых карт
    unsigned int openedCard1, openedCard2; // индексы карт в массиве cards которые сейчас открыты
    unsigned int points = 0; // очки

    sf::Clock clock; // создание часов
    float gameTime = 0.f; // подсчёт времени игры

    bool waiting = false; // true когда открыты 2 непарные карты, в этот момент 
    // нельзя открывать другие и можно посмотреть на типы не одинаковых
    float waitTimer = 0.f; // ждёт пока пройдёт 1 секунда, чтобы снова можно было открывать карты
    float dt;

    while (window.isOpen())
    {
        dt = clock.restart().asSeconds(); //возврщение знач с часов в секундах, сброс часов
        if (waiting)
        {
            waitTimer += dt; // увеличиваем время которое показывает сколько мы ждем

            if (waitTimer >= 1.f) // если подождали 1 секунду
            {
                cards[openedCard1].status = CardStatus::closed;//закрываем карты
                cards[openedCard2].status = CardStatus::closed;

                openedCardsCount = 0; // открытых карт нет 
                waiting = false; // больше не ждём
            }
        }

        gameTime += dt; // подсчёт кол-ва времени в игре, потом пойдёт в таймер

        while (const std::optional event = window.pollEvent()) // объяснил optional в примере выше
            // 
        {

            if (event->is<sf::Event::Closed>()) //закрытие окна
                window.close();

            if ((event->is<sf::Event::MouseButtonPressed>()) && (!waiting)) // если обнаружено нажатие мыши
            {
                const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) // если обнаружено левое нажатие мыши
                {
                    sf::Vector2f mousePos = window.mapPixelToCoords(
                        { mouseEvent->position.x, mouseEvent->position.y });//  координаты нажатия
                    for (int i = 0; i < allCardsCount; i++)
                    {
                        if (cards[i].shape.getGlobalBounds().contains(mousePos) && (cards[i].status == CardStatus::closed)) //координаты нажатия совпадают с областью карты
                        {
                            cards[i].status = CardStatus::opened;// изменить статус на открыта
                            openedCardsCount += 1; // увеличить кол-во открытых сейчас карт
                            if (openedCardsCount == 1) // если открыта 1 карта
                            {
                                openedCard1 = i; // запомнить её индекс
                            }
                            else if (openedCardsCount == 2) // если открыто 2 карты
                            {
                                openedCard2 = i; // запомнить её индекс
                                if (isPair(cards[openedCard1], cards[openedCard2])) //проверить на парность
                                {
                                    //dt = clock.restart().asSeconds();

                                    points += 100;//начислить очки за пару
                                    cards[openedCard1].status = CardStatus::discarded;//карты сделать сброшенными
                                    cards[openedCard2].status = CardStatus::discarded;

                                    openedCardsCount = 0;//обнулить кол-во открытых карт
                                }
                                else
                                {
                                    waiting = true;//включить ожидание на 1 сек
                                    waitTimer = 0.f;

                                }

                            }
                        }
                    }

                }
            }
        }

        unsigned int discardedCardsCount = 0;//счётчик сброшенных карт
        for (int i = 0; i < allCardsCount; i++)//пробег по всем картам на столе
        {
            if (cards[i].status == CardStatus::opened)//если карта открыта
            {
                sf::Texture* texture = getTextureByType(cards[i].type, textures); // получить текстуру для этого типа карты

                if (texture) {
                    cards[i].shape.setTexture(texture);//загрузить текстуру на карту
                    cards[i].shape.setFillColor(sf::Color::White);
                }
            }
            else if (cards[i].status == CardStatus::closed)//если карта закрыта
            {

                cards[i].shape.setTexture(nullptr);// убрать текстуру
                cards[i].shape.setFillColor(sf::Color::Green);//поставить зелёную рубашку

            }

            else //если карта сброшена
            {
                discardedCardsCount += 1;//считаем кол-во сброшенных карт
                sf::Texture* texture = getTextureByType(cards[i].type, textures); // получить текстуру для этого типа карты

                if (texture) {
                    cards[i].shape.setTexture(texture);//надеть текстуру на карту
                    cards[i].shape.setFillColor(sf::Color::White);
                }
                cards[i].timer += dt;// зафиксировать когда сброшено

                if (cards[i].timer >= 2.f)// если сброшено 2 секунды назад
                {
                    cards[i].shape.setTexture(nullptr);//убрать карту со стола = надеть чёрную текстуру 
                    cards[i].shape.setFillColor(sf::Color::Black);
                }

            }

            if (discardedCardsCount == allCardsCount) // если все карты сброшены
            {
                dealCards(cards);// заполнить стол новыми картами
                for (int i = 0; i < allCardsCount; i++)
                {
                    cards[i].timer = 0;// сброс таймера у всех карт
                }

            }
        }

        int i;
        window.clear();// очистить окно
        for (i = 0; i < (allCardsCount / 2); i++)
        {
            cards[i].shape.setPosition({ 100.f + i * 400, 100.f });//задаём позиции картам в 1 ряду
            cards[i].draw(window);
        }

        txtPoints.setPosition({ 100.f + i * 400, 100.f });
        txtPoints.setString(std::to_string(points)); //задаём позицию очкам справа от 1 ряда
        window.draw(txtPoints);

        for (i = allCardsCount / 2; i < allCardsCount; i++)
        {
            cards[i].shape.setPosition({ 100.f + (i - allCardsCount / 2) * 400, 600.f });//задаём позиции картам в 2 ряду
            cards[i].draw(window);
        }

        txtTimer.setPosition({ 100.f + (i - allCardsCount / 2) * 400, 600.f });//задаём позицию таймеру справа от 2 ряда
        txtTimer.setString(std::format("{:.2f}", 60 - gameTime));
        window.draw(txtTimer);


        window.display();//выводим в окно всё что нарисовали

        if (gameTime >= 60.f)//если игра длится дольше, закрываем окно
        {
            window.close();

        }
    }
}