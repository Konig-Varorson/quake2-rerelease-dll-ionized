// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"


/*
=================
BossPowerups

universal boss response powerups
=================
*/

void BossPowerArmor(edict_t* self)
{
	self->monsterinfo.power_armor_type = IT_ITEM_POWER_SHIELD;
	if (self->monsterinfo.power_armor_power <= 0)
		self->monsterinfo.power_armor_power += 200 * (skill->integer - 1);
	if (coop->integer)
		self->monsterinfo.power_armor_power += ((25 * skill->integer) + (25 * (CountPlayers() - 1)));
}

void BossRespondPowerup(edict_t* self, edict_t* other)
{
	if (other->s.effects & (EF_QUAD | EF_DOUBLE | EF_DUALFIRE | EF_PENT))
	{
		BossPowerArmor(self);
	}
}

void BossPowerups(edict_t* self)
{
	edict_t* ent;

	if (!coop->integer)
	{
		BossRespondPowerup(self, self->enemy);
	}
	else
	{
		for (uint32_t player = 1; player <= game.maxclients; player++)
		{
			ent = &g_edicts[player];
			if (!ent->inuse)
				continue;
			if (!ent->client)
				continue;
			BossRespondPowerup(self, ent);
		}
	}
}

/* KONIG - old attacks made universal*/

/*
=================
fire_acid

Fires a single acid bolt. Taken from gekk's loogie. Used by natural monsters.
=================
*/

TOUCH(acid_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
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

	if (other->takedamage)
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, tr.plane.normal, self->dmg, 1, DAMAGE_ENERGY, MOD_GEKK);

	gi.sound(self, CHAN_AUTO, gi.soundindex("gek/loogie_hit.wav"), 1.0f, ATTN_NORM, 0);

	G_FreeEdict(self);
};

void fire_acid(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed)
{
	edict_t* acid;
	trace_t	 tr;

	acid = G_Spawn();
	acid->s.origin = start;
	acid->s.old_origin = start;
	acid->s.angles = vectoangles(dir);
	acid->velocity = dir * speed;
	acid->movetype = MOVETYPE_FLYMISSILE;
	acid->clipmask = MASK_PROJECTILE;
	acid->solid = SOLID_BBOX;
	// Paril: this was originally the wrong effect,
	// but it makes it look more acid-y.
	acid->s.effects |= EF_GREENGIB;
	acid->s.renderfx |= RF_FULLBRIGHT;
	acid->s.modelindex = gi.modelindex("models/objects/loogy/tris.md2");
	acid->owner = self;
	acid->touch = acid_touch;
	acid->nextthink = level.time + 2_sec;
	acid->think = G_FreeEdict;
	acid->dmg = damage;
	acid->svflags |= SVF_PROJECTILE;
	gi.linkentity(acid);

	tr = gi.traceline(self->s.origin, acid->s.origin, acid, MASK_PROJECTILE);
	if (tr.fraction < 1.0f)
	{
		acid->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		acid->touch(acid, tr.ent, tr, false);
	}
}

/*
=================
monster_fire_plasma

Fires a phalanx explosive.
=================
*/

void monster_fire_plasma(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius, int radius_damage, monster_muzzleflash_id_t flashtype)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	fire_plasma(self, start, dir, damage, speed, damage_radius, radius_damage);
	monster_muzzleflash(self, start, flashtype);
}

/*
=================
fire_guardian_heat

Destructible heat-seeking rockets
=================
*/

static inline vec3_t heat_guardian_get_dist_vec(edict_t* heat, edict_t* target, float dist_to_target)
{
	return (((target->s.origin + vec3_t{ 0.f, 0.f, target->mins.z }) + (target->velocity * (clamp(dist_to_target / 500.f, 0.f, 1.f)) * 0.5f)) - heat->s.origin).normalized();
}

