// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

hound

==============================================================================
*/

#include "g_local.h"
#include "m_hound.h"

constexpr spawnflags_t SPAWNFLAG_HOUND_NOJUMPING = 8_spawnflag;

static cached_soundindex sound_pain1;
static cached_soundindex sound_pain2;
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

void hound_launch(edict_t* self)
{
	gi.sound(self, CHAN_WEAPON, sound_launch, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(hound_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(hound_search) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t hound_frames_stand1[] = {
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},  // 10

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
MMOVE_T(hound_stand1) = { FRAME_idle401, FRAME_idle419, hound_frames_stand1, nullptr };

mframe_t hound_frames_stand2[] = {
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}, // 10

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}, // 20

	{ai_stand}
};
MMOVE_T(hound_stand2) = { FRAME_idle501, FRAME_idle521, hound_frames_stand2, nullptr };

MONSTERINFO_STAND(hound_stand) (edict_t* self) -> void
{
	if (frandom() < 0.8f)
	{
		M_SetAnimation(self, &hound_stand1);
	}
	else
	{
		M_SetAnimation(self, &hound_stand2);
	}
}

//
// WALK
//

mframe_t hound_frames_walk[] =
{
	{ai_walk,  7},
	{ai_walk,  7},
	{ai_walk,  7},
	{ai_walk,  7},
	{ai_walk,  7},
	{ai_walk,  7},
	{ai_walk,  7},
	{ai_walk,  7}
};
MMOVE_T(hound_move_walk) = { FRAME_walk01, FRAME_walk08, hound_frames_walk, nullptr };

MONSTERINFO_WALK(hound_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &hound_move_walk);
}


//
// RUN
//

mframe_t hound_frames_run[] =
{
	{ai_run, 60},
	{ai_run, 60},
	{ai_run, 40},
	{ai_run, 30},
	{ai_run, 30},
	{ai_run, 30},
	{ai_run, 40}
};
MMOVE_T(hound_move_run) = { FRAME_run01, FRAME_run07, hound_frames_run, nullptr };

MONSTERINFO_RUN(hound_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		hound_stand(self);
	else
		M_SetAnimation(self, &hound_move_run);
}

//
// MELEE
//

void hound_bite(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (fire_hit(self, aim, irandom(1, 9), 100))
		gi.sound(self, CHAN_WEAPON, sound_bite, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_bitemiss, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

void hound_bite2(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (fire_hit(self, aim, irandom(1, 9), 100))
		gi.sound(self, CHAN_WEAPON, sound_bite, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_bitemiss, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

mframe_t hound_frames_attack1[] =
{
	{ai_charge, 0,	hound_launch},
	{ai_charge},
	{ai_charge, 0,	hound_bite},
	{ai_charge, 0,	hound_bite2}
};
MMOVE_T(hound_move_attack1) = { FRAME_attack101, FRAME_attack104, hound_frames_attack1, hound_run };

mframe_t hound_frames_attack2[] =
{
	{ai_charge, 0,	hound_launch},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	hound_bite},
	{ai_charge, 0,	hound_bite2},
	{ai_charge, 0,	hound_bite2},
	{ai_charge, 0,	hound_bite2},
	{ai_charge},
	{ai_charge},
};
MMOVE_T(hound_move_attack2) = { FRAME_attack201, FRAME_attack214, hound_frames_attack2, hound_run };

MONSTERINFO_MELEE(hound_melee) (edict_t* self) -> void
{
	if (frandom() < 0.6f)
	{
		M_SetAnimation(self, &hound_move_attack1);
	}
	else
	{
		M_SetAnimation(self, &hound_move_attack2);
	}
}

//
// ATTACK
//

TOUCH(hound_jump_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
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
			damage = (int)frandom(40, 50);
			T_Damage(other, self, self, self->velocity, point, normal, damage, damage, DAMAGE_NONE, MOD_UNKNOWN);
			self->style = 0;
		}
	}

	if (!M_CheckBottom(self))
	{
		if (self->groundentity)
		{
			self->monsterinfo.nextframe = FRAME_leap04;
			self->touch = nullptr;
		}
		return;
	}

	self->touch = nullptr;
}

void hound_jump_takeoff(edict_t* self)
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
	self->touch = hound_jump_touch;
}

void hound_check_landing(edict_t* self)
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
		self->monsterinfo.nextframe = FRAME_leap04;
	else
		self->monsterinfo.nextframe = FRAME_leap05;
}

void hound_check_landing2(edict_t* self)
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

	if (level.time > self->monsterinfo.attack_finished)
		self->monsterinfo.nextframe = FRAME_restrain14;
	else
		self->monsterinfo.nextframe = FRAME_restrain15;
}

mframe_t hound_frames_handlerjump[] =
{
	{ai_charge,  0},
	{ai_charge,  20,	hound_jump_takeoff},
	{ai_move,  40},
	{ai_move,  30,	hound_check_landing2},
	{ai_move,   0},
	{ai_move,  0},
	{ai_move,  0},
};
MMOVE_T(hound_move_handlerjump) = { FRAME_restrain12, FRAME_restrain18, hound_frames_handlerjump, hound_run };

mframe_t hound_frames_jump[] =
{
	{ai_charge,	20},
	{ai_charge,	20,	hound_jump_takeoff},
	{ai_move,	40},
	{ai_move,	30,	hound_check_landing},
	{ai_move,	 0},
	{ai_move,	 0},
	{ai_move,	 0}
};
MMOVE_T(hound_move_jump) = { FRAME_leap01, FRAME_leap07, hound_frames_jump, hound_run };

MONSTERINFO_ATTACK(hound_jump) (edict_t* self) -> void
{
	M_SetAnimation(self, &hound_move_jump);
}

