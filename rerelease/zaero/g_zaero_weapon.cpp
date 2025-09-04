// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

constexpr float FLASH_RANGE = 256;
void FoundTarget(edict_t* self);

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
fire_flare
=================
*/

void flare_flash(edict_t* ent)
{
	edict_t* target;
	float dist;
	float ratio;
	vec3_t delta;

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
		dist = delta.lengthSquared();
		ratio = 1 - (dist / FLASH_RANGE);
		if (ratio < 0)
			ratio = 0;

		if (target->client)
		{
			target->client->flashTime += ratio * 25;
			if (target->client->flashTime > 25)
				target->client->flashTime = 25;
			target->client->flashBase = 30;
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

THINK(flare_think) (edict_t* self) -> void
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

void fire_flare(edict_t* self, const vec3_t& start, const vec3_t& aimdir, int damage, int speed, float damage_radius, int radius_damage,
	float right_adjust, float up_adjust)
{
	edict_t* flare;
	vec3_t   dir;
	vec3_t   forward, right, up;

	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	flare = G_Spawn();
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
	flare->flags |= (FL_DODGE | FL_TRAP);
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

/*
=================
fire_empnuke
=================
*/

bool EMPNukeCheck(edict_t* ent, vec3_t pos)
{
	edict_t* check = nullptr;

	while ((check = G_FindByString<&edict_t::classname>(check, "EMPNukeCenter")) != nullptr)
	{
		vec3_t v;

		if (check->owner != ent)
		{
			v = check->s.origin - pos;

			if (v.length() <= check->dmg)
			{
				return true;
			}
		}
	}

	return false;
}

THINK(empnukeFinish) (edict_t* ent) -> void
{
	G_FreeEdict(ent);
}

THINK(empBlastAnim) (edict_t* ent) -> void
{
	ent->s.frame++;
	ent->s.skinnum++;

	if (ent->s.frame > 5)
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

void fire_empnuke(edict_t* ent, vec3_t center, int radius)
{
	edict_t* empnuke;

	gi.sound(ent, CHAN_VOICE, gi.soundindex("items/empnuke/emp_trg.wav"), 1, ATTN_NORM, 0);

	empnuke = G_Spawn();
	empnuke->owner = ent;
	empnuke->dmg = radius;
	empnuke->s.origin = center;
	empnuke->classname = "EMPNukeCenter";
	empnuke->movetype = MOVETYPE_NONE;
	empnuke->s.modelindex = gi.modelindex("models/objects/b_explode/tris.md2");
	empnuke->s.skinnum = 0;

	empnuke->think = empBlastAnim;
	// Setting nextthink to FRAME_TIME_MS makes the EMP animation barely visible. 10_hz appears to work the best.
	empnuke->nextthink = level.time + 10_hz;
	gi.linkentity(empnuke);
}