THINK(heat_guardian_think) (edict_t* self) -> void
{
	edict_t* acquire = nullptr;
	float	 oldlen = 0;
	float	 olddot = 1;

	if (self->timestamp < level.time)
	{
		vec3_t fwd = AngleVectors(self->s.angles).forward;

		if (self->oldenemy)
		{
			self->enemy = self->oldenemy;
			self->oldenemy = nullptr;
		}

		if (self->enemy)
		{
			acquire = self->enemy;

			if (acquire->health <= 0 ||
				!visible(self, acquire))
			{
				self->enemy = acquire = nullptr;
			}
			else
			{
				float dist_to_target = (self->s.origin - acquire->s.origin).normalize();
				self->pos1 = heat_guardian_get_dist_vec(self, acquire, dist_to_target);
			}
		}

		if (!acquire)
		{
			// acquire new target
			edict_t* target = nullptr;

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

				float dist_to_target = (self->s.origin - target->s.origin).normalize();
				vec3_t vec = heat_guardian_get_dist_vec(self, target, dist_to_target);

				float len = vec.normalize();
				float dot = vec.dot(fwd);

				// targets that require us to turn less are preferred
				if (dot >= olddot)
					continue;

				if (acquire == nullptr || dot < olddot || len < oldlen)
				{
					acquire = target;
					oldlen = len;
					olddot = dot;
					self->pos1 = vec;
				}
			}
		}
	}

	vec3_t preferred_dir = self->pos1;

	if (acquire != nullptr)
	{
		if (self->enemy != acquire)
		{
			gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/railgr1a.wav"), 1.f, 0.25f, 0);
			self->enemy = acquire;
		}
	}
	else
		self->enemy = nullptr;

	float t = self->accel;

	if (self->enemy)
		t *= 0.85f;

	float d = self->movedir.dot(preferred_dir);

	self->movedir = slerp(self->movedir, preferred_dir, t).normalized();
	self->s.angles = vectoangles(self->movedir);

	if (self->speed < self->yaw_speed)
	{
		self->speed += self->yaw_speed * gi.frame_time_s;
	}

	self->velocity = self->movedir * self->speed;
	self->nextthink = level.time + FRAME_TIME_MS;
}

DIE(guardian_heat_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	BecomeExplosion1(self);
}

void fire_guardian_heat(edict_t* self, const vec3_t& start, const vec3_t& dir, const vec3_t& rest_dir, int damage, int speed, float damage_radius, int radius_damage, float turn_fraction)
{
	edict_t* heat;

	heat = G_Spawn();
	heat->s.origin = start;
	heat->movedir = dir;
	heat->s.angles = vectoangles(dir);
	heat->velocity = dir * speed;
	heat->movetype = MOVETYPE_FLYMISSILE;
	heat->clipmask = MASK_PROJECTILE;
	heat->flags |= FL_DAMAGEABLE;
	heat->solid = SOLID_BBOX;
	heat->s.effects |= EF_ROCKET;
	heat->s.modelindex = gi.modelindex("models/objects/rocket/tris.md2");
	heat->s.scale = 1.5f;
	heat->owner = self;
	heat->touch = rocket_touch;
	heat->speed = speed / 2;
	heat->yaw_speed = speed * 2;
	heat->accel = turn_fraction;
	heat->pos1 = rest_dir;
	heat->mins = { -5, -5, -5 };
	heat->maxs = { 5, 5, 5 };
	heat->health = 15;
	heat->takedamage = true;
	heat->die = guardian_heat_die;

	heat->nextthink = level.time + 0.20_sec;
	heat->think = heat_guardian_think;

	heat->dmg = damage;
	heat->radius_dmg = radius_damage;
	heat->dmg_radius = damage_radius;
	heat->s.sound = gi.soundindex("weapons/rockfly.wav");

	if (visible(heat, self->enemy))
	{
		heat->oldenemy = self->enemy;
		heat->timestamp = level.time + 0.6_sec;
		gi.sound(heat, CHAN_WEAPON, gi.soundindex("weapons/railgr1a.wav"), 1.f, 0.25f, 0);
	}

	gi.linkentity(heat);
}

/* KONIG - new attacks*/

/*
========================
monster_fire_lightning

fires lightning bolts as projectiles
========================
*/

void monster_fire_lightning(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, monster_muzzleflash_id_t flashtype, effects_t effect)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	fire_lightning(self, start, dir, damage, speed, effect);
	monster_muzzleflash(self, start, flashtype);
}

