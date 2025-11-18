// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

/*
=================

fire_tblaster

=================
*/
THINK(TBolt_DOTThink) (edict_t* self) -> void
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
	{
		G_FreeEdict(self);
		return;
	}

	if (level.time >= self->timestamp)
	{
		G_FreeEdict(self);
		return;
	}

	T_Damage(self->enemy, self, self->owner, vec3_origin, self->enemy->s.origin, vec3_origin,
		self->dmg, 0, DAMAGE_NO_KNOCKBACK, MOD_BLASTER);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_BLASTER);
	gi.WritePosition(self->enemy->s.origin);
	gi.WriteDir(vec3_origin);
	gi.multicast(self->enemy->s.origin, MULTICAST_PVS, false);

	// Schedule next damage tick (10 ticks per second = 0.1s intervals)
	self->nextthink = level.time + 1000_ms;
}

void TBolt_ApplyDOT(edict_t* target, edict_t* attacker, int damage_per_tick, gtime_t duration)
{
	if (!target->takedamage || target->health <= 0)
		return;

	edict_t* existing_dot = nullptr;
	for (int i = 1; i < globals.num_edicts; i++)
	{
		edict_t* e = &g_edicts[i];
		if (!e->inuse)
			continue;
		if (e->enemy == target && e->owner == attacker && strcmp(e->classname, "tblaster_effect") == 0)
		{
			// Found existing DOT, refresh duration
			existing_dot = e;
			break;
		}
	}

	if (existing_dot)
	{
		// Refresh the duration
		existing_dot->timestamp = level.time + duration;
		return;
	}

	// Create new DOT effect entity
	edict_t* dot = G_Spawn();
	dot->classname = "tblaster_effect";
	dot->enemy = target;      // The entity taking damage
	dot->owner = attacker;    // The entity that fired the blaster
	dot->dmg = damage_per_tick;
	dot->timestamp = level.time + duration;
	dot->think = TBolt_DOTThink;
	dot->nextthink = level.time + 100_ms; // Start first tick after 0.1s
	dot->svflags = SVF_NOCLIENT; // Invisible helper entity
}

TOUCH(TBolt_Touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
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
	{
		// Apply initial impact damage
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, tr.plane.normal,
			self->dmg, 1, DAMAGE_ENERGY, MOD_BLASTER);

		// Apply DOT effect (5 second duration)
		// damage_per_tick can be adjusted - here it's set to deal total damage equal to initial hit over 5 seconds
		// 5 seconds = 50 ticks at 10Hz, so damage_per_tick = self->dmg / 50 for equal total damage
		// Or you can set it to a fixed amount
		int damage_per_tick = max(1, self->dmg); // Deals self->dmg total over 5 seconds (50 ticks)
		TBolt_ApplyDOT(other, self->owner, damage_per_tick, 5_sec);
	}
	else
	{
		// Hit a non-damageable surface
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_BLASTER);
		gi.WritePosition(self->s.origin);
		gi.WriteDir(tr.plane.normal);
		gi.multicast(self->s.origin, MULTICAST_PHS, false);
	}

	G_FreeEdict(self);
}

void fire_tblaster(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, effects_t effect)
{
	edict_t* bolt;
	trace_t  tr;

	bolt = G_Spawn();
	bolt->svflags = SVF_PROJECTILE;
	bolt->s.origin = start;
	bolt->s.old_origin = start;
	bolt->s.angles = vectoangles(dir);
	bolt->velocity = dir * speed;
	bolt->movetype = MOVETYPE_FLYMISSILE;
	bolt->clipmask = MASK_PROJECTILE;

	if (self->client && !G_ShouldPlayersCollide(true))
		bolt->clipmask &= ~CONTENTS_PLAYER;

	bolt->flags |= FL_DODGE;
	bolt->solid = SOLID_BBOX;
	bolt->s.effects |= effect;
	bolt->s.modelindex = gi.modelindex("models/objects/laser/tris.md2");
	bolt->s.sound = gi.soundindex("misc/lasfly.wav");
	bolt->owner = self;
	bolt->touch = TBolt_Touch;
	bolt->nextthink = level.time + 2_sec;
	bolt->think = G_FreeEdict;
	bolt->dmg = damage;
	bolt->classname = "tblaster_bolt";

	bolt->s.effects |= EF_COLOR_SHELL;
	bolt->s.renderfx |= RF_SHELL_RED;

	gi.linkentity(bolt);

	tr = gi.traceline(self->s.origin, bolt->s.origin, bolt, bolt->clipmask);
	if (tr.fraction < 1.0f)
	{
		bolt->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		bolt->touch(bolt, tr.ent, tr, false);
	}
}

