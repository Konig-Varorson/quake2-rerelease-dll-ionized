/*
==============================================================================

QUAKE ZOMBIE

==============================================================================
*/


#include "../g_local.h"
#include "m_zombie.h"


static int  sound_idleC;
static int	sound_idle;
static int	sound_idle2;
static int	sound_pain;
static int	sound_pain2;
static int  sound_fall;
static int  sound_gib;
static int  sound_shot;

void zombie_down(edict_t* self);
void zombie_fallagain(edict_t* self);
void zombie_pain2(edict_t* self);
void zombie_idle3(edict_t* self);
void zombie_run(edict_t* self);
void zombie_attack(edict_t* self);

//
// CRUCIFIED
//

void zombie_Cidle(edict_t* self)
{
	if (frandom() < 0.1)
		gi.sound(self, CHAN_VOICE, sound_idleC, 1, ATTN_IDLE, 0);
}

mframe_t zombie_frames_pain5[] =
{
	{ ai_move, 0, zombie_pain2 },
	{ ai_move, -8},
	{ ai_move, -5},
	{ ai_move, -3},
	{ ai_move, -1},
	{ ai_move, -2},
	{ ai_move, -1},
	{ ai_move, -1},
	{ ai_move, -2},
	{ ai_move, 0, zombie_down},		//10
	{ ai_move},
	{ ai_move, 0, zombie_idle3},
	{ ai_move, 0, zombie_fallagain},
	{ ai_move},
	{ ai_move},				//15
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},	
	{ ai_move},				//20
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move, 5},
	{ ai_move, 3},
	{ ai_move, 1},
	{ ai_move, -1},
	{ ai_move},
	{ ai_move},
	{ ai_move}			//30
};
MMOVE_T(zombie_move_pain5) = { FRAME_paine1, FRAME_paine30, zombie_frames_pain5, zombie_run };

//
// SOUNDS
//

void zombie_idle3(edict_t* self)
{
	trace_t tr;

	self->health = self->max_health;
	self->solid = SOLID_BBOX;
	self->count = 0;
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
	// Check if player is standing on top
	tr = gi.trace(self->s.origin, self->mins, self->maxs, self->s.origin, self, CONTENTS_PLAYERCLIP | CONTENTS_MONSTER);
	if (tr.ent && tr.ent->solid != SOLID_BSP)
	{
		self->solid = SOLID_NOT;
		self->count = 1;
		self->s.frame = FRAME_paine11;
		self->nextthink += 5_sec;
		self->pain_debounce_time = level.time + 3_sec;
	}
}

void zombie_idle(edict_t* self)
{
	if (frandom() > 0.8)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);

	if (self->solid == SOLID_NOT)
		self->solid = SOLID_BBOX;
}

void zombie_idle2(edict_t* self)
{
	if (frandom() > 0.2)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_idle2, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SIGHT(zombie_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);

	if (!self->style && (skill->integer > 0) && (range_to(self, self->enemy) >= RANGE_MID))
		zombie_attack(self);
}

void zombie_pain1(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);
	self->health = self->max_health / 2; //reduce health every revival to make killing easier eventually
	self->max_health = self->health;
}

void zombie_pain2(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
	self->health = self->max_health / 2; //reduce health every revival to make killing easier eventually
	self->max_health = self->health;
}

void zombie_fall(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_fall, 1, ATTN_NORM, 0);
	self->health = self->max_health;
}

//
// STAND
//

mframe_t zombie_frames_stand[] =
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
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(zombie_move_stand) = { FRAME_stand1, FRAME_stand15, zombie_frames_stand, nullptr };

MONSTERINFO_STAND(zombie_stand) (edict_t* self) -> void
{
	self->solid = SOLID_BBOX;
	self->count = 0;
	M_SetAnimation(self, &zombie_move_stand);
}


//Crucified
void zombie_crucify(edict_t* self);

mframe_t zombie_frames_cruc[] =
{
	{ai_stand},
	{ai_stand, 0, zombie_Cidle},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
};
MMOVE_T(zombie_crucified) = { FRAME_cruc_1, FRAME_cruc_6, zombie_frames_cruc, zombie_crucify };

void zombie_crucify(edict_t* self)
{
	M_SetAnimation(self, &zombie_crucified);
}