//
// CHECKATTACK
//

bool hound_check_melee(edict_t* self)
{
	return range_to(self, self->enemy) <= RANGE_MELEE && self->monsterinfo.melee_debounce_time <= level.time;
}

bool hound_check_jump(edict_t* self)
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

MONSTERINFO_CHECKATTACK(hound_checkattack) (edict_t* self) -> bool
{
	if (!self->enemy || self->enemy->health <= 0)
		return false;

	if (hound_check_melee(self))
	{
		self->monsterinfo.attack_state = AS_MELEE;
		return true;
	}

	if (!self->spawnflags.has(SPAWNFLAG_HOUND_NOJUMPING) && hound_check_jump(self))
	{
		self->monsterinfo.attack_state = AS_MISSILE;
		return true;
	}

	return false;
}

//
// PAIN
//

mframe_t hound_frames_pain1[] =
{
	{ai_move,  6},
	{ai_move, 16},
	{ai_move, -6},
	{ai_move, -7}
};
MMOVE_T(hound_move_pain1) = { FRAME_pain101, FRAME_pain104, hound_frames_pain1, hound_run };

mframe_t hound_frames_pain2[] =
{
	{ai_move,  0},
	{ai_move,  0},
	{ai_move,  0},
	{ai_move,  6},
	{ai_move, 16},
	{ai_move, -6},
	{ai_move, -7},
	{ai_move,  0}
};
MMOVE_T(hound_move_pain2) = { FRAME_pain201, FRAME_pain208, hound_frames_pain2, hound_run };

PAIN(hound_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float r;

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	r = frandom();
	if (r < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (r < 0.5f)
		M_SetAnimation(self, &hound_move_pain1);
	else
		M_SetAnimation(self, &hound_move_pain2);
}

MONSTERINFO_SETSKIN(hound_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// DEATH
//

void hound_shrink(edict_t* self)
{
	self->maxs[2] = 0;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t hound_frames_death[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0, hound_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(hound_move_death) = { FRAME_death01, FRAME_death11, hound_frames_death, monster_dead };

DIE(hound_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 4, "models/objects/gibs/sm_meat/tris.md2" },
			//{ "models/monsters/guard/hound/gibs/chest.md2", GIB_SKINNED },
			{ "models/objects/gibs/chest/tris.md2" },
			//{ "models/monsters/guard/hound/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
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
	M_SetAnimation(self, &hound_move_death);
}

void SP_monster_hound_precache()
{
	sound_pain1.assign("monsters/hound/hpain1.wav");
	sound_pain2.assign("monsters/hound/hpain2.wav");
	sound_die.assign("monsters/hound/hdeth1.wav");
	sound_launch.assign("monsters/hound/hlaunch.wav");
	sound_impact.assign("monsters/hound/himpact.wav");
	sound_sight.assign("monsters/hound/hsight1.wav");
	sound_search.assign("monsters/hound/hsearch1.wav");
	sound_jump.assign("monsters/hound/hjump.wav");
	sound_bite.assign("monsters/hound/hbite1.wav");
	sound_bitemiss.assign("monsters/hound/hbite2.wav");

	gi.modelindex("models/monsters/guard/hound/tris.md2");
}

/*QUAKED monster_hound (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void SP_monster_hound(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	SP_monster_hound_precache();

	self->monsterinfo.aiflags |= AI_STINKY;

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/guard/hound/tris.md2");

	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 24 };

	self->health = 100 * st.health_multiplier;
	self->gib_health = -75;
	self->mass = 250;

	self->pain = hound_pain;
	self->die = hound_die;

	self->monsterinfo.stand = hound_stand;
	self->monsterinfo.walk = hound_walk;
	self->monsterinfo.run = hound_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = hound_jump;
	self->monsterinfo.melee = hound_melee;
	self->monsterinfo.sight = hound_sight;
	self->monsterinfo.search = hound_search;
	self->monsterinfo.checkattack = hound_checkattack;
	self->monsterinfo.blocked = nullptr;
	self->monsterinfo.setskin = hound_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &hound_stand1);

	self->monsterinfo.combat_style = COMBAT_MELEE;

	self->monsterinfo.scale = MODEL_SCALE;
	self->monsterinfo.can_jump = !(self->spawnflags & SPAWNFLAG_HOUND_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	walkmonster_start(self);
}

THINK(HoundSpawn) (edict_t* self) -> void
{
	vec3_t	 vec;
	edict_t* player;

	SP_monster_hound(self);
	self->think(self);

	// jump at player
	if (self->enemy && self->enemy->inuse && self->enemy->health > 0)
		player = self->enemy;
	else
		player = AI_GetSightClient(self);

	if (!player)
		return;

	vec = player->s.origin - self->s.origin;
	self->s.angles[YAW] = vectoyaw(vec);
	vec.normalize();
	self->velocity = vec * 200;
	self->velocity[2] = 400;
	self->groundentity = nullptr;
	self->enemy = player;
	FoundTarget(self);
	self->monsterinfo.sight(self, self->enemy);
	self->s.frame = self->monsterinfo.nextframe = FRAME_restrain12;
}

void hound_createHound(edict_t* self, float healthPercent)
{
	edict_t* ent = G_Spawn();
	ent->classname = "monster_hound";
	ent->target = self->target;
	ent->s.origin = self->s.origin;
	ent->enemy = self->enemy;

	ent->health *= healthPercent;

	HoundSpawn(ent);

	// [Paril-KEX] set health bar over to Makron when we throw him out
	for (size_t i = 0; i < 2; i++)
		if (level.health_bar_entities[i] && level.health_bar_entities[i]->enemy == self)
			level.health_bar_entities[i]->enemy = ent;
}
