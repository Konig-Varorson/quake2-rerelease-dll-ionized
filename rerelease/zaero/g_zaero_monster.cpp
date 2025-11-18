// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

/*
=================

monster_fire_flare

=================
*/

void monster_fire_flare(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, float damage_radius, int radius_damage, float right_adjust, float up_adjust, monster_muzzleflash_id_t flashtype)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	fire_flare(self, start, aimdir, damage, speed, damage_radius, radius_damage, right_adjust, up_adjust);
	monster_muzzleflash(self, start, flashtype);
}

/*
=================

monster_fire_plasmaball

=================
*/

void monster_fire_plasmacannon(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int speed, float damage_radius, float distance, monster_muzzleflash_id_t flashtype)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	fire_plasmacannon(self, start, aimdir, damage, speed, damage_radius, distance);
	monster_muzzleflash(self, start, flashtype);
}