//
// WALK
//

mframe_t zombie_frames_walk[] =
{
	{ai_walk},
	{ai_walk, 2},
	{ai_walk, 3},
	{ai_walk, 2},
	{ai_walk, 1},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 1},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk, 0,  zombie_idle}
};
MMOVE_T(zombie_move_walk) = { FRAME_walk1, FRAME_walk19, zombie_frames_walk, nullptr };

MONSTERINFO_WALK(zombie_walk) (edict_t* self) -> void
{
	self->solid = SOLID_BBOX;
	self->count = 0;
	M_SetAnimation(self, &zombie_move_walk);
}

//
// RUN
//

mframe_t zombie_frames_run[] =
{
	{ai_run, 1},
	{ai_run, 1},
	{ai_run},
	{ai_run, 1},
	{ai_run, 2},
	{ai_run, 3},
	{ai_run, 4},
	{ai_run, 4},
	{ai_run, 2},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run, 2},
	{ai_run, 4},
	{ai_run, 6},
	{ai_run, 7},
	{ai_run, 3},
	{ai_run, 8, zombie_idle2}
};
MMOVE_T(zombie_move_run) = { FRAME_run1, FRAME_run18, zombie_frames_run, zombie_run };

MONSTERINFO_RUN(zombie_run) (edict_t* self) -> void
{
	self->solid = SOLID_BBOX;
	self->count = 0;

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &zombie_move_stand);
		return;
	}
	else
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
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge, 0, zombie_firegib1}
};
MMOVE_T(zombie_move_attack1) = { FRAME_atta1, FRAME_atta13, zombie_frames_attack1, zombie_run };

mframe_t zombie_frames_attack2[] =
{
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge, 0, zombie_firegib2}
};
MMOVE_T(zombie_move_attack2) = { FRAME_attb1, FRAME_attb14, zombie_frames_attack2, zombie_run };


mframe_t zombie_frames_attack3[] =
{
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge},
	{ ai_charge, 0, zombie_firegib3}
};
MMOVE_T(zombie_move_attack3) = { FRAME_attc1, FRAME_attc12, zombie_frames_attack3, zombie_run };


MONSTERINFO_ATTACK(zombie_attack) (edict_t* self) -> void
{
	float r = frandom();

	if (self->style)
		return;

	self->solid = SOLID_BBOX;
	self->count = 0;

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

mframe_t zombie_frames_pain1[] =
{
	{ ai_move, 0, zombie_pain1},
	{ ai_move, 3},
	{ ai_move, 1},
	{ ai_move, -1},
	{ ai_move, -3},
	{ ai_move, -1},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move}
};
MMOVE_T(zombie_move_pain1) = { FRAME_paina1, FRAME_paina12, zombie_frames_pain1, zombie_run };

mframe_t zombie_frames_pain2[] =
{
	{ ai_move, 0, zombie_pain2},
	{ ai_move, -2},
	{ ai_move, -8},
	{ ai_move, -6},
	{ ai_move, -2},
	{ ai_move, 0},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move, 0, zombie_fall},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move, 1},
	{ ai_move},
	{ ai_move}
};
MMOVE_T(zombie_move_pain2) = { FRAME_painb1, FRAME_painb28, zombie_frames_pain2, zombie_run };

mframe_t zombie_frames_pain3[] =
{
	{ ai_move, 0, zombie_pain2},
	{ ai_move},
	{ ai_move, -3},
	{ ai_move, -1},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move, 1},
	{ ai_move, 1},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move}
};
MMOVE_T(zombie_move_pain3) = { FRAME_painc1, FRAME_painc18, zombie_frames_pain3, zombie_run };

mframe_t zombie_frames_pain4[] =
{
	{ ai_move, 0, zombie_pain1},
	{ ai_move},
	{ ai_move},
	{ ai_move, 0},
	{ ai_move, 0},
	{ ai_move, 0},
	{ ai_move},
	{ ai_move},
	{ ai_move},
	{ ai_move, -1},
	{ ai_move},
	{ ai_move},
	{ ai_move}
};
MMOVE_T(zombie_move_pain4) = { FRAME_paind1, FRAME_paind13, zombie_frames_pain4, zombie_run };

// fall and stand up functions

