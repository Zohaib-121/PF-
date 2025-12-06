#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;

void spawn_ghost(float enemy_x[], float enemy_y[], float enemy_velocityX[], 
                 float enemy_velocityY[], bool enemy_alive[], int enemy_type[],
                 int index, float x, float y, float speed)
{
    enemy_x[index] = x;
    enemy_y[index] = y;
    enemy_velocityX[index] = speed;
    enemy_velocityY[index] = 0;
    enemy_alive[index] = true;
    enemy_type[index] = 1;
}

// SIMPLEST PAUSE FUNCTION
void pause_game(Event& ev, bool& paused)
{
    if (ev.key.code == Keyboard::P) paused = !paused;
}

void check_ceiling_collision(char** lvl, float& player_y, float& velocityY,
                             float player_x, int cell_size, int PlayerWidth)
{
    if (velocityY >= 0) return;
    int row = (int)(player_y + velocityY) / cell_size;
    int col_left  = (int)(player_x) / cell_size;
    int col_mid   = (int)(player_x + PlayerWidth/2) / cell_size;
    int col_right = (int)(player_x + PlayerWidth) / cell_size;
    char top_left  = lvl[row][col_left];
    char top_mid   = lvl[row][col_mid];
    char top_right = lvl[row][col_right];
    if (top_left == '#' || top_mid == '#' || top_right == '#')
    {
        velocityY = 0;
        player_y = (row + 1) * cell_size;
    }
}

void side_collision(char** lvl, float& player_x, float player_y, float move_x, int cell_size, int PlayerHeight, int PlayerWidth)
{
    if (move_x == 0) return;
    float new_x = player_x + move_x;
    char left_top = lvl[(int)(player_y) / cell_size][(int)(new_x) / cell_size];
    char left_mid = lvl[(int)(player_y + PlayerHeight/2) / cell_size][(int)(new_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + PlayerHeight - 1) / cell_size][(int)(new_x) / cell_size];
    char right_top = lvl[(int)(player_y) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
    char right_mid = lvl[(int)(player_y + PlayerHeight/2) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
    char right_bottom = lvl[(int)(player_y + PlayerHeight - 1) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
    if (left_top == '#' || left_mid == '#' || left_bottom == '#' ||
        right_top == '#' || right_mid == '#' || right_bottom == '#')
    {
        return;
    }
    player_x = new_x;
}

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
    window.draw(bgSprite); 
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            if (lvl[i][j] == '#')
            {
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
        }
    }
}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
    offset_y = player_y;
    offset_y += velocityY;
    char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
    char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
    char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];
    if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
    {
        onGround = true;
        int blockRow = (int)(offset_y + Pheight) / cell_size;
        player_y = blockRow * cell_size - Pheight;
    }
    else
    {
        player_y = offset_y;
        onGround = false;
    }
    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
    }
    else
    {
        velocityY = 0;
    }
}