/*
========================

monster_fire_flakcannon

Fires multiple nails.

========================
*/
void flechette_touch(edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self);

void fire_flakshot(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int hspread, int vspread, int kick)
{
	edict_t* flak;

	vec3_t end, dir, forward, right, up;
	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	float r = crandom() * hspread;
	float u = crandom() * vspread;
	end = forward * 8192; //start + (forward * 8192);
	end += (right * r);
	end += (up * u);

	flak = G_Spawn();
	flak->s.origin = start;
	flak->s.old_origin = start;
	flak->s.angles = dir;
	flak->velocity = end * speed / 8192;
	flak->svflags |= SVF_PROJECTILE;
	flak->movetype = MOVETYPE_FLYMISSILE;
	flak->clipmask = MASK_PROJECTILE;
	flak->flags |= FL_DODGE;

	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		flak->clipmask &= ~CONTENTS_PLAYER;

	flak->solid = SOLID_BBOX;
	flak->s.renderfx = RF_FULLBRIGHT;
	flak->s.modelindex = gi.modelindex("models/proj/flechette/tris.md2");

	flak->owner = self;
	flak->touch = flechette_touch;
	flak->nextthink = level.time + gtime_t::from_sec(8000.f / speed);
	flak->think = G_FreeEdict;
	flak->dmg = damage;
	flak->dmg_radius = (float)kick;

	gi.linkentity(flak);

	trace_t tr = gi.traceline(self->s.origin, flak->s.origin, flak, flak->clipmask);
	if (tr.fraction < 1.0f)
	{
		flak->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		flak->touch(flak, tr.ent, tr, false);
	}
}

void fire_flakcannon(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int hspread, int vspread, int kick, int count)
{
	for (int i = 0; i < count; i++)
		fire_flakshot(self, start, aimdir, damage, speed, hspread, vspread, kick);
}

void monster_fire_flakcannon(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int hspread, int vspread, int count, monster_muzzleflash_id_t flashtype)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		fire_flechette(self, start, aimdir, damage, speed, damage / 2);
		monster_muzzleflash(self, start, flashtype);
		return;
	}
	fire_flakcannon(self, start, aimdir, damage, speed, hspread, vspread, damage, count);
	monster_muzzleflash(self, start, flashtype);
}

/*
========================

monster_fire_flakblaster

Fires multiple blaster shots.

========================
*/
edict_t* fire_flakshot_blaster(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int hspread, int vspread, effects_t effect, int skin)
{
	edict_t* bolt;
	trace_t	 tr;

	vec3_t end, dir, forward, right, up;
	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	float r = crandom() * hspread;
	float u = crandom() * vspread;
	end = forward * 8192; //start + (forward * 8192);
	end += (right * r);
	end += (up * u);

	bolt = G_Spawn();
	bolt->svflags = SVF_PROJECTILE;
	bolt->s.origin = start;
	bolt->s.old_origin = start;
	bolt->s.angles = dir;
	bolt->velocity = end * speed / 8192;
	bolt->movetype = MOVETYPE_FLYMISSILE;
	bolt->clipmask = MASK_PROJECTILE;
	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		bolt->clipmask &= ~CONTENTS_PLAYER;
	bolt->flags |= FL_DODGE;
	bolt->solid = SOLID_BBOX;
	bolt->s.effects |= effect;
	if (skin == 2 && effect)
		bolt->s.effects |= EF_TRACKER;
	bolt->s.modelindex = gi.modelindex("models/objects/laser/tris.md2");
	bolt->s.skinnum |= skin; //0=yellow, 1=blue, 2=green
	bolt->s.sound = gi.soundindex("misc/lasfly.wav");
	bolt->owner = self;
	bolt->touch = blaster_touch;
	bolt->nextthink = level.time + 2_sec;
	bolt->think = G_FreeEdict;
	bolt->dmg = damage;
	bolt->classname = "bolt";
	bolt->style = MOD_BLASTER;
	gi.linkentity(bolt);

	tr = gi.traceline(self->s.origin, bolt->s.origin, bolt, bolt->clipmask);
	if (tr.fraction < 1.0f)
	{
		bolt->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		bolt->touch(bolt, tr.ent, tr, false);
	}

	return bolt;
}

