/*
==============================================================================

QUAKE KNIGHT

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_knight.h"

constexpr spawnflags_t SPAWNFLAG_KNIGHT_NOJUMPING = 8_spawnflag;
constexpr spawnflags_t SPAWNFLAG_KNIGHT_KNEEL = 16_spawnflag;

static cached_soundindex	sound_sword1;
static cached_soundindex	sound_sword2;
static cached_soundindex	sound_death;
static cached_soundindex	sound_gib;
static cached_soundindex	sound_pain;
static cached_soundindex	sound_idle;
static cached_soundindex	sound_sight;

void knight_stand(edict_t* self);
void knight_walk(edict_t* self);
void knight_run(edict_t* self);
void knight_check_dist(edict_t* self);

//
// SOUNDS
//

MONSTERINFO_SIGHT(knight_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(knight_search) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

void knight_sword_sound(edict_t* self)
{
	if (frandom() <= 0.5f)
		gi.sound(self, CHAN_VOICE, sound_sword1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_sword2, 1, ATTN_NORM, 0);
}

//
// KNEEL/BOW ANIMATIONS
//

/*mframe_t knight_frames_bow[] =
{
	{ai_stand},
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
MMOVE_T(knight_move_bow) = { FRAME_kneel1, FRAME_walk1, knight_frames_bow, knight_walk };

void knight_bow(edict_t* self)
{
	M_SetAnimation(self, &knight_move_bow);
}

mframe_t knight_frames_kneeling[] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(knight_move_kneeling) = { FRAME_kneel1, FRAME_kneel5, knight_frames_kneeling, knight_stand };

void knight_kneeling(edict_t* self)
{
	if (frandom() <= 0.25f)
		M_SetAnimation(self, &knight_move_kneeling);
}
*/
mframe_t knight_frames_standing[] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(knight_move_standing) = { FRAME_standing2, FRAME_standing5, knight_frames_standing, knight_walk };

void knight_standing(edict_t* self)
{
	if (frandom() <= 0.2f)
		M_SetAnimation(self, &knight_move_standing);
}

//
// STAND
//

mframe_t knight_frames_stand[] =
{
	{ai_stand/*, 0, knight_kneeling*/},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(knight_move_stand) = { FRAME_stand1, FRAME_stand9, knight_frames_stand, knight_stand };

MONSTERINFO_STAND(knight_stand) (edict_t* self) -> void
{
		M_SetAnimation(self, &knight_move_stand);
}

//
// WALK
//

mframe_t knight_frames_walk[] =
{
	{ai_walk,	3,		knight_search},
	{ai_walk,	2},
	{ai_walk,	3},
	{ai_walk,	4},
	{ai_walk,	3},
	{ai_walk,	3},
	{ai_walk,	3},
	{ai_walk,	4},
	{ai_walk,	3,		knight_standing},
	{ai_walk,	3},
	{ai_walk,	2},
	{ai_walk,	3},
	{ai_walk,	4},
	{ai_walk,	3}
};
MMOVE_T(knight_move_walk) = { FRAME_walk1, FRAME_walk14, knight_frames_walk, knight_walk };

MONSTERINFO_WALK(knight_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &knight_move_walk);
}

//
// RUN
//

void knight_check_dist(edict_t* self)
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
		return;

	if (range_to(self, self->enemy) <= RANGE_MELEE)
	{
		self->monsterinfo.nextframe = FRAME_attackb1;
	}
	else
	{
		if (frandom() > 0.6f)
			self->monsterinfo.nextframe = FRAME_runattack1;
		else
			self->monsterinfo.nextframe = FRAME_runb1;
	}
}

mframe_t knight_frames_run[] =
{
	{ai_run,	16,		knight_search},
	{ai_run,	20},
	{ai_run,	13},
	{ai_run,	7},
	{ai_run,	16},
	{ai_run,	20},
	{ai_run,	14},
	{ai_run,	6,		knight_check_dist}
};
MMOVE_T(knight_move_run) = { FRAME_runb1, FRAME_runb8, knight_frames_run, nullptr };

