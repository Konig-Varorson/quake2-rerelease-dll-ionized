// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"
#include "../m_player.h"

/*
======================================================================

Toxic Blaster

======================================================================
*/
void Weapon_TBolt_Fire(edict_t* ent)
{
	int damage = 15;
	int speed = 1500;

	if (is_quad)
		damage *= damage_multiplier;

	vec3_t start, dir;
	P_ProjectSource(ent, ent->client->v_angle, vec3_t{ 24, 8, -8 } + vec3_origin, start, dir);

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -1.f, 0.f, 0.f });

	fire_tblaster(ent, start, dir, damage, speed, EF_BLASTER);

	// send muzzle flash
	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	gi.WriteByte(MZ_BLASTER | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent, 1);
}

void Weapon_Toxic_Blaster(edict_t* ent)
{
	constexpr int pause_frames[] = { 19, 32, 0 };
	constexpr int fire_frames[] = { 5, 0 };

	Weapon_Generic(ent, 4, 8, 52, 55, pause_frames, fire_frames, Weapon_TBolt_Fire);
}

/*
======================================================================

Long-Range Shotgun

======================================================================
*/

void weapon_rangedshotgun_fire(edict_t* ent)
{
	int damage = 4;
	int kick = 8;

	vec3_t start, dir;
	// Paril: kill sideways angle on hitscan
	P_ProjectSource(ent, ent->client->v_angle, { 0, 0, -8 }, start, dir);

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -2.f, 0.f, 0.f });

	if (is_quad)
	{
		damage *= damage_multiplier;
		kick *= damage_multiplier;
	}

	G_LagCompensate(ent, start, dir);
	if (deathmatch->integer)
		fire_shotgun(ent, start, dir, damage, kick, 200, 200, DEFAULT_DEATHMATCH_SHOTGUN_COUNT, MOD_SHOTGUN);
	else
		fire_shotgun(ent, start, dir, damage, kick, 200, 200, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);
	G_UnLagCompensate();

	// send muzzle flash
	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	gi.WriteByte(MZ_SHOTGUN | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent);
}

void Weapon_LRS(edict_t* ent)
{
	constexpr int pause_frames[] = { 22, 28, 34, 0 };
	constexpr int fire_frames[] = { 8, 0 };

	Weapon_Generic(ent, 7, 18, 36, 39, pause_frames, fire_frames, weapon_rangedshotgun_fire);
}


/*
======================================================================

RailGrenades / RailGrenade Launcher

======================================================================
*/

void weapon_railgrenade_fire(edict_t* ent, bool held)
{
	int	  damage = 125;
	int	  speed;
	float radius;
	float damage_radius = 1000;

	radius = (float)(damage + 40);
	if (is_quad)
		damage *= damage_multiplier;

	vec3_t start, dir;
	// Paril: kill sideways angle on grenades
	// limit upwards angle so you don't throw behind you
	P_ProjectSource(ent, { max(-62.5f, ent->client->v_angle[0]), ent->client->v_angle[1], ent->client->v_angle[2] }, { 2, 0, -14 }, start, dir);

	gtime_t timer = ent->client->grenade_time - level.time;
	speed = (int)(ent->health <= 0 ? GRENADE_MINSPEED : min(GRENADE_MINSPEED + (GRENADE_TIMER - timer).seconds() * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / GRENADE_TIMER.seconds()), GRENADE_MAXSPEED));

	ent->client->grenade_time = 0_ms;

	fire_railgrenade2(ent, start, dir, damage, speed, timer, radius, held);

	G_RemoveAmmo(ent, 1);
}

void Weapon_RailGrenade(edict_t* ent)
{
	constexpr int pause_frames[] = { 29, 34, 39, 48, 0 };

	Throw_Generic(ent, 15, 48, 5, "weapons/hgrena1b.wav", 11, 12, pause_frames, true, "weapons/hgrenc1b.wav", weapon_railgrenade_fire, true);

	// [Paril-KEX] skip the duped frame
	if (ent->client->ps.gunframe == 1)
		ent->client->ps.gunframe = 2;
}

