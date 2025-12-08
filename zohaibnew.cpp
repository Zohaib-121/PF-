/*

Name of Student 1:    Zohaib Uddin
Roll No:              25i - 0602
Section:              E

Name of Student2:     Muhammad Mudassir Zia
Roll NO:              25i - 0894
Section:              E


 ---------Programming Fundamentals Project-----------

 Game:  Tumblepop


*/






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




//Functions for the code

void check_ceiling_collision(char** lvl, float& player_y, float& velocityY, float player_x, int cell_size, int PlayerWidth)
{

    //If no up movement, no collision
    if (velocityY >= 0) return;
 
    int row = (int)(player_y + velocityY) / cell_size;
    int col_left  = (int)(player_x) / cell_size;
    int col_mid   = (int)(player_x + PlayerWidth/2) / cell_size;
    int col_right = (int)(player_x + PlayerWidth) / cell_size;

    char top_left  = lvl[row][col_left];    
    char top_mid   = lvl[row][col_mid];
    char top_right = lvl[row][col_right];


    //Checking if any of tile is block
    if (top_left == '#' || top_mid == '#' || top_right == '#')
    {
        velocityY = 0;                     //player stops
        player_y = (row + 1) * cell_size;   //PLayer neeche push hoga
    }
}

void side_collision(char** lvl, float& player_x, float player_y, float move_x, int cell_size, int PlayerHeight, int PlayerWidth)
{

    //No left/right movement = no collision need
    if (move_x == 0) return;
   
    float new_x = player_x + move_x;     //new position on x axis
   


   // Block ke right pe 3 points 
    char left_top = lvl[(int)(player_y) / cell_size][(int)(new_x) / cell_size];
    char left_mid = lvl[(int)(player_y + PlayerHeight/2) / cell_size][(int)(new_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + PlayerHeight - 1) / cell_size][(int)(new_x) / cell_size];
   
    //Block ke left pe 3 points
    char right_top = lvl[(int)(player_y) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
    char right_mid = lvl[(int)(player_y + PlayerHeight/2) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
    char right_bottom = lvl[(int)(player_y + PlayerHeight - 1) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
   

    //If anyone of these points are blocks, 
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
        
        // Apply gravity
        enemy_velocityY[i] += gravity;
        if(enemy_velocityY[i] > 20) enemy_velocityY[i] = 20;
        
        // Vertical movement - check ground
        float next_y = enemy_y[i] + enemy_velocityY[i];
        int groundRow = (int)(next_y + enemy_height) / cell_size;
        int col_left = (int)(enemy_x[i]) / cell_size;
        int col_mid = (int)(enemy_x[i] + enemy_width/2) / cell_size;
        int col_right = (int)(enemy_x[i] + enemy_width - 1) / cell_size;
        
        // Bounds check and ground collision
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
        
        // Horizontal movement
        float next_x = enemy_x[i] + enemy_velocityX[i];
        
        // Check wall collision based on direction
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
        
        // Check ground ahead to prevent falling off
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
        
        // Turn around if hitting wall OR no ground ahead
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


void spawn_skeleton(float enemy_x[], float enemy_y[], float enemy_velocityX[], 
                    float enemy_velocityY[], bool enemy_alive[], int enemy_type[],
                    int index, float x, float y, float speed)
{
    enemy_x[index] = x;
    enemy_y[index] = y;
    enemy_velocityX[index] = speed;
    enemy_velocityY[index] = 0;
    enemy_alive[index] = true;
    enemy_type[index] = 2;
}


bool check_player_enemy_collision(float player_x, float player_y, int playerWidth, int playerHeight,
                                  float enemy_x, float enemy_y, int enemyWidth, int enemyHeight)
{

   //
    return (player_x < enemy_x + enemyWidth &&  player_x + playerWidth > enemy_x &&  player_y < enemy_y + enemyHeight &&   player_y + playerHeight > enemy_y);
}





// SIMPLEST PAUSE FUNCTION
void pause_game(Event& ev, bool& paused)
{
    if (ev.key.code == Keyboard::P) paused = !paused;
}


























int main()
{
    RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP",Style::Resize);
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


    // Block from the folder
    blockTexture.loadFromFile("Assets/Sprites/block3.png");
    blockSprite.setTexture(blockTexture);

    //background from the folder
    bgTex.loadFromFile("Assets/Sprites/bg4.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);


    //music file from folder
    Music lvlMusic;
    lvlMusic.openFromFile("Assets/Sound/mus.ogg");
    lvlMusic.setVolume(20);
    lvlMusic.play();
    lvlMusic.setLoop(true);

    float player_x = 500;
    float player_y = 150;
    float speed = 5;
    
    const float jumpStrength = -20;
    const float gravity = 1;

    int player_score=0;                     //score by capturing enemies
    bool bonusGiven = false;                //bonus for completing level 1


 //  Menu for player choosing

    Font player_Font;
    player_Font.loadFromFile("Assets/Font/menu.ttf");


    //Heading of choose player

    Text txtChoose;
    txtChoose.setFont(player_Font);
    txtChoose.setCharacterSize(32);
    txtChoose.setFillColor(Color::White);
    txtChoose.setPosition(380.f, 150.f);
    txtChoose.setString("Choose player");


//  choosing Green or Yellw

    Text GorY;
    GorY.setFont(player_Font);
    GorY.setCharacterSize(24);
    GorY.setFillColor(Color::Yellow);
    GorY.setPosition(260.f, 200.f);
    GorY.setString("Press G for green, Y for yellow");





 // Font and texture for the score   

    Font score_font;
    score_font.loadFromFile("Assets/Font/Kotton.ttf");

    Text score_text;
    score_text.setFont(score_font);
    score_text.setCharacterSize(30);
    score_text.setFillColor(Color::White);
    score_text.setPosition(10.0f , 10.0f);
    score_text.setString("Score: 0");




//font and texture for lives count

Font lives_Font;
lives_Font.loadFromFile("Assets/Font/Kotton.ttf");

Text livesText;
livesText.setFont(lives_Font);
livesText.setCharacterSize(30);
livesText.setFillColor(Color::Red);
livesText.setPosition(10.f, 50.f);   // top-right-ish
livesText.setString("Lives: 3");













































//Green player textures

    Texture player_LeftTxt_Idle;
    Texture player_RightTxt_Idle;
    Texture player_RightTexture[4];
    Texture player_LeftTexture[4];
    Sprite PlayerSprite;

    Texture playerL_vac_down;
    Texture playerL_vac_up;
    Texture playerL_vac_left;
    Texture playerR_vac_down;
    Texture playerR_vac_up;
    Texture playerR_vac_right;



//YEllow player's  texures

    Texture Yplayer_LeftTxt_Idle;
    Texture Yplayer_RightTxt_Idle;
    Texture Yplayer_RightTexture[4];
    Texture Yplayer_LeftTexture[4];
    Sprite YPlayerSprite;






    bool onGround = false;
    float offset_x = 0.0f;
    float offset_y = 0.0f;
    float velocityY = 0;
    float terminal_Velocity = 20;

    int PlayerHeight = 100;
    int PlayerWidth = 60;
    int animationFrame = 0;             //for animation
    int count = 0;                       //counter for animation
    bool facingLeft = false;           //player ka left,right
    bool facingUp = false;             //player ka up , down ke lye

    bool vacuumActive = false;       //vacuum ke lye
    bool isPaused = false;           // pause karne ke lye

    int captured_enemyCount = 0;             //count of how many enemies captured in the bag
    int playerLives = 3;                




//Green player sprites

    player_LeftTxt_Idle.loadFromFile("Assets/Sprites/playerl.png");
    player_LeftTexture[0].loadFromFile("Assets/Sprites/WalkL1.png");        //first animation
    player_LeftTexture[1].loadFromFile("Assets/Sprites/WalkL2.png");       //second animation
    player_LeftTexture[2].loadFromFile("Assets/Sprites/WalkL3.png");       //third animation
    player_LeftTexture[3].loadFromFile("Assets/Sprites/WalkL4.png");       //forth animation

    player_RightTxt_Idle.loadFromFile("Assets/Sprites/playerRight.png");
    player_RightTexture[0].loadFromFile("Assets/Sprites/WalkR1.png");     //first animation rigtside
    player_RightTexture[1].loadFromFile("Assets/Sprites/WalkR2.png");     //second animation rigtside
    player_RightTexture[2].loadFromFile("Assets/Sprites/WalkR3.png");       //third animation rigtside  
    player_RightTexture[3].loadFromFile("Assets/Sprites/WalkR4.png");       //forth animation rigtside

    PlayerSprite.setTexture(player_RightTexture[0]);
    PlayerSprite.setScale(2.5f,2.5f);
    PlayerSprite.setOrigin(0,0);
    PlayerSprite.setPosition(player_x, player_y);




    //Bag textures and sprite
    Texture bag_Tex;
    Sprite  bag_Sprite;

    bag_Tex.loadFromFile("Assets/Sprites/redbag1.png");     // bag image
    bag_Sprite.setTexture(bag_Tex);
    bag_Sprite.setScale(2.5f, 2.5f);              // matching our player



     float playerPosX = PlayerSprite.getPosition().x;
     float playerPosY = PlayerSprite.getPosition().y;


    





    //vacuum sprites

    playerL_vac_down.loadFromFile ("Assets/Sprites/playerL_vac_down.png");
    playerL_vac_up.loadFromFile ("Assets/Sprites/playerL_vac_up.png");
    playerL_vac_left.loadFromFile ("Assets/Sprites/WalkL1.png");

    playerR_vac_down.loadFromFile ("Assets/Sprites/playerR_vac_down.png");
    playerR_vac_up.loadFromFile ("Assets/Sprites/playerR_vac_up.png");
    playerR_vac_right.loadFromFile ("Assets/Sprites/WalkR1.png");
   

    


//Yellow player textures and sprites


    Yplayer_LeftTxt_Idle.loadFromFile("Assets/Sprites/yellowPlayer_Idle.png");
    Yplayer_LeftTexture[0].loadFromFile("Assets/Sprites/YplayerL1.png");
    Yplayer_LeftTexture[1].loadFromFile("Assets/Sprites/YplayerL2.png");
    Yplayer_LeftTexture[2].loadFromFile("Assets/Sprites/YplayerL3.png");
    Yplayer_LeftTexture[3].loadFromFile("Assets/Sprites/YplayerL4.png");

    Yplayer_RightTxt_Idle.loadFromFile("Assets/Sprites/yellowPlayer_RightIdle.png");
    Yplayer_RightTexture[0].loadFromFile("Assets/Sprites/y_right1.png");
    Yplayer_RightTexture[1].loadFromFile("Assets/Sprites/y_right2.png");
    Yplayer_RightTexture[2].loadFromFile("Assets/Sprites/y_right3.png");
    Yplayer_RightTexture[3].loadFromFile("Assets/Sprites/y_right4.png");

    YPlayerSprite.setTexture(Yplayer_RightTexture[0]);
    YPlayerSprite.setScale(2.5f,2.5f);
    YPlayerSprite.setOrigin(0,0);
    YPlayerSprite.setPosition(player_x, player_y);



    int playerChoice = 0;             // 0 is green,  1 is yellow
    bool chosen = false;      







    //vacuum capturing sprites

    Sprite vacuum_Sprite_left;
    Sprite vacuum_Sprite_right;
    Sprite vacuum_Sprite_up;
    Sprite vacuum_Sprite_down;
    Texture vacuum_Tex_left;
    Texture vacuum_Tex_right;
    Texture vacuum_Tex_up;
    Texture vacuum_Tex_down;

    vacuum_Tex_left.loadFromFile("Assets/Sprites/v_left.png");        // leftside vacuum
    vacuum_Sprite_left.setTexture(vacuum_Tex_left);

    vacuum_Tex_right.loadFromFile("Assets/Sprites/v_right.png");     //   right rainbow vac
    vacuum_Sprite_right.setTexture(vacuum_Tex_right);

    vacuum_Tex_up.loadFromFile("Assets/Sprites/v_up.png");      //  upside vacuum
    vacuum_Sprite_up.setTexture(vacuum_Tex_up);

    vacuum_Tex_down.loadFromFile("Assets/Sprites/v_down.png");    // downside vacuum
    vacuum_Sprite_down.setTexture(vacuum_Tex_down);


    //scaling the sprite to bigger

    vacuum_Sprite_left.setScale(2.0f, 2.0f);
    vacuum_Sprite_right.setScale(2.0f, 2.0f);    
    vacuum_Sprite_up.setScale(2.0f, 2.0f);
    vacuum_Sprite_down.setScale(2.0f, 2.0f);




    const int MAX_ENEMIES = 12;
    float enemy_x[MAX_ENEMIES];
    float enemy_y[MAX_ENEMIES];
    float enemy_velocityX[MAX_ENEMIES];
    float enemy_velocityY[MAX_ENEMIES];
    bool enemy_alive[MAX_ENEMIES];
    int enemy_type[MAX_ENEMIES];

	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		enemy_x[i] = 0;
		enemy_y[i] = 0;
		enemy_velocityX[i] = 0;
		enemy_velocityY[i] = 0;
		enemy_alive[i] = false;   // important
		enemy_type[i] = 0;
	}



  //Ghost sprites and textures

    Texture ghostTexture;
    Sprite enemySprite;
    ghostTexture.loadFromFile("Assets/Sprites/ghostL1.png");
    enemySprite.setTexture(ghostTexture);
    enemySprite.setScale(2, 2);

//Skeleton sprites and textures

    Texture skeletonTexture;
skeletonTexture.loadFromFile("Assets/Sprites/skele_L_Idle.png");  // your skeleton sprite file



//all enemies width and height

    const int ENEMY_WIDTH = 50;
    const int ENEMY_HEIGHT = 60;


	//mid right
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 0, 900, 200, 1.0);


    //neeche wali
   spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 1, 500, 400, -1.5);

   //mid left
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 2, 200, 300, 1.0);
   
	//mid left
	spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 3, 150, 300, 1.2);
   

	//top right
    spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 4, 900, 150, -1.0);
   
	//mid right
	 spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 5, 900, 250, 1.5);
  
	 //bottom right
	spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 6, 900, 500, -1.0);
  
	//top left
	spawn_ghost(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 7, 100, 20, 1.5);
    

  //All skeletons locations

    spawn_skeleton(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 8, 200, 500, 2);
    spawn_skeleton(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 9, 600, 500, 2);
    spawn_skeleton(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 10, 900, 600, 2);
    spawn_skeleton(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, enemy_type, 11, 350, 600, 2);
    








    








    // Create level array
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




   //HIghest block line

	lvl[0][0] = '#';
	lvl[0][1] = '#';
	lvl[0][2] = '#';
	lvl[0][3] = '#';
	lvl[0][4] = '#';
	lvl[0][5] = '#';
	lvl[0][6] = '#';
	lvl[0][7] = '#';
	lvl[0][8] = '#';
	lvl[0][9] = '#';
	lvl[0][10] = '#';
	lvl[0][11] = '#';
	lvl[0][12] = '#';
	lvl[0][13] = '#';
	lvl[0][14] = '#';
	lvl[0][15] = '#';
	lvl[0][16] = '#';
	lvl[0][17] = '#';


  //left side wall

	lvl[0][0] = '#';
	lvl[1][0] = '#';
	lvl[2][0] = '#';
	lvl[3][0] = '#';
	lvl[4][0] = '#';
	lvl[5][0] = '#';
	lvl[6][0] = '#';
	lvl[7][0] = '#';
	lvl[8][0] = '#';
	lvl[9][0] = '#';
	lvl[10][0] = '#';



   //right side wall

	lvl[0][17] = '#';
	lvl[1][17] = '#';
	lvl[2][17] = '#';
	lvl[3][17] = '#';
	lvl[4][17] = '#';
	lvl[5][17] = '#';
	lvl[6][17] = '#';
	lvl[7][17] = '#';
	lvl[8][17] = '#';
	lvl[9][17] = '#';
	lvl[10][17] = '#';
	
	



    // Horitzontal platforms on the sides , above ones

        lvl[3][1] = '#';
        lvl[3][2] = '#';
        lvl[3][3] = '#';
      //  lvl[3][4] = '#';


       // lvl[3][13] = '#';
        lvl[3][14] = '#';
        lvl[3][15] = '#';
        lvl[3][16] = '#';
    


    // Middle horizontal platform
     lvl[5][6] = '#';
     lvl[5][7] = '#';
     lvl[5][8] = '#';
     lvl[5][9] = '#';
     lvl[5][10] = '#';
     lvl[5][11] = '#';
     
    


    




    // Horizontal platforms on the sides  ,  lower ones

    lvl[7][1] = '#';
    lvl[7][2] = '#';
    lvl[7][3] = '#';
    lvl[7][4] = '#';

    lvl[7][13] = '#';
    lvl[7][14] = '#';
    lvl[7][15] = '#';
    lvl[7][16] = '#';




    // Bottom floor

    lvl[10][0] = '#';
    lvl[10][1] = '#';
    lvl[10][2] = '#';
    lvl[10][3] = '#';
    lvl[10][4] = '#';
    lvl[10][5] = '#';
    lvl[10][6] = '#';
	lvl[10][7] = '#';
	lvl[10][8] = '#';
	lvl[10][9] = '#';
    lvl[10][10] = '#';
    lvl[10][11] = '#';
    lvl[10][12] = '#';
    lvl[10][13] = '#';
    lvl[10][14] = '#';
    lvl[10][15] = '#';
    lvl[10][16] = '#';
    lvl[10][17] = '#';
   










    Event ev;
    


    

