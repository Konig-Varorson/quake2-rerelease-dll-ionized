/*
==============================================================================

QUAKE HELL KNIGHT

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_hknight.h"

static cached_soundindex sound_sword1;
static cached_soundindex sound_sword2;
static cached_soundindex sound_flame;
static cached_soundindex sound_death;
static cached_soundindex sound_gib;
static cached_soundindex sound_idle;
static cached_soundindex sound_pain;
static cached_soundindex sound_sight;

void hknight_check_dist(edict_t* self);
void hknight_melee(edict_t* self);
void hknight_charge_attack(edict_t* self);

//
// SOUNDS
//

MONSTERINFO_SIGHT(hknight_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
	hknight_check_dist(self);
}

MONSTERINFO_IDLE(hknight_idle) (edict_t* self) -> void
{
	if (frandom() < 0.2f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t hknight_frames_stand[] =
{
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
MMOVE_T(hknight_move_stand) = { FRAME_stand1, FRAME_stand9, hknight_frames_stand, nullptr };

MONSTERINFO_STAND(hknight_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &hknight_move_stand);
}

//
// WALK
//

mframe_t hknight_frames_walk[] =
{
	{ai_walk,	2,		hknight_idle},
	{ai_walk,	5},
	{ai_walk,	5},
	{ai_walk,	4},
	{ai_walk,	4},
	{ai_walk,	2},
	{ai_walk,	2},
	{ai_walk,	3},
	{ai_walk,	3},
	{ai_walk,	4},
	{ai_walk,	3},
	{ai_walk,	4},
	{ai_walk,	6},
	{ai_walk,	2},
	{ai_walk,	2},
	{ai_walk,	4},
	{ai_walk,	3},
	{ai_walk,	3},
	{ai_walk,	3},
	{ai_walk,	2}
};
MMOVE_T(hknight_move_walk) = { FRAME_walk1, FRAME_walk20, hknight_frames_walk, nullptr };

MONSTERINFO_WALK(hknight_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &hknight_move_walk);
}

//
// RUN
//

void hknight_check_charge(edict_t* self)
{
	if (!self->enemy || !self->enemy->inuse)
		return;
	if (!visible(self, self->enemy))
		return;
	if (level.time < self->monsterinfo.attack_finished)
		return;
	if (fabsf(self->s.origin[2] - self->enemy->s.origin[2]) > 20)
		return;

	float dist = (self->s.origin - self->enemy->s.origin).length();
	if (dist < 80)
		return;

	self->monsterinfo.attack_finished = level.time + 2_sec;
	hknight_charge_attack(self);
}

void hknight_check_dist(edict_t* self)
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
		return;

	if (range_to(self, self->enemy) <= RANGE_MELEE)
		hknight_melee(self);
}

mframe_t hknight_frames_run[] =
{
	{ai_run,	20,		hknight_check_charge},
	{ai_run,	25},
	{ai_run,	18},
	{ai_run,	16},
	{ai_run,	14},
	{ai_run,	25},
	{ai_run,	21},
	{ai_run,	13}
};
MMOVE_T(hknight_move_run) = { FRAME_run1, FRAME_run8, hknight_frames_run, nullptr };

MONSTERINFO_RUN(hknight_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &hknight_move_stand);
	else
		M_SetAnimation(self, &hknight_move_run);
}

//
// MELEE ATTACKS
//

void hknight_melee_damage(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 2 };
	if (fire_hit(self, aim, irandom(3, 9), 100))
		gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

mframe_t hknight_frames_slice[] =
{
	{ai_charge,	9},
	{ai_charge,	6},
	{ai_charge,	13},
	{ai_charge,	4},
	{ai_charge,	7,	hknight_melee_damage},
	{ai_charge,	15,	hknight_melee_damage},
	{ai_charge,	8,	hknight_melee_damage},
	{ai_charge,	2,	hknight_melee_damage},
	{ai_charge,	0,	hknight_melee_damage},
	{ai_charge,	3}
};
MMOVE_T(hknight_slice_attack) = { FRAME_slice1, FRAME_slice10, hknight_frames_slice, hknight_run };

mframe_t hknight_frames_smash[] =
{
	{ai_charge,	1},
	{ai_charge,	13},
	{ai_charge,	9},
	{ai_charge,	11},
	{ai_charge,	10,	hknight_melee_damage},
	{ai_charge,	7,	hknight_melee_damage},
	{ai_charge,	12,	hknight_melee_damage},
	{ai_charge,	2,	hknight_melee_damage},
	{ai_charge,	3,	hknight_melee_damage},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(hknight_smash_attack) = { FRAME_smash1, FRAME_smash11, hknight_frames_smash, hknight_run };

mframe_t hknight_frames_watk[] =
{
	{ai_charge,	2},
	{ai_charge},
	{ai_charge},
	{ai_charge,	0,	hknight_melee_damage},
	{ai_charge,	0,	hknight_melee_damage},
	{ai_charge,	0,	hknight_melee_damage},
	{ai_charge,	1},
	{ai_charge,	4},
	{ai_charge,	5},
	{ai_charge,	3,	hknight_melee_damage},
	{ai_charge,	2,	hknight_melee_damage},
	{ai_charge,	2,	hknight_melee_damage},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge,	1},
	{ai_charge,	1,	hknight_melee_damage},
	{ai_charge,	3,	hknight_melee_damage},
	{ai_charge,	4,	hknight_melee_damage},
	{ai_charge,	6},
	{ai_charge,	7},
	{ai_charge,	3}
};
MMOVE_T(hknight_watk_attack) = { FRAME_w_attack1, FRAME_w_attack22, hknight_frames_watk, hknight_run };

MONSTERINFO_MELEE(hknight_melee) (edict_t* self) -> void
{
	gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0);

	if (self->dmg == 0)
	{
		M_SetAnimation(self, &hknight_slice_attack);
		self->dmg = 1;
	}
	else if (self->dmg == 1)
	{
		M_SetAnimation(self, &hknight_smash_attack);
		self->dmg = 2;
	}
	else
	{
		M_SetAnimation(self, &hknight_watk_attack);
		self->dmg = 0;
	}
}

//
// MELEE
//

void hknight_charge_sound(edict_t* self)
{
	if (frandom() > 0.5f)
		gi.sound(self, CHAN_WEAPON, sound_sword2, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0);
}

void hknight_check_continue_charge(edict_t* self)
{
	if (level.time > self->monsterinfo.attack_finished)
	{
		self->monsterinfo.attack_finished = level.time + 3_sec;
		hknight_run(self);
		return;
	}
	hknight_charge_sound(self);
}

mframe_t hknight_frames_char_a[] =
{
	{ai_charge,	20},
	{ai_charge,	25},
	{ai_charge,	18},
	{ai_charge,	16},
	{ai_charge,	14},
	{ai_charge,	20,	hknight_melee_damage},
	{ai_charge,	21,	hknight_melee_damage},
	{ai_charge,	13,	hknight_melee_damage},
	{ai_charge,	20,	hknight_melee_damage},
	{ai_charge,	20,	hknight_melee_damage},
	{ai_charge,	18,	hknight_melee_damage},
	{ai_charge,	16},
	{ai_charge,	14},
	{ai_charge,	25},
	{ai_charge,	21},
	{ai_charge,	13}
};
MMOVE_T(hknight_charge_a_attack) = { FRAME_char_a1, FRAME_char_a16, hknight_frames_char_a, hknight_run };

mframe_t hknight_frames_char_b[] =
{
	{ai_charge,	23,	hknight_melee_damage},
	{ai_charge,	17,	hknight_melee_damage},
	{ai_charge,	12,	hknight_melee_damage},
	{ai_charge,	22,	hknight_melee_damage},
	{ai_charge,	18,	hknight_melee_damage},
	{ai_charge,	8,	hknight_check_continue_charge}
};
MMOVE_T(hknight_charge_b_attack) = { FRAME_char_b1, FRAME_char_b6, hknight_frames_char_b, nullptr };

void hknight_charge_attack(edict_t* self)
{
	M_SetAnimation(self, &hknight_charge_a_attack);
}

//
// ATTACK
//

void hknight_fire_flame(edict_t* self)
{
	float		lawl[] = { -2, -1, 0, 1, 2, 3 };
	int		rem = 165;
	int		arraynum = rem - self->s.frame;
	float	offang_y = 0;
	vec3_t	vec, forward, right, start;
	vec3_t	offang, v_forward;
	vec3_t temp = { 0, 0, 14 };

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, temp, forward, right);

	temp = { 0, lawl[arraynum], 0 };
	vec = self->s.angles - temp;
	AngleVectors(vec, forward, nullptr, nullptr);

	offang = self->enemy->s.origin - self->s.origin;
	offang = vectoangles(offang);
	offang_y = self->s.angles[1] + -lawl[arraynum] * 6;
	offang[1] = offang_y;

	AngleVectors(offang, v_forward, nullptr, nullptr);

	gi.sound(self, CHAN_WEAPON, sound_flame, 1, ATTN_NORM, 0);

	fire_flame(self, start, v_forward, 9, 500);
}

mframe_t hknight_frames_magica[] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(hknight_magica_attack) = { FRAME_magica1, FRAME_magica14, hknight_frames_magica, hknight_run };

mframe_t hknight_frames_magicb[] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge}
};
MMOVE_T(hknight_magicb_attack) = { FRAME_magicb1, FRAME_magicb13, hknight_frames_magicb, hknight_run };

mframe_t hknight_frames_magicc[] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame},
	{ai_charge,	0,	hknight_fire_flame}
};
MMOVE_T(hknight_magicc_attack) = { FRAME_magicc1, FRAME_magicc11, hknight_frames_magicc, hknight_run };

MONSTERINFO_ATTACK(hknight_magic) (edict_t* self) -> void
{
	float choice = frandom();

	if (choice < 0.33f)
		M_SetAnimation(self, &hknight_magica_attack);
	else if (choice < 0.66f)
		M_SetAnimation(self, &hknight_magicb_attack);
	else
		M_SetAnimation(self, &hknight_magicc_attack);
}

//
// PAIN
//

mframe_t hknight_frames_pain1[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(hknight_move_pain1) = { FRAME_pain1, FRAME_pain5, hknight_frames_pain1, hknight_run };

PAIN(hknight_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	if (level.time - self->pain_debounce_time > 5_sec)
	{
		self->pain_debounce_time = level.time + 1_sec;
	}
	else if (frandom() * 30 > damage)
	{
		return;
	}
	else
	{
		self->pain_debounce_time = level.time + 1_sec;
	}

	if (!M_ShouldReactToPain(self, mod))
		return;

	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);
	M_SetAnimation(self, &hknight_move_pain1);
}

MONSTERINFO_SETSKIN(hknight_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// DEATH
//

void hknight_shrink(edict_t* self)
{
	self->maxs[2] = 0;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t hknight_frames_death1[] =
{
	{ai_move,	10},
	{ai_move,	8},
	{ai_move,	7},
	{ai_move,	0,		hknight_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move,	10},
	{ai_move,	11},
	{ai_move},
	{ai_move}
};
MMOVE_T(hknight_move_death1) = { FRAME_death1, FRAME_death12, hknight_frames_death1, monster_dead };

mframe_t hknight_frames_death2[] =
{
	{ai_move},
	{ai_move},
	{ai_move,	0,		hknight_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(hknight_move_death2) = { FRAME_deathb1, FRAME_deathb9, hknight_frames_death2, monster_dead };

DIE(hknight_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		if (strcmp(self->classname, "monster_hstatue") == 0)
		{
			ThrowGibs(self, damage, {
			   { 2, "models/objects/gibs/bone/tris.md2" },
			   { 4, "models/objects/gibs/sm_meat/tris.md2" },
			   { "models/monsters/hknight/gibs/head.md2", GIB_SKINNED | GIB_DEBRIS | GIB_HEAD }
				});
		}
		else
		{
			ThrowGibs(self, damage, {
			   { 2, "models/objects/gibs/bone/tris.md2" },
			   { 4, "models/objects/gibs/sm_meat/tris.md2" },
			   { "models/objects/gibs/chest/tris.md2" },
			   { "models/monsters/hknight/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
				});
		}

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	if (frandom() > 0.5f)
		M_SetAnimation(self, &hknight_move_death1);
	else
		M_SetAnimation(self, &hknight_move_death2);
}

//
// STATUE ACTIVATION
//

USE(hstatue_activate) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);

	self->use = nullptr;
	self->takedamage = true;

	self->pain = hknight_pain;
	self->die = hknight_die;

	self->monsterinfo.stand = hknight_stand;
	self->monsterinfo.walk = hknight_walk;
	self->monsterinfo.run = hknight_run;
	self->monsterinfo.melee = hknight_melee;
	self->monsterinfo.attack = hknight_magic;
	self->monsterinfo.sight = hknight_sight;
	self->monsterinfo.idle = hknight_idle;

	if (activator && activator->client)
	{
		self->enemy = activator;
		FoundTarget(self);
	}
	else
	{
		self->monsterinfo.idle_time = level.time + 0.5_sec;
		self->monsterinfo.run(self);
	}
}

THINK(hstatue_think) (edict_t* self) -> void
{
	self->nextthink = level.time + FRAME_TIME_S;
}

void hstatue_notrigger(edict_t* self)
{
	self->use = nullptr;
	self->takedamage = true;

	self->pain = hknight_pain;
	self->die = hknight_die;

	self->monsterinfo.stand = hknight_stand;
	self->monsterinfo.walk = hknight_walk;
	self->monsterinfo.run = hknight_run;
	self->monsterinfo.melee = hknight_melee;
	self->monsterinfo.attack = hknight_magic;
	self->monsterinfo.sight = hknight_sight;
	self->monsterinfo.idle = hknight_idle;
}

//
// SPAWN
//

/*QUAKED SP_monster_hknight (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight
model="models/monsters/hknight/tris.md2"
*/
void SP_monster_hknight(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_death.assign("hknight/death1.wav");
	sound_pain.assign("hknight/pain1.wav");
	sound_idle.assign("hknight/idle.wav");
	sound_sight.assign("hknight/sight1.wav");
	sound_sword1.assign("knight/sword1.wav");
	sound_sword2.assign("knight/sword2.wav");
	sound_flame.assign("hknight/attack1.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex("models/monsters/hknight/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 40 };

	if (strcmp(self->classname, "monster_hstatue") == 0)
	{
		self->health = 350 * st.health_multiplier;
		self->gib_health = -100;
		self->mass = 230;
	}
	else
	{
		self->health = 250 * st.health_multiplier;
		self->gib_health = -40;
		self->mass = 175;
	}

	self->monsterinfo.combat_style = COMBAT_MIXED;

	self->pain = hknight_pain;
	self->die = hknight_die;

	self->monsterinfo.stand = hknight_stand;
	self->monsterinfo.walk = hknight_walk;
	self->monsterinfo.run = hknight_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = hknight_magic;
	self->monsterinfo.melee = hknight_melee;
	self->monsterinfo.sight = hknight_sight;
	self->monsterinfo.idle = hknight_idle;
	self->monsterinfo.setskin = hknight_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &hknight_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start(self);
}

