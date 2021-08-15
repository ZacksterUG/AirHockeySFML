#include <iostream>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

constexpr auto windowSizeX = 1920;
constexpr auto windowSizeY = 1080;
int currentWindowSizeX;
int currentWindowSizeY;
constexpr float barSizeX = 56;
constexpr float barSizeY = 331;
constexpr float movingBarSpeed = 20;
float winScaleX = currentWindowSizeX * 1.0f / windowSizeX;
float winScaleY = currentWindowSizeY * 1.0f / windowSizeY;
float Time;
float startPosX1;
float startPosY1;
float startPosX2;
float startPosY2;
float startBallPosX;
float startBallPosY;
int justScored;
int score1;
int score2;
float musicVolume;
float generalVolume;
class Bar2;
class Bar1;
class movingBall;
class customMouse;
class messageBox;
bool leftClickOld = false;
bool leftClickPressed = false;
bool gamePaused = false;
bool gameJustStarted = false;
bool gameFinished = false;
void startGame(Bar1& Bar1, Bar2& Bar2, movingBall& Ball, sf::RenderWindow &window);
enum buttonTypes
{
    Left,
    Right,
    Middle,
    None
};

buttonTypes oldMouseButton;
buttonTypes currentMouseButton;

inline bool dotInSquare(float X, float Y, float x1, float y1, float x2, float y2, float &sX, float &sY)
{
    if (X >= x1 && X <= x2)
    {
        if (Y >= y1 && Y <= y2)
        {
            sX = X;
            sY = Y;
            return true;
        }
        return false;
    }
    return false;
}

class movingBall
{
private:
    enum collisionType
    {
        Left,
        LeftUp,
        LeftDown,
        Right,
        RightUp,
        RightDown,
        Up,
        Down,
    };
    sf::Texture texture;
    sf::CircleShape Ball;
    float posX;
    float posY;
    float radius;
    float colX;
    int tick;
    static int tickUpdate;
    float colY;
    float velX;
    float velY;
    float angle;
    float angularVelocity;
    int soundTick;
    float currentSpeed;
    sf::SoundBuffer buffer;
    sf::Sound sound;
    sf::Color clr;
public:
    friend void checkCollision(movingBall& Ball, Bar1& bar);
    movingBall(float radius, float posX, float posY, sf::Color clr)
    {
        this->soundTick = 5;
        this->tick = tickUpdate;
        this->clr = clr;
        this->currentSpeed = 0;
        this->radius = radius;
        this->posX = posX;
        this->posY = posY;
        this->velX = 0;
        this->velY = 0;
        this->angle = 0;
        this->angularVelocity = 1;
        this->colX = winScaleX * radius * 2;
        this->colY = winScaleY * radius * 2;
        this->Ball.setScale(winScaleX, winScaleY);
        this->Ball.setRadius(this->radius);
        this->texture.loadFromFile("image/ball.png");
        this->Ball.setTexture(&texture);
        this->Ball.setPosition(sf::Vector2f(this->posX, this->posY));
        this->buffer.loadFromFile("sound/ballwallhit.wav");
        this->sound.setBuffer(buffer);
        this->sound.setVolume(generalVolume * 100);
    }
    void draw(sf::RenderWindow &window)
    {
        this->update();
        sf::Vector2f Vec(posX, posY);
        //std::cout << this->posX << " " << this->posY << std::endl;
        Ball.setPosition(Vec);
        window.draw(Ball);
    }
    void setVelocity(float velX, float velY)
    {
        this->velX = velX;
        this->velY = velY;
    }
    void getVelocity(float& velX, float& velY)
    {
        velX = this->velX;
        velY = this->velY;
    }
    void setPos(float posX, float posY)
    {
        this->posX = posX;
        this->posY = posY;
    }
    void getPos(float& posX, float& posY)
    {
        posX = this->posX;
        posY = this->posY;
    }
    void update()
    {
        if (this->soundTick != 0) this->soundTick--;
        this->currentSpeed = sqrtf(powf(this->velX, 2) + powf(this->velY, 2));
        if (this->tick == 0) this->tick = tickUpdate;
        if (gamePaused == true) return;
        if (this->posX <= 0 && this->velX < 0)
        {
            score2++;
            justScored = 2;
            this->velX *= -1; 
            this->sound.play(); 
        }
        if (this->posY <= 0 && this->velY > 0) { this->velY *= -1; this->sound.play(); }
        if (this->posX >= winScaleX * (windowSizeX - colX/winScaleX) && this->velX > 0) 
        {
            score1++;
            justScored = 1;
            this->velX *= -1; 
            this->sound.play();
        }
        if (this->posY >= winScaleY * (windowSizeY - colY/ winScaleY) && this->velY < 0) {this->velY *= -1; this->sound.play();}
        this->posX += this->velX;
        this->posY += -this->velY;
        float oldAngle = this->angle;
        this->angle += this->angularVelocity;
        //this->Ball.setRotation(this->angle);
        if (fabs(this->velX) > 0 || fabs(this->velY) > 0)
        {
            if (this->tick == tickUpdate)
            {
                this->velX /= 1.015f;
                this->velY /= 1.015f;
            }
            if (this->currentSpeed <= 0.2)
            {
                this->velX = 0;
                this->velY = 0;
            }
        }
        //std::cout << this->currentSpeed << std::endl;
        this->tick--;
    }
};

