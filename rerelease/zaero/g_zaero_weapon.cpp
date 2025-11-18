// Licensed under the GNU General Public License 2.0.
#include "../g_local.h"

constexpr float FLASH_RANGE = 256;
using search_callback_t = decltype(game_import_t::inPVS);
bool binary_positional_search(const vec3_t &viewer, const vec3_t &start, const vec3_t &end, search_callback_t cb, int32_t num_splits);

/*
=================
Prevent_Weapon_Firing
=================
*/
void Prevent_Weapon_Firing(edict_t *ent)
{
	ent->client->weapon_fire_buffered = false;
    ent->client->latched_buttons &= ~BUTTON_ATTACK;
    ent->client->buttons &= ~BUTTON_ATTACK;
}

/*
=================
angleToward
=================
*/
void angleToward(edict_t* self, vec3_t point, float speed)
{
	vec3_t forward;
	float yaw = 0.0;
	float vel = 0.0;
	vec3_t delta;
	vec3_t destAngles;

	if (!self)
	{
		return;
	}

	delta = point - self->s.origin;
	destAngles = vectoangles(delta);
	self->ideal_yaw = destAngles[YAW];
	self->yaw_speed = speed;
	M_ChangeYaw(self);
	yaw = self->s.angles[YAW];
	self->ideal_yaw = destAngles[PITCH];
	self->s.angles[YAW] = self->s.angles[PITCH];
	M_ChangeYaw(self);
	self->s.angles[PITCH] = self->s.angles[YAW];
	self->s.angles[YAW] = yaw;
	AngleVectors(self->s.angles, forward, nullptr, nullptr);
	vel = self->velocity.length();
	self->velocity = forward * vel;
}

/*
=================
flare_flash
=================
*/
void flare_flash(edict_t *ent)
{
    edict_t *target;
    float dist;
    float ratio;
	float dot;
    vec3_t delta;
    vec3_t forward;

    target = nullptr;

    while (1)
    {
        target = findradius(target, ent->s.origin, FLASH_RANGE);
        if (target == nullptr)
            break;
        if (!target->client && !(target->svflags & SVF_MONSTER))
            continue;
        if (target->deadflag)
            continue;
        if (!visible(ent, target))
            continue;

        delta = ent->s.origin - target->s.origin;
        dist = delta.length();
        ratio = 1 - (dist/FLASH_RANGE);
        if (ratio < 0)
            ratio = 0;

		AngleVectors(target->s.angles, forward, nullptr, nullptr);
        delta.normalize();
        dot = std::max(0.0f, delta.dot(forward));
        ratio *= dot;

        if (target->client)
        {
			// The flash values are 4x from their original. 
			// This results in a more gradual increase/decrease of screen blinding, making it easier on the eyes.
            target->client->flashTime += ratio * 100;
            if (target->client->flashTime > 100)
				target->client->flashTime = 100;
			target->client->flashBase = 120;
        }
        else if ((target->svflags & SVF_MONSTER) && strcmp(target->classname, "monster_zboss") != 0)
		{
			target->monsterinfo.flashTime =
				std::max(target->monsterinfo.flashTime, ratio * 150); // a little bit more advantageous
			target->monsterinfo.flashBase = 50;
			if (target->enemy == nullptr)
			{
				target->enemy = ent->owner;
				FoundTarget(target);
			}
		}
    }
}

/*
=================
flare_think
=================
*/

THINK(flare_think) (edict_t *self) -> void
{
    if (level.time > self->timestamp)
    {
        self->s.effects &= ~EF_ROCKET;
        self->think = G_FreeEdict;
        self->nextthink = level.time + 4_sec;
        self->s.frame = 0;
        self->s.sound = 0;
        return;
    }

    self->s.frame++;

    if (self->s.frame > 14)
        self->s.frame = 5;

    self->s.sound = gi.soundindex("weapons/flare/flarehis.wav");

    flare_flash(self);

    self->nextthink = level.time + 10_hz;
}

/*
=================
fire_flare
=================
*/