void zombie_down(edict_t* self)
{
	self->solid = SOLID_NOT;
	zombie_fall(self);
	self->nextthink += 5_sec;
}

PAIN(zombie_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float	r;

	self->health = self->max_health;
	
	if (gtime_t::from_sec(self->wait) > level.time)

	{
		self->dmg += damage;
		if (self->dmg > 30)
		{
			M_SetAnimation(self, &zombie_move_pain5);
			self->pain_debounce_time = level.time + 3_sec;
			self->dmg = 0;
			self->wait = 0;
			self->solid = SOLID_NOT;
			self->count = 1;
			return;
		}
	}
	else
	{
		gtime_t::from_sec(self->wait) = level.time + 1_sec;
		self->dmg = damage;
	}

	if (damage < 15)
		return;

	if (damage >= 25)
	{
		M_SetAnimation(self, &zombie_move_pain5);
		self->pain_debounce_time = level.time + 3_sec;
		self->count = 1;
		return;
	}

	if (self->pain_debounce_time > level.time)
		return;

	r = frandom();

	if (r < 0.25)
	{
		self->pain_debounce_time = level.time + 1.0_sec;
		gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zombie_move_pain1);
	}
	else if (r < 0.5)
	{
		self->pain_debounce_time = level.time + 1.5_sec;
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zombie_move_pain2);
	}
	else if (r < 0.75)
	{
		self->pain_debounce_time = level.time + 1.1_sec;
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zombie_move_pain3);
	}
	else
	{
		self->pain_debounce_time = level.time + 1.0_sec;
		M_SetAnimation(self, &zombie_move_pain4);
	}
}

void zombie_fallagain(edict_t* self)
{
	if (self->pain_debounce_time > level.time)
	{
		M_SetAnimation(self, &zombie_move_pain5);
	}
}

//
// DEATH
//

DIE(zombie_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	int		n;

	gi.sound(self, CHAN_VOICE, sound_gib, 1, ATTN_NORM, 0);

	ThrowGibs(self, 1000, {
		{ 2, "models/objects/gibs/bone/tris.md2" },
		{ 4, "models/objects/gibs/sm_meat/tris.md2" },
		{ "models/monsters/zombie/gibs/head.md2", GIB_HEAD }
		});

	self->deadflag = true;
	self->takedamage = true;
}

MONSTERINFO_CHECKATTACK(mummy_checkattack) (edict_t* self) -> bool
{
	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.8f, 0.8f, 0.f, 0.f);
}

/*QUAKED monster_zombie (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/zombie/tris.md2"
"sounds"  1 makes the zombie crucified
*/

void SP_misc_zombie_crucified(edict_t* self);
void SP_monster_zombie(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (self->sounds) // change to different entity because crucified code here is broken
	{
		SP_misc_zombie_crucified(self);
		return;
	}

	if (deathmatch->value)
	{
		G_FreeEdict(self);
		return;
	}

	sound_idleC = gi.soundindex("zombie/idle_w2.wav");
	sound_idle = gi.soundindex("zombie/z_idle.wav");
	sound_idle2 = gi.soundindex("zombie/z_idle1.wav");
	sound_pain = gi.soundindex("zombie/z_pain.wav");
	sound_pain2 = gi.soundindex("zombie/z_pain1.wav");
	sound_fall = gi.soundindex("zombie/z_fall.wav");
	sound_gib = gi.soundindex("zombie/z_gib.wav");
	sound_shot = gi.soundindex("zombie/z_shot1.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex("models/monsters/zombie/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	self->health = 60 * st.health_multiplier;
	self->gib_health = 0;
	self->mass = 100;
	self->count = 0;
	self->max_health = self->health;	// save custom health

	self->pain = zombie_pain;
	self->die = zombie_die;

	self->monsterinfo.stand = zombie_stand;
	self->monsterinfo.walk = zombie_walk;
	self->monsterinfo.run = zombie_run;
	self->monsterinfo.attack = zombie_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = zombie_sight;

	gi.linkentity(self);

	M_SetAnimation(self, &zombie_move_stand);
	walkmonster_start(self);
	self->monsterinfo.scale = MODEL_SCALE;
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

/*QUAKED monster_mummy (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/zombie/tris.md2"
"sounds"  1 makes the zombie crucified
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