void update_enemies(float enemy_x[], float enemy_y[], float enemy_velocityX[], float enemy_velocityY[], bool enemy_alive[], int count, char** lvl, int cell_size, int enemy_width, int enemy_height, float gravity)
{
    for(int i = 0; i < count; i++)
    {
        if(!enemy_alive[i]) continue;
        enemy_velocityY[i] += gravity;
        if(enemy_velocityY[i] > 20) enemy_velocityY[i] = 20;
        float next_y = enemy_y[i] + enemy_velocityY[i];
        int groundRow = (int)(next_y + enemy_height) / cell_size;
        int col_left = (int)(enemy_x[i]) / cell_size;
        int col_mid = (int)(enemy_x[i] + enemy_width/2) / cell_size;
        int col_right = (int)(enemy_x[i] + enemy_width - 1) / cell_size;
        if(groundRow >= 0 && groundRow < 14 && col_left >= 0 && col_right < 18)
        {
            char ground_left = lvl[groundRow][col_left];
            char ground_mid = lvl[groundRow][col_mid];
            char ground_right = lvl[groundRow][col_right];
            if(ground_left == '#' || ground_mid == '#' || ground_right == '#')
            {
                enemy_y[i] = groundRow * cell_size - enemy_height;
                enemy_velocityY[i] = 0;
            }
            else
            {
                enemy_y[i] = next_y;
            }
        }
        float next_x = enemy_x[i] + enemy_velocityX[i];
        int checkCol;
        if(enemy_velocityX[i] > 0)
        {
            checkCol = (int)(next_x + enemy_width - 1) / cell_size;
        }
        else
        {
            checkCol = (int)(next_x) / cell_size;
        }
        int midRow = (int)(enemy_y[i] + enemy_height/2) / cell_size;
        int futureGroundCol;
        if(enemy_velocityX[i] > 0)
        {
            futureGroundCol = (int)(next_x + enemy_width) / cell_size;
        }
        else
        {
            futureGroundCol = (int)(next_x - 1) / cell_size;
        }
        int futureGroundRow = (int)(enemy_y[i] + enemy_height + 5) / cell_size;
        bool wallAhead = false;
        bool groundAhead = false;
        if(checkCol >= 0 && checkCol < 18 && midRow >= 0 && midRow < 14)
        {
            wallAhead = (lvl[midRow][checkCol] == '#');
        }
        if(futureGroundCol >= 0 && futureGroundCol < 18 && futureGroundRow >= 0 && futureGroundRow < 14)
        {
            groundAhead = (lvl[futureGroundRow][futureGroundCol] == '#');
        }
        if(wallAhead || !groundAhead)
        {
            enemy_velocityX[i] *= -1;
        }
        else
        {
            enemy_x[i] = next_x;
        }
    }
}

