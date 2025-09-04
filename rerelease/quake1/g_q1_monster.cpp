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

edict_t* fire_lavaball(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius, int radius_damage)
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

	return lavaball;
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

edict_t* fire_flame(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed)
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

	return flame;
}

/*
=================
q1_fire_gib

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
fire_plasmaball

Fires a ball of lightning that explodes on impact. Magical partial-BFG.
=================
*/

THINK(plasmaball_explode) (edict_t* self) -> void
{
	edict_t* ent;
	float	 points;
	vec3_t	 v;
	float	 dist;

	if (self->s.frame == 0)
	{
		// the BFG effect
		ent = nullptr;
		while ((ent = findradius(ent, self->s.origin, self->dmg_radius)) != nullptr)
		{
			if (!ent->takedamage)
				continue;
			if (ent == self->owner)
				continue;
			if (!CanDamage(ent, self))
				continue;
			if (!CanDamage(ent, self->owner))
				continue;
			// ROGUE - make tesla hurt by bfg
			if (!(ent->svflags & SVF_MONSTER) && !(ent->flags & FL_DAMAGEABLE) && (!ent->client) && (strcmp(ent->classname, "misc_explobox") != 0))
				continue;
			// ZOID
			// don't target players in CTF
			if (CheckTeamDamage(ent, self->owner))
				continue;
			// ZOID

			v = ent->mins + ent->maxs;
			v = ent->s.origin + (v * 0.5f);
			vec3_t centroid = v;
			v = self->s.origin - centroid;
			dist = v.length();
			points = self->radius_dmg * (1.0f - sqrtf(dist / self->dmg_radius));

			T_Damage(ent, self, self->owner, self->velocity, centroid, vec3_origin, (int)points, 0, DAMAGE_ENERGY, MOD_BFG_EFFECT);

			// Paril: draw BFG lightning laser to enemies
			gi.WriteByte(svc_temp_entity);
			gi.WriteByte(TE_LIGHTNING);
			gi.WriteEntity(self);	// source entity
			gi.WriteEntity(world); // destination entity
			gi.WritePosition(self->s.origin);
			gi.WritePosition(centroid);
			gi.multicast(self->s.origin, MULTICAST_PHS, false);
		}
	}

	self->nextthink = level.time;
	self->think = G_FreeEdict;
}

TOUCH(plasmaball_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (other == self->owner)
		return;

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(self);
		return;
	}

	if (self->owner->client)
		PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);

	// core explosion - prevents firing it into the wall/floor
	if (other->takedamage)
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, tr.plane.normal, 200, 0, DAMAGE_ENERGY, MOD_BFG_BLAST);
	T_RadiusDamage(self, self->owner, 200, other, 100, DAMAGE_ENERGY, MOD_BFG_BLAST);

	gi.sound(self, CHAN_VOICE, gi.soundindex("weapons/bfg__x1b.wav"), 1, ATTN_NORM, 0);
	self->solid = SOLID_NOT;
	self->touch = nullptr;
	self->s.origin += self->velocity * (-1 * gi.frame_time_s);
	self->velocity = {};
//	self->s.modelindex = gi.modelindex("sprites/s_bfg3.sp2");
	self->s.frame = 0;
	self->s.sound = 0;
	self->s.effects &= ~EF_ANIM_ALLFAST;
	self->think = plasmaball_explode;
	self->nextthink = level.time + 10_hz;
	self->enemy = other;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_NUKEBLAST);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);
}

void fire_plasmaball(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius)
{
	edict_t* plasma;

	plasma = G_Spawn();
	plasma->s.origin = start;
	plasma->s.angles = vectoangles(dir);
	plasma->velocity = dir * speed;
	plasma->svflags = SVF_PROJECTILE;
	plasma->movetype = MOVETYPE_FLYMISSILE;
	plasma->clipmask = MASK_PROJECTILE;
	plasma->flags |= FL_DODGE;

	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		plasma->clipmask &= ~CONTENTS_PLAYER;

	plasma->solid = SOLID_BBOX;
	plasma->s.effects |= EF_PLASMA;
	plasma->s.modelindex = gi.modelindex("models/proj/plasma/tris.md2");
	plasma->touch = plasmaball_touch;

	plasma->owner = self;
	plasma->nextthink = level.time + gtime_t::from_sec(8000.f / speed);
	plasma->think = G_FreeEdict;
	plasma->radius_dmg = damage;
	plasma->dmg_radius = damage_radius;
	plasma->classname = "plasma blast";
	plasma->s.sound = gi.soundindex("weapons/plasma__l1a.wav");

	plasma->teammaster = plasma;
	plasma->teamchain = nullptr;

	gi.linkentity(plasma);
}