/*
=================

fire_railgrenade

=================
*/

constexpr spawnflags_t SPAWNFLAG_GRENADE_HAND = 1_spawnflag;
constexpr spawnflags_t SPAWNFLAG_GRENADE_HELD = 2_spawnflag;

THINK(RailGrenade_Explode) (edict_t* ent) -> void
{
	vec3_t origin;
	mod_t  mod;

	if (ent->owner->client)
		PlayerNoise(ent->owner, ent->s.origin, PNOISE_IMPACT);

	if (ent->spawnflags.has(SPAWNFLAG_GRENADE_HELD))
		mod = MOD_HELD_GRENADE;
	else if (ent->spawnflags.has(SPAWNFLAG_GRENADE_HAND))
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;

	T_RadiusDamage(ent, ent->owner, (float)ent->dmg, nullptr, ent->dmg_radius, DAMAGE_NONE, mod);

	origin = ent->s.origin + (ent->velocity * -0.02f);
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

	vec3_t rail_start = ent->s.origin;

	int rail_damage = 100;
	int rail_kick = 200;

	for (int i = 0; i < 4; i++)
	{
		float horizontal_angle = frandom(360.0f);

		float min_elevation = 10.0f;
		float max_elevation = 60.0f;
		float vertical_angle = min_elevation + frandom(max_elevation - min_elevation);

		float h_rad = horizontal_angle * (PIf / 180.0f);
		float v_rad = vertical_angle * (PIf / 180.0f);

		vec3_t rail_dir;
		rail_dir.x = cos(v_rad) * cos(h_rad);
		rail_dir.y = cos(v_rad) * sin(h_rad);
		rail_dir.z = sin(v_rad);

		rail_dir.normalize();

		fire_rail(ent->owner, rail_start, rail_dir, rail_damage, rail_kick);
	}

	G_FreeEdict(ent);
}

TOUCH(RailGrenade_Touch) (edict_t* ent, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
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
		// Bounce sound
		if (ent->spawnflags.has(SPAWNFLAG_GRENADE_HAND))
		{
			if (frandom() > 0.5f)
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/hgrenb1a.wav"), 1, ATTN_NORM, 0);
			else
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/hgrenb2a.wav"), 1, ATTN_NORM, 0);
		}
		else
		{
			gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);
		}
		return;
	}

	RailGrenade_Explode(ent);
}

