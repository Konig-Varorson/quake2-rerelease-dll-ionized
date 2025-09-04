// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

/*
=================
fire_disintegrator
=================
*/

THINK(Dist_Gib_Think) (edict_t* ent) -> void
{
	if (ent->owner->s.frame != 5)
	{
		G_FreeEdict(ent);
		return;
	}

	vec3_t forward, right, up;
	vec3_t vec;

	AngleVectors(ent->owner->s.angles, forward, right, up);

	// rotate us around the center
	float degrees = (720.f * gi.frame_time_s) + ent->owner->delay;
	vec3_t diff = ent->owner->s.origin - ent->s.origin;

	float max_radius = 64.f;
	float current_radius = diff.length();

	if (current_radius > max_radius)
	{
		diff.normalize();
		diff *= max_radius;
	}

	vec = RotatePointAroundVector(up, diff, degrees);
	ent->s.angles[1] += degrees;
	vec3_t new_origin = ent->owner->s.origin - vec;

	trace_t tr = gi.traceline(ent->s.origin, new_origin, ent, MASK_SOLID);
	ent->s.origin = tr.endpos;

	// pull us towards the trap's center
//	diff.normalize();
	//ent->s.origin += diff * (15.0f * gi.frame_time_s);

	ent->watertype = gi.pointcontents(ent->s.origin);
	if (ent->watertype & MASK_WATER)
		ent->waterlevel = WATER_FEET;

	ent->nextthink = level.time + FRAME_TIME_S;
	gi.linkentity(ent);
}

TOUCH(dist_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (other == self->owner)
		return;

	if (other->svflags & SVF_MONSTER || other->client) {
		return;
	}

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

	gi.sound(self, CHAN_VOICE, gi.soundindex("weapons/dist/energy_burst.wav"), 1, ATTN_NORM, 0);
	self->solid = SOLID_NOT;
	self->touch = nullptr;
	self->s.origin += self->velocity * (-1 * gi.frame_time_s);
	self->velocity = {};
	self->enemy = other;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_TRACKER_EXPLOSION);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->nextthink = level.time;
	self->think = G_FreeEdict;
}

THINK(dist_think) (edict_t* ent) -> void
{
	edict_t* target = nullptr;
	edict_t* best = nullptr;
	vec3_t	 vec;
	float	 len;
	float	 oldlen = 8000;

	while ((target = findradius(target, ent->s.origin, 256)) != nullptr)
	{
		if (target == ent)
			continue;

		if (!(target->svflags & SVF_MONSTER) && !target->client)
			continue;
		if (target != ent->teammaster && CheckTeamDamage(target, ent->teammaster))
			continue;
		// [Paril-KEX]
		if (!deathmatch->integer && target->client)
			continue;
		if (target->health <= 0)
			continue;
		if (!visible(ent, target))
			continue;
		vec = ent->s.origin - target->s.origin;
		len = vec.length();
		if (!best)
		{
			best = target;
			oldlen = len;
			continue;
		}
		if (len < oldlen)
		{
			oldlen = len;
			best = target;
		}
	}

	// pull the enemy in
	if (best)
	{
		if (best->groundentity)
		{
			best->s.origin[2] += 1;
			best->groundentity = nullptr;
		}
		vec = ent->s.origin - best->s.origin;
		len = vec.normalize();

		float max_speed = best->client ? 290.f : 150.f;
		float distance = (ent->s.origin - best->s.origin).length();
		float pull_strength = clamp((512.f - distance) / 512.f, 0.2f, 1.f); // scale down with distance
		float pull_speed = (best->client ? 300.f : 200.f) * pull_strength;
		if (best->mass <= 400)
		{
			float mass_factor = clamp(1.0f / (best->mass * 0.0005f), 0.3f, 2.0f);
			best->velocity = vec * (pull_speed * mass_factor);
		}
		else
		{
			float mass_factor = clamp(1.0f / (best->mass * 0.005f), 0.2f, 1.0f);
			best->velocity = vec * (pull_speed * mass_factor);
		}

		//	best->velocity += (vec * clamp(max_speed - len, 64.f, max_speed));

		ent->s.sound = gi.soundindex("weapons/trapsuck.wav");

		if (len < 48)
		{
			if (best->mass <= 400)
			{
				ent->takedamage = false;
				ent->die = nullptr;

				T_Damage(best, ent, ent->teammaster, vec3_origin, best->s.origin, vec3_origin, 100000, 1, DAMAGE_NONE, MOD_TRAP);

				if (best->svflags & SVF_MONSTER)
					M_ProcessPain(best);

				ent->enemy = best;
				ent->wait = 64;
				ent->s.old_origin = ent->s.origin;
				ent->timestamp = level.time + 30_sec;
				ent->accel = best->mass;
				if (deathmatch->integer)
					ent->mass = best->mass / 4;
				else
					ent->mass = best->mass / 10;
				// ok spawn the food cube
				ent->s.frame = 5;

				// link up any gibs that this monster may have spawned
				for (uint32_t i = 0; i < globals.num_edicts; i++)
				{
					edict_t* e = &g_edicts[i];

					if (!e->inuse)
						continue;
					else if (strcmp(e->classname, "gib"))
						continue;
					else if ((e->s.origin - ent->s.origin).length() > 128.f)
						continue;

					e->movetype = MOVETYPE_NONE;
					e->nextthink = level.time + FRAME_TIME_S;
					e->think = Dist_Gib_Think;
					e->owner = ent;
					Dist_Gib_Think(e);
				}
			}
			else
			{
				ent->takedamage = false;
				ent->die = nullptr;

				T_Damage(best, ent, ent->teammaster, vec3_origin, best->s.origin, vec3_origin, 10, 1, DAMAGE_NONE, MOD_TRAP);

				if (best->svflags & SVF_MONSTER)
					M_ProcessPain(best);

				ent->enemy = best;
				ent->wait = 64;
				ent->s.old_origin = ent->s.origin;
				ent->timestamp = level.time + 30_sec;
				ent->accel = best->mass;
				if (deathmatch->integer)
					ent->mass = best->mass / 4;
				else
					ent->mass = best->mass / 10;
				// ok spawn the food cube
				ent->s.frame = 5;

				// link up any gibs that this monster may have spawned
				for (uint32_t i = 0; i < globals.num_edicts; i++)
				{
					edict_t* e = &g_edicts[i];

					if (!e->inuse)
						continue;
					else if (strcmp(e->classname, "gib"))
						continue;
					else if ((e->s.origin - ent->s.origin).length() > 128.f)
						continue;

					e->movetype = MOVETYPE_NONE;
					e->nextthink = level.time + FRAME_TIME_S;
					e->think = Dist_Gib_Think;
					e->owner = ent;
					Dist_Gib_Think(e);
				}
			}
		}
	}

	ent->nextthink = level.time + FRAME_TIME_S;
}