int movingBall::tickUpdate = 1;
class Bar2;
class Bar1
{
protected:
    float posX;
    float posY;
    float colX;
    float colY;
    sf::Texture barTexture;
    sf::Sprite barSprite;
    enum class moveDirection
    {
        Left,
        Right,
        Up,
        Down,
        None,
    };
    moveDirection X;
    moveDirection Y;
public:
    friend void checkCollision(movingBall& Ball, Bar1& bar);
    enum class color
    {
        Red,
        Blue,
        Yellow,
        Green,
        Purple,
        Cyan,
        Pink
    };
    Bar1(color cl, float posX, float posY)
    {
        switch (cl)
        {
        case color::Red:
            barTexture.loadFromFile("image/bar-red.png");
            break;
        case color::Blue:
            barTexture.loadFromFile("image/bar-blue.png");
            break;
        case color::Yellow:
            barTexture.loadFromFile("image/bar-yellow.png");
            break;
        case color::Green:
            barTexture.loadFromFile("image/bar-green.png");
            break;
        case color::Purple:
            barTexture.loadFromFile("image/bar-purple.png");
            break;
        case color::Cyan:
            barTexture.loadFromFile("image/bar-cyan.png");
            break;
        case color::Pink:
            barTexture.loadFromFile("image/bar-pink.png");
            break;
        }
        X = moveDirection::None;
        Y = moveDirection::None;
        barSprite.setTexture(barTexture);
        barSprite.setScale(winScaleX, winScaleY);
        barSprite.setPosition(sf::Vector2f(posX, posY));
        this->posX = posX;
        this->posY = posY;
        this->colX = barSizeX * (winScaleX);
        this->colY = barSizeY * (winScaleY);
    }
    
    void setPos(float posX, float posY)
    {
        this->posX = posX;
        this->posY = posY;
    }