void weapon_railgrenadelauncher_fire(edict_t* ent)
{
	int	  damage = 120;
	float radius;

	radius = (float)(damage + 40);
	if (is_quad)
		damage *= damage_multiplier;

	vec3_t start, dir;
	// Paril: kill sideways angle on grenades
	// limit upwards angle so you don't fire it behind you
	P_ProjectSource(ent, { max(-62.5f, ent->client->v_angle[0]), ent->client->v_angle[1], ent->client->v_angle[2] }, { 8, 0, -8 }, start, dir);

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -1.f, 0.f, 0.f });

	fire_railgrenade(ent, start, dir, damage, 600, 2.5_sec, radius, (crandom_open() * 10.0f), (200 + crandom_open() * 10.0f));

	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	gi.WriteByte(MZ_GRENADE | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent);
}

void Weapon_RailGrenadeLauncher(edict_t* ent)
{
	constexpr int pause_frames[] = { 34, 51, 59, 0 };
	constexpr int fire_frames[] = { 6, 0 };

	Weapon_Generic(ent, 5, 16, 59, 64, pause_frames, fire_frames, weapon_railgrenadelauncher_fire);
}

/*
======================================================================

BFGrenade / BFGrenade Launcher

======================================================================
*/
void weapon_bfgrenade_fire(edict_t* ent, bool held)
{
	int	  damage = 125;
	int	  speed;
	float radius;
	float damage_radius = 1000;

	radius = (float)(damage + 40);
	if (is_quad)
		damage *= damage_multiplier;

	vec3_t start, dir;
	// Paril: kill sideways angle on grenades
	// limit upwards angle so you don't throw behind you
	P_ProjectSource(ent, { max(-62.5f, ent->client->v_angle[0]), ent->client->v_angle[1], ent->client->v_angle[2] }, { 2, 0, -14 }, start, dir);

	gtime_t timer = ent->client->grenade_time - level.time;
	speed = (int)(ent->health <= 0 ? GRENADE_MINSPEED : min(GRENADE_MINSPEED + (GRENADE_TIMER - timer).seconds() * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / GRENADE_TIMER.seconds()), GRENADE_MAXSPEED));

	ent->client->grenade_time = 0_ms;

	fire_bfgrenade2(ent, start, dir, damage, speed, timer, radius, held);

	G_RemoveAmmo(ent, 1);
}

void Weapon_BFGrenade(edict_t* ent)
{
	constexpr int pause_frames[] = { 29, 34, 39, 48, 0 };

	Throw_Generic(ent, 15, 48, 5, "weapons/hgrena1b.wav", 11, 12, pause_frames, true, "weapons/hgrenc1b.wav", weapon_bfgrenade_fire, true);

	// [Paril-KEX] skip the duped frame
	if (ent->client->ps.gunframe == 1)
		ent->client->ps.gunframe = 2;
}

void weapon_bfgrenadelauncher_fire(edict_t* ent)
{
	int	  damage = 120;
	float radius;

	radius = (float)(damage + 40);
	if (is_quad)
		damage *= damage_multiplier;

	vec3_t start, dir;
	// Paril: kill sideways angle on grenades
	// limit upwards angle so you don't fire it behind you
	P_ProjectSource(ent, { max(-62.5f, ent->client->v_angle[0]), ent->client->v_angle[1], ent->client->v_angle[2] }, { 8, 0, -8 }, start, dir);

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -1.f, 0.f, 0.f });

	fire_bfgrenade(ent, start, dir, damage, 600, 2.5_sec, radius, (crandom_open() * 10.0f), (200 + crandom_open() * 10.0f));

	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	gi.WriteByte(MZ_GRENADE | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent);
}

void Weapon_BFGrenadeLauncher(edict_t* ent)
{
	constexpr int pause_frames[] = { 34, 51, 59, 0 };
	constexpr int fire_frames[] = { 6, 0 };

	Weapon_Generic(ent, 5, 16, 59, 64, pause_frames, fire_frames, weapon_bfgrenadelauncher_fire);
}

/*
======================================================================

GYR Flare Launcher

======================================================================
*/
void Weapon_GFlareLauncher_Fire(edict_t* ent)
{
	int	  damage;
	float damage_radius;
	int	  radius_damage;

	damage = irandom(100, 120);
	radius_damage = 120;
	damage_radius = 120;
	if (is_quad)
	{
		damage *= damage_multiplier;
		radius_damage *= damage_multiplier;
	}

	vec3_t start, dir;
	P_ProjectSource(ent, ent->client->v_angle, { 8, 8, -8 }, start, dir);
	fire_greenflare(ent, start, dir, damage, 650);

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -1.f, 0.f, 0.f });

	// send muzzle flash
	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	gi.WriteByte(MZ_ROCKET | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent);
}