/*
=================
fire_multigrenade

Fires a grenade that splits into four smaller grenades. Used by multigrenade ogre.
=================
*/
//PLACEHOLDER - Functions like grenades for now.
static void MultiGrenade_ExplodeReal(edict_t* ent, edict_t* other, vec3_t normal)
{
	vec3_t origin;
	mod_t  mod;

	if (ent->owner->client)
		PlayerNoise(ent->owner, ent->s.origin, PNOISE_IMPACT);

	// FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (other)
	{
		vec3_t dir = other->s.origin - ent->s.origin;
		mod = MOD_GRENADE;
		T_Damage(other, ent, ent->owner, dir, ent->s.origin, normal, ent->dmg, ent->dmg, mod.id == MOD_HANDGRENADE ? DAMAGE_RADIUS : DAMAGE_NONE, mod);
	}

	mod = MOD_G_SPLASH;
	T_RadiusDamage(ent, ent->owner, (float)ent->dmg, other, ent->dmg_radius, DAMAGE_NONE, mod);

	origin = ent->s.origin + normal;
	gi.WriteByte(svc_temp_entity);
	if (ent->waterlevel)
	{
		if (ent->groundentity)
			gi.WriteByte(TE_GRENADE_EXPLOSION_WATER);
		else
			gi.WriteByte(TE_ROCKET_EXPLOSION_WATER);
	}
	else
	{
		if (ent->groundentity)
			gi.WriteByte(TE_GRENADE_EXPLOSION);
		else
			gi.WriteByte(TE_ROCKET_EXPLOSION);
	}
	gi.WritePosition(origin);
	gi.multicast(ent->s.origin, MULTICAST_PHS, false);

	G_FreeEdict(ent);
}

THINK(MultiGrenade_Explode) (edict_t* ent) -> void
{
	MultiGrenade_ExplodeReal(ent, nullptr, ent->velocity * -0.02f);
}

TOUCH(MultiGrenade_Touch) (edict_t* ent, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (other == ent->owner)
		return;

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(ent);
		return;
	}

	if (!other->takedamage)
	{
		gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);
	
		return;
	}

	MultiGrenade_ExplodeReal(ent, other, tr.plane.normal);
}

void fire_multigrenade(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, gtime_t timer, float damage_radius, float right_adjust, float up_adjust, bool monster)
{
	edict_t* grenade;
	vec3_t	 dir;
	vec3_t	 forward, right, up;

	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	grenade = G_Spawn();
	grenade->s.origin = start;
	grenade->velocity = aimdir * speed;

	if (up_adjust)
	{
		float gravityAdjustment = level.gravity / 800.f;
		grenade->velocity += up * up_adjust * gravityAdjustment;
	}

	if (right_adjust)
		grenade->velocity += right * right_adjust;

	grenade->movetype = MOVETYPE_BOUNCE;
	grenade->clipmask = MASK_PROJECTILE;
	if (self->client && !G_ShouldPlayersCollide(true))
		grenade->clipmask &= ~CONTENTS_PLAYER;
	grenade->solid = SOLID_BBOX;
	grenade->svflags |= SVF_PROJECTILE;
	grenade->flags |= (FL_DODGE | FL_TRAP);
	grenade->s.effects |= EF_GRENADE;
	grenade->speed = speed;
	grenade->avelocity = { crandom() * 360, crandom() * 360, crandom() * 360 };
	grenade->s.modelindex = gi.modelindex("models/objects/grenade/tris.md2");
	grenade->nextthink = level.time + timer;
	grenade->think = MultiGrenade_Explode;
	grenade->s.effects |= EF_GRENADE_LIGHT;
	grenade->owner = self;
	grenade->touch = MultiGrenade_Touch;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "grenade";

	gi.linkentity(grenade);
}

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
	fire_multigrenade(self, start, aimdir, damage, speed, 2.5_sec, damage + 40.f, right_adjust, up_adjust, true);
	monster_muzzleflash(self, start, flashtype);
}