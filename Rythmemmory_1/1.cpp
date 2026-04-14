#include <SFML/Graphics.hpp>
#include <optional>// ??? Ответ: обяснено в код блоке выше, 
//используется в while (const std::optional event = window.pollEvent())
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <map>


const unsigned int count_all_cards = 8;

enum class CardStatus { open, close, discarded };
enum class CardType { sunflower, gentian, ghost_pipe, waterlily };
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


std::ostream& operator<<(std::ostream& os, CardStatus status) { //две функции для вывода в cout новых типов
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
    CardType type; // растения
    CardStatus status; // открыто, закрыто, сброшено
    sf::RectangleShape shape; // визуальное представление карты, которое будет отриовано в окне

    Card() //конструктор по умолчанию - если не было передано параметров, нужен
        // чтобы создать массив карт без заполнения данных
    {
        shape.setSize({ 250.f, 350.f }); // задать размер карты
        shape.setFillColor(sf::Color::Green); //задать цвет заливки карты
    }

    Card(CardType p_type, CardStatus p_status) // конструктор с параметрами
    {
        type = p_type;
        status = p_status;
        shape.setSize({ 250.f, 350.f });
        shape.setFillColor(sf::Color::Green);
    }

    float timer = 0.f; // время после изменения состояния


    void draw(sf::RenderWindow& window) {//функция отрисовки карты добавлена в класс для удобства
        window.draw(shape);
    }


};

void full_table(Card Cards[]) //фунция разложения стола
{
    //Копируем types 
    std::vector<CardType> shuffledTypes = types;

    // Перемешиваем
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffledTypes.begin(), shuffledTypes.end(), g);

    // Заполняем карты
    for (unsigned int i = 0; i < count_all_cards; i++)
    {
        Cards[i].status = CardStatus::close;
        Cards[i].type = shuffledTypes[i];
    }
}

