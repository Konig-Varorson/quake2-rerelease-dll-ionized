/*
==============================================================================

QUAKE ROTFISH

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_rotfish.h"

static cached_soundindex	sound_chomp;
static cached_soundindex	sound_death;
static cached_soundindex	sound_idle;

void rotfish_stand(edict_t* self);

void rotfish_idle (edict_t *self)
{
	if (frandom() < 0.5)
		gi.sound (self, CHAN_AUTO, sound_idle, 1, ATTN_IDLE, 0);
}


mframe_t rotfish_frames_stand [] =
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
	{ai_stand}
};	
MMOVE_T(rotfish_move_stand) = {FRAME_swim1, FRAME_swim18, rotfish_frames_stand, nullptr };

MONSTERINFO_STAND(rotfish_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &rotfish_move_stand);
}

mframe_t rotfish_frrun_loop [] =
{
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12}
};	
MMOVE_T(rotfish_move_run_loop) = {FRAME_swim1, FRAME_swim18, rotfish_frrun_loop, nullptr };

MONSTERINFO_RUN(rotfish_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &rotfish_move_run_loop);
}

/* Standard Swimming */ 
mframe_t rotfish_walk_loop [] =
{
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8},
	{ai_walk, 8}
};	
MMOVE_T(rotfish_move_walk_loop) = {FRAME_swim1, FRAME_swim18, rotfish_walk_loop, nullptr };

MONSTERINFO_WALK(rotfish_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &rotfish_move_walk_loop);
}

mframe_t rotfish_frames_pain1 [] =
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
MMOVE_T(rotfish_move_pain1) = {FRAME_pain1, FRAME_pain9, rotfish_frames_pain1, rotfish_run};

void rotfish_bite (edict_t *self)
{
	vec3_t aim = { MELEE_DISTANCE, 0, 0 };
	fire_hit(self, aim, 5, 0);
}

mframe_t rotfish_frames_attack [] =
{
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 0,	rotfish_bite},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 0,	rotfish_bite},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 0,	rotfish_bite},
	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge, 10},
};
MMOVE_T(rotfish_move_attack) = {FRAME_attack1, FRAME_attack18, rotfish_frames_attack, rotfish_run};

MONSTERINFO_MELEE(rotfish_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &rotfish_move_attack);
}

PAIN(rotfish_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 1_sec;
		
	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare
	
	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	M_SetAnimation(self, &rotfish_move_pain1);
}

MONSTERINFO_SETSKIN(rotfish_setskin) (edict_t* self) -> void
{
	/* KONIG - allow multiple skins */
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

void rotfish_dead(edict_t* self)
{
	self->mins = { -16, -16, -8 };
	self->maxs = { 16, 16, 8 };
	monster_dead(self);
}

mframe_t rotfish_frames_death [] =
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
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(rotfish_move_death) = {FRAME_death1, FRAME_death21, rotfish_frames_death, rotfish_dead};

MONSTERINFO_SIGHT(rotfish_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
}

DIE(rotfish_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	// check for gib
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);
		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/objects/gibs/sm_meat/tris.md2", GIB_HEAD }
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
	self->svflags |= SVF_DEADMONSTER;
	M_SetAnimation(self, &rotfish_move_death);
}

static void rotfish_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 30.f;
	self->monsterinfo.fly_speed = 110.f;
	// only melee, so get in close
	self->monsterinfo.fly_min_distance = 10.f;
	self->monsterinfo.fly_max_distance = 10.f;
}


//
// SPAWN
//

/*QUAKED monster_rotfish (1 .5 0) (-16 -16 -24) (16 16 24) Ambush Trigger_Spawn Sight
model="models/monsters/fish/tris.md2"
*/
void SP_monster_rotfish (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_death.assign("fish/death.wav");	
	sound_chomp.assign("fish/bite.wav");
	sound_idle.assign("fish/idle.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/fish/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 24 };

	self->health = 40 * st.health_multiplier;
	self->gib_health = -20;
	self->mass = 100;

	self->pain = rotfish_pain;
	self->die = rotfish_die;



	self->monsterinfo.stand = rotfish_stand;
	self->monsterinfo.walk = rotfish_walk;
	self->monsterinfo.run = rotfish_run;
	self->monsterinfo.melee = rotfish_melee;
	self->monsterinfo.sight = rotfish_sight;
	self->monsterinfo.setskin = rotfish_setskin;

	gi.linkentity (self);

	M_SetAnimation(self, &rotfish_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	rotfish_set_fly_parameters(self);

	swimmonster_start (self);
}
