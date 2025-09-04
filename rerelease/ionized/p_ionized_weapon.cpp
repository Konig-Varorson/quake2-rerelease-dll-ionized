// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"
#include "../m_player.h"

/*
======================================================================

DISINTEGRATOR

======================================================================
*/
void weapon_disint_fire(edict_t* self)
{
	vec3_t start, dir;
	int		damage = 750;
	int damage_radius = 1000;

	P_ProjectSource(self, self->client->v_angle, { 24, 8, -8 }, start, dir);

	P_AddWeaponKick(self, self->client->v_forward * -2, { -1.f, 0.f, 0.f });

	if (is_quad)
		damage *= damage_multiplier;

	gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/dist/fire.wav"), 1, ATTN_IDLE, 0);

	fire_disintegrator(self, start, dir, damage, 350, damage_radius);

	// send muzzle flash
	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(self);
	gi.WriteByte(MZ_TRACKER | is_silenced);
	gi.multicast(self->s.origin, MULTICAST_PVS, false);

	PlayerNoise(self, start, PNOISE_WEAPON);

	G_RemoveAmmo(self);
}

void Weapon_Disintegrator(edict_t* ent)
{
	constexpr int pause_frames[] = { 30, 37, 45, 0 };
	constexpr int fire_frames[] = { 11, 0 };

	Weapon_Generic(ent, 8, 18, 43, 49, pause_frames, fire_frames, weapon_disint_fire);
}