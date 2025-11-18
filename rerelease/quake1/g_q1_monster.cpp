// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

/*
=================
fire_lavaball

Fires a single explosive lavaball. Used by eldritch monsters.
=================
*/

TOUCH(lavaball_touch) (edict_t* ent, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	vec3_t origin;

	if (other == ent->owner)
		return;

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(ent);
		return;
	}

	if (ent->owner->client)
		PlayerNoise(ent->owner, ent->s.origin, PNOISE_IMPACT);

	// calculate position for the explosion entity
	origin = ent->s.origin + tr.plane.normal;

	if (other->takedamage)
	{
		T_Damage(other, ent, ent->owner, ent->velocity, ent->s.origin, tr.plane.normal, ent->dmg, ent->dmg, DAMAGE_NONE, MOD_ROCKET);
	}
	else
	{
		// don't throw any debris in net games
		if (!deathmatch->integer && !coop->integer)
		{
			if (tr.surface && !(tr.surface->flags & (SURF_WARP | SURF_TRANS33 | SURF_TRANS66 | SURF_FLOWING)))
			{
				BecomeExplosion1(ent);
			}
		}
	}

	T_RadiusDamage(ent, ent->owner, (float)ent->radius_dmg, other, ent->dmg_radius, DAMAGE_NONE, MOD_R_SPLASH);

	gi.WriteByte(svc_temp_entity);
	if (ent->waterlevel)
		gi.WriteByte(TE_ROCKET_EXPLOSION_WATER);
	else
		gi.WriteByte(TE_ROCKET_EXPLOSION);
	gi.WritePosition(origin);
	gi.multicast(ent->s.origin, MULTICAST_PHS, false);

	G_FreeEdict(ent);
}

void fire_lavaball(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius, int radius_damage)
{
	edict_t* lavaball;

	lavaball = G_Spawn();
	lavaball->s.origin = start;
	lavaball->s.angles = vectoangles(dir);
	lavaball->s.effects |= EF_FIREBALL;
	lavaball->velocity = dir * speed;
	lavaball->movetype = MOVETYPE_FLYMISSILE;
	lavaball->svflags |= SVF_PROJECTILE;
	lavaball->flags |= FL_DODGE;
	lavaball->clipmask = MASK_PROJECTILE;
	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		lavaball->clipmask &= ~CONTENTS_PLAYER;
	lavaball->solid = SOLID_BBOX;
	lavaball->s.modelindex = gi.modelindex("models/objects/gibs/sm_meat/tris.md2");
	lavaball->owner = self;
	lavaball->touch = lavaball_touch;
	lavaball->nextthink = level.time + gtime_t::from_sec(8000.f / speed);
	lavaball->think = G_FreeEdict;
	lavaball->dmg = damage;
	lavaball->radius_dmg = radius_damage;
	lavaball->dmg_radius = damage_radius;
	lavaball->classname = "lavaball";

	gi.linkentity(lavaball);
}

/*
=================
fire_vorepod

Fires a homing explosive spiked ball. Used by eldritch monsters.
=================
*/

TOUCH(vorepod_touch) (edict_t* ent, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	vec3_t origin;

	if (other == ent->owner)
		return;

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(ent);
		return;
	}

	if (ent->owner->client)
		PlayerNoise(ent->owner, ent->s.origin, PNOISE_IMPACT);

	// calculate position for the explosion entity
	origin = ent->s.origin + tr.plane.normal;

	if (other->takedamage)
	{
		T_Damage(other, ent, ent->owner, ent->velocity, ent->s.origin, tr.plane.normal, ent->dmg, ent->dmg, DAMAGE_NONE, MOD_ROCKET);
	}

	T_RadiusDamage(ent, ent->owner, (float)ent->radius_dmg, other, ent->dmg_radius, DAMAGE_NONE, MOD_R_SPLASH);

	gi.WriteByte(svc_temp_entity);
	if (ent->waterlevel)
		gi.WriteByte(TE_ROCKET_EXPLOSION_WATER);
	else
		gi.WriteByte(TE_ROCKET_EXPLOSION);
	gi.WritePosition(origin);
	gi.multicast(ent->s.origin, MULTICAST_PHS, false);

	G_FreeEdict(ent);
}