void fire_flakblaster(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int hspread, int vspread, int count, effects_t effect, int skin)
{
	for (int i = 0; i < count; i++)
		fire_flakshot_blaster(self, start, aimdir, damage, speed, hspread, vspread, effect, skin);
}

void monster_fire_flakblaster(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int hspread,
	int vspread, int count, monster_muzzleflash_id_t flashtype, effects_t effect, int skin)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		fire_blaster(self, start, aimdir, damage, speed, effect, MOD_BLASTER);
		monster_muzzleflash(self, start, flashtype);
		return;
	}
	fire_flakblaster(self, start, aimdir, damage, speed, hspread, vspread, count, effect, skin);
	monster_muzzleflash(self, start, flashtype);
}

/*
========================

monster_fire_flakripper

Fires multiple ion ripper shots.

========================
*/
void ionripper_touch(edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self);
void ionripper_sparks(edict_t* self);

void fire_flakshot_ripper(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int hspread, int vspread, int kick, effects_t effect)
{
	edict_t* ion;
	trace_t	 tr;

	vec3_t end, dir, forward, right, up;
	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	float r = crandom() * hspread;
	float u = crandom() * vspread;
	end = forward * 8192; //start + (forward * 8192);
	end += (right * r);
	end += (up * u);

	ion = G_Spawn();
	ion->s.origin = start;
	ion->s.old_origin = start;
	ion->s.angles = dir;
	ion->velocity = end * speed / 8192;
	ion->movetype = MOVETYPE_WALLBOUNCE;
	ion->clipmask = MASK_PROJECTILE;

	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		ion->clipmask &= ~CONTENTS_PLAYER;

	ion->solid = SOLID_BBOX;
	ion->s.effects |= effect;
	ion->svflags |= SVF_PROJECTILE;
	ion->flags |= FL_DODGE;
	ion->s.renderfx |= RF_FULLBRIGHT;
	ion->s.modelindex = gi.modelindex("models/objects/boomrang/tris.md2");
	ion->s.sound = gi.soundindex("misc/lasfly.wav");
	ion->owner = self;
	ion->touch = ionripper_touch;
	ion->nextthink = level.time + 3_sec;
	ion->think = ionripper_sparks;
	ion->dmg = damage;
	ion->dmg_radius = 100;
	gi.linkentity(ion);

	tr = gi.traceline(self->s.origin, ion->s.origin, ion, ion->clipmask);
	if (tr.fraction < 1.0f)
	{
		ion->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		ion->touch(ion, tr.ent, tr, false);
	}
}

void fire_flakripper(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int hspread, int vspread, int kick, int count, effects_t effect)
{
	for (int i = 0; i < count; i++)
		fire_flakshot_ripper(self, start, aimdir, damage, speed, hspread, vspread, kick, effect);
}

void monster_fire_flakripper(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, int kick, int hspread,
	int vspread, int count, monster_muzzleflash_id_t flashtype, effects_t effect)
{
	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		fire_ionripper(self, start, aimdir, damage, speed, effect);
		monster_muzzleflash(self, start, flashtype);
		return;
	}
	fire_flakripper(self, start, aimdir, damage, speed, kick, hspread, vspread, count, effect);
	monster_muzzleflash(self, start, flashtype);
}

/*
=================
fire_dropper
=================
*/

constexpr gtime_t DROPPER_TIME_TO_LIVE = 30_sec;
constexpr float	  DROPPER_DAMAGE_RADIUS = 128;
constexpr int32_t DROPPER_DAMAGE = 3;
constexpr int32_t DROPPER_KNOCKBACK = 8;

constexpr gtime_t DROPPER_ACTIVATE_TIME = 2_sec;
constexpr int32_t DROPPER_EXPLOSION_DAMAGE_MULT = 50; // this is the amount the damage is multiplied by for underwater explosions
constexpr float	  DROPPER_EXPLOSION_RADIUS = 200;