THINK(RailGrenade_Think) (edict_t* self) -> void
{
	if (level.time >= self->timestamp)
	{
		RailGrenade_Explode(self);
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

void fire_railgrenade(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, gtime_t timer, float damage_radius, float right_adjust, float up_adjust)
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

	grenade->s.modelindex = gi.modelindex("models/objects/grenade4/tris.md2");
	grenade->s.angles = vectoangles(grenade->velocity);
	grenade->nextthink = level.time + FRAME_TIME_S;
	grenade->timestamp = level.time + timer;
	grenade->think = RailGrenade_Think;
	grenade->s.renderfx |= RF_MINLIGHT;

	grenade->owner = self;
	grenade->touch = RailGrenade_Touch;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "railgrenade";

	gi.linkentity(grenade);
}

//Hand-thrown version
void fire_railgrenade2(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, gtime_t timer, float damage_radius, bool held)
{
	edict_t* grenade;
	vec3_t   dir;
	vec3_t   forward, right, up;

	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	grenade = G_Spawn();
	grenade->s.origin = start;
	grenade->velocity = aimdir * speed;

	// Add randomized upward and side velocity
	float gravityAdjustment = level.gravity / 800.f;
	grenade->velocity += up * (200 + crandom() * 10.0f) * gravityAdjustment;
	grenade->velocity += right * (crandom() * 10.0f);

	grenade->avelocity = { crandom() * 360, crandom() * 360, crandom() * 360 };
	grenade->movetype = MOVETYPE_BOUNCE;
	grenade->clipmask = MASK_PROJECTILE;

	if (self->client && !G_ShouldPlayersCollide(true))
		grenade->clipmask &= ~CONTENTS_PLAYER;

	grenade->solid = SOLID_BBOX;
	grenade->svflags |= SVF_PROJECTILE;
	grenade->flags |= (FL_DODGE | FL_TRAP);
	grenade->s.effects |= EF_GRENADE;

	// Use hand grenade model
	grenade->s.modelindex = gi.modelindex("models/objects/grenade3/tris.md2");
	grenade->owner = self;
	grenade->touch = RailGrenade_Touch;
	grenade->nextthink = level.time + timer;
	grenade->think = RailGrenade_Explode;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "railgrenade";
	grenade->spawnflags = SPAWNFLAG_GRENADE_HAND;

	if (held)
		grenade->spawnflags |= SPAWNFLAG_GRENADE_HELD;

	grenade->s.sound = gi.soundindex("weapons/hgrenc1b.wav");

	if (timer <= 0_ms)
		RailGrenade_Explode(grenade);
	else
	{
		gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/hgrent1a.wav"), 1, ATTN_NORM, 0);
		gi.linkentity(grenade);
	}
}

/*
=================

fire_bfgrenade

=================
*/

THINK(BFGrenade_Explode) (edict_t* ent) -> void
{
	vec3_t origin;
	mod_t  mod;

	if (ent->owner->client)
		PlayerNoise(ent->owner, ent->s.origin, PNOISE_IMPACT);

	if (ent->spawnflags.has(SPAWNFLAG_GRENADE_HELD))
		mod = MOD_HELD_GRENADE;
	else if (ent->spawnflags.has(SPAWNFLAG_GRENADE_HAND))
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;

	T_RadiusDamage(ent, ent->owner, (float)ent->dmg, nullptr, ent->dmg_radius, DAMAGE_NONE, mod);

	origin = ent->s.origin + (ent->velocity * -0.02f);
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

	vec3_t bfg_start = ent->s.origin;
	vec3_t bfg_dir = { 0, 0, 1 };

	int bfg_damage = 200;
	int bfg_speed = 400;
	float bfg_radius = 512;

	fire_bfg(ent->owner, bfg_start, bfg_dir, bfg_damage, bfg_speed, bfg_radius);

	G_FreeEdict(ent);
}

TOUCH(BFGrenade_Touch) (edict_t* ent, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
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
		if (ent->spawnflags.has(SPAWNFLAG_GRENADE_HAND))
		{
			if (frandom() > 0.5f)
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/hgrenb1a.wav"), 1, ATTN_NORM, 0);
			else
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/hgrenb2a.wav"), 1, ATTN_NORM, 0);
		}
		else
		{
			gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);
		}
		return;
	}

	BFGrenade_Explode(ent);
}