    virtual void updateBar()
    {
        if (gamePaused == false)
        {
            this->onKeyChange();
            if (this->posX <= 0) this->posX = 0;
            if (this->posY <= 0) this->posY = 0;
            if (this->posX >= winScaleX * ((windowSizeX - barSizeX) / 2 - 35)) this->posX = winScaleX * ((windowSizeX - barSizeX) / 2 - 35);
            if (this->posY >= winScaleY * (windowSizeY - barSizeY)) this->posY = winScaleY * (windowSizeY - barSizeY);
        }
        barSprite.setPosition(sf::Vector2f(this->posX, this->posY));
    }
    virtual void onKeyChange()
    {
        X = moveDirection::None;
        Y = moveDirection::None;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { X = moveDirection::Left; this->posX -= winScaleX * movingBarSpeed; }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { X = moveDirection::Right; this->posX += winScaleX * movingBarSpeed; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { Y = moveDirection::Up; this->posY -= winScaleY * movingBarSpeed; }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { Y = moveDirection::Down; this->posY += winScaleY * movingBarSpeed; }
    }
    void drawBar(sf::RenderWindow& window)
    {
        this->updateBar();
        window.draw(barSprite);
    }
};

class Bar2 : public Bar1
{
public:
    Bar2(color cl, float posX, float posY) : Bar1(cl, posX, posY)
    {

    }
    void onKeyChange() override
    {
        X = moveDirection::None;
        Y = moveDirection::None;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { X = moveDirection::Left; this->posX -= winScaleX * movingBarSpeed; }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { X = moveDirection::Right; this->posX += winScaleX * movingBarSpeed; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { Y = moveDirection::Up; this->posY -= winScaleY * movingBarSpeed; }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { Y = moveDirection::Down; this->posY += winScaleY * movingBarSpeed; }
    }
    void updateBar() override
    {
        if (gamePaused == false)
        {
            this->onKeyChange();
            if (this->posX <= 0) this->posX = 0;
            if (this->posY <= 0) this->posY = 0;
            if (this->posX <= winScaleX * ((windowSizeX - barSizeX) / 2 + 30)) this->posX = winScaleX * ((windowSizeX - barSizeX) / 2 + 30);
            if (this->posX >= winScaleX * (windowSizeX - barSizeX)) this->posX = winScaleX * (windowSizeX - barSizeX);
            if (this->posY >= winScaleY * (windowSizeY - barSizeY)) this->posY = winScaleY * (windowSizeY - barSizeY);
        }
        barSprite.setPosition(sf::Vector2f(this->posX, this->posY));
    }
};

void checkCollision(movingBall& Ball, Bar1& bar);

class customButton
{
private:
    float posX;
    float posY;
    float sizeX;
    float sizeY;
    float scaleX;
    float scaleY;
    std::string text;
    // sf::Text sfText;
    sf::Texture buttonTexture;
    sf::Sprite buttonSprite;
    sf::SoundBuffer buffer;
    sf::Sound sound;
    bool active;
    bool leftPressed;
    bool leftReleased;
    bool justClicked;
    bool soundPlayed;
    int tickBetweenClicks;
public:
    bool pushable;
    enum class alignment
    {
        Left,
        Right,
        Center
    };
    enum class Event
    {
        None,
        OnMouse,
        OnClicked
    };
    alignment align;
    Event Event;
    customButton(std::string text, float posX, float posY, alignment align, float scaleX = 1, float scaleY = 1)
    {
        this->Event = Event::None;
        this->leftPressed = false;
        this->leftReleased = false;
        this->justClicked = false;
        this->pushable = true;
        this->align = align;
        this->text = text;
        this->buttonTexture.loadFromFile("image/button.png");
        this->buttonSprite.setTexture(buttonTexture);
        this->posX = posX;
        this->posY = posY;
        this->sizeX = 530 * winScaleX *scaleX;
        this->sizeY = 160 * winScaleY * scaleY;
        this->buttonSprite.setScale(winScaleX* scaleX, winScaleY* scaleY);
        this->soundPlayed = false;
        //this->sfText.setPosition(winScaleX * (posX + 530) / 2, winScaleY * (posY + 160) / 2);
        this->buttonSprite.setPosition(this->posX * winScaleX, this->posY * winScaleY);
        this->active = false;
        this->tickBetweenClicks = 0;
        this->buffer.loadFromFile("sound/buttonclick.wav");
        this->sound.setBuffer(this->buffer);
        this->sound.setVolume(100 * generalVolume);
        this->scaleX = scaleX;
        this->scaleY = scaleY;
    }
    void hide()
    {
        this->active = false;
    }
    void activate()
    {
        this->active = true;
    }
    bool draw(sf::RenderWindow& window)
    {
        if (this->active == true)
        {
            this->update();
            sf::Font buttFont;
            buttFont.loadFromFile("menufont.ttf");

            sf::Text buttText(this->text, buttFont, 30);
            buttText.setScale(winScaleX*this->scaleX, winScaleY*this->scaleY);
            int count = this->text.size();
            if (this->align == alignment::Center) buttText.setPosition(winScaleX * (this->posX + this->sizeX / 2 / winScaleX - count * 7* this->scaleX - 10), winScaleY * (this->posY + this->sizeY / 2 / winScaleY - 15));
            else if (this->align == alignment::Left) buttText.setPosition(winScaleX * (this->posX + this->sizeX / 2 / winScaleX), winScaleY * (this->posY + this->sizeY / 2 / winScaleY - 15));
            else if (this->align == alignment::Right)
            {
                sf::FloatRect recSize;
                recSize = buttText.getGlobalBounds();
                float X = recSize.width;
                buttText.setPosition(winScaleX * (this->posX - X), winScaleY * (this->posY + this->sizeY / 2 / winScaleY - 15));
            }
            window.draw(buttonSprite);
            window.draw(buttText);
            if (justClicked == true)
            {
                justClicked = false;
                return true;
            }
            return false;
        }
        return false;
    }
    void update()
    {
        if (tickBetweenClicks != 0) tickBetweenClicks--;
        if (this->active == true)
        {
            sf::Vector2i mousePos;
            mousePos = sf::Mouse::getPosition();
            if (this->pushable == true)
            {
                if (mousePos.x >= this->posX * winScaleX && mousePos.x <= winScaleX*(this->posX + this->sizeX / winScaleX) && mousePos.y >= this->posY * winScaleY && mousePos.y <= winScaleY * (this->posY + this->sizeY / winScaleY))
                {
                    if (Event != Event::OnClicked && Event != Event::OnMouse)
                    {
                        //std::cout << "On Mouse" << std::endl;
                        this->Event = Event::OnMouse;
                        this->buttonTexture.loadFromFile("image/activebutton.png");
                        this->buttonSprite.setTexture(buttonTexture);
                    }
                    else if (Event == Event::OnMouse)
                    {
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && tickBetweenClicks == 0)
                        {
                            if (this->soundPlayed == false)
                            {
                                this->soundPlayed = true;
                                this->sound.play();
                            }
                            tickBetweenClicks = 5;
                            this->Event = Event::OnClicked;
                            this->buttonTexture.loadFromFile("image/buttonpressed.png");
                            this->buttonSprite.setTexture(buttonTexture);
                            return;
                        }
                    }
                    else if (Event == Event::OnClicked)
                    {
                        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        {
                            this->justClicked = true;
                            this->soundPlayed = false;
                            this->Event = Event::OnMouse;
                            this->buttonTexture.loadFromFile("image/activebutton.png");
                            this->buttonSprite.setTexture(buttonTexture);
                        }
                    }
                }
                else
                {
                    if (Event != Event::None)
                    {
                        Event = Event::None;
                        this->buttonTexture.loadFromFile("image/button.png");
                        this->buttonSprite.setTexture(buttonTexture);
                    }
                }
            }
            else
            {
                if (Event != Event::None)
                {
                    Event = Event::None;
                    this->buttonTexture.loadFromFile("image/button.png");
                    this->buttonSprite.setTexture(buttonTexture);
                }
            }
        }
    }
};

class messageBox
{
private:
    float posX;
    float posY;
    float sizeX;
    float sizeY;
    std::string messageString;
    sf::Texture messageBoxTexture;
    sf::Sprite messageBoxSprite;
    bool active;
public:
    sf::Text textString;
    enum class alignment
    {
        Left,
        Right,
        Center,
    };
    alignment align;
    messageBox(float posX, float posY, float sizeX, float sizeY, alignment align)
    {
        this->posX = posX;
        this->posY = posY;
        this->sizeX = sizeX;
        this->sizeY = sizeY;
        this->active = false;
        messageBoxTexture.loadFromFile("image/messagebox.png");
        messageBoxSprite.setTexture(messageBoxTexture);
        messageBoxSprite.setPosition(this->posX*winScaleX, this->posY*winScaleY);
        messageBoxSprite.setScale(winScaleX * this->sizeX, winScaleY * this->sizeY);
        this->align = align;
        this->textString.setPosition(this->posX + 10 * winScaleX, this->posY + 10 * winScaleY);
    }
    void setString(std::string str)
    {
        messageString = str;
        textString.setString(messageString);
    }
    bool isActive()
    {
        return this->active;
    }
    void activate(bool boolean)
    {
        this->active = boolean;
    }
    void show(sf::RenderWindow& window)
    {
        if (this->active == true)
        {
            window.draw(this->messageBoxSprite);
            //window.draw(this->textString);
        }
    }
};

int main()
{
    leftClickOld = false;
    leftClickPressed = false;
    gamePaused = false;
    gameJustStarted = false;
    gameFinished = false;
    justScored = 0;
    int intTickBeforeStart = 50;
    currentWindowSizeX = 1920;
    currentWindowSizeY = 1080;
    winScaleX = currentWindowSizeX * 1.0f / windowSizeX;
    winScaleY = currentWindowSizeY * 1.0f / windowSizeY;
    musicVolume = 1.0f;
    generalVolume = 1.0f;
    sf::Image icon;
    icon.loadFromFile("icon.png");
    int GM = 0;
    sf::RenderWindow window(sf::VideoMode(currentWindowSizeX, currentWindowSizeY), "Testing", sf::Style::Fullscreen);
    window.setIcon(32,32,icon.getPixelsPtr());
    sf::Music mainMenuMusic;
    mainMenuMusic.setVolume(musicVolume * generalVolume * 50);
    mainMenuMusic.openFromFile("sound/mainmusic.wav");
    srand((unsigned int)time(NULL));
    sf::Texture backGround;
    bool musicPlaying = false;
    backGround.loadFromFile("image/background.jpg");
    sf::Sprite spriteBG;
    sf::Texture mainMenuTexture;
    sf::Sprite mainMenuBG;
    mainMenuTexture.loadFromFile("image/mainmenu.png");
    mainMenuBG.setTexture(mainMenuTexture);
    mainMenuBG.setScale(winScaleX, winScaleY);
    spriteBG.setTexture(backGround);
    spriteBG.setScale(winScaleX, winScaleY);
    spriteBG.setPosition(0, 0);
    sf::Texture cursorTX;
    cursorTX.loadFromFile("image/cursor.png");
    sf::Sprite cursorSprite;
    startPosX1 = 0;
    startPosY1 = winScaleY * (windowSizeY / 2.0f - 168);
    startPosX2 = winScaleX * (windowSizeX - barSizeX);
    startPosY2 = winScaleY * (windowSizeY / 2.0f - 168);
    startBallPosX = winScaleX * (windowSizeX / 2.0f - 50);
    startBallPosY = winScaleY * (windowSizeY / 2.0f - 50);
    cursorSprite.setTexture(cursorTX);
    cursorSprite.setScale(0.5f * winScaleX, 0.5f * winScaleY);
    movingBall BALL(50, startBallPosX, startBallPosY, sf::Color::Green);
    Bar1 bar1(Bar1::color::Red, startPosX1, startPosY1);
    Bar2 bar2(Bar2::color::Cyan, startPosX2, startPosY2);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);
    sf::Vector2i mousePos;
    customButton playButton("Play", windowSizeX / 2.0f - 280 , windowSizeY / 2.0f - 50, customButton::alignment::Center);
    customButton settingsButton("Settings", windowSizeX / 2.0f - 280, windowSizeY / 2.0f+125, customButton::alignment::Center);
    customButton exitButton("Exit", windowSizeX / 2.0f - 280, windowSizeY / 2.0f + 125 + 175, customButton::alignment::Center);
    settingsButton.activate();
    playButton.activate();
    exitButton.activate();
    int nextSec;
    int tick = 0;
    int playSeconds = 0;
    sf::Texture scoreTexture;
    sf::Sprite scoreSprite;
    sf::Font scoreTimeTextFont;
    sf::Font countDownFont;
    countDownFont.loadFromFile("NFS.ttf");
    scoreTimeTextFont.loadFromFile("anvyl.ttf");
    sf::Text scoreTimeText("0:00", scoreTimeTextFont, 30);
    scoreTimeText.setScale(winScaleX, winScaleY);
    scoreTimeText.setPosition(winScaleX * 935, winScaleY * 15);
    scoreTexture.loadFromFile("image/scoreboard.png");
    scoreSprite.setTexture(scoreTexture);
    scoreSprite.setScale(winScaleX * 0.7f, winScaleY * 0.7f);
    scoreSprite.setPosition(winScaleX * 655, 0);