void fire_flare(edict_t *self, const vec3_t &start, const vec3_t &aimdir, int damage, int speed, float damage_radius, int radius_damage, 
                float right_adjust, float up_adjust)
{
    edict_t *flare;
    vec3_t   dir;
    vec3_t   forward, right, up;

    dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	flare = G_Spawn();
	flare->s.angles = dir;
	flare->s.origin = start;
	flare->velocity = aimdir * speed;

    if (up_adjust)
	{
		float gravityAdjustment = level.gravity / 800.f;
		flare->velocity += up * up_adjust * gravityAdjustment;
	}

	if (right_adjust)
		flare->velocity += right * right_adjust;

	flare->movetype = MOVETYPE_BOUNCE;
	flare->clipmask = MASK_SHOT;

    if (self->client && !G_ShouldPlayersCollide(true))
		flare->clipmask &= ~CONTENTS_PLAYER;

	flare->solid = SOLID_BBOX;
	flare->svflags |= SVF_PROJECTILE;
	flare->flags |= ( FL_DODGE | FL_TRAP );
	flare->s.effects |= EF_ROCKET;
	flare->speed = speed;
    flare->mins = { -4, -4, -4 };
	flare->maxs = { 4, 4, 4 };
    flare->s.modelindex = gi.modelindex("models/objects/flare/tris.md2");
	flare->owner = self;
	flare->timestamp = level.time + gtime_t::from_sec(8000.f / speed);
	flare->nextthink = level.time + 1_sec;
	flare->think = flare_think;
	flare->dmg = damage;
	flare->radius_dmg = radius_damage;
	flare->dmg_radius = damage_radius;
	flare->classname = "flare";

    gi.linkentity(flare);
}
#if 0
/*
=================
fire_lasertripbomb
=================
*/
bool fire_lasertripbomb(edict_t *self, vec3_t start, vec3_t dir, int damage, int damage_radius, bool quad)
{
	// trace a line
	trace_t tr;
	vec3_t endPos;
	vec3_t _dir;
	edict_t *bomb = nullptr;

	_dir = dir * 64;
	endPos = start + _dir;

	// trace ahead, looking for a wall
	tr = gi.traceline(start, endPos, self, MASK_SHOT);
	if (tr.fraction == 1.0f)
	{
		// not close enough
		return false;
	}

	if (Q_strcasecmp(tr.ent->classname, "worldspawn") != 0)
	{
		return false;
	}

	// create the bomb
	bomb = G_Spawn();
	bomb->s.origin = tr.endpos + (tr.plane.normal * 3.0f);
	bomb->s.angles = vectoangles(tr.plane.normal);
	bomb->owner = self;
	Setup_Bomb(bomb, "ired", damage, damage_radius);
	gi.linkentity(bomb);

	// play a sound
	gi.sound(self, CHAN_VOICE, gi.soundindex("weapons/ired/las_set.wav"), 1, ATTN_NORM, 0);
	return true;
}
#endif
/*
=================
EMPNukeCheck
=================
*/
bool EMPNukeCheck (edict_t *ent, vec3_t pos)
{
	edict_t *check = nullptr;

	while ((check = G_FindByString<&edict_t::classname>(check, "EMPNukeCenter")) != nullptr)
	{
		vec3_t v;

		if (check->owner != ent)
		{
			v = check->s.origin - pos;

			if (v.length() <= check->dmg)
				return true;
		}
	}
	return false;
}

/*
=================
empnukeFinish
=================
*/
THINK (empnukeFinish) (edict_t *ent) -> void
{
	G_FreeEdict(ent);
}

/*
=================
empBlastAnim
=================
*/
THINK(empBlastAnim) (edict_t *ent) -> void 
{
	ent->s.frame++;
	ent->s.skinnum++;

	if(ent->s.frame > 5)
	{
		ent->svflags |= SVF_NOCLIENT;
		ent->s.modelindex = 0;
		ent->s.frame = 0;
		ent->s.skinnum = 0;

		ent->think = empnukeFinish;
		ent->nextthink = level.time + 10_sec;
	}
	else
	{
		ent->nextthink = level.time + 10_hz;
	}
}

/*
=================
fire_empnuke
=================
*/
void fire_empnuke(edict_t *ent, vec3_t center, int radius)
{
	edict_t	*empnuke;

	gi.sound(ent, CHAN_VOICE, gi.soundindex("items/empnuke/emp_trg.wav"), 1, ATTN_NORM, 0);

	empnuke = G_Spawn();
	empnuke->owner = ent;
	empnuke->dmg = radius;
	empnuke->s.origin = center;
	empnuke->classname = "EMPNukeCenter";
	empnuke->movetype = MOVETYPE_NONE;
	empnuke->s.modelindex = gi.modelindex("models/objects/b_explode/tris.md2");
	empnuke->s.skinnum = 0;
	empnuke->s.effects |= EF_ANIM_ALLFAST;

	empnuke->think = empBlastAnim;
	// Setting nextthink to FRAME_TIME_MS makes the EMP animation barely visible. 10_hz appears to work the best.
	empnuke->nextthink = level.time + 10_hz;
	gi.linkentity (empnuke);
}