THINK(BFGrenade_Think) (edict_t* self) -> void
{
	if (level.time >= self->timestamp)
	{
		BFGrenade_Explode(self);
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

void fire_bfgrenade(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, gtime_t timer, float damage_radius, float right_adjust, float up_adjust)
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

	grenade->s.modelindex = gi.modelindex("models/objects/grenade4/tris.md2");
	grenade->s.angles = vectoangles(grenade->velocity);
	grenade->nextthink = level.time + FRAME_TIME_S;
	grenade->timestamp = level.time + timer;
	grenade->think = BFGrenade_Think;
	grenade->s.renderfx |= RF_MINLIGHT;

	grenade->owner = self;
	grenade->touch = BFGrenade_Touch;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "bfgrenade";

	// Optional: Add a unique sound or visual indicator
	// grenade->s.sound = gi.soundindex("weapons/bfg__l1a.wav");

	gi.linkentity(grenade);
}

//Hand grenade version
void fire_bfgrenade2(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, gtime_t timer, float damage_radius, bool held)
{
	edict_t* grenade;
	vec3_t   dir;
	vec3_t   forward, right, up;

	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	grenade = G_Spawn();
	grenade->s.origin = start;
	grenade->velocity = aimdir * speed;

	// Add randomized upward and side velocity
	float gravityAdjustment = level.gravity / 800.f;
	grenade->velocity += up * (200 + crandom() * 10.0f) * gravityAdjustment;
	grenade->velocity += right * (crandom() * 10.0f);

	grenade->avelocity = { crandom() * 360, crandom() * 360, crandom() * 360 };
	grenade->movetype = MOVETYPE_BOUNCE;
	grenade->clipmask = MASK_PROJECTILE;

	if (self->client && !G_ShouldPlayersCollide(true))
		grenade->clipmask &= ~CONTENTS_PLAYER;

	grenade->solid = SOLID_BBOX;
	grenade->svflags |= SVF_PROJECTILE;
	grenade->flags |= (FL_DODGE | FL_TRAP);
	grenade->s.effects |= EF_GRENADE;

	grenade->s.modelindex = gi.modelindex("models/objects/grenade3/tris.md2");
	grenade->owner = self;
	grenade->touch = BFGrenade_Touch;
	grenade->nextthink = level.time + timer;
	grenade->think = BFGrenade_Explode;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "bfgrenade";
	grenade->spawnflags = SPAWNFLAG_GRENADE_HAND;

	if (held)
		grenade->spawnflags |= SPAWNFLAG_GRENADE_HELD;

	grenade->s.sound = gi.soundindex("weapons/hgrenc1b.wav");

	if (timer <= 0_ms)
		BFGrenade_Explode(grenade);
	else
	{
		gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/hgrent1a.wav"), 1, ATTN_NORM, 0);
		gi.linkentity(grenade);
	}
}

/*
=================

fire_greenflare

Fires a green flare projectile that sticks to surfaces and lights up the area
=================
*/

THINK(Flare_WaterCheck) (edict_t* self) -> void
{
	if (gi.pointcontents(self->s.origin) & MASK_WATER)
	{
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_SPLASH);
		gi.WriteByte(8);
		gi.WritePosition(self->s.origin);
		gi.WriteDir(vec3_origin);

		if (gi.pointcontents(self->s.origin) & CONTENTS_WATER)
			gi.WriteByte(SPLASH_BLUE_WATER);
		else if (gi.pointcontents(self->s.origin) & CONTENTS_SLIME)
			gi.WriteByte(SPLASH_SLIME);
		else if (gi.pointcontents(self->s.origin) & CONTENTS_LAVA)
			gi.WriteByte(SPLASH_LAVA);
		else
			gi.WriteByte(SPLASH_UNKNOWN);

		gi.multicast(self->s.origin, MULTICAST_PVS, false);

		G_FreeEdict(self);
		return;
	}

	self->nextthink = level.time + FRAME_TIME_S;
}


THINK(GreenFlare_Fade) (edict_t* self) -> void
{
	G_FreeEdict(self);
}

TOUCH(GreenFlare_Touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (other == self->owner)
		return;

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(self);
		return;
	}

	if (tr.contents & MASK_WATER)
	{
		// Create splash effect
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_SPLASH);
		gi.WriteByte(8);
		gi.WritePosition(tr.endpos);
		gi.WriteDir(tr.plane.normal);

		// Determine water type for appropriate color
		if (tr.contents & CONTENTS_WATER)
			gi.WriteByte(SPLASH_BLUE_WATER);
		else if (tr.contents & CONTENTS_SLIME)
			gi.WriteByte(SPLASH_SLIME);
		else if (tr.contents & CONTENTS_LAVA)
			gi.WriteByte(SPLASH_LAVA);
		else
			gi.WriteByte(SPLASH_UNKNOWN);

		gi.multicast(tr.endpos, MULTICAST_PVS, false);

		G_FreeEdict(self);
		return;
	}

	if (self->owner && self->owner->client)
		PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);

	if (other->takedamage)
	{
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, tr.plane.normal,
			self->dmg, 1, DAMAGE_ENERGY, MOD_BLASTER);
	}

	// Impact effect
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_BLASTER);
	gi.WritePosition(self->s.origin);
	gi.WriteDir(tr.plane.normal);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	// Convert to stationary light source
	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_NOT;
	self->velocity = vec3_origin;
	self->avelocity = vec3_origin;
	self->touch = nullptr;

	// Position slightly off the surface to prevent z-fighting
	self->s.origin = tr.endpos + (tr.plane.normal * 2.0f);

	self->s.effects |= EF_BFG | EF_BLASTER | EF_TRACKER;
	self->s.renderfx = RF_MINLIGHT | RF_GLOW;

	self->think = GreenFlare_Fade;
	self->nextthink = level.time + 15_sec;

	self->s.sound = gi.soundindex("world/lite_out.wav"); // Gentle humming sound

	gi.linkentity(self);
}

