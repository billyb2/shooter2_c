#include <stdint.h>
#include "weapon.h"

ProjectileType get_projectile_for_weapon(Weapon weapon) {
	ProjectileType projectile_type = StandardBullet;

	switch (weapon) {
		case AssaultRifle:
			projectile_type = StandardBullet;
			break;

		case Pistol:
			projectile_type = StandardBullet;
			break;

		case Shotgun:
			projectile_type = StandardBullet;
			break;

		case Sniper:
			projectile_type = SniperBullet;
			break;

	};

	return projectile_type;

}

uint8_t get_ammo_count(Weapon weapon) {
	uint8_t ammo_count;

	switch (weapon) {
		case AssaultRifle:
			ammo_count = 30;
			break;

		case Pistol:
			ammo_count = 12;
			break;

		case Shotgun:
			ammo_count = 8;
			break;

		case Sniper:
			ammo_count = 3;
			break;

	};

	return ammo_count;

}

uint16_t get_num_reload_frames(Weapon weapon) {
	uint16_t reload_frames;

	switch (weapon) {
		case AssaultRifle:
			// 2 seconds
			reload_frames = 60 * 2;
			break;

		
		case Pistol:
			// 1 seconds
			reload_frames = 60 * 1;
			break;

		case Shotgun:
			// 3 seconds
			reload_frames = 60 * 3;
			break;

		case Sniper:
			// 5 seconds
			reload_frames = 60 * 5;

	};

	return reload_frames;

}

float get_projectile_speed(Weapon weapon) {
	float proj_speed;

	switch (weapon) {
		case AssaultRifle:
			proj_speed = 10.0;
			break;

		case Pistol:
			proj_speed = 12.0;
			break;

		case Shotgun:
			proj_speed = 11.0;
			break;

		case Sniper:
			proj_speed = 90.0;
			break;

	};

	return proj_speed;

}

uint16_t get_projectile_damage(Weapon weapon) {
	uint16_t damage;

	switch (weapon) {
		case AssaultRifle:
			damage = 35;
			break;

		case Pistol:
			damage = 170;
			break;

		case Shotgun:
			damage = 80;
			break;

		case Sniper:
			damage = 1000;
			break;

	};

	return damage;


}

uint16_t get_cooldown_frames(Weapon weapon) {
	uint16_t cooldown_frames;

	switch (weapon) {
		case AssaultRifle:
			cooldown_frames = 5;
			break;

		case Pistol:
			cooldown_frames = 20;
			break;

		case Shotgun:
			cooldown_frames = 30;
			break;

		case Sniper:
			cooldown_frames = 90;
			break;

	};

	return cooldown_frames;
}
