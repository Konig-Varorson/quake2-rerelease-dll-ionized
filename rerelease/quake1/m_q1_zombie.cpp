/*
==============================================================================

QUAKE ZOMBIE

==============================================================================
*/


#include "../g_local.h"
#include "m_q1_zombie.h"

static cached_soundindex sound_sight;
static cached_soundindex sound_search;
static cached_soundindex sound_fling;
static cached_soundindex sound_pain1;
static cached_soundindex sound_pain2;
static cached_soundindex sound_fall;
static cached_soundindex sound_miss;
static cached_soundindex sound_hit;
static cached_soundindex sound_gib;
static cached_soundindex  sound_shot;

void zombie_down(edict_t* self);
void zombie_get_up_attempt(edict_t* self);

//
// SOUNDS
//

MONSTERINFO_SIGHT(zombie_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(zombie_search) (edict_t* self) -> void
{
	if (frandom() < 0.2)
		gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t zombie_frames_stand[] =
{
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
MMOVE_T(zombie_move_stand) = { 0, 14, zombie_frames_stand, nullptr };

MONSTERINFO_STAND(zombie_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &zombie_move_stand);
}

void zombie_reset_state(edict_t* self)
{
	self->zombie_state = 0;
}

//
// WALK
// 

mframe_t zombie_frames_walk[] =
{
	{ ai_walk },
	{ ai_walk, 2},
	{ ai_walk, 3},
	{ ai_walk, 2},
	{ ai_walk, 1},
	{ ai_walk },
	{ ai_walk },
	{ ai_walk },
	{ ai_walk },
	{ ai_walk },
	{ ai_walk, 2},
	{ ai_walk, 2},
	{ ai_walk, 1},
	{ ai_walk },
	{ ai_walk },
	{ ai_walk },
	{ ai_walk },
	{ ai_walk },
	{ ai_walk }
};
MMOVE_T(zombie_move_walk) = { 15, 33, zombie_frames_walk, nullptr };

MONSTERINFO_WALK(zombie_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &zombie_move_walk);
}

//
// RUN
// 

mframe_t zombie_frames_run[] =
{
	{ ai_run, 1, zombie_reset_state},
	{ ai_run, 1},
	{ ai_run },
	{ ai_run, 1},
	{ ai_run, 2},
	{ ai_run, 3},
	{ ai_run, 4},
	{ ai_run, 4},
	{ ai_run, 2},
	{ ai_run },
	{ ai_run },
	{ ai_run },
	{ ai_run, 2},
	{ ai_run, 4},
	{ ai_run, 6},
	{ ai_run, 7},
	{ ai_run, 3},
	{ ai_run, 8}
};
MMOVE_T(zombie_move_run) = { 34, 51, zombie_frames_run, nullptr };

MONSTERINFO_RUN(zombie_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &zombie_move_run);
}

//
// ATTACK
//

void zombie_firegib(edict_t* self, vec3_t offset)
{
	vec3_t	start;
	vec3_t	forward, right;
	vec3_t	target;
	vec3_t	aim;
	int damage;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);

	target = self->enemy->s.origin;

	if (range_to(self, self->enemy) <= RANGE_MELEE)
	{
		target[2] += self->enemy->viewheight;
	}
	else if (range_to(self, self->enemy) <= RANGE_NEAR)
	{
		target = target + (self->enemy->velocity * -0.04);
		target[2] += (self->enemy->viewheight * 0.9);
	}
	else if (range_to(self, self->enemy) <= RANGE_MID)
	{
		target = target + (self->enemy->velocity * -0.08);
		target[2] += (self->enemy->viewheight * 0.8);
	}
	else
	{
		target = target + (self->enemy->velocity * -0.01);
		target[2] += (rand() & self->enemy->viewheight);
	}

	aim = target - start;
	aim.normalized();

	if (strcmp(self->classname, "monster_mummy") == 0)
	{
		damage = 40;
	}
	else
	{
		damage = 10;
	}

	gi.sound(self, CHAN_WEAPON | CHAN_RELIABLE, sound_shot, 1.0, ATTN_NORM, 0);

	if (M_CalculatePitchToFire(self, target, start, aim, 500, 2.5f, false))
		fire_gib(self, start, aim, damage, 500, (crandom_open() * 10.0f), frandom() * 10.f);
	else
		fire_gib(self, start, aim, damage, 500, (crandom_open() * 10.0f), frandom() * 10.f);

}

void zombie_firegib1(edict_t* self)
{
	vec3_t offset;
	offset[0] = -8;
	offset[1] = -18;
	offset[2] = 30;

	zombie_firegib(self, offset);
}

