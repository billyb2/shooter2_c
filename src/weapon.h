#ifndef WEAPON_H
#define WEAPON_H
#include <stdbool.h>
#include <stdint.h>

typedef enum ProjectileType {
	StandardBullet,
	SniperBullet,
	GrenadeProj,

} ProjectileType;

typedef enum WeaponIndex {
	Primary,
	Tertiary,

} WeaponIndex;

#define NUM_WEAPONS 4

typedef enum Weapon {
	Pistol = 0,
	AssaultRifle = 1,
	Shotgun = 2,
	Sniper = 3,

} Weapon;

typedef enum Throwable {
	Grenade,

} Throwable;

ProjectileType get_projectile_for_weapon(Weapon weapon);
uint8_t get_ammo_count(Weapon weapon);
uint16_t get_num_reload_frames(Weapon weapon);
float get_projectile_speed(Weapon weapon);
uint16_t get_projectile_damage(Weapon weapon);
uint16_t get_cooldown_frames(Weapon weapon);

int weapon_to_text(Weapon weapon, char* text_buffer);
bool text_to_weapon(const char* weapon_text, const int weapon_text_len, Weapon* weapon);
#endif