THINK(vorepod_think) (edict_t* self) -> void
{
	edict_t* acquire = nullptr;
	float	 oldlen = 0;
	float	 olddot = 1;

	vec3_t fwd = AngleVectors(self->s.angles).forward;

	// try to stay on current target if possible
	if (self->enemy)
	{
		acquire = self->enemy;

		if (acquire->health <= 0 ||
			!visible(self, acquire))
		{
			self->enemy = acquire = nullptr;
		}
	}

	if (!acquire)
	{
		edict_t* target = nullptr;

		// acquire new target
		while ((target = findradius(target, self->s.origin, 1024)) != nullptr)
		{
			if (self->owner == target)
				continue;
			if (!target->client)
				continue;
			if (target->health <= 0)
				continue;
			if (!visible(self, target))
				continue;

			vec3_t vec = self->s.origin - target->s.origin;
			float len = vec.length();

			float dot = vec.normalized().dot(fwd);

			// targets that require us to turn less are preferred
			if (dot >= olddot)
				continue;

			if (acquire == nullptr || dot < olddot || len < oldlen)
			{
				acquire = target;
				oldlen = len;
				olddot = dot;
			}
		}
	}

	if (acquire != nullptr)
	{
		vec3_t vec = (acquire->s.origin - self->s.origin).normalized();
		float t = self->accel;

		float d = self->movedir.dot(vec);

		if (d < 0.45f && d > -0.45f)
			vec = -vec;

		self->movedir = slerp(self->movedir, vec, t).normalized();
		self->s.angles = vectoangles(self->movedir);

		if (self->enemy != acquire)
		{
			self->enemy = acquire;
		}
	}
	else
		self->enemy = nullptr;

	self->velocity = self->movedir * self->speed;
	self->nextthink = level.time + FRAME_TIME_MS;
}

// RAFAEL
void fire_vorepod(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius, int radius_damage, float turn_fraction, int skin)
{
	edict_t* vorepod;

	vorepod = G_Spawn();
	vorepod->s.origin = start;
	vorepod->movedir = dir;
	vorepod->s.angles = vectoangles(dir);
	vorepod->velocity = dir * speed;
	vorepod->flags |= FL_DODGE;
	vorepod->movetype = MOVETYPE_FLYMISSILE;
	vorepod->svflags |= SVF_PROJECTILE;
	vorepod->clipmask = MASK_PROJECTILE;
	vorepod->solid = SOLID_BBOX;
	vorepod->s.effects |= EF_TRACKER;
	vorepod->s.modelindex = gi.modelindex("models/proj/pod/tris.md2");
	vorepod->s.skinnum |= skin; //0 = purple, 1 = red
	vorepod->owner = self;
	vorepod->touch = vorepod_touch;
	vorepod->speed = speed;
	vorepod->accel = turn_fraction;

	vorepod->nextthink = level.time + FRAME_TIME_MS;
	vorepod->think = vorepod_think;

	vorepod->dmg = damage;
	vorepod->radius_dmg = radius_damage;
	vorepod->dmg_radius = damage_radius;

	if (visible(vorepod, self->enemy))
	{
		vorepod->enemy = self->enemy;
	}

	gi.linkentity(vorepod);
}

/*
=================
fire_flame

Fires a flame bolt. Used by eldritch monsters.
=================
*/
TOUCH(flame_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (other == self->owner)
		return;

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(self);
		return;
	}

	// PMM - crash prevention
	if (self->owner && self->owner->client)
		PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);

	if (other->takedamage)
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, tr.plane.normal, self->dmg, 1, DAMAGE_ENERGY, static_cast<mod_id_t>(self->style));
	else
	{
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_SPARKS);
		gi.WritePosition(self->s.origin);
		gi.WriteDir(tr.plane.normal);
		gi.multicast(self->s.origin, MULTICAST_PHS, false);
	}

	G_FreeEdict(self);
}