/*
=================
fire_scannon_effects
=================
*/

void SpawnDamage(int type, const vec3_t &origin, const vec3_t &normal, int damage);

void fire_scannon_effects (edict_t *self)
{
	trace_t		tr;
	vec3_t		forward, right, up;
	vec3_t		offset, v;
	vec3_t		start, end;

	AngleVectors(self->client->v_angle, forward, right, up);

	P_AddWeaponKick(self, self->client->v_forward * -3, { -3.f, 0.f, 0.f });

	offset = { 0.0f, 7.0f, self->viewheight - 8.0f};
	P_ProjectSource (self, self->client->v_angle, offset, start, forward);

	end = start + (forward * 8192);

  	tr = gi.traceline (start, end, self, MASK_SHOT|CONTENTS_SLIME|CONTENTS_LAVA);

	end = tr.endpos + (forward * -5);

	v = { crandom() * 10 - 20, crandom() * 10 - 20, crandom() * 10 - 20};

  	SpawnDamage(TE_SHIELD_SPARKS, end, v, 0);
}

/*
=================
scexplode_think
=================
*/

THINK(scexplode_think) (edict_t *self) -> void
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_ROCKET_EXPLOSION);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	G_FreeEdict(self);
}

/*
=================
fire_scannon
=================
*/

void fire_scannon (edict_t *self, int SC_MAXCELLS)
{
	constexpr int SC_BASEDAMAGE     = 10;        // minimum damage
	constexpr int SC_DAMAGERANGE    = 990;       // maximum damaged range (max damage possible is SC_BASEDAMAGE + SC_DAMAGERANGE)
	constexpr int SC_MAXRADIUS      = 500;       // maximum blast radius
	vec3_t		  start, end, explodepos;
	vec3_t		  dir, forward, right, up;
	vec3_t		  offset;
	trace_t		  tr;
	float 		  damage;
	float 		  radius;

	damage = self->dmg_radius / SC_MAXCELLS;
	radius = damage * SC_MAXRADIUS;
	damage = SC_BASEDAMAGE + (damage * SC_DAMAGERANGE);

	AngleVectors(self->client->v_angle, forward, right, up);

	P_AddWeaponKick(self, self->client->v_forward * -3, { -3.f, 0.f, 0.f });

	offset = { 0.0f, 7.0f, self->viewheight - 8.0f};
	P_ProjectSource(self, self->client->v_angle, offset, start, forward);

	end = start + (forward * 8192);

  	tr = gi.traceline(start, end, self, MASK_SHOT|CONTENTS_SLIME|CONTENTS_LAVA);

	dir = tr.endpos - start;
	dir.normalize();

	if ((tr.ent != self) && (tr.ent->takedamage))
	{
		T_Damage(tr.ent, self, self, dir, tr.endpos, tr.plane.normal, damage, 0, DAMAGE_NONE, MOD_SONICCANNON);
	}

	// Create a temporary entity at the impact point for radius damage
    edict_t *blast = G_Spawn();
    blast->s.origin = tr.endpos;
    blast->owner = self;
    blast->dmg = damage;
    blast->dmg_radius = radius;
    blast->classname = "scannon_blast";

	// Apply radius damage from this entity
	T_RadiusDamage(blast, self, damage, nullptr, radius, DAMAGE_NONE, MOD_SONICCANNON);

	blast->nextthink = level.time + 100_ms;
    blast->think = G_FreeEdict;

	end = tr.endpos + (forward * -5);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_ROCKET_EXPLOSION);
	gi.WritePosition(end);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	damage -= 100;
	radius = 0.1;

	while (damage > 0)
	{
		edict_t	*explode;

		explodepos = end + (forward * (50 * crandom() - 5));
		explodepos += (right * (50 * crandom() - 5));
		explodepos += (up * (50 * crandom() - 5));

		explode = G_Spawn();
		explode->s.origin = explodepos;
		explode->classname = "scannon_explode";
		explode->nextthink = level.time + gtime_t::from_sec(radius);
		explode->think = scexplode_think;

		radius += 0.1;
		damage -= 100;
	}
}

/*
=================
fire_sniper_bullet_pierce_t
=================
*/