void zombie_firegib2(edict_t* self)
{
	vec3_t offset;
	offset[0] = -8;
	offset[1] = -18;
	offset[2] = 30;
	zombie_firegib(self, offset);
}

void zombie_firegib3(edict_t* self)
{
	vec3_t offset;
	offset[0] = -8;
	offset[1] = 22;
	offset[2] = 30;
	zombie_firegib(self, offset);
}

mframe_t zombie_frames_attack1[] =
{
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, zombie_firegib1}
};
MMOVE_T(zombie_move_attack1) = { 52, 64, zombie_frames_attack1, zombie_run };

mframe_t zombie_frames_attack2[] =
{
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, zombie_firegib2}
};
MMOVE_T(zombie_move_attack2) = { 65, 78, zombie_frames_attack2, zombie_run };

mframe_t zombie_frames_attack3[] =
{
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, zombie_firegib3}
};
MMOVE_T(zombie_move_attack3) = { 79, 90, zombie_frames_attack3, zombie_run };

MONSTERINFO_ATTACK(zombie_attack)(edict_t* self) ->void
{
	float r = frandom();
	if (r < 0.3)
		M_SetAnimation(self, &zombie_move_attack1);
	else if (r < 0.6)
		M_SetAnimation(self, &zombie_move_attack2);
	else
		M_SetAnimation(self, &zombie_move_attack3);
}

//
// PAIN
//

mframe_t zombie_frames_get_up[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
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
MMOVE_T(zombie_move_get_up) = { 173, 191, zombie_frames_get_up, zombie_run };

void zombie_pain1(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
}

void zombie_pain2(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
}

void zombie_hit_floor(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_fall, 1, ATTN_NORM, 0);
}

void zombie_get_up(edict_t* self)
{
	self->maxs = { 16,16,40 };
	self->takedamage = true;
	self->health = 60;
	zombie_sight(self, self->enemy);

	if (!M_walkmove(self, 0, 0))
	{
		zombie_get_up_attempt(self);
		return;
	}
	M_SetAnimation(self, &zombie_move_get_up);
}

void zombie_start_fall(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
}

mframe_t zombie_frames_get_up_attempt[] =
{
	{ ai_move, 0,		zombie_get_up_attempt}
};
MMOVE_T(zombie_move_get_up_attempt) = { 173, 173, zombie_frames_get_up_attempt, nullptr };

void zombie_get_up_attempt(edict_t* self)
{
	static int down = 0;
	zombie_down(self);

	// Try getting up in 5 seconds
	if (down >= 500) {
		down = 0;
		zombie_get_up(self);
		return;
	}
	self->s.frame = 172;
	M_SetAnimation(self, &zombie_move_get_up_attempt);
	down++;
}

void zombie_down(edict_t* self)
{
	self->takedamage = false;
	self->health = 60;
	self->maxs = { 16, 16, 0 };
}

mframe_t zombie_frames_pain1[] =
{
	{ ai_move, 0, zombie_pain1},
	{ ai_move, 3},
	{ ai_move, 1},
	{ ai_move, 1},
	{ ai_move, 3},
	{ ai_move, 1},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(zombie_move_pain1) = { 91, 102, zombie_frames_pain1, zombie_run };

mframe_t zombie_frames_pain2[] =
{
	{ ai_move, 0, zombie_pain2},
	{ ai_move, 2},
	{ ai_move, 8},
	{ ai_move, 6},
	{ ai_move, 2},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, zombie_hit_floor},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 1},
	{ ai_move },
	{ ai_move }
};
MMOVE_T(zombie_move_pain2) = { 103, 130, zombie_frames_pain2, zombie_run };

mframe_t zombie_frames_pain3[] =
{
	{ ai_move, 0, zombie_pain2},
	{ ai_move },
	{ ai_move, 3},
	{ ai_move, 1},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 1},
	{ ai_move, 1},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(zombie_move_pain3) = { 131, 148, zombie_frames_pain3, zombie_run };

mframe_t zombie_frames_pain4[] =
{
	{ ai_move, 0, zombie_pain1},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 1},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(zombie_move_pain4) = { 149, 161, zombie_frames_pain4, zombie_run };

mframe_t zombie_frames_fall_start[] =
{
	{ ai_move, 0,	zombie_start_fall},
	{ ai_move, -8},
	{ ai_move, -5},
	{ ai_move, -3},
	{ ai_move, -1},
	{ ai_move, -2},
	{ ai_move, -1},
	{ ai_move, -1},
	{ ai_move, -2},
	{ ai_move, 0,	zombie_hit_floor},
	{ ai_move, 0,	zombie_down}
};
MMOVE_T(zombie_move_fall_start) = { 162, 172, zombie_frames_fall_start, zombie_get_up_attempt };

