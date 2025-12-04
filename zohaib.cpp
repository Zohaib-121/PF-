#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;
//global variables

//32323232323222
int screen_x = 1136;
int screen_y = 896;


// Fixed side collision - checks both left and right edges of player
void side_collision(char** lvl, float& player_x, float player_y, float move_x, int cell_size, int PlayerHeight, int PlayerWidth)
{
    // Don't move if move_x is 0
    if (move_x == 0) return;
   
    float new_x = player_x + move_x;
   
    // Check left and right edges at multiple heights (top, middle, bottom of player)
    char left_top = lvl[(int)(player_y) / cell_size][(int)(new_x) / cell_size];
    char left_mid = lvl[(int)(player_y + PlayerHeight/2) / cell_size][(int)(new_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + PlayerHeight - 1) / cell_size][(int)(new_x) / cell_size];
   
    char right_top = lvl[(int)(player_y) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
    char right_mid = lvl[(int)(player_y + PlayerHeight/2) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
    char right_bottom = lvl[(int)(player_y + PlayerHeight - 1) / cell_size][(int)(new_x + PlayerWidth) / cell_size];
   
    // If any collision point hits a wall, don't move
    if (left_top == '#' || left_mid == '#' || left_bottom == '#' ||
        right_top == '#' || right_mid == '#' || right_bottom == '#')
    {
        return;
    }
   
    player_x = new_x;
}




/*


// Add ceiling collision check
void check_ceiling_collision(char** lvl, float& player_y, float& velocityY, float player_x, int cell_size, int PlayerWidth)
{
    if (velocityY < 0) // Only check when moving up
    {
        char top_left = lvl[(int)(player_y + velocityY) / cell_size][(int)(player_x) / cell_size];
        char top_mid = lvl[(int)(player_y + velocityY) / cell_size][(int)(player_x + PlayerWidth/2) / cell_size];
        char top_right = lvl[(int)(player_y + velocityY) / cell_size][(int)(player_x + PlayerWidth) / cell_size];
       
        if (top_left == '#' || top_mid == '#' || top_right == '#')
        {
            velocityY = 0; // Stop upward movement
            // Align to bottom of block
            int blockRow = (int)(player_y + velocityY) / cell_size;
            player_y = (blockRow + 1) * cell_size;
        }
    }
}


*/













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


int main()
{

	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	//level specifics
	const int cell_size = 64;
	const int height = 14;
	const int width = 18;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	blockTexture.loadFromFile("Data/block2.png");
	blockSprite.setTexture(blockTexture);

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);
	

	

	//Music initialisation
	Music lvlMusic;

	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	//player data
	float player_x = 500;
	float player_y = 150;

	float speed = 5;
	float move_in_x = 500;
	float move_in_y = 0;

	
	const float jumpStrength = -20; // Initial jump velocity
	const float gravity = 1;  // Gravity acceleration

	bool isJumping = true;  // Track if jumping

	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;




	//Texture player_RightTexture;
	//Texture player_LeftTexture;
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

	int PlayerHeight = 102;
	int PlayerWidth = 96;




	int animationFrame = 0; // will be from 0 to 3
	int count = 0; // slows the animation
	bool facingLeft = false; // last direction





	bool up_button = true;

	char top_left = '\0';
	char top_right = '\0';
	char top_mid = '\0';

	char left_mid = '\0';
	char right_mid = '\0';

	char bottom_left = '\0';
	char bottom_right = '\0';
	char bottom_mid = '\0';

	char bottom_left_down = '\0';
	char bottom_right_down = '\0';
	char bottom_mid_down = '\0';

	char top_right_up = '\0';
	char top_mid_up = '\0';
	char top_left_up = '\0';

	 



	//for the Left player
	player_LeftTxt_Idle.loadFromFile("Data/playerL.png");
	PlayerSprite.setTexture(player_LeftTxt_Idle);
	PlayerSprite.setScale(3,3);
	PlayerSprite.setOrigin(0,0);
	PlayerSprite.setPosition(player_x, player_y);

	//for left animations
	
	player_LeftTexture[0].loadFromFile("Data/WalkL1.png");
    player_LeftTexture[1].loadFromFile("Data/WalkL2.png");
    player_LeftTexture[2].loadFromFile("Data/WalkL3.png");
    player_LeftTexture[3].loadFromFile("Data/WalkL4.png");

    



	//For the right player

	player_RightTxt_Idle.loadFromFile("Data/playerR.png");
	PlayerSprite.setTexture(player_RightTxt_Idle);
	
	PlayerSprite.setScale(3,3);
	PlayerSprite.setOrigin(0,0);
	PlayerSprite.setPosition(player_x, player_y);   
	
	 // for right animations
    player_RightTexture[0].loadFromFile("Data/WalkR1.png");
    player_RightTexture[1].loadFromFile("Data/WalkR2.png");
    player_RightTexture[2].loadFromFile("Data/WalkR3.png");
    player_RightTexture[3].loadFromFile("Data/WalkR4.png");


// Start facing right, first frame
    PlayerSprite.setTexture(player_RightTexture[0]);
    PlayerSprite.setScale(3,3);
    PlayerSprite.setOrigin(0,0);
    PlayerSprite.setPosition(player_x, player_y);



/*					ENEMYIIIIIIIIIIISSSSS*/


float skeleton_x = 300;
float skeleton_y = 150;

float skeleton_speed = 2;

const float skeleton_gravity = 1;
float skeleton_velocityY = 0;
bool skeleton_onGround = false;

bool skele_up_collide;
bool skele_left_collide;
bool skele_right_collide;

int SkeletonWidth = 96;
int SkeletonHeight = 102;

float skeleton_offset_x, skeleton_offset_y;


char skele_top_left, skele_top_right, skele_top_mid;
char skele_left_mid, skele_right_mid;
char skele_bottom_left, skele_bottom_right, skele_bottom_mid;
char skele_bottom_left_down, skele_bottom_right_down, skele_bottom_mid_down;
char skele_top_right_up, skele_top_mid_up, skele_top_left_up;


Texture SkeletonTexture;
Sprite SkeletonSprite;
SkeletonTexture.loadFromFile("Data/skeleton.png");
SkeletonSprite.setScale(3,3);
SkeletonSprite.setPosition(skeleton_x, skeleton_y);










  
















	


	//PlayerSprite.setRotation(180);

	//creating level array
	lvl = new char* [height];

	for (int i = 0; i < height; i += 1)
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

	//left side blocks
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


	//right side

	
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
	
	
	






  //Highest block level

    lvl[3][0] = '#';  lvl[3][1] = '#';  lvl[3][2] = '#';  lvl[3][3] = '#';
    lvl[3][4] = '#';
    lvl[3][13] = '#';lvl[3][14] = '#';
    lvl[3][15] = '#'; lvl[3][16] = '#'; lvl[3][17] = '#';  
    
    

  //Middle upper blocks

    //lvl[5][6] = '#';
    lvl[5][7] = '#';
    lvl[5][8] = '#';
    lvl[5][9] = '#';
    lvl[5][10] = '#';


    //Sides  blocks (mid screen)
    
    lvl[7][0] = '#';  lvl[7][1] = '#';  lvl[7][2] = '#';  lvl[7][3] = '#';
    lvl[7][4] = '#';
    lvl[7][13] = '#';lvl[7][14] = '#';
    lvl[7][15] = '#'; lvl[7][16] = '#'; lvl[7][17] = '#'; 
    

    //Middle blocks (near bottom of screen)








//bottom blocks
    


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
	//main loop
	while (window.isOpen())
	{

		

		while (window.pollEvent(ev))
		{

            if (ev.type == Event::Closed)
            {
                window.close();

            }
        }


        bool keyA = Keyboard::isKeyPressed(Keyboard::A);
		bool keyD = Keyboard::isKeyPressed(Keyboard::D);

		if (keyA || keyD)
		{
		count++;
		if (count % 10 == 0) // change “10” to adjust speed
		{
		animationFrame = (animationFrame + 1) % 4; // It will always be 1,2,3,0
		}



			if (ev.type == Event::Closed) 
			{
				window.close();
			}   

		}



// Escape to close
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }








 if (Keyboard::isKeyPressed(Keyboard::W) && onGround)
			{
	
				
				// Track if jumping

				velocityY = jumpStrength;
        		//onGround = false;
				
			}
            
			 // Horizontal movement
            if ( Keyboard::isKeyPressed(Keyboard::A) )
				
			{ 


            side_collision(lvl, player_x, player_y, -speed, cell_size, PlayerHeight, PlayerWidth);
            PlayerSprite.setTexture(player_LeftTxt_Idle);


                /*


				move_in_x -= speed;
				player_x = move_in_x;
				PlayerSprite.setTexture(player_LeftTxt_Idle);
				
                facingLeft = true;

                */



			}
			
			


            if (Keyboard::isKeyPressed(Keyboard::D) )
			{

            side_collision(lvl, player_x, player_y, speed, cell_size, PlayerHeight, PlayerWidth);
            PlayerSprite.setTexture(player_RightTxt_Idle);

			/*	move_in_x += speed;
				player_x = move_in_x;
			PlayerSprite.setTexture(player_RightTxt_Idle);
			facingLeft = false;

                */
               




			}







			//AIAIIAIAIAIAIAIAIIIIIII




		



/*AIIIIIII




hashsahahha


*/


								if (keyA)
					{
						PlayerSprite.setTexture(player_LeftTexture[animationFrame]);
					}
					else if (keyD)
					{
						PlayerSprite.setTexture(player_RightTexture[animationFrame]);
					}
					

					/*if
					{
					count = 0;
					animationFrame = 0; // idle frame
					if (facingLeft)
					PlayerSprite.setTexture(player_LeftTexture[animationFrame]);
					else
					PlayerSprite.setTexture(player_RightTexture[animationFrame]);
					}
 					*/	



































		//presing escape to close
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		window.clear();

		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		PlayerSprite.setPosition(player_x, player_y);
		window.draw(PlayerSprite);

		window.display();
	}

	//stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	return 0;
}