// Press G for green, Y for yellow
while (window.isOpen() && !chosen)
{
    Event ev;
    while (window.pollEvent(ev))
    {
        if (ev.type == Event::Closed)
            window.close();
    }

    if (Keyboard::isKeyPressed(Keyboard::G))      //Green chosen
    {
        playerChoice = 0;
        chosen = true;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Y))       //Yellow chosen
    {
        playerChoice = 1;
        chosen = true;
    }

    window.clear();
    
    window.draw(bgSprite);
    
    window.draw(txtChoose);   
    window.draw(GorY);         
    


    // greenplayer idle on left side

    PlayerSprite.setTexture(player_RightTxt_Idle);
    PlayerSprite.setPosition(300, 400);
    window.draw(PlayerSprite);


    // yellow player idle on right side
    YPlayerSprite.setPosition(700, 400);
    window.draw(YPlayerSprite);

    window.display();
}









if(playerChoice ==1 )                 // YELLOW
{
    //Yellow textures assigned
    player_LeftTxt_Idle  = Yplayer_LeftTxt_Idle;
    player_RightTxt_Idle = Yplayer_RightTxt_Idle;

    player_LeftTexture[0] = Yplayer_LeftTexture[0];
    player_LeftTexture[1] = Yplayer_LeftTexture[1];
    player_LeftTexture[2] = Yplayer_LeftTexture[2];
    player_LeftTexture[3] = Yplayer_LeftTexture[3];

    player_RightTexture[0] = Yplayer_RightTexture[0];
    player_RightTexture[1] = Yplayer_RightTexture[1];
    player_RightTexture[2] = Yplayer_RightTexture[2];
    player_RightTexture[3] = Yplayer_RightTexture[3];

}