MONSTERINFO_RUN(knight_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &knight_move_stand);
	else
		M_SetAnimation(self, &knight_move_run);
}

//
// ATTACKS
//

void knight_attack(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 2 };
	fire_hit(self, aim, irandom(3, 9), 100);
}

mframe_t knight_frames_attack_run[] =
{
	{ai_charge,	20,		knight_sword_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, knight_attack},
	{ai_charge, 0, knight_attack},
	{ai_charge, 0, knight_attack},
	{ai_charge, 0, knight_attack},
	{ai_charge, 0, knight_attack},
	{ai_charge},
	{ai_charge,	10,		knight_check_dist}
};
MMOVE_T(knight_move_attack_run) = { FRAME_runattack1, FRAME_runattack11, knight_frames_attack_run, knight_run };

MONSTERINFO_ATTACK(knight_attack_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &knight_move_attack_run);
}

void knight_hit(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 4 };
	if (fire_hit(self, aim, irandom(3, 9), 100))
		gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_sword1, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

mframe_t knight_frames_attack[] =
{
	{ai_charge, 0,		knight_sword_sound},
	{ai_charge, 7},
	{ai_charge, 4},
	{ai_charge},
	{ai_charge, 3},
	{ai_charge, 4,		knight_hit},
	{ai_charge, 1,		knight_hit},
	{ai_charge, 3,		knight_hit},
	{ai_charge, 1},
	{ai_charge, 5,		knight_check_dist}
};
MMOVE_T(knight_move_attack) = { FRAME_attackb1, FRAME_attackb10, knight_frames_attack, knight_run };

MONSTERINFO_MELEE(knight_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &knight_move_attack);
}

//
// PAIN
//