struct fire_sniper_bullet_pierce_t : pierce_args_t
{
	edict_t *self;
	vec3_t	 aimdir;
	int		 damage;
	int		 kick;
	bool	 water = false;

	inline fire_sniper_bullet_pierce_t(edict_t *self, vec3_t aimdir, int damage, int kick) :
		pierce_args_t(),
		self(self),
		aimdir(aimdir),
		damage(damage),
		kick(kick)
	{
	}

	// we hit an entity; return false to stop the piercing.
	// you can adjust the mask for the re-trace (for water, etc).
	bool hit(contents_t &mask, vec3_t &end) override
	{
		if (tr.contents & (CONTENTS_SLIME | CONTENTS_LAVA | CONTENTS_WINDOW))
		{
			mask &= ~(CONTENTS_SLIME | CONTENTS_LAVA | CONTENTS_WINDOW);
			water = true;
			return true;
		}
		else
		{
			// try to kill it first
			if ((tr.ent != self) && (tr.ent->takedamage))
				T_Damage(tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal, damage, kick, DAMAGE_NONE, MOD_SNIPERRIFLE);

			// dead, so we don't need to care about checking pierce
			if (!tr.ent->inuse || (!tr.ent->solid || tr.ent->solid == SOLID_TRIGGER))
				return true;

			// ZOID--added so rail goes through SOLID_BBOX entities (gibs, etc)
			if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client) ||
				// ROGUE
				(tr.ent->flags & FL_DAMAGEABLE) ||
				// ROGUE
				// Zaero - pierce through Plasma Shield
				(Q_strcasecmp(tr.ent->classname, "PlasmaShield") == 0) ||
				// Zaero
				(tr.ent->solid == SOLID_BBOX))
			{
				if (!mark(tr.ent))
					return false;

				return true;
			}
		}

		return false;
	}
};

/*
=================
fire_sniper_bullet
=================
*/

void fire_sniper_bullet (edict_t *self, const vec3_t &start, const vec3_t &aimdir, int damage, int kick)
{
	fire_sniper_bullet_pierce_t args = {
		self,
		aimdir,
		damage,
		kick
	};

	contents_t mask = MASK_PROJECTILE | CONTENTS_SLIME | CONTENTS_LAVA | CONTENTS_WINDOW;
	
	if (self->client && !G_ShouldPlayersCollide(true))
		mask &= ~CONTENTS_PLAYER;

	vec3_t end = start + (aimdir * 8192);

	pierce_trace(start, end, self, args, mask);

	uint32_t unicast_key = GetUnicastKey();
    
    for (auto player : active_players())
    {
        vec3_t org = player->s.origin + player->client->ps.viewoffset + vec3_t{ 0, 0, (float) player->client->ps.pmove.viewheight };
        if (binary_positional_search(org, start, args.tr.endpos, gi.inPHS, 3))
        {
            gi.WriteByte(svc_temp_entity);
            gi.WriteByte(TE_RAILTRAIL2);
            gi.WritePosition(start);
            gi.WritePosition(args.tr.endpos);
            gi.unicast(player, false, unicast_key);
        }
    }

	gi.WriteByte(svc_temp_entity);
	if (self->waterlevel)
	{
		if (self->groundentity)
			gi.WriteByte(TE_GRENADE_EXPLOSION_WATER);
		else
			gi.WriteByte(TE_ROCKET_EXPLOSION_WATER);
	}
	else
	{
		if (self->groundentity)
			gi.WriteByte(TE_GRENADE_EXPLOSION);
		else
			gi.WriteByte(TE_ROCKET_EXPLOSION);
	}
	gi.WritePosition(args.tr.endpos);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	if (self->client)
		PlayerNoise(self, args.tr.endpos, PNOISE_IMPACT);

	return;
}

/*
=================
push_hit
=================
*/