    sf::Text PlayerOneText("Player 1", scoreTimeTextFont, 30);
    sf::Text PlayerTwoText("Player 2", scoreTimeTextFont, 30);
    PlayerOneText.setScale(winScaleX*0.9f, winScaleY*0.9f);
    PlayerTwoText.setScale(winScaleX*0.9f, winScaleY*0.9f);
    PlayerOneText.setPosition(winScaleX*790, winScaleY * 20);
    PlayerTwoText.setPosition(winScaleX*1030, winScaleY * 20);

    sf::Text ScoreText1("0", scoreTimeTextFont, 30);
    sf::Text ScoreText2("0", scoreTimeTextFont, 30);
    ScoreText1.setScale(winScaleX, winScaleY);
    ScoreText2.setScale(winScaleX, winScaleY);
    ScoreText1.setPosition(winScaleX*725, winScaleY*15);
    ScoreText2.setPosition(winScaleX*1175, winScaleY*15);
    std::string scoreTextString1 = "0";
    std::string scoreTextString2 = "0";

    int countDown = 4;
    sf::Text countDownText;
    sf::SoundBuffer getReadyBuf;
    getReadyBuf.loadFromFile("sound/getready.wav");
    sf::Sound getReadySound(getReadyBuf);
    getReadySound.setVolume(100 * generalVolume);
    countDownText.setFont(countDownFont);
    countDownText.setFillColor(sf::Color::Color(255, 196, 0));
    countDownText.setOutlineThickness(5);
    countDownText.setOutlineColor(sf::Color::Color(130, 13, 13));
    countDownText.setPosition(currentWindowSizeX / 2.0f - 400 * winScaleX, currentWindowSizeX / 2.0f - 100 * winScaleY);
    countDownText.setCharacterSize(150);
    countDownText.setScale(winScaleX, winScaleY);
    sf::SoundBuffer goalBuffer;
    goalBuffer.loadFromFile("sound/goal.wav");
    sf::Sound goalSound(goalBuffer);
    goalSound.setVolume(100 * generalVolume);
    sf::SoundBuffer bufSS;
    bufSS.loadFromFile("sound/sound2.wav");
    sf::Sound soundGo(bufSS);
    soundGo.setVolume(100 * generalVolume);

