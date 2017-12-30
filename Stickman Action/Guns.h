#pragma once
#include <SFML\System.hpp>
#include "Object.h"
#include "Map"
#include "Sticklib.h"
#include "sfVector.h"

// Enums
enum gunType
	{
	Knife,
	Pistol,
	Rifle,
	Shotgun,
	Machinegun,
	sRifle
	};
enum damageType
	{
	Slash, 
	Kinetic,
	Electric,
	Plasma
	};
enum triggerType
	{
	SemiAuto,
	Auto,
	Hold
	};
enum gunSlot
	{
	Primary,
	Secondary,
	Melee
	};

class Bullet: public Object
	{
	private:
		sf::IntRect txtr;
		float distance = 0, maxDist = 0;
	public:
		Bullet ()
			{
			}

		Bullet (sf::Image &img, sf::Vector2f POS, sf::Vector2f VEL, float ANG, sf::IntRect TXTR, float maxD, float M): Object (img, POS, M)
			{
			angle = ANG;
			velocity = VEL;
			txtr = TXTR;
			maxDist = maxD;
			}

		void Control (sf::Vector2f target, float time)
			{
			if (distance > maxDist)
				{
				life = false;
				}
			distance += vecL (velocity)*time;
			}

		void CheckBorders (Level &level, float time)
			{
			if (level.PhysicalMap [int (position.y/100)] [int (position.x/100)])
				{
				life = false;
				}
			}

		void Draw (sf::RenderWindow &window, float time)
			{
			Draw::SimpleTxtr (window, texture, txtr, position, -angle*toDeg+90);
			//Test with big red point
			/*sf::CircleShape cs;
			cs.setPosition (position);
			cs.setRadius (15);
			cs.setFillColor (sf::Color::Red);
			window.draw (cs);*/
			}

		void setInitParam (sf::Vector2f pos, sf::Vector2f vel, float ang)
			{
			position = pos;
			velocity = vel;
			angle = ang;
			}
	};

class Gun
	{
	private:
		// Damage of Bullet Shot, Range of shot, speed of bullet, mass of gun, length of gun, dispersion of shot
		float damage = 0, range = 0, bulletSpeed = 0, mass = 0, length = 0, dispersion = 0, reloadTime = 0, shotDelay = 0;
		// Type of trigger, Type of damage, Type of gun, Nuber of Bullets per Shot
		int trgType = 0, dmgType = 0, gType = 0, nBpS = 0, magazine = 0;
		// Charging animation (for Plasma guns)
		bool chargeAnimation = false;

		// Bullet texture rect
		sf::IntRect bulTxtrRect;

		// TIMERS
		float reloadTimer = 0, shootTimer = 0, bulletsLeftInMagazine = magazine;
		bool readyToFire = false;
		float power = 0;

		// Gun id
		int id = 0;

		// Bullet copy
		Bullet bullet;

	public:
		// Default constructor
		Gun ()
			{
			}

		// Constructor for guns' copies
		Gun (sf::Image &img, 
			 int ID, int GTYPE, int TRIGGER,
			 float DMG, int DMGTYPE, float bSPEED, int nBPS, int MAG,
			 float RANGE, float DISP,
			 float MASS, float LEN,
			 float relT, float sDelay,
			 sf::IntRect BULLETRECT,
			 bool chrgAnim = false)
			{
			id = ID;
			gType = GTYPE;
			trgType = TRIGGER;

			damage = DMG;
			dmgType = DMGTYPE;
			bulletSpeed = bSPEED;
			nBpS = nBPS;
			magazine = MAG;

			range = RANGE;
			dispersion = DISP;

			mass = MASS;
			length = LEN;

			reloadTime = relT;
			shotDelay = sDelay;

			bulTxtrRect = BULLETRECT;

			chargeAnimation = chrgAnim;

			bullet = Bullet (img, sf::Vector2f (0, 0), sf::Vector2f (0, 0), 0, BULLETRECT, range, 1.0f);
			}

		void update (float time)
			{
			//std::cout << readyToFire << std::endl;

			if (bulletsLeftInMagazine == 0)
				{
				reloadTimer = reloadTime;
				bulletsLeftInMagazine = magazine;
				}

			if (shootTimer > 0)
				shootTimer -= time;
			else
				shootTimer = 0;

			if (reloadTimer > 0)
				reloadTimer -= time;
			else
				reloadTimer = 0;

			if ((reloadTimer == 0 && shootTimer == 0) || (reloadTimer < reloadTime/1.5f && trgType == triggerType::Hold))
				readyToFire = true;
			else
				readyToFire = false;


			//std::cout << readyToFire << std::endl;

			}

		//---------------GETTERS---------------//
		// Returns nuber of bullets per shot
		int nBulletsPerShot ()
			{
			return nBpS;
			}
		// Returns gun's length from right hand
		float getLength ()
			{
			return length;
			}
		// Returns gun's id
		int getID ()
			{
			return id;
			}
		// Returns true if gun is ready
		bool isReady ()
			{
			return readyToFire;
			}
		// Returns true if charge animation is available
		bool animationAvailable ()
			{
			return chargeAnimation;
			}
		// Returns percentage of charge
		float rechargePercentage ()
			{
			return (reloadTime-reloadTimer)/reloadTime;
			}
		// Returns type of trigger
		int getTriggerType ()
			{
			return trgType;
			}

		void fire ()
			{
			readyToFire = false;
			power = rechargePercentage ();
			shootTimer = shotDelay;
			bulletsLeftInMagazine--;
			}

		Bullet getBullet (sf::Vector2f position, float angle, float currentDisp, sf::Vector2f additionalVel = sf::Vector2f (0, 0))
			{
			float disp = rangeRand (-currentDisp-dispersion, currentDisp+dispersion);
			bullet.setInitParam (position, 
								 sf::Vector2f (bulletSpeed*sin (-angle-disp)*power, bulletSpeed*cos (-angle-disp)*power),
								 -(angle+disp));
			return bullet;
			/*
			return Bullet (img,
						   position,
						   sf::Vector2f (bulletSpeed*sin (-angle-disp)*power, bulletSpeed*cos (-angle-disp)*power),
						   -(angle+disp),
						   bulTxtrRect,
						   range,
						   1.0f);*/
			}
	};

	void CreateBulletsFromGun (Bullet* empty, int &id, sf::Vector2f position, float angle, float currentDisp, Gun gun, sf::Vector2f additionalVel)
	{
	for (int i = 0; i < gun.nBulletsPerShot(); i++)
		{
		*empty = gun.getBullet (position, angle, currentDisp, additionalVel);
		id++;
		}	
	}

// All Guns models declaration:
Gun hands;
Gun PSR400;
Gun F12;

void initGuns (sf::Image &img)
	{
	hands  = Gun (img, 0, gunType::Knife, triggerType::SemiAuto,   10, damageType::Kinetic, 60.f, 1, 1,  2.f,   0,       0,    120.f, 0.5f, 0,     sf::IntRect (280, 0, 25, 60),  false);
	PSR400 = Gun (img, 1, gunType::sRifle, triggerType::Hold,     200, damageType::Plasma,  80.f, 1, 1,  500.f, Pi/32.f, 10,   210.f, 3.f,  0,     sf::IntRect (540, 88, 36, 5),  true);
	F12    = Gun (img, 2, gunType::Pistol, triggerType::SemiAuto, 35,  damageType::Kinetic, 60.f, 1, 12, 200.f, Pi/16.f, 1.5f, 142.f, 3.f,  0.25f, sf::IntRect (275, 155, 10, 6), false);
	}