void fire_flame(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed)
{
	edict_t* flame;
	trace_t	 tr;

	flame = G_Spawn();
	flame->svflags = SVF_PROJECTILE;
	flame->s.origin = start;
	flame->s.old_origin = start;
	flame->s.angles = vectoangles(dir);
	flame->velocity = dir * speed;
	flame->movetype = MOVETYPE_FLYMISSILE;
	flame->clipmask = MASK_PROJECTILE;
	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		flame->clipmask &= ~CONTENTS_PLAYER;
	flame->flags |= FL_DODGE;
	flame->solid = SOLID_BBOX;
	flame->s.effects |= EF_IONRIPPER;
	flame->s.modelindex = gi.modelindex("models/proj/firebolt/tris.md2");
	flame->s.sound = gi.soundindex("monsters/hknight/attack1.wav");
	flame->owner = self;
	flame->touch = flame_touch;
	flame->nextthink = level.time + 2_sec;
	flame->think = G_FreeEdict;
	flame->dmg = damage;
	flame->classname = "flame";
	gi.linkentity(flame);

	tr = gi.traceline(self->s.origin, flame->s.origin, flame, flame->clipmask);
	if (tr.fraction < 1.0f)
	{
		flame->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		flame->touch(flame, tr.ent, tr, false);
	}
}

/*
=================
fire_gib

Fires a gib projectile.  Used by the Zombie.
=================
*/

TOUCH(zombiegib_touch) (edict_t *ent, edict_t *other, const trace_t& tr, bool surf) -> void
{
	edict_t* owner = ent;
	vec3_t normal;

	if (other == ent->owner)
		return;

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict (ent);
		return;
	}

	if (other->takedamage)
	{
		vec3_t dir = other->s.origin - ent->s.origin;
		// FIX MOD
		T_Damage(other, ent, owner, dir, ent->s.origin, normal, ent->dmg, ent->dmg, DAMAGE_NONE, MOD_GEKK);

		gi.sound (ent, CHAN_RELIABLE|CHAN_WEAPON, gi.soundindex ("q1zombie/z_hit.wav"), 1.0, ATTN_NORM, 0);	
	}
	else
	{
		gi.sound (ent, CHAN_RELIABLE|CHAN_WEAPON, gi.soundindex ("q1zombie/z_miss.wav"), 1.0, ATTN_NORM, 0);
		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_BLOOD);
		gi.WritePosition (ent->s.origin);
		gi.WriteDir(tr.plane.normal);
		gi.multicast (ent->s.origin, MULTICAST_PVS, false);
	}
		
//	no more touches	
	ent->touch = nullptr ;

	ent->nextthink = level.time + 3_sec;
	ent->think = G_FreeEdict;
}

void fire_gib (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float right_adjust, float up_adjust)
{
	edict_t	*gib;
	vec3_t	dir;
	vec3_t	 forward, right, up;

	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	gib = G_Spawn();
	gib->s.origin = start;
	gib->velocity = aimdir * speed;

	if (up_adjust)
	{
		float gravityAdjustment = level.gravity / 800.f;

		gib->velocity += up * up_adjust * gravityAdjustment;
	}

	if (right_adjust)
	gib->velocity += right * right_adjust;

	gib->movetype = MOVETYPE_BOUNCE;
	gib->clipmask = MASK_PROJECTILE;
	if (self->client && !G_ShouldPlayersCollide(true))
		gib->clipmask &= ~CONTENTS_PLAYER;
	gib->solid = SOLID_BBOX;
	gib->svflags |= SVF_PROJECTILE;
	gib->flags |= FL_DODGE;
	gib->s.effects |= EF_GIB;
	gib->speed = speed;
	gib->mins = { -6, -6, -6 };
	gib->maxs = { 6, 6, 6 };
	gib->avelocity = { crandom() * 360, crandom() * 360, crandom() * 360 };
	gib->s.modelindex = gi.modelindex ("models/proj/zomgib/tris.md2");
	gib->owner = self;
	gib->touch = zombiegib_touch;
	gib->nextthink = level.time + 2.5_sec;
	gib->think = G_FreeEdict;
	gib->dmg = damage;
	gib->classname = "gib";

	gi.linkentity (gib);
}

/*
=================

monster_fire_multigrenade

=================
*/

void monster_fire_multigrenade(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed,
	monster_muzzleflash_id_t flashtype, float right_adjust, float up_adjust)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		fire_grenade(self, start, aimdir, damage, speed, 2.5_sec, damage + 40.f, right_adjust, up_adjust, true);
		monster_muzzleflash(self, start, flashtype);
		return;
	}
	fire_multigrenade(self, start, aimdir, damage, speed, 2.5_sec, damage + 40.f, right_adjust, up_adjust);
	monster_muzzleflash(self, start, flashtype);
}