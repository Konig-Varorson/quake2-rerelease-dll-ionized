// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

SENTINEL

==============================================================================
*/

#include "../g_local.h"
#include "m_oblivion_sentinel.h"
#include "../m_flash.h"

static cached_soundindex sound_pain;
static cached_soundindex sound_death;
static cached_soundindex sound_sight;
static cached_soundindex sound_idle;
static cached_soundindex sound_attack;
static cached_soundindex sound_step;

//
//SOUNDS
//

MONSTERINFO_IDLE(sentinel_idle) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SEARCH(sentinel_search) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(sentinel_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

void sentinel_footstep(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_step, 0.5f, ATTN_IDLE, 0.0f);
}

//
//STAND
//

mframe_t sentinel_frames_stand[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(sentinel_move_stand) = { FRAME_stand1, FRAME_stand20, sentinel_frames_stand, nullptr };

MONSTERINFO_STAND(sentinel_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &sentinel_move_stand);
}

//
//WALKRUN
//

mframe_t sentinel_frames_walk[] = {
	{ ai_walk, 4 },
	{ ai_walk, 5 },
	{ ai_walk, 3 },
	{ ai_walk, 2 },
	{ ai_walk, 5 },

	{ ai_walk, 5 },
	{ ai_walk, 4, monster_footstep },
	{ ai_walk, 3 },
	{ ai_walk, 5 },
	{ ai_walk, 4 },

	{ ai_walk, 5 },
	{ ai_walk, 7 },
	{ ai_walk, 6 },
	{ ai_walk, 6, monster_footstep }
};
MMOVE_T(sentinel_move_walk) = { FRAME_walk1, FRAME_walk14, sentinel_frames_walk, nullptr };

MONSTERINFO_WALK(sentinel_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &sentinel_move_walk);
}

mframe_t sentinel_frames_run[] = {
	{ ai_run, 8, monster_footstep },
	{ ai_run, 10 },
	{ ai_run, 6 },
	{ ai_run, 10 },
	{ ai_run, 8, monster_footstep },

	{ ai_run, 8 },
	{ ai_run, 10 },
	{ ai_run, 8 }
};
MMOVE_T(sentinel_move_run) = { FRAME_run1, FRAME_run8, sentinel_frames_run, nullptr };


MONSTERINFO_RUN(sentinel_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &sentinel_move_stand);
		return;
	}

	M_SetAnimation(self, &sentinel_move_run);
}

//
//ATTACK
//

void sentinel_rocket(edict_t* self, monster_muzzleflash_id_t mz)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	trace_t trace; // PMM - check target
	int radius_damage = 45;
	vec3_t target;
	bool   blindfire = false;

	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
		blindfire = true;
	else
		blindfire = false;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[mz], forward, right);

	if (blindfire)
		target = self->monsterinfo.blind_fire_target;
	else
		target = self->enemy->s.origin;

	if (blindfire)
	{
		vec = target;
		dir = vec - start;
	}
	else if (frandom() < 0.33f || (start[2] < self->enemy->absmin[2]))
	{
		vec = target;
		vec[2] += self->enemy->viewheight;
		dir = vec - start;
	}
	else
	{
		vec = target;
		vec[2] = self->enemy->absmin[2] + 1;
		dir = vec - start;
	}

	if ((!blindfire) && (frandom() < 0.35f))
		PredictAim(self, self->enemy, start, 650, false, 0.f, &dir, &vec);

	dir.normalize();

	trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
	if (blindfire)
	{
		if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
		{
			monster_fire_plasma(self, start, dir, 50, 650, radius_damage, radius_damage, mz);
		}
		else
		{
			vec = target;
			vec += (right * -10);
			dir = vec - start;
			dir.normalize();
			trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
			if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
				monster_fire_plasma(self, start, dir, 50, 650, radius_damage, radius_damage, mz);
			else
			{
				vec = target;
				vec += (right * 10);
				dir = vec - start;
				dir.normalize();
				trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
				if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
					monster_fire_plasma(self, start, dir, 50, 650, radius_damage, radius_damage, mz);
			}
		}
	}
	else
	{
		if (trace.fraction > 0.5f || trace.ent->solid != SOLID_BSP)
			monster_fire_plasma(self, start, dir, 50, 650, radius_damage, radius_damage, mz);
	}
}

void sentinel_fire(edict_t* self, monster_muzzleflash_id_t id)
{
	sentinel_rocket(self, id);
}

void sentinel_fire_left(edict_t* self)
{
	sentinel_fire(self, MZ2_ARACHNID_RAIL1);
}

void sentinel_fire_right(edict_t* self)
{
	sentinel_fire(self, MZ2_ARACHNID_RAIL2);
}

mframe_t sentinel_frames_attack[] = {
	{ ai_charge, 0, sentinel_fire_left },
	{ ai_charge },
	{ ai_charge, 0, sentinel_fire_right },
	{ ai_charge }
};
MMOVE_T(sentinel_move_attack) = { FRAME_attack1, FRAME_attack4, sentinel_frames_attack, sentinel_run };