    messageBox exitBoxMessage(700, 400, 2.75, 1,messageBox::alignment::Center);
    sf::Font boxTextFont;
    boxTextFont.loadFromFile("messagefont.ttf");
    sf::Text exitBoxMessageText("Are you sure that you want to exit?", boxTextFont, 30);
    exitBoxMessageText.setScale(winScaleX, winScaleY);
    sf::Vector2f posText = exitBoxMessage.textString.getPosition();
    exitBoxMessageText.setPosition(posText.x*winScaleX + 13*winScaleX, posText.y*winScaleY);
    customButton buttonNoExitBox("No",730,500,customButton::alignment::Center, 0.5, 0.5);
    customButton buttonYesExitBox("Yes", 970, 500, customButton::alignment::Center, 0.5, 0.5);

    sf::Music gameMusic;
    gameMusic.openFromFile("sound/musicingame.wav");
    gameMusic.setLoop(true);
    gameMusic.setVolume(musicVolume* generalVolume * 50);

    messageBox pauseBox(810, 300, 1.5f, 2, messageBox::alignment::Center);
    sf::Text pauseBoxText("PAUSE", boxTextFont, 30);
    pauseBoxText.setScale(winScaleX, winScaleY);
    posText = pauseBox.textString.getPosition();
    pauseBoxText.setPosition(winScaleX*posText.x + winScaleX * 100, posText.y* winScaleY);
    customButton resumePauseButton("Resume", 815, 350, customButton::alignment::Center, 0.55f, 0.55f);
    customButton settingsPauseButton("Settings", 815, 450, customButton::alignment::Center, 0.55f, 0.55f);
    customButton exitPauseButton("Exit", 815, 550, customButton::alignment::Center, 0.55f, 0.55f);
    int pauseTick = 20;

    sf::SoundBuffer gameOverBuf;
    gameOverBuf.loadFromFile("sound/gameover.wav");
    sf::Sound gameOverSound(gameOverBuf);
    messageBox gameOverBox(560,150,4,4,messageBox::alignment::Center);
    std::string gameOverString[5] = {"Game Over!",
                                     "Player",
                                     "Time: ",
                                     "Player 1 score: ",
                                     "Player 2 score: "};
    sf::Text gameOverText[5];
    for (int i = 0; i < 5; i++)
    {
        gameOverText[i].setFont(boxTextFont);
        gameOverText[i].setScale(winScaleX, winScaleY);
        gameOverText[i].setString(gameOverString[i]);
    }
    gameOverText[0].setFillColor(sf::Color(255, 54, 33,255));
    gameOverText[0].setOutlineThickness(4);
    gameOverText[0].setOutlineColor(sf::Color(47, 127, 189, 255));
    gameOverText[0].setPosition(winScaleX*760,winScaleY*230);
    gameOverText[0].setCharacterSize(80);
    gameOverText[1].setFillColor(sf::Color(30, 255, 96, 255));
    gameOverText[1].setCharacterSize(50);
    gameOverText[1].setPosition(winScaleX*800, winScaleY*350);
    for (int i = 2; i < 5; i++)
    {
        gameOverText[i].setFillColor(sf::Color(0, 161, 158, 255));
        gameOverText[i].setCharacterSize(40);
        gameOverText[i].setPosition(winScaleX * 600, winScaleY * (350 + 60 * i));
    }
    customButton playAgainGameOverButton("PLAY AGAIN",695,700,customButton::alignment::Center, 1, 0.5);
    customButton exitGameOverButton("Exit", 695, 800, customButton::alignment::Center, 1, 0.5);

