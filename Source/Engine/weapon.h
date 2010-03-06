/*
 * Filename      : weapon.h
 * Author(s)     : Shawn Reynolds (eb0s@yahoo.com)
 * Date Created  : Friday, November 21, 2009
 * Last Modified : Friday, November 21, 2009
 * Purpose       : 
 * Notes         :
 */
#ifndef __H_WEAPON__
#define __H_WEAPON__

#include "Sprites/sprite.h"
#include "Audio/sound.h"
#include "Utilities/components.h"

#define WEAPON_PROJECTILE  0
#define WEAPON_ENERGY  1

enum FireStatus {FireSuccess, FireNoWeapons, FireNotReady, FireNoAmmo};

class Weapon : public Component {
	public:
		Weapon(void);
		Weapon& operator=(const Weapon& other);
		Weapon( string _name, Image* _image, Image* _pic, int _weaponType, int _payload, int _velocity, int _acceleration, int _ammoType, int _ammoConsumption, int _fireDelay, int _lifetime, Sound* _sound);
		~Weapon(void);

		bool parserCB( string sectionName, string subName, string value );
		void _dbg_PrintInfo( void );
		xmlNodePtr ToXMLNode(string componentName);

		Image *GetImage(void) {return image;}
		Image *GetPicture(void) {return pic;}
		int GetType(void) {return weaponType;}
		int GetPayload(void) {return payload;}
		int GetVelocity(void) {return velocity;}
		int GetAcceleration(void) {return acceleration;}
		int GetAmmoType(void) {return ammoType;}
		int GetAmmoConsumption(void) { return ammoConsumption;}
		int GetFireDelay(void) {return fireDelay;}
		int GetLifetime(void) {return lifetime;}
		Sound *sound; //Sound the weapon makes

	private:
		Image *image;
		Image *pic;
		int weaponType; //(energy, explosive, laser, etc)
		int payload; //intesity of explosion
		int velocity; //speed of travel
		int acceleration; //speed of acceleration
		int ammoType; //type of ammo used, unique id
		int ammoConsumption; //ammount of ammo to consume per shot
		int fireDelay; //delay between being able to fire agian in ticks
		int lifetime; //ticks until weapon is destroyed
		//TODO tracking factor
};

#endif
