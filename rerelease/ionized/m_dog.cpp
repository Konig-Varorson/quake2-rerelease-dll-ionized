// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

dog

==============================================================================
*/

#include "../g_local.h"
#include "m_dog.h"

constexpr spawnflags_t SPAWNFLAG_DOG_NOJUMPING = 8_spawnflag;

static cached_soundindex sound_pain1;
static cached_soundindex sound_die;
static cached_soundindex sound_launch;
static cached_soundindex sound_impact;
static cached_soundindex sound_sight;
static cached_soundindex sound_search;
static cached_soundindex sound_bite;
static cached_soundindex sound_bitemiss;
static cached_soundindex sound_jump;

///
/// SOUNDS
/// 

void dog_launch(edict_t* self)
{
	gi.sound(self, CHAN_WEAPON, sound_launch, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(dog_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(dog_search) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t dog_frames_stand[] = {
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(dog_stand) = { FRAME_stand1, FRAME_stand9, dog_frames_stand, nullptr };

MONSTERINFO_STAND(dog_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &dog_stand);
}

//
// WALK
//

mframe_t dog_frames_walk[] =
{
	{ai_walk, 7},
	{ai_walk, 7},
	{ai_walk, 7},
	{ai_walk, 7},
	{ai_walk, 7},
	{ai_walk, 7},
	{ai_walk, 7},
	{ai_walk, 7}
};
MMOVE_T(dog_move_walk) = { FRAME_walk1, FRAME_walk8, dog_frames_walk, nullptr };

MONSTERINFO_WALK(dog_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &dog_move_walk);
}


//
// RUN
//

mframe_t dog_frames_run[] =
{
	{ai_run, 12},
	{ai_run, 20},
	{ai_run, 20},
	{ai_run, 16},
	{ai_run, 22},
	{ai_run, 20},
	{ai_run, 12},
	{ai_run, 20},
	{ai_run, 20},
	{ai_run, 16},

	{ai_run, 22},
	{ai_run, 20}
};
MMOVE_T(dog_move_run) = { FRAME_run1, FRAME_run12, dog_frames_run, nullptr };

MONSTERINFO_RUN(dog_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		dog_stand(self);
	else
		M_SetAnimation(self, &dog_move_run);
}

//
// MELEE
//

void dog_bite(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (fire_hit(self, aim, irandom(1, 24), 100))
		gi.sound(self, CHAN_WEAPON, sound_bite, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_bitemiss, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

mframe_t dog_frames_attack[] =
{
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10, dog_bite},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10}
};
MMOVE_T(dog_move_attack) = { FRAME_attack1, FRAME_attack8, dog_frames_attack, dog_run };

MONSTERINFO_MELEE(dog_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &dog_move_attack);
}

//
// ATTACK
//

TOUCH(dog_jump_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (self->health <= 0)
	{
		self->touch = nullptr;
		return;
	}

	if (self->style == 1 && other->takedamage)
	{
		if (self->velocity.length() > 400)
		{
			vec3_t	point;
			vec3_t	normal;
			int		damage;

			normal = self->velocity;
			normal.normalize();
			point = self->s.origin + (normal * self->maxs[0]);
			damage = (int)frandom(20, 25);
			T_Damage(other, self, self, self->velocity, point, normal, damage, damage, DAMAGE_NONE, MOD_UNKNOWN);
			self->style = 0;
		}
	}

	if (!M_CheckBottom(self))
	{
		if (self->groundentity)
		{
			self->monsterinfo.nextframe = FRAME_attack4;
			self->touch = nullptr;
		}
		return;
	}

	self->touch = nullptr;
}

void dog_jump_takeoff(edict_t* self)
{
	vec3_t	forward;

	gi.sound(self, CHAN_VOICE, sound_jump, 1, ATTN_NORM, 0);
	AngleVectors(self->s.angles, forward, nullptr, nullptr);
	self->s.origin[2] += 1;
	self->velocity = forward * 400;
	self->velocity[2] = 200;
	self->groundentity = nullptr;
	self->monsterinfo.aiflags |= AI_DUCKED;
	self->monsterinfo.attack_finished = level.time + 3_sec;
	self->style = 1;
	self->touch = dog_jump_touch;
}

void dog_check_landing(edict_t* self)
{
	monster_jump_finished(self);

	if (self->groundentity)
	{
		gi.sound(self, CHAN_WEAPON, sound_impact, 1, ATTN_NORM, 0);
		self->monsterinfo.attack_finished = level.time + random_time(500_ms, 1.5_sec);

		if (self->monsterinfo.unduck)
			self->monsterinfo.unduck(self);

		if (range_to(self, self->enemy) <= RANGE_MELEE * 2.f)
			self->monsterinfo.melee(self);

		return;
	}

	if (level.time > self->monsterinfo.attack_finished)
		self->monsterinfo.nextframe = FRAME_leap4;
	else
		self->monsterinfo.nextframe = FRAME_leap5;
}

void dog_check_landing2(edict_t* self)
{
	monster_jump_finished(self);

	self->owner = nullptr;

	if (self->groundentity)
	{
		gi.sound(self, CHAN_WEAPON, sound_impact, 1, ATTN_NORM, 0);
		self->monsterinfo.attack_finished = level.time + random_time(500_ms, 1.5_sec);

		if (self->monsterinfo.unduck)
			self->monsterinfo.unduck(self);

		if (range_to(self, self->enemy) <= RANGE_MELEE * 2.f)
			self->monsterinfo.melee(self);

		return;
	}
}

mframe_t dog_frames_jump[] =
{
	{ai_charge,	20},
	{ai_charge,	20,	dog_jump_takeoff},
	{ai_move,	40},
	{ai_move,	30,	dog_check_landing},
	{ai_move,	 0},
	{ai_move,	 0},
	{ai_move,	 0}
};
MMOVE_T(dog_move_jump) = { FRAME_leap1, FRAME_leap9, dog_frames_jump, dog_run };

MONSTERINFO_ATTACK(dog_jump) (edict_t* self) -> void
{
	M_SetAnimation(self, &dog_move_jump);
}

//
// CHECKATTACK
//

bool dog_check_melee(edict_t* self)
{
	return range_to(self, self->enemy) <= RANGE_MELEE && self->monsterinfo.melee_debounce_time <= level.time;
}

bool dog_check_jump(edict_t* self)
{
	vec3_t	v;
	float	distance;

	if (self->absmin[2] > (self->enemy->absmin[2] + 0.75 * self->enemy->size[2]))
		return false;

	if (self->absmax[2] < (self->enemy->absmin[2] + 0.25 * self->enemy->size[2]))
		return false;

	v[0] = self->s.origin[0] - self->enemy->s.origin[0];
	v[1] = self->s.origin[1] - self->enemy->s.origin[1];
	v[2] = 0;
	distance = v.length();

	if (distance < 100 && self->monsterinfo.melee_debounce_time <= level.time)
		return false;
	if (distance > 100)
	{
		if (frandom() < 0.9f)
			return false;
	}

	return true;
}

MONSTERINFO_CHECKATTACK(dog_checkattack) (edict_t* self) -> bool
{
	if (!self->enemy || self->enemy->health <= 0)
		return false;

	if (dog_check_melee(self))
	{
		self->monsterinfo.attack_state = AS_MELEE;
		return true;
	}

	if (!self->spawnflags.has(SPAWNFLAG_DOG_NOJUMPING) && dog_check_jump(self))
	{
		self->monsterinfo.attack_state = AS_MISSILE;
		return true;
	}

	return false;
}

//
// PAIN
//

mframe_t dog_frames_pain1[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(dog_move_pain1) = { FRAME_pain1, FRAME_pain6, dog_frames_pain1, dog_run };

mframe_t dog_frames_pain2[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move,  -4},
	{ai_move,  -12},
	{ai_move,  -12},
	{ai_move,  -2},
	{ai_move},
	{ai_move,  -4},
	{ai_move},
	{ai_move,  -10},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(dog_move_pain2) = { FRAME_painb1, FRAME_painb16, dog_frames_pain2, dog_run };

PAIN(dog_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float r;

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	r = frandom();
	gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (r < 0.5f)
		M_SetAnimation(self, &dog_move_pain1);
	else
		M_SetAnimation(self, &dog_move_pain2);
}

MONSTERINFO_SETSKIN(dog_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// DEATH
//

void dog_shrink(edict_t* self)
{
	self->maxs[2] = 0;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t dog_frames_death1[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0, dog_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(dog_move_death1) = { FRAME_death1, FRAME_death9, dog_frames_death1, monster_dead };

mframe_t dog_frames_death2[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0, dog_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(dog_move_death2) = { FRAME_deathb1, FRAME_deathb9, dog_frames_death2, monster_dead };

DIE(dog_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 4, "models/objects/gibs/sm_meat/tris.md2" },
			//{ "models/monsters/guard/dog/gibs/chest.md2", GIB_SKINNED },
			{ "models/objects/gibs/chest/tris.md2" },
			//{ "models/monsters/guard/dog/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
			{ "models/monsters/gibs/sm_meat/head.md2", GIB_SKINNED | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_die, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;
	M_SetAnimation(self, &dog_move_death);
}

/*QUAKED monster_dog (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void SP_monster_dog(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain1.assign("monsters/dog/dpain1.wav");
	sound_die.assign("monsters/dog/death1.wav");
	sound_launch.assign("monsters/hound/hlaunch.wav");
	sound_impact.assign("monsters/hound/himpact.wav");
	sound_sight.assign("monsters/dog/dsight.wav");
	sound_search.assign("monsters/dog/idle.wav");
	sound_jump.assign("monsters/hound/hjump.wav");
	sound_bite.assign("monsters/dog/dattack1.wav");
	sound_bitemiss.assign("monsters/hound/hbite2.wav");

	gi.modelindex("models/monsters/guard/dog/tris.md2");

	self->monsterinfo.aiflags |= AI_STINKY;

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/guard/dog/tris.md2");

	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	self->health = 50 * st.health_multiplier;
	self->gib_health = -35;
	self->mass = 80;

	self->pain = dog_pain;
	self->die = dog_die;

	self->monsterinfo.stand = dog_stand;
	self->monsterinfo.walk = dog_walk;
	self->monsterinfo.run = dog_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = dog_jump;
	self->monsterinfo.melee = dog_melee;
	self->monsterinfo.sight = dog_sight;
	self->monsterinfo.search = dog_search;
	self->monsterinfo.checkattack = dog_checkattack;
	self->monsterinfo.blocked = nullptr;
	self->monsterinfo.setskin = dog_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &dog_stand);

	self->monsterinfo.combat_style = COMBAT_MELEE;

	self->monsterinfo.scale = MODEL_SCALE;
	self->monsterinfo.can_jump = !(self->spawnflags & SPAWNFLAG_DOG_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	walkmonster_start(self);
}