void Weapon_GFlareLauncher(edict_t* ent)
{
	constexpr int pause_frames[] = { 25, 33, 42, 50, 0 };
	constexpr int fire_frames[] = { 5, 0 };

	Weapon_Generic(ent, 4, 12, 50, 54, pause_frames, fire_frames, Weapon_GFlareLauncher_Fire);
}


void Weapon_YFlareLauncher_Fire(edict_t* ent)
{
	int	  damage;
	float damage_radius;
	int	  radius_damage;

	damage = irandom(100, 120);
	radius_damage = 120;
	damage_radius = 120;
	if (is_quad)
	{
		damage *= damage_multiplier;
		radius_damage *= damage_multiplier;
	}

	vec3_t start, dir;
	P_ProjectSource(ent, ent->client->v_angle, { 8, 8, -8 }, start, dir);
	fire_yellowflare(ent, start, dir, damage, 650, damage_radius, radius_damage);

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -1.f, 0.f, 0.f });

	// send muzzle flash
	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	gi.WriteByte(MZ_ROCKET | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent);
}

void Weapon_YFlareLauncher(edict_t* ent)
{
	constexpr int pause_frames[] = { 25, 33, 42, 50, 0 };
	constexpr int fire_frames[] = { 5, 0 };

	Weapon_Generic(ent, 4, 12, 50, 54, pause_frames, fire_frames, Weapon_YFlareLauncher_Fire);
}

void Weapon_RFlareLauncher_Fire(edict_t* ent)
{
	int	  damage;
	int	  radius_damage;

	damage = irandom(100, 120);
	radius_damage = 120;
	if (is_quad)
	{
		damage *= damage_multiplier;
		radius_damage *= damage_multiplier;
	}

	vec3_t start, dir;
	P_ProjectSource(ent, ent->client->v_angle, { 8, 8, -8 }, start, dir);
	fire_redflare(ent, start, dir, damage, 650, radius_damage);

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -1.f, 0.f, 0.f });

	// send muzzle flash
	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	gi.WriteByte(MZ_ROCKET | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent);
}

void Weapon_RFlareLauncher(edict_t* ent)
{
	constexpr int pause_frames[] = { 25, 33, 42, 50, 0 };
	constexpr int fire_frames[] = { 5, 0 };

	Weapon_Generic(ent, 4, 12, 50, 54, pause_frames, fire_frames, Weapon_RFlareLauncher_Fire);
}

/*
======================================================================

BFG 20k

======================================================================
*/
void weapon_homing_bfg_fire(edict_t* ent) //TODO: not homing yet, instead double damage/double cost
{
	int	  damage;
	float damage_radius = 1000;
	float turn_fraction = 0.05f;

	if (deathmatch->integer)
		damage = 200;
	else
		damage = 500;

	if (ent->client->ps.gunframe == 9)
	{
		gi.WriteByte(svc_muzzleflash);
		gi.WriteEntity(ent);
		gi.WriteByte(MZ_BFG | is_silenced);
		gi.multicast(ent->s.origin, MULTICAST_PVS, false);

		PlayerNoise(ent, ent->s.origin, PNOISE_WEAPON);
		return;
	}

	if (ent->client->pers.inventory[ent->client->pers.weapon->ammo] < 75)
		return;

	if (is_quad)
		damage *= damage_multiplier;

	vec3_t start, dir;
	P_ProjectSource(ent, ent->client->v_angle, { 8, 8, -8 }, start, dir);
	fire_bfghoming(ent, start, dir, damage, 400, damage_radius, turn_fraction);

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -20.f, 0, crandom() * 8 });
	ent->client->kick.total = DAMAGE_TIME();
	ent->client->kick.time = level.time + ent->client->kick.total;

	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	gi.WriteByte(MZ_BFG2 | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent);
}

void Weapon_BFG20K(edict_t* ent)
{
	constexpr int pause_frames[] = { 39, 45, 50, 55, 0 };
	constexpr int fire_frames[] = { 9, 17, 0 };

	Weapon_Generic(ent, 8, 32, 54, 58, pause_frames, fire_frames, weapon_homing_bfg_fire);
}