PAIN(zombie_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float r;
	self->health = 60;

	if (damage < 9)
		return;
	if (self->zombie_state == 2)
		return;
	if (damage >= 25)
	{
		self->zombie_state = 2;
		M_SetAnimation(self, &zombie_move_fall_start);
		return;
	}
	if (self->pain_debounce_time > level.time)
	{
		self->zombie_state = 2;
		M_SetAnimation(self, &zombie_move_fall_start);
		return;
	}
	if (self->zombie_state)
	{
		self->pain_debounce_time = level.time + 3_sec;
		return;
	}
	self->zombie_state = 1;

	// decino: No pain animations in Nightmare mode
	if (skill->value >= 3)
		return;
	r = frandom();

	if (r < 0.25)
		M_SetAnimation(self, &zombie_move_pain1);
	else if (r < 0.5)
		M_SetAnimation(self, &zombie_move_pain2);
	else if (r < 0.75)
		M_SetAnimation(self, &zombie_move_pain3);
	else
		M_SetAnimation(self, &zombie_move_pain4);
}

//
// DEATH
//

DIE(zombie_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	gi.sound(self, CHAN_VOICE, sound_gib, 1, ATTN_NORM, 0);

	ThrowGibs(self, damage, {
		{ "models/objects/gibs/bone/tris.md2" },
		{ "models/objects/gibs/sm_meat/tris.md2" },
		{ "models/objects/gibs/head2/tris.md2", GIB_HEAD }
		});
	self->deadflag = true;
}

MONSTERINFO_CHECKATTACK(mummy_checkattack) (edict_t* self) -> bool
{
	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.8f, 0.8f, 0.f, 0.f);
}


/*QUAKED monster_zombie (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/zombie/tris.md2"
*/

void SP_monster_zombie(edict_t* self)
{
	self->s.modelindex = gi.modelindex("models/monsters/zombie/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 40 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = 60;

	sound_sight.assign("zombie/z_idle.wav");
	sound_search.assign("zombie/idle_w2.wav");
	sound_fling.assign("zombie/z_shot1.wav");
	sound_pain1.assign("zombie/z_pain.wav");
	sound_pain2.assign("zombie/z_pain1.wav");
	sound_fall.assign("zombie/z_fall.wav");
	sound_miss.assign("zombie/z_miss.wav");
	sound_hit.assign("zombie/z_hit.wav");
	sound_gib.assign("zombie/z_gib.wav");
	sound_shot.assign("zombie/z_shot1.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->gib_health = -5;
	self->mass = 60;
	self->flags |= FL_DEEPONE;

	self->monsterinfo.stand = zombie_stand;
	self->monsterinfo.walk = zombie_walk;
	self->monsterinfo.run = zombie_run;
	self->monsterinfo.attack = zombie_attack;
	self->monsterinfo.sight = zombie_sight;
	self->monsterinfo.search = zombie_search;

	self->pain = zombie_pain;
	self->die = zombie_die;

	gi.linkentity(self);

	M_SetAnimation(self, &zombie_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start(self);
}

/*QUAKED monster_mummy (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/zombie/tris.md2"
*/
void SP_monster_mummy(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	SP_monster_zombie(self);

	self->s.skinnum = 1;
	self->health = max(1500, 1500 + 1000 * (skill->integer - 1)) * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(350, 350 + 100 * (skill->integer - 1));
	if (coop->integer)
	{
		self->health += (250 * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (100 * (CountPlayers() - 1));
	}
	self->max_health = self->health;

	self->monsterinfo.checkattack = mummy_checkattack;
}

//======================================================================
// CRUCIFIED ZOMBIE
//======================================================================

void misc_zombie_crucified_think(edict_t* self)
{
	float r1;

	if (++self->s.frame < 198) // was FRAME_death4_22
		self->nextthink = level.time + FRAME_TIME_S;
	else
	{
		self->s.frame = 192;
		self->nextthink = level.time + FRAME_TIME_S;
	}
	r1 = frandom();
	if (r1 <= .017)
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("q1zombie/idle_w2.wav"), 1, ATTN_IDLE, 0);
	}
}

/*QUAKED misc_zombie_crucified (1 .5 0) (-16 -16 -24) (16 16 32)
model="models/monsters/q1zombie/"
frame="192"
*/
void SP_misc_zombie_crucified(edict_t* self)
{
	// precache
	gi.soundindex("q1zombie/idle_w2.wav");

	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_NOT;
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };
	self->s.modelindex = gi.modelindex("models/monsters/q1zombie/tris.md2");
	self->s.frame = 192;

	gi.linkentity(self);

	self->think = misc_zombie_crucified_think;
	self->nextthink = level.time + FRAME_TIME_S;
}