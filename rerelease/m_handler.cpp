/*
==============================================================================

handler handler

==============================================================================
*/

#include "g_local.h"
#include "m_handler.h"

constexpr spawnflags_t SPAWNFLAG_HANDLER_HOLD_DELAY = 8_spawnflag;
constexpr spawnflags_t SPAWNFLAG_HANDLER_ONE_ENTITY = 16_spawnflag;

static cached_soundindex sound_idle;
static cached_soundindex sound_idle1;
static cached_soundindex sound_idle2;
static cached_soundindex sound_attack;
static cached_soundindex sound_pain;
static cached_soundindex sound_sit;
static cached_soundindex sound_stand;

void SP_monster_hound(edict_t *self);
void SP_monster_infantry(edict_t *self);
void handler_standWhatNext(edict_t* self);
void handler_standSitWhatNext (edict_t *self);
void handler_stand (edict_t *self);
void handler_attack (edict_t *self);
void hound_createHound(edict_t *self, float healthPercent);
void handler_ConvertToInfantry(edict_t *self);

void hound_sight (edict_t *self, edict_t *other);
void infantry_sight (edict_t *self, edict_t *other);

MONSTERINFO_SIGHT(handler_sight) (edict_t* self, edict_t* other) -> void
{
	hound_sight(self, other);
	infantry_sight(self, other);
}

MONSTERINFO_IDLE(handler_idle) (edict_t* self) -> void
{
	if (brandom())
		gi.sound(self, CHAN_VOICE, sound_idle1, 1, ATTN_IDLE, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_idle2, 1, ATTN_IDLE, 0);
}

//
// STAND
//

mframe_t handler_frames_stand1 [] =
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
	{ai_stand},
	{ai_stand},

	{ai_stand}
};
MMOVE_T(handler_stand1) = { FRAME_idle101, FRAME_idle131, handler_frames_stand1, nullptr };

void handler_scratch(edict_t *self)
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

mframe_t handler_frames_stand2 [] =
{
	{ai_stand, 0, handler_scratch},
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
MMOVE_T(handler_stand2) = { FRAME_idle201, FRAME_idle230, handler_frames_stand2, nullptr };

mframe_t handler_frames_stand3 [] =
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
	{ai_stand},
	{ai_stand},
};
MMOVE_T(handler_stand3) = { FRAME_idle301, FRAME_idle330, handler_frames_stand3, nullptr };

mframe_t handler_frames_stand4 [] =
{
	{ai_stand}, //standup
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
};
MMOVE_T(handler_stand4) = { FRAME_stand01, FRAME_stand11, handler_frames_stand4, handler_standWhatNext};

mframe_t handler_frames_stand5 [] =
{
	{ai_stand}, //sitdown
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
};
MMOVE_T(handler_stand5) = { FRAME_sit01, FRAME_sit10, handler_frames_stand5, nullptr };

void handler_standWhatNext (edict_t *self)
{
	if(frandom() < 0.90)
		M_SetAnimation(self, &handler_stand3);
	else 
		M_SetAnimation(self, &handler_stand5);
}


void handler_standSitWhatNext (edict_t *self)
{
	M_SetAnimation(self, &handler_stand4);
}


MONSTERINFO_STAND(handler_stand) (edict_t* self) -> void
{
	float r;

	r = frandom();

	if (r < 0.70)
		M_SetAnimation(self, &handler_stand1);
	else if (r < 0.85)
		M_SetAnimation(self, &handler_stand2);
	else
		M_SetAnimation(self, &handler_stand5);
}

//
// PAIN
//

PAIN(handler_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);
}

//
// ATTACK and MELEE
//

void handler_createHound(edict_t *self)
{
	self->s.modelindex2 = 0;
	hound_createHound(self, (self->health / 175.0));
}


void CheckIdleLoop(edict_t *self)
{
	if (!self->spawnflags.has(SPAWNFLAG_HANDLER_HOLD_DELAY))
		self->powerarmor_time = level.time + (FRAME_TIME_S * frandom() * 3);

	if(self->powerarmor_time > level.time)
		self->s.frame -= 2;
}

void CheckForEnemy(edict_t *self)
{
	if(self->enemy && (self->enemy->client || (self->enemy->svflags & SVF_MONSTER)))
	{
		self->powerarmor_time = 0_ms;
		return;
	}

	if(self->powerarmor_time < level.time)
	{
		self->enemy = NULL;
		handler_stand(self);
		return;
	}

	self->s.frame--;
}

void StartCount(edict_t *self)
{
	self->powerarmor_time = level.time + 3000_ms;
}

mframe_t handler_frames_attack1 [] = {
	{ai_run, 0, StartCount},
	{ai_run, 0, CheckForEnemy},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},

	{ai_charge},
	{ai_charge},
	{ai_charge, 0, CheckIdleLoop},
	{ai_charge},

	{ai_charge},
	{ai_charge, 0, handler_createHound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
};
MMOVE_T(handler_move_attack1) = { FRAME_restrain01, FRAME_restrain18, handler_frames_attack1, handler_ConvertToInfantry};


MONSTERINFO_ATTACK(handler_attack) (edict_t *self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_attack, 1, ATTN_NORM, 0);

	M_SetAnimation(self, &handler_move_attack1);

	self->powerarmor_time = 0_ms;
}

//
// DEATH
//
#if 0
void handler_dead (edict_t *self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8	};
	self->movetype = MOVETYPE_TOSS;
	self->svflags |= SVF_DEADMONSTER;
	self->nextthink = 0;
	gi.linkentity (self);
}
#endif

DIE(handler_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	self->health = 1; // can't die while together...
}

void SP_monster_hound_precache();

void HandlerPrecache(void)
{
	SP_monster_hound_precache();

	sound_attack.assign("monsters/guard/hhattack.wav");
	sound_idle.assign("monsters/guard/hhscratch.wav");
	sound_sit.assign("monsters/guard/hhsitdown.wav");
	sound_stand.assign("monsters/guard/hhstandup.wav");
	sound_idle1.assign("infantry/infidle1.wav");
	sound_idle2.assign("monsters/hound/hsearch1.wav");

	gi.modelindex("models/monsters/guard/tris.md2");
}


/*QUAKED monster_handler (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void SP_monster_handler(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	HandlerPrecache();

	self->s.modelindex = gi.modelindex("models/monsters/handler/tris.md2");
	self->s.modelindex2 = gi.modelindex("models/monsters/hound/tris.md2");

	/*
		Handler
		X = -36 to 3
		Y = -3  to 27
		Z = -24 to 28

		Hound
		X = -12 to 11
		Y = -30 to 30
		Z = -24  to 8
	*/

	self->mins = { -32, -32, -24 };
	self->maxs = { 32, 32, 32 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = 275 * st.health_multiplier;
	self->gib_health = -65;
	self->mass = 450;

	self->pain = handler_pain;
	self->die = handler_die;

	self->monsterinfo.stand = handler_stand;
	self->monsterinfo.walk = handler_stand;
	self->monsterinfo.run = handler_attack;
	self->monsterinfo.attack = handler_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = handler_sight;
	self->monsterinfo.idle = handler_idle;

	gi.linkentity (self);

	M_SetAnimation(self, &handler_stand1);

	self->monsterinfo.scale = MODEL_SCALE;

	if (!self->spawnflags.has(SPAWNFLAG_HANDLER_ONE_ENTITY))
	{
		level.total_monsters++; // add one for the hound which is created later :)
	}

	walkmonster_start (self);
}

