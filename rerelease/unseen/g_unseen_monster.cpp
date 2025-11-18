// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

/*
=================
monster_fire_tbolt

Fires a toxic blaster
=================
*/
void monster_fire_tblaster(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, monster_muzzleflash_id_t flashtype, effects_t effect)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	fire_tblaster(self, start, dir, damage, speed, effect);
	monster_muzzleflash(self, start, flashtype);
}

/*
=================
monster_fire_railgrenade

Fires a grenade that releases 4 rail shots on explosion.
=================
*/
void monster_fire_railgrenade(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed,
	monster_muzzleflash_id_t flashtype, float right_adjust, float up_adjust)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		fire_grenade(self, start, aimdir, damage, speed, 2.5_sec, damage + 40.f, right_adjust, up_adjust, true);
		monster_muzzleflash(self, start, flashtype);
		return;
	}
	fire_railgrenade(self, start, aimdir, damage, speed, 2.5_sec, damage + 40.f, right_adjust, up_adjust);
	monster_muzzleflash(self, start, flashtype);
}

/*
=================
monster_fire_bfgrenade

Fires a grenade that releases a bfg shot on explosion.
=================
*/
void monster_fire_bfgrenade(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed,
	monster_muzzleflash_id_t flashtype, float right_adjust, float up_adjust)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		fire_grenade(self, start, aimdir, damage, speed, 2.5_sec, damage + 40.f, right_adjust, up_adjust, true);
		monster_muzzleflash(self, start, flashtype);
		return;
	}
	fire_bfgrenade(self, start, aimdir, damage, speed, 2.5_sec, damage + 40.f, right_adjust, up_adjust);
	monster_muzzleflash(self, start, flashtype);
}

/*
=================
monster_fire_greenflare

Fires a green flare
=================
*/
void monster_fire_greenflare(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, monster_muzzleflash_id_t flashtype)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	fire_greenflare(self, start, dir, damage, speed);
	monster_muzzleflash(self, start, flashtype);
}

/*
=================
monster_fire_yellowflare

Fires a yellow flare
=================
*/
void monster_fire_yellowflare(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, monster_muzzleflash_id_t flashtype)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	fire_yellowflare(self, start, dir, damage, speed, damage / 2, damage / 2 );
	monster_muzzleflash(self, start, flashtype);
}

/*
=================
monster_fire_redflare

Fires a red flare
=================
*/
void monster_fire_redflare(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, monster_muzzleflash_id_t flashtype)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	fire_redflare(self, start, dir, damage, speed, damage);
	monster_muzzleflash(self, start, flashtype);
}

/*
=================
monster_fire_bfghoming

Fires a homing bfg
=================
*/
void monster_fire_bfghoming(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int kick,
	float damage_radius, float turn_fraction, monster_muzzleflash_id_t flashtype)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		fire_bfg(self, start, aimdir, damage, speed, damage_radius);
		monster_muzzleflash(self, start, flashtype);
		return;
	}
	fire_bfghoming(self, start, aimdir, damage, speed, damage_radius, turn_fraction);
	monster_muzzleflash(self, start, flashtype);
}