    while (window.isOpen())
    {
        //mouse.update();
        if (intTickBeforeStart != 0)
        {
            intTickBeforeStart--;
            continue;
        }
        if (tick > 0) tick--;
        if (musicPlaying == false && GM == 0)
        {
            mainMenuMusic.play();
            mainMenuMusic.setLoop(true);
            musicPlaying = true;
        }
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                mainMenuMusic.stop();
                window.clear();
                window.close();
            }
            if (event.type == sf::Event::Resized)
            {
                std::cout << "Resized" << std::endl;
            }
            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    leftClickOld = true;
                    //std::cout << "Left Button Released" << std::endl;
                }
                else leftClickOld = false;
            }
            else if(event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    //std::cout << "Left Button Pressed" << std::endl;
                    leftClickOld = false;
                    leftClickPressed = true;
                }
                else
                {
                    leftClickOld = false;
                    leftClickPressed = false;
                }
            }
        }
        window.clear();
        mousePos = sf::Mouse::getPosition();
        cursorSprite.setPosition((sf::Vector2f)mousePos);
        if (GM == 0)
        {
            window.draw(mainMenuBG);
            if (playButton.draw(window))
            {
                mainMenuMusic.stop();
                gameJustStarted = true;
                startGame(bar1, bar2, BALL, window);
                countDown = 4;
                BALL.setVelocity(0, 0);
                GM = 1;
                playSeconds = 0;
                gamePaused = true;
                nextSec = (int)clock() + 1000;
                scoreTimeText.setString("0:00");
                std::cout << "Play Button Just clicked!" << std::endl;
            }
            if (exitButton.draw(window))
            {
                exitBoxMessage.activate(true);
                playButton.pushable = false;
                exitButton.pushable = false;
                settingsButton.pushable = false;
                buttonNoExitBox.activate();
                buttonYesExitBox.activate();
                std::cout << "Exit Button Just clicked!" << std::endl;

            }
            if (settingsButton.draw(window))
            {
                std::cout << "Settings Button Just clicked!" << std::endl;
            }
        }
        if (GM == 1)
        {
            if (pauseTick != 0) pauseTick--;
            if (justScored != 0)
            {
                BALL.setVelocity(0, 0);
                if (justScored == 2) BALL.setPos(winScaleX * 200, startBallPosY);
                else if (justScored == 1) BALL.setPos(winScaleX * 1600, startBallPosY);
                bar1.setPos(startPosX1, startPosY1);
                bar2.setPos(startPosX2, startPosY2);
                goalSound.play();
                if (score1 == 20 || score2 == 20)
                {
                    gamePaused = true;
                    gameFinished = true;
                    gameOverBox.activate(true);
                    gameMusic.stop();
                    gameOverSound.play();
                    if(score1 == 20) gameOverText[1].setString("Player 1 Wins!");
                    else gameOverText[1].setString("Player 2 Wins!");
                    int minutes = playSeconds / 60;
                    if (playSeconds % 60 >= 10) gameOverText[2].setString("TIME: "+std::to_string(minutes) + ':' + std::to_string(playSeconds % 60));
                    else gameOverText[2].setString("TIME: " +std::to_string(minutes) + ':' + '0' + std::to_string(playSeconds % 60));
                    gameOverText[3].setString("Player 1 score: " + std::to_string(score1));
                    gameOverText[4].setString("Player 2 score: " + std::to_string(score2));
                    playAgainGameOverButton.activate();
                    exitGameOverButton.activate();
                }
                justScored = 0;
            }
            window.draw(spriteBG);
            bar1.drawBar(window);
            bar2.drawBar(window);
            sf::Vector2i Vec;
            BALL.draw(window);
            checkCollision(BALL, bar1);
            checkCollision(BALL, bar2);
            window.draw(scoreSprite);
            window.draw(scoreTimeText);
            window.draw(PlayerOneText);
            window.draw(PlayerTwoText);
            if (std::stoi(scoreTextString1) != score1)
            {
                scoreTextString1 = std::to_string(score1);
                ScoreText1.setString(std::to_string(score1));
            }
            if (std::stoi(scoreTextString2) != score2)
            {
                scoreTextString2 = std::to_string(score2);
                ScoreText2.setString(std::to_string(score2));
            }
            window.draw(ScoreText1);
            window.draw(ScoreText2);
            if (clock() >= nextSec && gamePaused == false)
            {
                nextSec = (int)clock() + 1000;
                playSeconds++;
                int minutes = playSeconds / 60;
                if (playSeconds % 60 >= 10) scoreTimeText.setString(std::to_string(minutes)+':'+std::to_string(playSeconds%60));
                else scoreTimeText.setString(std::to_string(minutes) + ':' + '0'+std::to_string(playSeconds % 60));
            }
            if (gameJustStarted == true)
            {
                if (countDown == 4)
                {
                    countDown--;
                    nextSec = (int)clock() + 1000;
                    countDownText.setPosition(currentWindowSizeX / 2.0f - 400 * winScaleX, currentWindowSizeY / 2.0f - 100 * winScaleY);
                    countDownText.setString("GET READY!");
                    getReadySound.play();
                }
                else if (countDown >= 1 && clock() >= nextSec)
                {
                    nextSec = (int)clock() + 1000;
                    countDownText.setString(std::to_string(countDown));
                    countDownText.setPosition(currentWindowSizeX / 2.0f - 75 * winScaleX, currentWindowSizeY / 2.0f - 100 * winScaleY);
                    getReadySound.play();
                    countDown--;
                }
                else if (countDown == 0 && clock() >= nextSec)
                {
                    nextSec = (int)clock() + 1000;
                    soundGo.play();
                    countDownText.setString("GO");
                    gamePaused = false;
                    gameJustStarted = false;
                    gameMusic.play();
                }
                window.draw(countDownText);
            }
            if (gameJustStarted == false)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    if (pauseTick == 0)
                    {
                        pauseTick = 20;
                        if (gamePaused == false)
                        {
                            gamePaused = true;
                            pauseBox.activate(true);
                            if (gameFinished == false)
                            {
                                gameMusic.pause();
                                resumePauseButton.activate();
                                settingsPauseButton.activate();
                                exitPauseButton.activate();
                            }
                        }
                        else
                        {
                            if (gameFinished == false)
                            {
                                resumePauseButton.hide();
                                settingsPauseButton.hide();
                                exitPauseButton.hide();
                                pauseBox.activate(false);
                                gamePaused = false;
                                gameMusic.play();
                            }
                        }
                    }
                }
                if (gamePaused && gameFinished == false)
                {
                    pauseBox.show(window);
                    window.draw(pauseBoxText);
                    if (resumePauseButton.draw(window))
                    {
                        resumePauseButton.hide();
                        settingsPauseButton.hide();
                        exitPauseButton.hide();
                        pauseBox.activate(false);
                        gamePaused = false;
                        gameMusic.play();
                    }
                    if (exitPauseButton.draw(window))
                    {
                        mainMenuMusic.play();
                        playSeconds = 0;
                        gameMusic.stop();
                        GM = 0;
                        gamePaused = false;
                    }
                    settingsPauseButton.draw(window);
                }
            }
            if (gameFinished == true)
            {
                gameOverBox.show(window);
                for (int i = 0; i < 5; i++) window.draw(gameOverText[i]);
                if (playAgainGameOverButton.draw(window))
                {
                    gameOverBox.activate(false);
                    exitGameOverButton.hide();
                    playAgainGameOverButton.hide();
                    gameJustStarted = true;
                    startGame(bar1, bar2, BALL, window);
                    countDown = 4;
                    BALL.setVelocity(0, 0);
                    GM = 1;
                    playSeconds = 0;
                    gamePaused = true;
                    nextSec = (int)clock() + 1000;
                    scoreTimeText.setString("0:00");
                    gameFinished = false;
                }
                if (exitGameOverButton.draw(window))
                {
                    gameOverBox.activate(false);
                    exitGameOverButton.hide();
                    playAgainGameOverButton.hide();
                    gameFinished = false;
                    gamePaused = false;
                    GM = 0;
                    mainMenuMusic.play();
                }
            }
        }
        if (exitBoxMessage.isActive())
        {
            exitBoxMessage.show(window);
            window.draw(exitBoxMessageText);
            if (buttonNoExitBox.draw(window))
            {
                exitBoxMessage.activate(false);
                playButton.pushable = true;
                exitButton.pushable = true;
                settingsButton.pushable = true;
                buttonNoExitBox.hide();
                buttonYesExitBox.hide();
            }
            if (buttonYesExitBox.draw(window))
            {
                window.clear();
                window.close();
            }
        }
        if(GM == 0 || (gamePaused == true && gameJustStarted == false)) window.draw(cursorSprite);
        window.display();
    }
    return 0;
}