PlayerSprite.setTexture(player_RightTxt_Idle);
PlayerSprite.setPosition(player_x, player_y);






    while (window.isOpen())
    {
        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed)             //closing window
            {
                window.close();
            }

            if (ev.type == Event::KeyPressed) 
            {
                pause_game(ev, isPaused);              //game paused
                        
            }
        }

if (!isPaused)
        {

      

        if (Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();




        // Vacuum ka pata chale ga
        vacuumActive = Keyboard::isKeyPressed(Keyboard::Space);


        // Jump
        if (Keyboard::isKeyPressed(Keyboard::Up) && onGround)
            velocityY = jumpStrength;


        // horizontal movement
        bool keyLeft = Keyboard::isKeyPressed(Keyboard::Left);
        bool keyRight = Keyboard::isKeyPressed(Keyboard::Right);




        if (keyLeft)           //player ius  facing Left side 
        {
            side_collision(lvl, player_x, player_y, -speed, cell_size, PlayerHeight, PlayerWidth);
            facingLeft = true;
        } 
        if (keyRight)              //player ius  facing right side
        { 
            side_collision(lvl, player_x, player_y,  speed, cell_size, PlayerHeight, PlayerWidth);
            facingLeft = false;
        }





        // Vertical facing for vacuum (W means up, S means down)

        if (Keyboard::isKeyPressed(Keyboard::W))
            facingUp = true;
        if (Keyboard::isKeyPressed(Keyboard::S))
            facingUp = false;

        
            //walking animation

        if (keyLeft || keyRight)
        {
            count++;
            if (count % 10 == 0)
                animationFrame = (animationFrame + 1) % 4;
        }
        else
        {
            animationFrame = 0; // idle
        }

       










float playerPosX = PlayerSprite.getPosition().x;
float playerPosY = PlayerSprite.getPosition().y;










        if (vacuumActive)         //Vacuum on
        {
           bool upPressed   = Keyboard::isKeyPressed(Keyboard::W);
           bool downPressed = Keyboard::isKeyPressed(Keyboard::S);

            if (upPressed) // Vacuum sprites
          
            {
                if (facingLeft)
                    PlayerSprite.setTexture(playerL_vac_up);
                else
                    PlayerSprite.setTexture(playerR_vac_up);
            }
            else if (downPressed)
            {
                if (facingLeft)
                    PlayerSprite.setTexture(playerL_vac_down);
                else
                    PlayerSprite.setTexture(playerR_vac_down);
            }

            else if ( !(upPressed && downPressed) )      //not up nor down,
            {

             if (facingLeft)
                    PlayerSprite.setTexture(playerL_vac_left);
                else
                    PlayerSprite.setTexture(playerR_vac_right);

            }



        }




        else           //vacuum is Off
        {
            // Normal walk / idle
            if (facingLeft)
                PlayerSprite.setTexture(player_LeftTexture[animationFrame]);
            else
                PlayerSprite.setTexture(player_RightTexture[animationFrame]);
        }


        // position of vacuum sprite with respect to player

        if (vacuumActive)
        {
            bool upPressed   = Keyboard::isKeyPressed(Keyboard::W);
            bool downPressed = Keyboard::isKeyPressed(Keyboard::S);

            if (upPressed)           //Vacuum up from the player
             {
               
                offset_x = (PlayerWidth / 2.0f ) - 25.0f;                  //  infront of gun
                offset_y     = -PlayerHeight - 20.0f;               //little above than the player
            }

            else if (downPressed)
            {
              
                offset_x = (PlayerWidth / 2.0f ) -25.0f;                //infront of gun
                offset_y = PlayerHeight + 10.0f ;                    //little below than the player
            }

            else
            {
                if (facingLeft)               //Vaccum on left
                {
                    offset_x = PlayerWidth+ 40.0f;                  // little left
                    offset_y = PlayerHeight / 2.0f -20.0f ;     // middle of body
                }

                else   //Vacuum on righT
                {
                    offset_x = PlayerWidth +25.0f;                   // small right
                    offset_y = (PlayerHeight / 2.0f) -20.0f ;  // middle of body
                }
            }

        
            vacuum_Sprite_left.setPosition(playerPosX - offset_x , playerPosY + offset_y);
            vacuum_Sprite_right.setPosition(playerPosX + offset_x, playerPosY + offset_y);

            vacuum_Sprite_up.setPosition(playerPosX + offset_x, playerPosY + offset_y);
            vacuum_Sprite_down.setPosition(playerPosX + offset_x, playerPosY + offset_y);
     


        }




        

        if (vacuumActive)
        {
            bool upPressed   = Keyboard::isKeyPressed(Keyboard::W);
            bool downPressed = Keyboard::isKeyPressed(Keyboard::S);

            // choosing which vacuum sprite is active
            float vacX, vacY;
            float vacuum_width= vacuum_Tex_up.getSize().x * 2.0f;   // because setScale(2,2)
            float wacuum_height = vacuum_Tex_up.getSize().y * 2.0f;

            if (upPressed)
            {
                vacX = vacuum_Sprite_up.getPosition().x;
                vacY = vacuum_Sprite_up.getPosition().y;
            }
            else if (downPressed)
            {
                vacX = vacuum_Sprite_down.getPosition().x;
                vacY = vacuum_Sprite_down.getPosition().y;
            }
            else if (facingLeft)
            {
                vacX = vacuum_Sprite_left.getPosition().x;
                vacY = vacuum_Sprite_left.getPosition().y;
            }
            else        //facing right
            {
                vacX = vacuum_Sprite_right.getPosition().x;
                vacY = vacuum_Sprite_right.getPosition().y;
            }

        

            // All 4 Points of rainbow vacuum

            float vacLeft   = vacX;
            float vacRight  = vacX +vacuum_width;
            float vacTop    = vacY;
            float vacBottom = vacY + wacuum_height;



            // check all enemies
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (!enemy_alive[i])     
                 continue;

                float enX = enemy_x[i];
                float enY = enemy_y[i];

                float enLeft   = enX;
                float enRight  = enX + ENEMY_WIDTH;
                float enTop    = enY;
                float enBottom = enY + ENEMY_HEIGHT;


                //Crossover means enemies vacuum mein aa jayen

                bool crossover= (vacLeft < enRight &&  vacRight>enLeft  &&vacTop<enBottom &&vacBottom>enTop);

                if (crossover && captured_enemyCount < 3 )
                 {
                    enemy_alive[i]= false;  //sucked into the vacuum
                    captured_enemyCount++;

                    if (enemy_type[i] == 1)      // ghost
                    player_score +=50;

                    else if (enemy_type[i] == 2) // skeleton
                    player_score += 75;
                 }
            }
           
        }

score_text.setString("Score: " + to_string(player_score));   //for update of score of player


livesText.setString("Lives: " + to_string(playerLives));   //update lives of player


                // Bag follows player if carrying enemies
        if (captured_enemyCount > 0)
        {
            float playerPosX = PlayerSprite.getPosition().x;
            float playerPosY = PlayerSprite.getPosition().y;

            float bagOffsetX;
            float bagOffsetY = 15.0f;  

            if (facingLeft)
            {
                // bag on right side of player
                bagOffsetX = PlayerWidth;      
            }
            else
            {
                // bag on left side of player
                bagOffsetX = -10.0f;                 
            }

            bag_Sprite.setPosition(playerPosX + bagOffsetX,
                                playerPosY + bagOffsetY);    //it follows player all the time
        }


            //To empty the bag

            if (Keyboard::isKeyPressed(Keyboard::F) && captured_enemyCount > 0)
            {
                captured_enemyCount = 0;
            }

        //when all enemies finished
            bool allDead = true;
            for(int i = 0; i < MAX_ENEMIES; i++)
            {
                if(enemy_alive[i])
                {
                    allDead = false;
                    break;
                }
            }

            if(allDead&&!bonusGiven)       //Bonus will be 1 time
            {
                player_score += 1000;
                bonusGiven=true;
                // Game will end
            }






            for(int i = 0; i < MAX_ENEMIES; i++)
            {

            if(enemy_alive[i])
                {
                    if(check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                                                enemy_x[i], enemy_y[i], ENEMY_WIDTH, ENEMY_HEIGHT))
                    {
                        playerLives--;       //lives end hongi
                        player_x = 500;
                        player_y = 150;
                        velocityY = 0;
                        captured_enemyCount = 0;
                        if(playerLives <= 0)
                        {
                            window.close();
                        }
                        break;
                    }
                }

            }




        window.clear();
        display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
        
        update_enemies(enemy_x, enemy_y, enemy_velocityX, enemy_velocityY, enemy_alive, MAX_ENEMIES, lvl, cell_size, ENEMY_WIDTH, ENEMY_HEIGHT, gravity);
        
      
        for(int i = 0; i < MAX_ENEMIES; i++)
        {

			
            if(enemy_alive[i])
            {
                if(enemy_type[i] == 1)
                {
                    enemySprite.setTexture(ghostTexture);         //ghost enemy
                }
                else if(enemy_type[i] == 2)
                { 
                    enemySprite.setTexture(skeletonTexture);            //skeleton enemy
                }
                enemySprite.setPosition(enemy_x[i], enemy_y[i]);
                window.draw(enemySprite);
            }
		
        }

        player_gravity(lvl, offset_y, velocityY, onGround, gravity, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
        check_ceiling_collision(lvl, player_y, velocityY, player_x, cell_size, PlayerWidth);
        

         window.draw(score_text);

         window.draw(livesText);


        PlayerSprite.setPosition(player_x, player_y);
        window.draw(PlayerSprite);

    
                if (captured_enemyCount > 0)       // bag appears only when carrying enemies
                 window.draw(bag_Sprite);


                if (vacuumActive)
                {
                    bool upPressed   = Keyboard::isKeyPressed(Keyboard::W);
                    bool downPressed = Keyboard::isKeyPressed(Keyboard::S);

                    if (upPressed)
                        window.draw(vacuum_Sprite_up);
                    else if (downPressed)
                        window.draw(vacuum_Sprite_down);
                    else if (facingLeft)
                        window.draw(vacuum_Sprite_left);
                    else
                        window.draw(vacuum_Sprite_right);
                }

       


        window.display();
     
    }



    

}
lvlMusic.stop(); 
    for (int i = 0; i < height; i++)
    {
        delete[] lvl[i];
    }
    delete[] lvl;

    return 0;
}