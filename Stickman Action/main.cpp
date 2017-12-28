#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <list>

#include "Display.h"
#include "Draw.h"
#include "Objects.h"
#include "Camera.h"

void setup ();
void loop  (WindowParameters wp);

void SinglePlayer (sf::RenderWindow &window);
short int MainMenu (sf::RenderWindow &window);

int main ()
	{
	loop (LoadWindowParameters ());

	return 0;
	}

void setup ()
	{
	
	}
void loop  (WindowParameters wp)
	{
	sf::RenderWindow window (sf::VideoMode (wp.vm.width, wp.vm.height), "", wp.Mode, wp.cs);
	window.setFramerateLimit (wp.fpsLim);
	printf ("\n%d\n", wp.vm.height);
	short int mode = 1;
	while (mode != 0)
		{
		SinglePlayer (window);
		}
	}

void SinglePlayer (sf::RenderWindow &window)
	{
	sf::Image stickman = LoadImage ("stickman.png");
	sf::Image guns; guns.loadFromFile ("Data/img/guns.png");
	guns.createMaskFromColor (sf::Color (0, 0, 255));
	sf::Texture tx; tx.loadFromFile ("Data/img/tileset.png");
	sf::Sprite map_sprite; map_sprite.setTexture (tx);

	std::vector <Stickman*> stickmans;
	std::vector <Bullet*> bullets;
	std::vector <Object*> mapObjects;
	stickmans.push_back (new Player (stickman, guns, sf::Vector2f (5000, 300), 80));

	Level level (0, 0);

	Camera camera (sf::FloatRect (0, 0, 1920, 1080));
	
	sf::Clock delayTimer;
	unsigned long int tickTimer = 0;
	float avgDelay = 0;
	bool lMousePrsd = false, rMousePrsd = false;

	while (window.isOpen())
		{
		//Time Block
		float time = delayTimer.getElapsedTime().asSeconds();
		delayTimer.restart ();

		if (tickTimer = 0) avgDelay = time;

		if (time < avgDelay*10.0f)
			avgDelay += (time - avgDelay)/100.0f;
			
		//Events
		sf::Event windowEvent;
		while (window.pollEvent(windowEvent))
			{
			if (windowEvent.type == sf::Event::Closed)
				window.close();
			if (windowEvent.type == sf::Event::MouseButtonPressed)
				{
				if (windowEvent.key.code == sf::Mouse::Left)
					lMousePrsd = true;
				if (windowEvent.key.code == sf::Mouse::Right)
					rMousePrsd = true;
				}
			if (windowEvent.type == sf::Event::MouseButtonReleased)
				{
				if (windowEvent.key.code == sf::Mouse::Left)
					lMousePrsd = false;
				if (windowEvent.key.code == sf::Mouse::Right)
					rMousePrsd = false;
				}
			}

		sf::Vector2f thisPlayerPos;

		// Physics
		// Stickmans
		for (auto i = stickmans.begin (); i != stickmans.end ();)
			{
			Stickman *a = *i;

			if (a->getType () == "Player")
				{
				thisPlayerPos = a->getPos ();
				
				a->Update (level, time, sf::Vector2f (sf::Mouse::getPosition (window))*1920.f/float (window.getSize ().x), lMousePrsd);
				}
			else
				a->Update (level, time, sf::Vector2f (sf::Mouse::getPosition (window))*1920.f/float (window.getSize ().x));

			if (a->isShoot ())
				CreateBulletsFromGun (bullets, guns, a->getBulletStart (), a->getHandAngle (), a->getDisp (), a->getGun (), a->getVel());

			if (!a->getLife ())
				{
				i = stickmans.erase (i);
				delete a;
				}
			else
				i++;
			}
		// Bullets
		for (auto i = bullets.begin (); i != bullets.end ();)
			{
			Object *a = *i;

			a->Update (level, time);

			if (!a->getLife ()) 
				{ 
				i = bullets.erase (i);
				delete a; 
				}
			else 
				i++;
			}

		// Graphics
		window.setView (camera.PlayerCam (sf::Vector2f (thisPlayerPos.x, thisPlayerPos.y-200)));
		window.clear (sf::Color (32, 32, 32));
		level.Draw (window, map_sprite, thisPlayerPos);
		for (auto a: stickmans)
			a->Draw(window, time);
		for (auto a: bullets)
			a->Draw (window, time);

		tickTimer++;

		window.display();
		}
	}