bool is_pair(Card open_card1, Card open_card2)// проверка карт на парность
{
    if (open_card1.type == open_card2.type)
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
    sf::Texture ghost_pipeTexture;
    sf::Texture waterlilyTexture;
    std::map<CardType, sf::Texture> textures; // создание и заполнение словоря (тип карты -> текстура)
    textures[CardType::sunflower].loadFromFile("../Flowers/sunflower.png");
    textures[CardType::gentian].loadFromFile("../Flowers/gentian.png");
    textures[CardType::ghost_pipe].loadFromFile("../Flowers/ghost_pipe.png");
    textures[CardType::waterlily].loadFromFile("../Flowers/waterlily.png");



    sf::Font font;
    font.openFromFile("../fonts/BRLNSR.TTF");//подгрузка шрифтов

    sf::Text points_txt(font); // создание объекта текст для вывода очков
    points_txt.setCharacterSize(100);
    points_txt.setFillColor(sf::Color::White);

    sf::Text timer_txt(font); // создание объекта текст для вывода таймера
    timer_txt.setCharacterSize(100);
    timer_txt.setFillColor(sf::Color::White);



    Card Cards[count_all_cards]; // создание массива с картами, все карты созданы без параметров

    for (unsigned int i = 0; i < count_all_cards; i++)
    {
        Cards[i].status = CardStatus::discarded; // присвоение всем картам статуса сброшено
        // это происходит до создания окна и зацикливания программы.
        // это необходимо, т.к. условие раскладывания нового стола - это все карты сброшены.
        // при первом проходе программа пройдёт почти до конца ничего не выполнив, так с сброшенными картами
        // ничего делать нельзя. В конце программы будет совершен расклад стола и программа начнёт нормальное выполнение.
    }



    Cards[0].shape.setPosition({ 100.f, 100.f }); // лишняя строка, забыл удалить



    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "My window"); //создание объекта окно под именем window
    //задаётся разрешение окна и его название
    window.setVerticalSyncEnabled(true); // включение вертикальной синхронизации
    window.setFramerateLimit(60); // установлен фреймрейт




    unsigned int count_open_cards = 0; // подсчёт кол-ва открытых карт
    unsigned int open_card1, open_card2; // индексы карт в массиве Cards которые сейчас открыты
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
                Cards[open_card1].status = CardStatus::close;//закрываем карты
                Cards[open_card2].status = CardStatus::close;

                count_open_cards = 0; // открытых карт нет 
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
                    for (int i = 0; i < count_all_cards; i++)
                    {
                        if (Cards[i].shape.getGlobalBounds().contains(mousePos) && (Cards[i].status == CardStatus::close)) //координаты нажатия совпадают с областью карты
                        {
                            Cards[i].status = CardStatus::open;// изменить статус на открыта
                            count_open_cards += 1; // увеличить кол-во открытых сейчас карт
                            if (count_open_cards == 1) // если открыта 1 карта
                            {
                                open_card1 = i; // запомнить её индекс
                            }
                            else if (count_open_cards == 2) // если открыто 2 карты
                            {
                                open_card2 = i; // запомнить её индекс
                                if (is_pair(Cards[open_card1], Cards[open_card2])) //проверить на парность
                                {
                                    //dt = clock.restart().asSeconds();

                                    points += 100;//начислить очки за пару
                                    Cards[open_card1].status = CardStatus::discarded;//карты сделать сброшенными
                                    Cards[open_card2].status = CardStatus::discarded;

                                    count_open_cards = 0;//обнулить кол-во открытых карт
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

        unsigned int count_discarded_cards = 0;//счётчик сброшенных карт
        for (int i = 0; i < count_all_cards; i++)//пробег по всем картам на столе
        {
            if (Cards[i].status == CardStatus::open)//если карта открыта
            {
                sf::Texture* texture = getTextureByType(Cards[i].type, textures); // получить текстуру для этого типа карты

                if (texture) {
                    Cards[i].shape.setTexture(texture);//загрузить текстуру на карту
                    Cards[i].shape.setFillColor(sf::Color::White);
                }
            }
            else if (Cards[i].status == CardStatus::close)//если карта закрыта
            {

                Cards[i].shape.setTexture(nullptr);// убрать текстуру
                Cards[i].shape.setFillColor(sf::Color::Green);//поставить зелёную рубашку

            }

            else //если карта сброшена
            {
                count_discarded_cards += 1;//считаем кол-во сброшенных карт
                sf::Texture* texture = getTextureByType(Cards[i].type, textures); // получить текстуру для этого типа карты

                if (texture) {
                    Cards[i].shape.setTexture(texture);//надеть текстуру на карту
                    Cards[i].shape.setFillColor(sf::Color::White);
                }
                Cards[i].timer += dt;// зафиксировать когда сброшено

                if (Cards[i].timer >= 2.f)// если сброшено 2 секунды назад
                {
                    Cards[i].shape.setTexture(nullptr);//убрать карту со стола = надеть чёрную текстуру 
                    Cards[i].shape.setFillColor(sf::Color::Black);
                }

            }

            if (count_discarded_cards == count_all_cards) // если все карты сброшены
            {
                full_table(Cards);// заполнить стол новыми картами
                for (int i = 0; i < count_all_cards; i++)
                {
                    Cards[i].timer = 0;// сброс таймера у всех карт
                }

            }
        }

        int i;
        window.clear();// очистить окно
        for (i = 0; i < (count_all_cards / 2); i++)
        {
            Cards[i].shape.setPosition({ 100.f + i * 400, 100.f });//задаём позиции картам в 1 ряду
            Cards[i].draw(window);
        }

        points_txt.setPosition({ 100.f + i * 400, 100.f });
        points_txt.setString(std::to_string(points)); //задаём позицию очкам справа от 1 ряда
        window.draw(points_txt);

        for (i = count_all_cards / 2; i < count_all_cards; i++)
        {
            Cards[i].shape.setPosition({ 100.f + (i - count_all_cards / 2) * 400, 600.f });//задаём позиции картам в 2 ряду
            Cards[i].draw(window);
        }

        timer_txt.setPosition({ 100.f + (i - count_all_cards / 2) * 400, 600.f });//задаём позицию таймеру справа от 2 ряда
        timer_txt.setString(std::format("{:.2f}", 60 - gameTime));
        window.draw(timer_txt);


        window.display();//выводим в окно всё что нарисовали

        if (gameTime >= 60.f)//если игра длится дольше, закрываем окно
        {
            window.close();

        }
    }
}