void fire_greenflare(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed)
{
	edict_t* flare;
	trace_t  tr;

	flare = G_Spawn();
	flare->svflags = SVF_PROJECTILE;
	flare->s.origin = start;
	flare->s.old_origin = start;
	flare->s.angles = vectoangles(dir);
	flare->velocity = dir * speed;
	flare->movetype = MOVETYPE_FLYMISSILE;
	flare->clipmask = MASK_PROJECTILE;

	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		flare->clipmask &= ~CONTENTS_PLAYER;

	flare->flags |= FL_DODGE;
	flare->solid = SOLID_BBOX;

	// Green shell effect while flying
	self->s.effects |= EF_BFG | EF_BLASTER | EF_TRACKER;

	flare->s.modelindex = gi.modelindex("models/objects/laser/tris.md2");
	flare->s.sound = gi.soundindex("misc/lasfly.wav");
	flare->owner = self;
	flare->touch = GreenFlare_Touch;
	flare->nextthink = level.time + FRAME_TIME_S;
	flare->think = Flare_WaterCheck;
	flare->dmg = damage;
	flare->classname = "greenflare";

	gi.linkentity(flare);

	// Check for immediate collision
	tr = gi.traceline(self->s.origin, flare->s.origin, flare, flare->clipmask);
	if (tr.fraction < 1.0f)
	{
		flare->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		flare->touch(flare, tr.ent, tr, false);
	}
}

/*
=================

fire_yellowflare

Explosive yellow flare
=================
*/
TOUCH(YellowFlare_Touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	vec3_t origin;

	if (other == self->owner)
		return;

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(self);
		return;
	}

	if (tr.contents & MASK_WATER)
	{
		// Create splash effect
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_SPLASH);
		gi.WriteByte(8);
		gi.WritePosition(tr.endpos);
		gi.WriteDir(tr.plane.normal);

		// Determine water type for appropriate color
		if (tr.contents & CONTENTS_WATER)
			gi.WriteByte(SPLASH_BLUE_WATER);
		else if (tr.contents & CONTENTS_SLIME)
			gi.WriteByte(SPLASH_SLIME);
		else if (tr.contents & CONTENTS_LAVA)
			gi.WriteByte(SPLASH_LAVA);
		else
			gi.WriteByte(SPLASH_UNKNOWN);

		gi.multicast(tr.endpos, MULTICAST_PVS, false);

		G_FreeEdict(self);
		return;
	}

	if (self->owner->client)
		PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);

	origin = self->s.origin + tr.plane.normal;

	if (other->takedamage)
	{
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, tr.plane.normal,
			self->dmg, self->dmg, DAMAGE_NONE, MOD_ROCKET);
	}

	T_RadiusDamage(self, self->owner, (float)self->radius_dmg, other, self->dmg_radius, DAMAGE_NONE, MOD_R_SPLASH);

	gi.WriteByte(svc_temp_entity);
	if (self->waterlevel)
		gi.WriteByte(TE_ROCKET_EXPLOSION_WATER);
	else
		gi.WriteByte(TE_ROCKET_EXPLOSION);
	gi.WritePosition(origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	G_FreeEdict(self);
}

void fire_yellowflare(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius, int radius_damage)
{
	edict_t* flare;
	trace_t  tr;

	flare = G_Spawn();
	flare->svflags = SVF_PROJECTILE;
	flare->s.origin = start;
	flare->s.old_origin = start;
	flare->s.angles = vectoangles(dir);
	flare->velocity = dir * speed;
	flare->movetype = MOVETYPE_FLYMISSILE;
	flare->clipmask = MASK_PROJECTILE;

	// [Paril-KEX]
	if (self->client && !G_ShouldPlayersCollide(true))
		flare->clipmask &= ~CONTENTS_PLAYER;

	flare->flags |= FL_DODGE;
	flare->solid = SOLID_BBOX;

	// Yellow shell effect while flying
	flare->s.effects = EF_BLASTER;

	flare->s.modelindex = gi.modelindex("models/objects/laser/tris.md2");
	flare->s.sound = gi.soundindex("misc/lasfly.wav");
	flare->owner = self;
	flare->touch = YellowFlare_Touch;
	flare->nextthink = level.time + FRAME_TIME_S;
	flare->think = Flare_WaterCheck;
	flare->dmg = damage;
	flare->radius_dmg = radius_damage;
	flare->dmg_radius = damage_radius;
	flare->classname = "yellowflare";

	gi.linkentity(flare);

	tr = gi.traceline(self->s.origin, flare->s.origin, flare, flare->clipmask);
	if (tr.fraction < 1.0f)
	{
		flare->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		flare->touch(flare, tr.ent, tr, false);
	}
}


