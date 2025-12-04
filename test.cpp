#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;


//Sir Shehryar GOAT



int screen_x = 1136;
int screen_y = 896;

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

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);

	bgTex.loadFromFile("Data/bg2.png");
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
	
	
	





/*
  //Highest block level

    lvl[3][0] = '#';  lvl[3][1] = '#';  lvl[3][2] = '#';  lvl[3][3] = '#';
    lvl[3][4] = '#';
    lvl[3][13] = '#';lvl[3][14] = '#';
    lvl[3][15] = '#'; lvl[3][16] = '#'; lvl[3][17] = '#';  
    */
    

  //Middle upper blocks

    lvl[5][6] = '#';
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










			//AIAIIAIAIAIAIAIAIIIIIII




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











 if (Keyboard::isKeyPressed(Keyboard::W) && onGround)
			{
	
				
				// Track if jumping

				velocityY = jumpStrength;
        		//onGround = false;
				
			}
			
			 // Horizontal movement
            if ( Keyboard::isKeyPressed(Keyboard::A) )
				
			{ 
				move_in_x -= speed;
				player_x = move_in_x;
				PlayerSprite.setTexture(player_LeftTxt_Idle);
				
                facingLeft = true;
			}
			
			


            if (Keyboard::isKeyPressed(Keyboard::D) )
			{
				move_in_x += speed;
				player_x = move_in_x;
			PlayerSprite.setTexture(player_RightTxt_Idle);
			facingLeft = false;
			}



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