void checkCollision(movingBall& Ball, Bar1& bar)
{
    if (gamePaused == true) return;
    float workX = -1, workY = -1;
    float bufX1, bufX2, bufY1, bufY2;
    if (bar.X != Bar1::moveDirection::None || bar.Y != Bar1::moveDirection::None)
    {
        if (dotInSquare(Ball.posX + Ball.velX, Ball.posY + Ball.velY, bar.posX, bar.posY, bar.posX + bar.colX, bar.posY + bar.colY, workX, workY) ||
            dotInSquare(Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.velY, bar.posX, bar.posY, bar.posX + bar.colX, bar.posY + bar.colY, workX, workY) ||
            dotInSquare(Ball.posX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, bar.posX, bar.posY, bar.posX + bar.colX, bar.posY + bar.colY, workX, workY) ||
            dotInSquare(Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, bar.posX, bar.posY, bar.posX + bar.colX, bar.posY + bar.colY, workX, workY) ||
            dotInSquare(bar.posX, bar.posY, Ball.posX + Ball.velX, Ball.posY + Ball.velY, Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, workX, workY) ||
            dotInSquare(bar.posX + bar.colX, bar.posY, Ball.posX + Ball.velX, Ball.posY + Ball.velY, Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, workX, workY) ||
            dotInSquare(bar.posX, bar.posY + bar.colY, Ball.posX + Ball.velX, Ball.posY + Ball.velY, Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, workX, workY) ||
            dotInSquare(bar.posX + bar.colX, bar.posY + bar.colY, Ball.posX + Ball.velX, Ball.posY + Ball.velY, Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, workX, workY))
        {
            if (bar.X == Bar1::moveDirection::Left) Ball.velX = -winScaleX * (movingBarSpeed + 10);
            else if (bar.X == Bar1::moveDirection::Right) Ball.velX =  winScaleX * (movingBarSpeed + 10);
            if (bar.Y == Bar1::moveDirection::Up) Ball.velY = winScaleY * (movingBarSpeed + 10);
            else if (bar.Y == Bar1::moveDirection::Down) Ball.velY = -winScaleY * (movingBarSpeed + 10);
            if (Ball.soundTick == 0)
            {
                Ball.sound.play();
                Ball.soundTick = 10;
            }
        }

    }
    else
    {
        float workX = -1, workY = -1;
        if (dotInSquare(Ball.posX + Ball.velX, Ball.posY + Ball.velY, bar.posX, bar.posY, bar.posX + bar.colX, bar.posY + bar.colY, workX, workY)) goto Function1;
        if (dotInSquare(Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.velY, bar.posX, bar.posY, bar.posX + bar.colX, bar.posY + bar.colY, workX, workY)) goto Function1;
        if (dotInSquare(Ball.posX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, bar.posX, bar.posY, bar.posX + bar.colX, bar.posY + bar.colY, workX, workY)) goto Function1;
        if (dotInSquare(Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, bar.posX, bar.posY, bar.posX + bar.colX, bar.posY + bar.colY, workX, workY)) goto Function1;
        if (dotInSquare(bar.posX, bar.posY, Ball.posX + Ball.velX, Ball.posY + Ball.velY, Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, workX, workY)) goto Function2;
        if (dotInSquare(bar.posX + bar.colX, bar.posY, Ball.posX + Ball.velX, Ball.posY + Ball.velY, Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, workX, workY)) goto Function2;
        if (dotInSquare(bar.posX, bar.posY + bar.colY, Ball.posX + Ball.velX, Ball.posY + Ball.velY, Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, workX, workY)) goto Function2;
        if (dotInSquare(bar.posX + bar.colX, bar.posY + bar.colY, Ball.posX + Ball.velX, Ball.posY + Ball.velY, Ball.posX + Ball.colX + Ball.velX, Ball.posY + Ball.colY + Ball.velY, workX, workY)) goto Function2;
        return;
    Function1:
        {
            Ball.sound.play();
            std::cout << "Function1" << std::endl;
            if (Ball.velX == 0 && Ball.velY != 0)
            {
                if (Ball.velY > 0) Ball.posY = bar.posY + bar.colY;
                else Ball.posY = bar.posY - Ball.colY;
                Ball.velY *= -1;
            }
            else if (Ball.velX != 0 && Ball.velY == 0)
            {
                Ball.velX *= -1;
            }
            else if (Ball.velX != 0 && Ball.velY != 0)
            {
                std::cout << "\t Type 3" << std::endl;
                bufX1 = bar.posX;
                bufX2 = bar.posX + bar.colX;
                bufY1 = Ball.velY / Ball.velX * (bufX1 - workX) + workY;
                bufY2 = Ball.velY / Ball.velX * (bufX2 - workX) + workY;
                if (bufY1 >= bar.posY && bufY1 <= bar.posY + bar.colY && bufY2 >= bar.posY && bufY2 <= bar.posY + bar.colY)
                {
                    std::cout << "\t Type 3-1" << std::endl;
                    Ball.velX *= -1;
                }
                else if (!(bufY2 >= bar.posY && bufY2 <= bar.posY + bar.colY))
                {
                    std::cout << "\t Type 3-2" << std::endl;
                    if (Ball.velX < 0)
                    {
                        if (Ball.velY < 0)
                        {
                            std::cout << "\t Type 3-2-1" << std::endl;
                            //Ball.posY = bar.posY - Ball.colY;
                        }
                        else if (Ball.velY > 0)
                        {
                            std::cout << "\t Type 3-2-2" << std::endl;
                            //Ball.posY = bar.posY + bar.colY;
                        }
                        Ball.velY *= -1;
                    }
                    else Ball.velX *= -1;
                }
                else if (!(bufY1 >= bar.posY && bufY1 <= bar.posY + bar.colY))
                {
                    std::cout << "\t Type 3-3" << std::endl;
                    if (Ball.velX > 0)
                    {
                        if (Ball.velY < 0)
                        {
                            std::cout << "\t Type 3-3-1" << std::endl;
                            //Ball.posY = bar.posY - Ball.colY;
                        }
                        else if (Ball.velY > 0)
                        {
                            std::cout << "\t Type 3-3-2" << std::endl;
                            //Ball.posY = bar.posY + bar.colY;
                        }
                        Ball.velY *= -1;
                    }
                    else Ball.velX *= -1;
                }
                else
                {
                    if (Ball.velY > 0)
                    {
                        Ball.posY = bar.posY + bar.colY;
                    }
                    else
                    {
                        Ball.posY = bar.posY - Ball.colY;
                    }
                }
            }
            return;
        }
    Function2:
        {
            if (Ball.soundTick == 0)
            {
                Ball.sound.play();
                Ball.soundTick = 10;
            }
            std::cout << "Function2" << std::endl;
            if (Ball.velX == 0 && Ball.velY != 0)
            {
                if (Ball.velY > 0) Ball.posY = bar.posY + bar.colY;
                else Ball.posY = bar.posY - Ball.colY;
                Ball.velY *= -1;
            }
            else if (Ball.velX != 0 && Ball.velY == 0)
            {
                Ball.velX *= -1;
            }
            else if (Ball.velX != 0 && Ball.velY != 0)
            {
                std::cout << "\t Type 3-1" << std::endl;
                bufX1 = Ball.posX;
                bufX2 = Ball.posX + Ball.colX;
                bufY1 = Ball.velY / Ball.velX * (bufX1 - workX) + workY;
                bufY2 = Ball.velY / Ball.velX * (bufX2 - workX) + workY;
                if (bufY1 >= Ball.posY && bufY1 <= Ball.posY + Ball.colY && bufY2 >= Ball.posY && bufY2 <= Ball.posY + Ball.colY)
                {
                    std::cout << "\t Type 3-2" << std::endl;
                    Ball.velX *= -1;
                }
                else if (!(bufY2 >= Ball.posY && bufY2 <= Ball.posY + Ball.colY))
                {
                    std::cout << "\t Type 3-3" << std::endl;
                    if (Ball.velX < 0)
                    {
                        if (Ball.velY < 0)
                        {
                            std::cout << "\t Type 3-3-1" << std::endl;
                            //Ball.posY = bar.posY - Ball.colY;
                        }
                        else if (Ball.velY > 0)
                        {
                            std::cout << "\t Type 3-3-2" << std::endl;
                            //Ball.posY = bar.posY + bar.colY;
                        }
                        Ball.velY *= -1;
                    }
                    else Ball.velX *= -1;
                }
                else if (!(bufY1 >= Ball.posY && bufY1 <= Ball.posY + Ball.colY))
                {
                    std::cout << "\t Type 4-1" << std::endl;
                    if (Ball.velX > 0)
                    {
                        if (Ball.velY < 0)
                        {
                            std::cout << "\t Type 4-1-1" << std::endl;
                            //Ball.posY = bar.posY - Ball.colY;
                        }
                        else if (Ball.velY > 0)
                        {
                            std::cout << "\t Type 4-1-2" << std::endl;
                            //Ball.posY = bar.posY + bar.colY;
                        }
                        Ball.velY *= -1;
                    }
                    else Ball.velX *= -1;
                }
                else
                {
                    if (Ball.velY > 0)
                    {
                        Ball.posY = bar.posY + bar.colY;
                    }
                    else
                    {
                        Ball.posY = bar.posY - Ball.colY;
                    }
                }
            }
            return;
        }
    }
}
bool playingAnimation = false;
void startGame(Bar1 &Bar1, Bar2 &Bar2, movingBall &Ball, sf::RenderWindow &window)
{
    if (playingAnimation == false) playingAnimation = true;
    score1 = 0;
    score2 = 0;
    movingBall BALL(50, startBallPosX, startBallPosY, sf::Color::Green);
    Ball.setPos(startBallPosX, startBallPosY);
    Bar1.setPos(startPosX1, startPosY1);
    Bar2.setPos(startPosX2, startPosY2);
}