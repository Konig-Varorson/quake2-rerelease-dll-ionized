/*
==============================================================================

QUAKE HELL KNIGHT

==============================================================================
*/

#include "../g_local.h"
#include "m_hknight.h"

static cached_soundindex sound_sword1;
static cached_soundindex sound_flame;
static cached_soundindex sound_death;
static cached_soundindex sound_gib;
static cached_soundindex sound_idle;
static cached_soundindex sound_pain;
static cached_soundindex sound_sight;

void hknight_check_dist (edict_t *self);
void hknight_melee (edict_t *self);

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
	if (frandom() < 0.1)
		gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

//
// STAND
//

mframe_t hknight_frames_stand [] =
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
MMOVE_T(hknight_move_stand) = {FRAME_stand1, FRAME_stand9, hknight_frames_stand, nullptr };

MONSTERINFO_STAND (hknight_stand) (edict_t *self) -> void
{
	M_SetAnimation(self, &hknight_move_stand);
}

//
// WALK
//

mframe_t hknight_frames_walk [] =
{
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
	{ai_walk,	3},
	{ai_walk,	2}
};
MMOVE_T(hknight_move_walk) = {FRAME_walk1, FRAME_walk20, hknight_frames_walk, nullptr };

MONSTERINFO_WALK (hknight_walk) (edict_t *self) -> void
{
	M_SetAnimation(self, &hknight_move_walk);
}

//
// RUN
//

void hknight_check_dist (edict_t *self)
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
		return;

	if (range_to(self, self->enemy) <= RANGE_MELEE)
		hknight_melee(self);
}


mframe_t hknight_frames_run [] =
{
	{ai_run,	16},
	{ai_run,	20},
	{ai_run,	13},
	{ai_run,	7},
	{ai_run,	16},
	{ai_run,	20},
	{ai_run,	14},
	{ai_run,	6,		hknight_check_dist}
};
MMOVE_T(hknight_move_run) = {FRAME_run1, FRAME_run8, hknight_frames_run, nullptr };


MONSTERINFO_RUN (hknight_run) (edict_t *self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &hknight_move_stand);
	else
		M_SetAnimation(self, &hknight_move_run);
}

void hknight_melee_damage (edict_t *self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 2 };
	if (fire_hit(self, aim, irandom(1, 9), 100))
		gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

mframe_t hknight_frames_smash [] =
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
MMOVE_T(hknight_smash_attack) = {FRAME_smash1, FRAME_smash11, hknight_frames_smash, hknight_run};

mframe_t hknight_frames_watk [] =
{
	{ai_charge,	2},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
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
MMOVE_T(hknight_watk_attack) = {FRAME_w_attack1, FRAME_w_attack22, hknight_frames_watk, hknight_run};

mframe_t hknight_frames_srice [] =
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
MMOVE_T(hknight_slice_attack) = {FRAME_slice1, FRAME_slice10, hknight_frames_srice, hknight_run};

MONSTERINFO_MELEE (hknight_melee) (edict_t *self) -> void
{
	gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0.1f);

	if (self->dmg == 0)
	{
		M_SetAnimation(self, &hknight_slice_attack);
		self->dmg++;
	}
	else if (self->dmg == 1)
	{
		M_SetAnimation(self, &hknight_smash_attack);
		self->dmg++;
	}
	else if (self->dmg == 2)
	{
		M_SetAnimation(self, &hknight_watk_attack);
		self->dmg = 0;
	}
}

//
// PAIN
//

mframe_t hknight_frames_pain1 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(hknight_move_pain1) = {FRAME_pain1, FRAME_pain5, hknight_frames_pain1, hknight_run};

