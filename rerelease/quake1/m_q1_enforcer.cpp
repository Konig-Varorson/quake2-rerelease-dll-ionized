/*
==============================================================================

QUAKE ENFORCER

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_enforcer.h"
#include "../m_flash.h"

static int	sound_pain1;
static int	sound_pain2;
static int	sound_death;
static int	sound_gib;
static int	sound_idle;
static int	sound_sight1;
static int	sound_sight2;
static int	sound_sight3;
static int	sound_sight4;
static int	sound_fire;

void enforcer_fire (edict_t *self);

MONSTERINFO_IDLE(enforcer_idlesound) (edict_t* self) -> void
{
	gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SIGHT (enforcer_sight) (edict_t *self, edict_t *other) -> void
{
	float r = frandom();

	if (r < 0.25f)
		gi.sound (self, CHAN_VOICE, sound_sight1, 1, ATTN_NORM, 0);
	else if (r < 0.5f)
		gi.sound (self, CHAN_VOICE, sound_sight2, 1, ATTN_NORM, 0);
	else if (r < 0.75f)
		gi.sound (self, CHAN_VOICE, sound_sight3, 1, ATTN_NORM, 0);
	else
		gi.sound (self, CHAN_VOICE, sound_sight4, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(enforcer_search) (edict_t *self) -> void
{
	gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

mframe_t enforcer_frames_stand [] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(enforcer_move_stand) = {FRAME_stand1, FRAME_stand7, enforcer_frames_stand, nullptr };

MONSTERINFO_STAND(enforcer_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &enforcer_move_stand);
}

mframe_t enforcer_frames_walk [] =
{
	{ai_walk, 3},
	{ai_walk, 4},
	{ai_walk, 4},
	{ai_walk, 5},
	{ai_walk, 4},
	{ai_walk, 3},
	{ai_walk, 2},
	{ai_walk},
	{ai_walk},
	{ai_walk, 2},
	{ai_walk, 3},
	{ai_walk, 4},
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 4},
	{ai_walk, 3}
};
MMOVE_T(enforcer_move_walk) = {FRAME_walk1, FRAME_walk16, enforcer_frames_walk, nullptr };

MONSTERINFO_WALK(enforcer_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &enforcer_move_walk);
}

mframe_t enforcer_frames_run [] =
{
	{ai_run, 22},
	{ai_run, 8},
	{ai_run, 7},
	{ai_run, 8},
	{ai_run, 13},
	{ai_run, 12},
	{ai_run, 11},
	{ai_run, 7}
};

MMOVE_T(enforcer_move_run) = {FRAME_run1, FRAME_run8, enforcer_frames_run, nullptr };

MONSTERINFO_RUN(enforcer_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &enforcer_move_stand);
	else
		M_SetAnimation(self, &enforcer_move_run);
}

mframe_t enforcer_frames_pain3 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(enforcer_move_pain3) = {FRAME_paina1, FRAME_paina4, enforcer_frames_pain3, enforcer_run};

mframe_t enforcer_frames_pain2 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(enforcer_move_pain2) = {FRAME_painb1, FRAME_painb5, enforcer_frames_pain2, enforcer_run};

mframe_t enforcer_frames_pain1 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(enforcer_move_pain1) = {FRAME_painc1, FRAME_painc8, enforcer_frames_pain1, enforcer_run};

PAIN(enforcer_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum = 1;

	if (level.time < self->pain_debounce_time)
		return;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	self->pain_debounce_time = level.time + 3_sec;

	if (rand()&1)
		gi.sound (self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	else
		gi.sound (self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);

	if (damage <= 10)
		M_SetAnimation(self, &enforcer_move_pain3);
	else if (damage <= 25)
		M_SetAnimation(self, &enforcer_move_pain2);
	else
		M_SetAnimation(self, &enforcer_move_pain1);
}

MONSTERINFO_SETSKIN(enforcer_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

void enforcer_dead (edict_t *self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

static void enfrocer_shrink(edict_t* self)
{
	self->maxs[2] = -4;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t enforcer_frames_death1 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
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
MMOVE_T(enforcer_move_death1) = {FRAME_death1, FRAME_death14, enforcer_frames_death1, enforcer_dead};

mframe_t enforcer_frames_death2 [] =
{
	{ai_move},
	{ai_move},
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
MMOVE_T(enforcer_move_death2) = {FRAME_fdeath1, FRAME_fdeath11, enforcer_frames_death2, enforcer_dead};

DIE(enforcer_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	// check for gib
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/monsters/enforcer/head/tris.md2", GIB_HEAD | GIB_SKINNED }
			});
		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	self->deadflag = true;
	self->takedamage = true;
	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);

	if (frandom() < 0.5f)
		M_SetAnimation(self, &enforcer_move_death1);
	else
		M_SetAnimation(self, &enforcer_move_death2);
}

void enforcer_fire (edict_t *self)
{
	vec3_t	forward, right;
	vec3_t	start, aim;

	if ((!self->enemy) || (!self->enemy->inuse))
	{
		self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;
		return;
	}

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[MZ2_SOLDIER_BLASTER_1], forward, right);
	PredictAim(self, self->enemy, start, 0, true, -0.2f, &aim, nullptr);

	gi.sound (self, CHAN_WEAPON|CHAN_RELIABLE, sound_fire, 1, ATTN_NORM, 0);
	monster_fire_blaster(self, start, aim, 15, 600, MZ2_SOLDIER_BLASTER_1, EF_BLASTER);
}

void enforcer_secondfire (edict_t *self)
{
	if (!self->radius_dmg)
	{
		self->monsterinfo.nextframe = 36;
		self->radius_dmg = 1;
	}
	else
		self->radius_dmg = 0;
}

mframe_t enforcer_frames_start_chain [] =
{
	{ai_charge,   0},
	{ai_charge,   1},
	{ai_charge,   1},
	{ai_charge,   0},
	{ai_charge,   0},
	{ai_charge, 0, enforcer_fire},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, enforcer_secondfire},
	{ai_charge}
};
MMOVE_T(enforcer_move_start_fire) = {FRAME_attack1, FRAME_attack10, enforcer_frames_start_chain, enforcer_run};

MONSTERINFO_ATTACK(enforcer_attack) (edict_t* self) -> void
{
	M_SetAnimation(self, &enforcer_move_start_fire);
}


//
// SPAWN
//

/*QUAKED SP_monster_enforcer (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/enforcer/tris.md2"
*/
void SP_monster_enforcer (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain1		= gi.soundindex ("enforcer/pain1.wav");
	sound_pain2		= gi.soundindex ("enforcer/pain2.wav");
	sound_death		= gi.soundindex ("enforcer/death1.wav");
	sound_gib		= gi.soundindex ("q1player/udeath.wav");
	sound_idle		= gi.soundindex ("enforcer/idle1.wav");
	sound_sight1	= gi.soundindex ("enforcer/sight1.wav");
	sound_sight2	= gi.soundindex ("enforcer/sight2.wav");
	sound_sight3	= gi.soundindex ("enforcer/sight3.wav");
	sound_sight4	= gi.soundindex ("enforcer/sight4.wav");
	sound_fire		= gi.soundindex ("enforcer/enfire.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/enforcer/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 40 };

	self->health = 80 * st.health_multiplier;
	self->gib_health = -35;
	self->mass = 175;

	self->monsterinfo.aiflags |= AI_STINKY;

	self->pain = enforcer_pain;
	self->die = enforcer_die;

	self->monsterinfo.stand = enforcer_stand;
	self->monsterinfo.walk = enforcer_walk;
	self->monsterinfo.run = enforcer_run;
	self->monsterinfo.dodge = nullptr ;
	self->monsterinfo.attack = enforcer_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = enforcer_sight;
	self->monsterinfo.search = enforcer_search;

	gi.linkentity (self);
	self->flags |= FL_DEEPONE;

	M_SetAnimation(self, &enforcer_move_stand);	
	self->monsterinfo.scale = MODEL_SCALE;
	
	walkmonster_start (self);
}