/*QUAKED monster_hstatue (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight
Stone statue hell knight that is invulnerable until triggered.

Keys:
frame - frame to be frozen in (default 73 = stand1)

model="models/monsters/hknight/tris.md2"
*/
void SP_monster_hstatue(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	if (!self->targetname)
	{
		hstatue_notrigger(self);
	}

	SP_monster_hknight(self);

	sound_death.assign("statue/death.wav");
	sound_pain.assign("statue/pain.wav");
	sound_idle.assign("statue/idle.wav");
	sound_sight.assign("statue/see.wav");

	self->s.skinnum = 2;
	self->flags |= FL_MECHANICAL | FL_DEEPONE;

	if (!st.was_key_specified("frame"))
		self->s.frame = FRAME_stand1;

	self->takedamage = false;
	self->use = hstatue_activate;

	self->pain = nullptr;
	self->die = nullptr;

	self->monsterinfo.stand = nullptr;
	self->monsterinfo.walk = nullptr;
	self->monsterinfo.run = nullptr;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = hknight_magic;
	self->monsterinfo.melee = hknight_melee;
	self->monsterinfo.sight = nullptr;
	self->monsterinfo.idle = nullptr;
	self->monsterinfo.setskin = hknight_setskin;

	self->think = hstatue_think;
	self->nextthink = level.time + FRAME_TIME_S;
}