PAIN(hknight_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 1.1_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	gi.sound (self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

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


mframe_t hknight_frames_death1 [] =
{
	{ai_move,	10},
	{ai_move,	8},
	{ai_move,	7},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move,	10},
	{ai_move,	11},
	{ai_move},
	{ai_move}
};
MMOVE_T(hknight_move_death1) = {FRAME_death1, FRAME_death12, hknight_frames_death1, monster_dead };


mframe_t hknight_frames_death2 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(hknight_move_death2) = {FRAME_deathb1, FRAME_deathb9, hknight_frames_death2, monster_dead };

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
			   //{ "models/monsters/knight/gibs/chest.md2", GIB_SKINNED | GIB_DEBRIS },
			   //{ "models/monsters/knight/gibs/thigh.md2", GIB_SKINNED | GIB_DEBRIS },
			   //{ "models/monsters/knight/gibs/statgib.md2", GIB_SKINNED | GIB_DEBRIS },
			   { "models/monsters/hknight/gibs/head.md2", GIB_SKINNED | GIB_DEBRIS | GIB_HEAD }
				});
		}
		else
		{
			ThrowGibs(self, damage, {
			   { 2, "models/objects/gibs/bone/tris.md2" },
			   { 4, "models/objects/gibs/sm_meat/tris.md2" },
			   //{ "models/monsters/hknight/gibs/chest.md2", GIB_SKINNED },
			   //{ "models/monsters/hknight/gibs/thigh.md2", GIB_SKINNED },
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

	if (frandom() < 0.5f)
		M_SetAnimation(self, &hknight_move_death1);
	else
		M_SetAnimation(self, &hknight_move_death2);
}

void hknight_fire_flame (edict_t *self)
{
	float		lawl[] = { -2, -1, 0, 1, 2, 3 };
	int		rem = 165;
	int		arraynum = rem-self->s.frame;
	float	offang_y = 0;
	vec3_t	vec, forward, right, start;
	vec3_t	offang, v_forward;
	vec3_t temp = { 0, 0, 14 };

	AngleVectors (self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, temp, forward, right);

	temp = { 0, lawl[arraynum], 0 };
	vec = self->s.angles - temp;
	AngleVectors (vec, forward, nullptr, nullptr);

	offang = self->enemy->s.origin - self->s.origin;
	offang = vectoangles(offang);
	offang_y = self->s.angles[1] + -lawl[arraynum] * 6;
	offang[1] = offang_y;

	AngleVectors (offang, v_forward, nullptr, nullptr);

	gi.sound (self, CHAN_WEAPON, sound_flame, 1, ATTN_NORM, 0);

	fire_flame(self, start, v_forward, 9, 500);
}

mframe_t hknight_frames_magic [] =
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
MMOVE_T(hknight_magic_attack) = { FRAME_magicc1, FRAME_magicc11, hknight_frames_magic, hknight_run };

MONSTERINFO_ATTACK (hknight_magic) (edict_t* self) -> void
{
	M_SetAnimation(self, &hknight_magic_attack);
}


//
// SPAWN
//

/*QUAKED SP_monster_hknight (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight
model="models/monsters/hknight/tris.md2"
*/
void SP_monster_hknight (edict_t *self)
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
	sound_sword1.assign("hknight/slash1.wav");
	sound_flame.assign("hknight/attack1.wav");
		
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/hknight/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 40 };

	if (strcmp(self->classname, "monster_statue") == 0)
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
	self->monsterinfo.dodge = nullptr ;
	self->monsterinfo.attack = hknight_magic; 
	self->monsterinfo.melee = hknight_melee;
	self->monsterinfo.sight = hknight_sight;
	self->monsterinfo.idle = hknight_idle;

	gi.linkentity (self);
	
	M_SetAnimation(self, &hknight_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start (self);
}


/*QUAKED monster_hstatue (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight NoJumping Kneeling Stationary
model="models/monsters/knight/tris.md2"
*/
void SP_monster_hstatue(edict_t* self)
{
	SP_monster_hknight(self);

	sound_death.assign("statue/death.wav");
	sound_pain.assign("statue/pain.wav");
	sound_idle.assign("statue/idle.wav");
	sound_sight.assign("statue/see.wav");

	self->s.skinnum = 2;

	self->flags |= FL_MECHANICAL;
}