void dropper_remove(edict_t* self)
{
	edict_t* cur, * next;

	self->takedamage = false;
	if (self->teamchain)
	{
		cur = self->teamchain;
		while (cur)
		{
			next = cur->teamchain;
			G_FreeEdict(cur);
			cur = next;
		}
	}
	else if (self->air_finished)
		gi.Com_Print("dropper_mine without a field!\n");

	self->owner = self->teammaster;
	self->enemy = nullptr;

	if ((self->dmg_radius) && (self->dmg > (DROPPER_DAMAGE * DROPPER_EXPLOSION_DAMAGE_MULT)))
		gi.sound(self, CHAN_ITEM, gi.soundindex("items/damage3.wav"), 1, ATTN_NORM, 0);

	Grenade_Explode(self);
}

DIE(dropper_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	dropper_remove(self);
}

void dropper_blow(edict_t* self)
{
	self->dmg *= DROPPER_EXPLOSION_DAMAGE_MULT;
	self->dmg_radius = DROPPER_EXPLOSION_RADIUS;
	dropper_remove(self);
}

TOUCH(dropper_lava) (edict_t* ent, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (tr.contents & (CONTENTS_SLIME | CONTENTS_LAVA))
	{
		dropper_blow(ent);
		return;
	}

	if (frandom() > 0.5f)
		gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/hgrenb1a.wav"), 1, ATTN_NORM, 0);
	else
		gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/hgrenb2a.wav"), 1, ATTN_NORM, 0);
}

THINK(dropper_think)(edict_t* self) -> void
{
	vec3_t f, r, offset, startpoint, spawnpoint;

	// Hardcoded offset
	offset = { -24.f, -124.f, 0.f };

	AngleVectors(self->s.angles, f, r, nullptr);
	if (self->s.scale != 0.0f)
		offset = offset * self->s.scale;

	startpoint = M_ProjectFlashSource(self, offset, f, r);
	startpoint[2] += 10 * (self->s.scale != 0.0f ? self->s.scale : 1.0f);

	vec3_t mins = { -16, -16, -24 };
	vec3_t maxs = { 16,  16,  32 };

	static const char* skill0_monsters[] = {
		"monster_soldier_light",
		"monster_psoldier_light"
	};

	static const char* skill1_monsters[] = {
		"monster_soldier_light",
		"monster_psoldier_light",
		"monster_soldier_cobalt"
	};

	static const char* skill2_monsters[] = {
		"monster_soldier_cobalt",
		"monster_soldier_hypergun",
		"monster_soldier_ripper"
	};

	static const char* skill3_monsters[] = {
		"monster_soldier_hypergun",
		"monster_soldier_ripper",
		"monster_soldier_elite",
		"monster_soldier_special"
	};

	const char** monster_types = nullptr;
	int monster_count = 0;

	switch (skill->integer)
	{
	case 0:
		monster_types = skill0_monsters;
		monster_count = sizeof(skill0_monsters) / sizeof(skill0_monsters[0]);
		break;
	case 1:
		monster_types = skill1_monsters;
		monster_count = sizeof(skill1_monsters) / sizeof(skill1_monsters[0]);
		break;
	case 2:
		monster_types = skill2_monsters;
		monster_count = sizeof(skill2_monsters) / sizeof(skill2_monsters[0]);
		break;
	default:
		monster_types = skill3_monsters;
		monster_count = sizeof(skill3_monsters) / sizeof(skill3_monsters[0]);
		break;
	}

	const char* chosen_monster = monster_types[rand() % monster_count];

	if (FindSpawnPoint(startpoint, mins, maxs, spawnpoint, 32))
	{
		if (CheckGroundSpawnPoint(spawnpoint, mins, maxs, 256, -1))
		{
			edict_t* ent = CreateGroundMonster(spawnpoint, self->s.angles, mins, maxs, chosen_monster, 256);

			if (!ent)
			{
				return;
			}

			ent->nextthink = level.time;
			ent->think(ent);

			gi.sound(ent, CHAN_AUTO, gi.soundindex("medic_commander/monsterspawn1.wav"), 1.0f, ATTN_NORM, 0.0f);

			if (self->enemy && self->enemy->inuse && self->enemy->health > 0)
			{
				ent->enemy = self->enemy;
				FoundTarget(ent);
			}

			vec3_t center;
			center = mins + maxs;
			center = spawnpoint + (center * 1.0f);
			float radius = maxs.length() * 0.5f;
			SpawnGrow_Spawn(center, radius, radius * 2.0f);
		}
	}

	dropper_remove(self);
}

