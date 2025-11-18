// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"


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
void Grenade_Touch (edict_t* ent, edict_t* other, const trace_t& tr, bool other_touching_self);

THINK(MultiGrenade_Split) (edict_t* self) -> void
{
	vec3_t vel;

	if (self->owner && self->owner->client)
		PlayerNoise(self->owner, self->s.origin, PNOISE_WEAPON);

	for (int i = 0; i < 4; i++)
	{
		edict_t* mini = G_Spawn();
		mini->s.origin = self->s.origin;
		mini->s.origin[2] += 16;

		vel[0] = crandom() * 300;
		vel[1] = crandom() * 300;
		vel[2] = 200 + frandom() * 200;

		mini->velocity = vel;
		mini->avelocity = { crandom() * 360, crandom() * 360, crandom() * 360 };
		mini->movetype = MOVETYPE_BOUNCE;
		mini->clipmask = MASK_PROJECTILE;

		if (self->owner && self->owner->client && !G_ShouldPlayersCollide(true))
			mini->clipmask &= ~CONTENTS_PLAYER;

		mini->solid = SOLID_BBOX;
		mini->svflags |= SVF_PROJECTILE;
		mini->flags |= (FL_DODGE | FL_TRAP);
		mini->s.effects |= EF_GRENADE;
		mini->s.modelindex = gi.modelindex("models/objects/grenade2/tris.md2");
		mini->owner = self->owner;
		mini->teammaster = self->teammaster;
		mini->touch = Grenade_Touch;
		mini->nextthink = level.time + 1_sec + gtime_t::from_ms(frandom() * 1000);
		mini->think = Grenade_Explode;
		mini->dmg = self->dmg / 2;
		mini->dmg_radius = self->dmg_radius * 0.75f;
		mini->classname = "mini_grenade";
		mini->s.renderfx |= RF_MINLIGHT;

		gi.linkentity(mini);
	}

	vel[0] = crandom() * 300;
	vel[1] = crandom() * 300;
	vel[2] = 200 + frandom() * 200;

	self->velocity = vel;
	self->avelocity = { crandom() * 360, crandom() * 360, crandom() * 360 };
	self->s.modelindex = gi.modelindex("models/objects/grenade2/tris.md2");
	self->touch = Grenade_Touch;
	self->nextthink = level.time + 1_sec + gtime_t::from_ms(frandom() * 1000);
	self->think = Grenade_Explode;
	self->dmg = self->dmg / 2;
	self->dmg_radius = self->dmg_radius * 0.75f;
	self->classname = "mini_grenade";

	gi.linkentity(self);
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

	MultiGrenade_Split(ent);
}

THINK(MultiGrenade_Think) (edict_t* self) -> void
{
	if (level.time >= self->timestamp)
	{
		MultiGrenade_Split(self);
		return;
	}

	if (self->velocity)
	{
		float p = self->s.angles.x;
		float z = self->s.angles.z;
		float speed_frac = clamp(self->velocity.lengthSquared() / (self->speed * self->speed), 0.f, 1.f);
		self->s.angles = vectoangles(self->velocity);
		self->s.angles.x = LerpAngle(p, self->s.angles.x, speed_frac);
		self->s.angles.z = z + (gi.frame_time_s * 360 * speed_frac);
	}

	self->nextthink = level.time + FRAME_TIME_S;
}

void fire_multigrenade(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, gtime_t timer, float damage_radius, float right_adjust, float up_adjust)
{
	edict_t* grenade;
	vec3_t   dir;
	vec3_t   forward, right, up;

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
	grenade->s.modelindex = gi.modelindex("models/objects/grenade/tris.md2");
	grenade->s.angles = vectoangles(grenade->velocity);
	grenade->nextthink = level.time + FRAME_TIME_S;
	grenade->timestamp = level.time + timer;
	grenade->think = MultiGrenade_Think;
	grenade->s.renderfx |= RF_MINLIGHT;
	grenade->owner = self;
	grenade->teammaster = self;
	grenade->touch = Grenade_Touch; //MultiGrenade_Touch
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "multigrenade";

	gi.linkentity(grenade);
}