mframe_t knight_frames_pain1[] =
{
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(knight_move_pain1) = { FRAME_pain1, FRAME_pain3, knight_frames_pain1, knight_run };

mframe_t knight_frames_pain2[] =
{
	{ai_move},
	{ai_move, 3},
	{ai_move},
	{ai_move},
	{ai_move, 2},
	{ai_move, 4},
	{ai_move, 2},
	{ai_move, 5},
	{ai_move, 5},
	{ai_move},
	{ai_move}
};
MMOVE_T(knight_move_pain2) = { FRAME_painb1, FRAME_painb11, knight_frames_pain2, knight_run };

PAIN(knight_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 1_sec;

	if (!M_ShouldReactToPain(self, mod))
		return;

	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (frandom() < 0.85f)
		M_SetAnimation(self, &knight_move_pain1);
	else
		M_SetAnimation(self, &knight_move_pain2);
}

MONSTERINFO_SETSKIN(knight_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// DEATH
//
void knight_shrink(edict_t* self)
{
	self->maxs[2] = 0;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t knight_frames_death1[] =
{
	{ai_move},
	{ai_move},
	{ai_move, 0,	knight_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(knight_move_death1) = { FRAME_death1, FRAME_death10, knight_frames_death1, monster_dead };

mframe_t knight_frames_death2[] =
{
	{ai_move},
	{ai_move},
	{ai_move, 0,	knight_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(knight_move_death2) = { FRAME_deathb1, FRAME_deathb11, knight_frames_death2, monster_dead };

DIE(knight_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		if (strcmp(self->classname, "monster_statue") == 0)
		{
			ThrowGibs(self, damage, {
			   { 2, "models/objects/gibs/bone/tris.md2" },
			   { 4, "models/objects/gibs/sm_meat/tris.md2" },
			   { "models/monsters/knight/gibs/head.md2", GIB_SKINNED | GIB_DEBRIS | GIB_HEAD }
				});
		}
		else
		{
			ThrowGibs(self, damage, {
			   { 2, "models/objects/gibs/bone/tris.md2" },
			   { 4, "models/objects/gibs/sm_meat/tris.md2" },
			   { "models/objects/gibs/chest/tris.md2" },
			   { "models/monsters/knight/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
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
		M_SetAnimation(self, &knight_move_death1);
	else
		M_SetAnimation(self, &knight_move_death2);
}

//
// STATUE ACTIVATION
//

USE(statue_activate) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);

	self->use = nullptr;
	self->takedamage = true;

	self->pain = knight_pain;
	self->die = knight_die;

	self->monsterinfo.stand = knight_stand;
	self->monsterinfo.walk = knight_walk;
	self->monsterinfo.run = knight_run;
	self->monsterinfo.sight = knight_sight;
	self->monsterinfo.search = knight_search;

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

THINK(statue_think) (edict_t* self) -> void
{
	self->nextthink = level.time + FRAME_TIME_S;
}

void statue_notrigger(edict_t* self)
{
	self->use = nullptr;
	self->takedamage = true;

	self->pain = knight_pain;
	self->die = knight_die;

	self->monsterinfo.stand = knight_stand;
	self->monsterinfo.walk = knight_walk;
	self->monsterinfo.run = knight_run;
	self->monsterinfo.sight = knight_sight;
	self->monsterinfo.search = knight_search;

}

/*QUAKED monster_knight (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight NoJumping Kneeling
model="models/monsters/knight/tris.md2"
*/
void SP_monster_knight(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_sword1.assign("knight/sword1.wav");
	sound_sword2.assign("knight/sword2.wav");
	sound_death.assign("knight/kdeath.wav");
	sound_pain.assign("knight/khurt.wav");
	sound_idle.assign("knight/idle.wav");
	sound_sight.assign("knight/ksight.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex("models/monsters/knight/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	if (strcmp(self->classname, "monster_statue") == 0)
	{
		self->health = 125 * st.health_multiplier;
		self->gib_health = -100;
		self->mass = 175;
	}
	else
	{
		self->health = 75 * st.health_multiplier;
		self->gib_health = -40;
		self->mass = 120;
	}

	self->pain = knight_pain;
	self->die = knight_die;

	self->monsterinfo.stand = knight_stand;
	self->monsterinfo.walk = knight_walk;
	self->monsterinfo.run = knight_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = knight_attack_run;
	self->monsterinfo.melee = knight_melee;
	self->monsterinfo.sight = knight_sight;
	self->monsterinfo.search = knight_search;
	self->monsterinfo.setskin = knight_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &knight_move_stand);

	self->monsterinfo.combat_style = COMBAT_MELEE;

	self->monsterinfo.scale = MODEL_SCALE;
	self->monsterinfo.can_jump = !(self->spawnflags & SPAWNFLAG_KNIGHT_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	walkmonster_start(self);
}

/*QUAKED monster_statue (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight NoJumping Kneeling Stationary
Stone statue knight that is invulnerable until triggered.

Keys:
frame - frame to be frozen in (default 44 = kneel5)

model="models/monsters/knight/tris.md2"
*/
void SP_monster_statue(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	if (!self->targetname)
	{
		statue_notrigger(self);
	}

	SP_monster_knight(self);

	sound_death.assign("statue/death.wav");
	sound_pain.assign("statue/pain.wav");
	sound_idle.assign("statue/idle.wav");
	sound_sight.assign("statue/see.wav");

	self->s.skinnum = 2;
	self->flags |= FL_MECHANICAL | FL_DEEPONE;

	if (!st.was_key_specified("frame"))
		self->s.frame = FRAME_kneel5;

	self->takedamage = false;
	self->use = statue_activate;

	self->pain = nullptr;
	self->die = nullptr;

	self->monsterinfo.sight = nullptr;
	self->monsterinfo.search = nullptr;
	self->monsterinfo.stand = nullptr;
	self->monsterinfo.walk = nullptr;
	self->monsterinfo.run = nullptr;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = knight_attack_run;
	self->monsterinfo.melee = knight_melee;
	self->monsterinfo.setskin = knight_setskin;

	self->think = statue_think;
	self->nextthink = level.time + FRAME_TIME_S;
}