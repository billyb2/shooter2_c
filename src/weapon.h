#ifndef WEAPON_H
#define WEAPON_H
#include <stdint.h>

typedef enum ProjectileType {
	StandardBullet,
	GrenadeProj,

} ProjectileType;

typedef enum WeaponIndex {
	Primary,
	Tertiary,

} WeaponIndex;

typedef enum Weapon {
	AssaultRifle,
	Pistol,
	Shotgun,
	None,

} Weapon;

typedef enum Throwable {
	Grenade,

} Throwable;

ProjectileType get_projectile_for_weapon(Weapon weapon);
uint8_t get_ammo_count(Weapon weapon);
uint16_t get_num_reload_frames(Weapon weapon);
float get_projectile_speed(Weapon weapon);
uint8_t get_projectile_damage(Weapon weapon);
uint16_t get_cooldown_frames(Weapon weapon);
#endif