void fire_dropper(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int speed)
{
	edict_t* dropper;
	vec3_t	 dir;
	vec3_t	 forward, right, up;

	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	dropper = G_Spawn();
	dropper->s.origin = start;
	dropper->velocity = aimdir * speed;

	float gravityAdjustment = level.gravity / 800.f;

	dropper->velocity += up * (200 + crandom() * 10.0f) * gravityAdjustment;
	dropper->velocity += right * (crandom() * 10.0f);

	dropper->s.angles = {};
	dropper->movetype = MOVETYPE_BOUNCE;
	dropper->solid = SOLID_BBOX;
	dropper->s.effects |= EF_GRENADE;
	dropper->s.renderfx |= RF_IR_VISIBLE;
	dropper->mins = { -16, -16, -24 };
	dropper->maxs = { 16, 16, 32 };
	dropper->s.modelindex = gi.modelindex("models/objects/dopplebase/tris.md2");
	dropper->owner = self;
	dropper->teammaster = self;

	dropper->enemy = self->enemy; // Needed later for targeting

	dropper->wait = (level.time + DROPPER_TIME_TO_LIVE).seconds();
	dropper->think = dropper_think;
	dropper->nextthink = level.time + DROPPER_ACTIVATE_TIME;

	// blow up on contact with lava & slime code
	dropper->touch = dropper_lava;

	if (deathmatch->integer)
		// PMM - lowered from 50 - 7/29/1998
		dropper->health = 20;
	else
		dropper->health = 50; // FIXME - change depending on skill?

	dropper->takedamage = true;
	dropper->die = dropper_die;
	dropper->classname = "teleport_dropper";
	dropper->flags |= (FL_DAMAGEABLE | FL_TRAP);
	dropper->clipmask = (MASK_PROJECTILE | CONTENTS_SLIME | CONTENTS_LAVA) & ~CONTENTS_DEADMONSTER;

	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		dropper->clipmask &= ~CONTENTS_PLAYER;

	dropper->flags |= FL_MECHANICAL;

	gi.linkentity(dropper);
}


//For enemies weak/strong only to energy weapons
bool IsEnergyWeapon(const mod_t& mod)
{
	switch (mod.id)
	{
	case MOD_BLASTER:
	case MOD_TESLA:
	case MOD_HYPERBLASTER:
	case MOD_RIPPER:
	case MOD_HEATBEAM:
	case MOD_PHALANX:
	case MOD_BFG_LASER:
	case MOD_BFG_BLAST:
	case MOD_BFG_EFFECT:
//	case MOD_SONICCANNON:
//	case MOD_DISCHARGER:
	case MOD_DEFENDER_SPHERE:
	case MOD_TARGET_LASER:
	case MOD_TARGET_BLASTER:
	case MOD_BLASTER2:
	case MOD_BLUEBLASTER:
		return true;
	default:
		return false;
	}
}

//For enemies weak/strong only to energy weapons
bool IsExplosiveWeapon(const mod_t& mod)
{
	switch (mod.id)
	{
	case MOD_GRENADE:
	case MOD_G_SPLASH:
	case MOD_ROCKET:
	case MOD_R_SPLASH:
	case MOD_HANDGRENADE:
	case MOD_HG_SPLASH:
	case MOD_HELD_GRENADE:
	case MOD_EXPLOSIVE:
	case MOD_BARREL:
	case MOD_BOMB:
	case MOD_PHALANX:
//	case MOD_SONICCANNON:
	case MOD_PROX:
	case MOD_NUKE:
	case MOD_DOPPLE_EXPLODE:
//	case MOD_A2K:
//	case MOD_TRIPBOMB:
	case MOD_HUNTER_SPHERE:
		return true;
	default:
		return false;
	}
}