void fire_disintegrator(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius)
{
	edict_t* dist;

	dist = G_Spawn();
	dist->s.origin = start;
	dist->s.angles = vectoangles(dir);
	dist->velocity = dir * speed;
	dist->movetype = MOVETYPE_FLYMISSILE;
	dist->clipmask = MASK_SOLID;
	dist->svflags = SVF_PROJECTILE;
	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		dist->clipmask &= ~CONTENTS_PLAYER;
	dist->solid = SOLID_BBOX;
	dist->s.effects |= EF_TRACKER | EF_ROTATE;
	dist->s.modelindex = gi.modelindex("models/items/spawngro3/tris.md2");
	dist->s.alpha = 0.75;
	dist->s.skinnum = 1;
	dist->s.frame = 2;
	dist->owner = self;
	dist->touch = dist_touch;
	dist->nextthink = level.time + gtime_t::from_sec(8000.f / speed);
	dist->think = G_FreeEdict;
	dist->radius_dmg = damage;
	dist->dmg_radius = damage_radius;
	dist->classname = "dist blast";
	dist->s.sound = gi.soundindex("weapons/dist/energy_loop.wav");

	dist->nextthink = level.time + FRAME_TIME_S;
	dist->think = dist_think;
	dist->teammaster = dist;
	dist->teamchain = nullptr;

	gi.linkentity(dist);
}

/*
=================
fire_lightning

Fires a single lightning  bolt. Used by the discharger.
=================
*/

void fire_lightning(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, effects_t effect)
{
	edict_t* bolt;
	trace_t	 tr;

	bolt = G_Spawn();
	bolt->s.origin = start;
	bolt->s.old_origin = start;
	bolt->s.angles = vectoangles(dir);
	bolt->velocity = dir * speed;
	bolt->svflags |= SVF_PROJECTILE;
	bolt->movetype = MOVETYPE_FLYMISSILE;
	bolt->flags |= FL_DODGE;
	bolt->clipmask = MASK_PROJECTILE;
	bolt->solid = SOLID_BBOX;
	bolt->s.effects |= effect;
	bolt->s.modelindex = gi.modelindex("models/proj/lightning/tris.md2");
	bolt->s.skinnum = 1;
	bolt->s.sound = gi.soundindex("weapons/tesla.wav");
	bolt->owner = self;
	bolt->touch = blaster_touch;
	bolt->nextthink = level.time + 2_sec;
	bolt->think = G_FreeEdict;
	bolt->dmg = damage;
	bolt->classname = "bolt";
	bolt->style = MOD_BLUEBLASTER;
	gi.linkentity(bolt);

	tr = gi.traceline(self->s.origin, bolt->s.origin, bolt, bolt->clipmask);

	if (tr.fraction < 1.0f)
	{
		bolt->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		bolt->touch(bolt, tr.ent, tr, false);
	}
}