/*
=================

fire_redflare

Fires a red flare projectile that sticks to surfaces, lights up the area,
and continuously damages nearby entities
=================
*/

THINK(RedFlare_DamageThink) (edict_t* self) -> void
{
	if (level.time >= self->timestamp)
	{
		G_FreeEdict(self);
		return;
	}

	edict_t* ent = nullptr;
	while ((ent = findradius(ent, self->s.origin, self->dmg_radius)) != nullptr)
	{
		if (!ent->takedamage)
			continue;

		if (ent == self->owner)
			continue;

		if (!(ent->svflags & SVF_MONSTER) && !(ent->flags & FL_DAMAGEABLE) && (!ent->client) &&
			(strcmp(ent->classname, "misc_explobox") != 0))
			continue;

		if (CheckTeamDamage(ent, self->owner))
			continue;

		vec3_t v = ent->s.origin - self->s.origin;
		float dist = v.length();
		float damage_scale = 1.0f - (dist / self->dmg_radius);
		int damage_amount = (int)(self->dmg * damage_scale);

		if (damage_amount > 0)
		{
			// Apply damage with no knockback (it's a continuous burn effect)
			T_Damage(ent, self, self->owner, vec3_origin, ent->s.origin, vec3_origin,
				damage_amount, 0, DAMAGE_NO_KNOCKBACK, MOD_BLASTER);
		}
	}

	// Small visual effect to show it's active
	if (irandom(10) == 0) // Only occasionally to avoid spam
	{
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_BLASTER);
		gi.WritePosition(self->s.origin);
		gi.WriteDir(vec3_origin);
		gi.multicast(self->s.origin, MULTICAST_PVS, false);
	}

	self->nextthink = level.time + 10_ms;
}

TOUCH(RedFlare_Touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (other == self->owner)
		return;

	if (gi.pointcontents(self->s.origin) & MASK_WATER)
	{
		// splash and vanish
		G_FreeEdict(self);
		return;
	}

	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(self);
		return;
	}

	if (tr.contents & MASK_WATER)
	{
		// Create splash effect
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_SPLASH);
		gi.WriteByte(8);
		gi.WritePosition(tr.endpos);
		gi.WriteDir(tr.plane.normal);

		// Determine water type for appropriate color
		if (tr.contents & CONTENTS_WATER)
			gi.WriteByte(SPLASH_BLUE_WATER);
		else if (tr.contents & CONTENTS_SLIME)
			gi.WriteByte(SPLASH_SLIME);
		else if (tr.contents & CONTENTS_LAVA)
			gi.WriteByte(SPLASH_LAVA);
		else
			gi.WriteByte(SPLASH_UNKNOWN);

		gi.multicast(tr.endpos, MULTICAST_PVS, false);

		G_FreeEdict(self);
		return;
	}

	if (self->owner && self->owner->client)
		PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);

	if (other->takedamage)
	{
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, tr.plane.normal,
			self->dmg * 2, 1, DAMAGE_ENERGY, MOD_BLASTER);
	}

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_BLASTER);
	gi.WritePosition(self->s.origin);
	gi.WriteDir(tr.plane.normal);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_NOT;
	self->velocity = vec3_origin;
	self->avelocity = vec3_origin;
	self->touch = nullptr;

	self->s.origin = tr.endpos + (tr.plane.normal * 2.0f);

	self->s.effects |= EF_GIB;
	self->s.renderfx = RF_MINLIGHT | RF_GLOW;

	self->timestamp = level.time + 15_sec;

	self->think = RedFlare_DamageThink;
	self->nextthink = level.time + 10_ms;

	// Make it emit sound (fire/burning sound)
	self->s.sound = gi.soundindex("world/fire1.wav");

	gi.linkentity(self);
}