bool push_hit (edict_t *self, vec3_t start, vec3_t aim, int damage, int kick)
{
	trace_t tr;
	vec3_t end;
	vec3_t v;

	//see if enemy is in range
	end = start + (aim * 64);
	tr = gi.traceline(start, end, self, MASK_SHOT);
	if (tr.fraction >= 1.0f)
		return false;

	// play sound
	gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/push/contact.wav"), 1, ATTN_NORM, 0);

	if (tr.ent->svflags & SVF_MONSTER || tr.ent->client)
	{
		// do our special form of knockback here
		v = tr.ent->absmin + (tr.ent->size * 0.75f);
		v -= start;
		v.normalize();
		tr.ent->velocity += v * kick;
		if (tr.ent->velocity[2] > 0)
			tr.ent->groundentity = nullptr;
	}
	else if (tr.ent->movetype == MOVETYPE_FALLFLOAT)
	{
		if (tr.ent->touch)
		{
			float mass = tr.ent->mass;
			tr.ent->mass *= 0.05f;
			tr.ent->touch(tr.ent, self, tr, true);
			tr.ent->mass = mass;
		}
	}

	// ok, we hit something, damage it
	if (!tr.ent->takedamage)
		return false;

	// do the damage
	T_Damage (tr.ent, self, self, aim, tr.endpos, vec3_origin, damage, kick/2, DAMAGE_NO_KNOCKBACK, MOD_HIT);

	return true;
}

/*
=================

Plasma Cannon

=================
*/

THINK(PlasmaballBlastAnim) (edict_t* ent) -> void
{
	ent->s.frame++;
	ent->s.skinnum++;

	if (ent->s.frame > 1)
	{
		G_FreeEdict(ent);
		return;
	}
	else
	{
		ent->nextthink = level.time + 0.1_sec;
	}
}

THINK(Plasmaball_Explode) (edict_t* ent) -> void
{
	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (ent->enemy)
	{
		float	points;
		vec3_t	v;
		vec3_t	dir;

		v = ent->enemy->mins + ent->enemy->maxs;
		v = ent->enemy->s.origin + (v * 0.5);
		v = ent->s.origin - v;
		points = ent->dmg - 0.5 * v.length();
		dir = ent->enemy->s.origin - ent->s.origin;
		T_Damage(ent->enemy, ent, ent->owner, dir, ent->s.origin, vec3_origin, (int)points, (int)points, DAMAGE_RADIUS, MOD_UNKNOWN);
	}

	T_RadiusDamage(ent, ent->owner, ent->dmg, ent->enemy, ent->dmg_radius, DAMAGE_ENERGY, MOD_UNKNOWN);

	ent->s.origin = ent->s.origin + (ent->velocity * -0.02);
	ent->velocity = {};

	ent->movetype = MOVETYPE_NONE;
	ent->s.modelindex = gi.modelindex("models/objects/b_explode/tris.md2");
	ent->s.effects &= ~EF_BFG & ~EF_ANIM_ALLFAST;
	ent->s.frame = 0;
	ent->s.skinnum = 6;

	gi.sound(ent, CHAN_AUTO, gi.soundindex("bossz/bpbexplode.wav"), 1, ATTN_NORM, 0);

	ent->think = PlasmaballBlastAnim;
	ent->nextthink = level.time + 0.1_sec;
}

TOUCH(Plasmaball_Touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(self);
		return;
	}

	self->enemy = other;
	Plasmaball_Explode(self);
}

void fire_plasmacannon(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int speed, float damage_radius, float distance)
{
	edict_t* plasmaball;
	vec3_t	dir;
	vec3_t	forward, right, up;

	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	plasmaball = G_Spawn();
	plasmaball->s.origin = start;
	plasmaball->velocity = aimdir * speed;
	plasmaball->velocity = plasmaball->velocity + (up * ((distance - 500) + crandom() * 10.0));
	plasmaball->velocity = plasmaball->velocity + (right * (crandom() * 10.0));
	plasmaball->avelocity = { 300, 300, 300 };
	plasmaball->movetype = MOVETYPE_BOUNCE;
	plasmaball->clipmask = MASK_SHOT;
	plasmaball->solid = SOLID_BBOX;
	plasmaball->mins = {};
	plasmaball->maxs = {};
	plasmaball->s.modelindex = gi.modelindex("sprites/plasma1.sp2");
	plasmaball->s.effects = EF_BFG | EF_ANIM_ALLFAST;
	plasmaball->owner = self;
	plasmaball->touch = Plasmaball_Touch;
	plasmaball->nextthink = level.time + 2.5_sec;
	plasmaball->think = Plasmaball_Explode;
	plasmaball->dmg = damage;
	plasmaball->dmg_radius = damage_radius;
	plasmaball->classname = "plasmaball";
	plasmaball->s.sound = gi.soundindex("bossz/bpbfly.wav");

	gi.sound(self, CHAN_AUTO, gi.soundindex("bossz/bpbfire.wav"), 1, ATTN_NORM, 0);
	gi.linkentity(plasmaball);
}