int main()
{
    RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    const int cell_size = 64;
    const int height = 14;
    const int width = 18;
    char** lvl;

    Texture bgTex;
    Sprite bgSprite;
    Texture blockTexture;
    Sprite blockSprite;

    blockTexture.loadFromFile("Data/block3.png");
    blockSprite.setTexture(blockTexture);

    bgTex.loadFromFile("Data/bg4.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);

    Music lvlMusic;
    lvlMusic.openFromFile("Data/mus.ogg");
    lvlMusic.setVolume(20);
    lvlMusic.play();
    lvlMusic.setLoop(true);

    float player_x = 500;
    float player_y = 150;
    float speed = 5;
    const float jumpStrength = -20;
    const float gravity = 1;

    Texture player_LeftTxt_Idle;
    Texture player_RightTxt_Idle;
    Texture player_RightTexture[4];
    Texture player_LeftTexture[4];
    Sprite PlayerSprite;

    bool onGround = false;
    float offset_x = 0;
    float offset_y = 0;
    float velocityY = 0;
    float terminal_Velocity = 20;

    int PlayerHeight = 115;
    int PlayerWidth = 96;
    int animationFrame = 0;
    int count = 0;
    bool facingLeft = false;
    bool isPaused = false;  // PAUSE VARIABLE

    player_LeftTxt_Idle.loadFromFile("Data/playerL.png");
    player_LeftTexture[0].loadFromFile("Data/WalkL1.png");
    player_LeftTexture[1].loadFromFile("Data/WalkL2.png");
    player_LeftTexture[2].loadFromFile("Data/WalkL3.png");
    player_LeftTexture[3].loadFromFile("Data/WalkL4.png");

    player_RightTxt_Idle.loadFromFile("Data/playerR.png");
    player_RightTexture[0].loadFromFile("Data/WalkR1.png");
    player_RightTexture[1].loadFromFile("Data/WalkR2.png");
    player_RightTexture[2].loadFromFile("Data/WalkR3.png");
    player_RightTexture[3].loadFromFile("Data/WalkR4.png");

    PlayerSprite.setTexture(player_RightTexture[0]);
    PlayerSprite.setScale(3,3);
    PlayerSprite.setOrigin(0,0);
    PlayerSprite.setPosition(player_x, player_y);

    const int MAX_ENEMIES = 8;
    float enemy_x[MAX_ENEMIES];
    float enemy_y[MAX_ENEMIES];
    float enemy_velocityX[MAX_ENEMIES];
    float enemy_velocityY[MAX_ENEMIES];
    bool enemy_alive[MAX_ENEMIES];
    int enemy_type[MAX_ENEMIES];

    Texture enemyTexture;
    Sprite enemySprite;
    enemyTexture.loadFromFile("Data/ghostL1.png");
    enemySprite.setTexture(enemyTexture);
    enemySprite.setScale(2, 2);

    const int ENEMY_WIDTH = 64;
    const int ENEMY_HEIGHT = 64;

    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 0, 300, 200, 2.0);
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 1, 500, 400, -1.5);
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 2, 700, 300, 1.0);
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 3, 150, 300, 1.2);
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 4, 600, 150, -1.0);
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 5, 900, 250, 2.5);
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 6, 400, 350, -2.0);
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 7, 800, 400, 1.5);
    
    lvl = new char*[height];
    for (int i = 0; i < height; i++)
    {
        lvl[i] = new char[width];
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            lvl[i][j] = ' ';
        }
    }

    for(int j = 0; j < 18; j++) lvl[0][j] = '#';
    for(int i = 0; i <= 10; i++) lvl[i][0] = '#';
    for(int i = 0; i <= 10; i++) lvl[i][17] = '#';
    for(int j = 0; j <= 4; j++) lvl[3][j] = '#';
    for(int j = 13; j <= 17; j++) lvl[3][j] = '#';
    for(int j = 7; j <= 10; j++) lvl[5][j] = '#';
    for(int j = 0; j <= 4; j++) lvl[7][j] = '#';
    for(int j = 13; j <= 17; j++) lvl[7][j] = '#';
    for(int j = 0; j < 18; j++) lvl[10][j] = '#';

    Event ev;
    
    while (window.isOpen())
    {
        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed) window.close();
            if (ev.type == Event::KeyPressed) pause_game(ev, isPaused);
        }

        if (!isPaused)
        {
            bool keyA = Keyboard::isKeyPressed(Keyboard::A);
            bool keyD = Keyboard::isKeyPressed(Keyboard::D);

            if (keyA || keyD)
            {
                count++;
                if (count % 10 == 0)
                {
                    animationFrame = (animationFrame + 1) % 4;
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
            }

            if (Keyboard::isKeyPressed(Keyboard::W) && onGround)
            {
                velocityY = jumpStrength;
            }
            
            if (Keyboard::isKeyPressed(Keyboard::A))
            { 
                side_collision(lvl, player_x, player_y, -speed, cell_size, PlayerHeight, PlayerWidth);
                PlayerSprite.setTexture(player_LeftTexture[animationFrame]);
                facingLeft = true;
            }
            
            if (Keyboard::isKeyPressed(Keyboard::D))
            {
                side_collision(lvl, player_x, player_y, speed, cell_size, PlayerHeight, PlayerWidth);
                PlayerSprite.setTexture(player_RightTexture[animationFrame]);
                facingLeft = false;
            }

            player_gravity(lvl, offset_y, velocityY, onGround, gravity, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
            check_ceiling_collision(lvl, player_y, velocityY, player_x, cell_size, PlayerWidth);
            update_enemies(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, MAX_ENEMIES, lvl, cell_size, ENEMY_WIDTH, ENEMY_HEIGHT, gravity);
        }

        window.clear();
        display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
        
        for(int i = 0; i < MAX_ENEMIES; i++)
        {
            if(enemy_alive[i])
            {
                enemySprite.setPosition(enemy_x[i], enemy_y[i]);
                window.draw(enemySprite);
            }
        }

        PlayerSprite.setPosition(player_x, player_y);
        window.draw(PlayerSprite);

     

        window.display();
    }

    lvlMusic.stop();
    for (int i = 0; i < height; i++)
    {
        delete[] lvl[i];
    }
    delete[] lvl;

    return 0;
}