MONSTERINFO_ATTACK(sentinel_attack) (edict_t* self) -> void
{
	M_SetAnimation(self, &sentinel_move_attack);
}

//
//PAIN
//

mframe_t sentinel_frames_pain[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(sentinel_move_pain) = { FRAME_pain1, FRAME_pain10, sentinel_frames_pain, sentinel_run };

PAIN(sentinel_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;
	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;

	float r = frandom();

	M_SetAnimation(self, &sentinel_move_pain);
}

MONSTERINFO_SETSKIN(sentinel_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
//DEATH
//

void sentinel_dead(edict_t* self)
{
	self->mins = { -16, -16, -8 };
	self->maxs = { 16, 16, 16 };
	self->movetype = MOVETYPE_TOSS;
	self->svflags |= SVF_DEADMONSTER;
	self->nextthink = 0_ms;
	gi.linkentity(self);
}

mframe_t sentinel_frames_death1[] = {
	{ ai_move, 0 },
	{ ai_move, -1.23f },
	{ ai_move, -1.23f },
	{ ai_move, -1.23f },
	{ ai_move, -1.23f },
	{ ai_move, -1.64f },
	{ ai_move, -1.64f },
	{ ai_move, -2.45f },
	{ ai_move, -8.63f },
	{ ai_move, -4.0f },
	{ ai_move, -4.5f },
	{ ai_move, -6.8f },
	{ ai_move, -8.0f },
	{ ai_move, -5.4f },
	{ ai_move, -3.4f },
	{ ai_move, -1.9f },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(sentinel_move_death) = { FRAME_death1, FRAME_death20, sentinel_frames_death1, sentinel_dead };

DIE(sentinel_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);
		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 2, "models/objects/gibs/sm_meat/tris.md2" },
			{ 1, "models/objects/gibs/gear/tris.md2" },
			{ 1, "models/monsters/badass/gib_larm.md2" },
			{ 1, "models/monsters/badass/gib_lleg.md2" },
			{ 1, "models/monsters/badass/gib_rarm.md2" },
			{ 1, "models/monsters/badass/gib_rleg.md2" },
			{ 1, "models/monsters/badass/gib_torso.md2" },
			{ "models/objects/gibs/head2/tris.md2", GIB_HEAD }
			});
		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	// regular death
	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;

	M_SetAnimation(self, &sentinel_move_death);
}

//
//DODGE
//

MONSTERINFO_BLOCKED(sentinel_blocked) (edict_t* self, float dist) -> bool
{
	if (blocked_checkplat(self, dist))
		return true;

	return false;
}

MONSTERINFO_SIDESTEP(sentinel_sidestep) (edict_t* self) -> bool
{
	if ((self->monsterinfo.active_move == &sentinel_move_attack) ||
		(self->monsterinfo.active_move == &sentinel_move_pain))
	{
		// if we're shooting, don't dodge
		return false;
	}

	if (self->monsterinfo.active_move != &sentinel_move_run)
		M_SetAnimation(self, &sentinel_move_run);

	return true;
}

/*QUAKED monster_sentinel (1 .5 0) (-45 -48 -64) (38 48 30) Ambush Trigger_Spawn Sight Corpse
*/
void SP_monster_sentinel(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain.assign("sentinel/pain.wav");
	sound_death.assign("sentinel/death.wav");
	sound_sight.assign("sentinel/sight.wav");
	sound_idle.assign("sentinel/idle.wav");
	sound_attack.assign("chick/chkatck2.wav");
	sound_step.assign("insane/insane11.wav");

	self->s.modelindex = gi.modelindex("models/monsters/badass/tris.md2");
	self->mins = { -45, -48, -64 };
	self->maxs = { 38, 48, 38 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = 1000 * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_COMBAT;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = 500;
	self->gib_health = -200;
	self->mass = 450;

	self->monsterinfo.scale = MODEL_SCALE;

	self->pain = sentinel_pain;
	self->die = sentinel_die;

	self->monsterinfo.sight = sentinel_sight;
	self->monsterinfo.idle = sentinel_idle;
	self->monsterinfo.search = sentinel_search;

	self->monsterinfo.stand = sentinel_stand;
	self->monsterinfo.walk = sentinel_walk;
	self->monsterinfo.run = sentinel_run;

	self->monsterinfo.attack = sentinel_attack;
	self->monsterinfo.setskin = sentinel_setskin;

	self->monsterinfo.sidestep = sentinel_sidestep;
	self->monsterinfo.blocked = sentinel_blocked;

	self->monsterinfo.aiflags |= AI_IGNORE_SHOTS;

	gi.linkentity(self);

	//M_SetAnimation(self, &sentinel_move_stand1);

	walkmonster_start(self);
}