void fire_redflare(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius)
{
	edict_t* flare;
	trace_t  tr;

	flare = G_Spawn();
	flare->svflags = SVF_PROJECTILE;
	flare->s.origin = start;
	flare->s.old_origin = start;
	flare->s.angles = vectoangles(dir);
	flare->velocity = dir * speed;
	flare->movetype = MOVETYPE_FLYMISSILE;
	flare->clipmask = MASK_PROJECTILE;

	if (self->client && !G_ShouldPlayersCollide(true))
		flare->clipmask &= ~CONTENTS_PLAYER;

	flare->flags |= FL_DODGE;
	flare->solid = SOLID_BBOX;

	// Red shell effect while flying
	flare->s.effects = EF_GIB;

	flare->s.modelindex = gi.modelindex("models/objects/laser/tris.md2");
	flare->s.sound = gi.soundindex("misc/lasfly.wav");
	flare->owner = self;
	flare->touch = RedFlare_Touch;
	flare->nextthink = level.time + FRAME_TIME_S;
	flare->think = Flare_WaterCheck;
	flare->dmg = damage;
	flare->dmg_radius = damage_radius;
	flare->classname = "redflare";

	gi.linkentity(flare);

	tr = gi.traceline(self->s.origin, flare->s.origin, flare, flare->clipmask);
	if (tr.fraction < 1.0f)
	{
		flare->s.origin = tr.endpos + (tr.plane.normal * 1.f);
		flare->touch(flare, tr.ent, tr, false);
	}
}

/*
=================

fire_bfghoming

=================
*/

void bfg_spawn_laser(edict_t* self);
void bfg_touch(edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self);

THINK(bfghoming_think) (edict_t* self) -> void
{
	edict_t* acquire = nullptr;
	float	 oldlen = 0;
	float	 olddot = 1;

	vec3_t fwd = AngleVectors(self->s.angles).forward;

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

		while ((target = findradius(target, self->s.origin, 1024)) != nullptr)
		{
			if (self->owner == target)
				continue;
			if (!target->takedamage)
				continue;
			if (!(target->svflags & SVF_MONSTER) && !target->client)
				continue;
			if (target->health <= 0)
				continue;
			if (!visible(self, target))
				continue;
			if (CheckTeamDamage(target, self->owner))
				continue;

			vec3_t vec = self->s.origin - target->s.origin;
			float len = vec.length();

			float dot = vec.normalized().dot(fwd);

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
		float t = self->accel * (self->speed / 500.0f);

		float d = self->movedir.dot(vec);

		if (d < 0.45f && d > -0.45f)
			vec = -vec;

		self->movedir = slerp(self->movedir, vec, t).normalized();
		self->s.angles = vectoangles(self->movedir);

		if (self->enemy != acquire)
		{
			gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/railgr1a.wav"), 1.f, 0.25f, 0);
			self->enemy = acquire;
		}
	}
	else
		self->enemy = nullptr;

	self->velocity = self->movedir * self->speed;

	bfg_spawn_laser(self);

	self->nextthink = level.time + FRAME_TIME_MS;
}

void fire_bfghoming(edict_t* self, const vec3_t& start, const vec3_t& dir, int damage, int speed, float damage_radius, float turn_fraction)
{
	edict_t* bfg;

	bfg = G_Spawn();
	bfg->s.origin = start;
	bfg->movedir = dir;
	bfg->s.angles = vectoangles(dir);
	bfg->velocity = dir * speed;
	bfg->flags |= FL_DODGE;
	bfg->movetype = MOVETYPE_FLYMISSILE;
	bfg->svflags |= SVF_PROJECTILE;
	bfg->clipmask = MASK_PROJECTILE;

	if (self->client && !G_ShouldPlayersCollide(true))
		bfg->clipmask &= ~CONTENTS_PLAYER;

	bfg->solid = SOLID_BBOX;
	bfg->s.effects |= EF_BFG | EF_ANIM_ALLFAST;
	bfg->s.modelindex = gi.modelindex("sprites/s_bfg1.sp2");
	bfg->owner = self;
	bfg->touch = bfg_touch;
	bfg->speed = speed;
	bfg->accel = turn_fraction;

	bfg->nextthink = level.time + 10_hz;
	bfg->think = bfghoming_think;

	bfg->radius_dmg = damage;
	bfg->dmg_radius = damage_radius;
	bfg->s.sound = gi.soundindex("weapons/bfg__l1a.wav");

	if (self->enemy && visible(bfg, self->enemy))
	{
		bfg->enemy = self->enemy;
		gi.sound(bfg, CHAN_WEAPON, gi.soundindex("weapons/railgr1a.wav"), 1.f, 0.25f, 0);
	}

	bfg->teammaster = bfg;
	bfg->teamchain = nullptr;
	bfg->classname = "bfg_homing";

	gi.linkentity(bfg);
}