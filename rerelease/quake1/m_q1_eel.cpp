/*
==============================================================================

QUAKE EEL

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_eel.h"

static cached_soundindex sound_chomp;
static cached_soundindex sound_death;
static cached_soundindex sound_idle;
static cached_soundindex sound_attack;

void eel_stand(edict_t* self);

void eel_idle(edict_t* self)
{
	if (frandom() < 0.5f)
		gi.sound(self, CHAN_AUTO, sound_idle, 1, ATTN_IDLE, 0);
}

//
// STAND
//

mframe_t eel_frames_stand[] =
{
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(eel_move_stand) = { FRAME_eelswim1, FRAME_eelswim6, eel_frames_stand, nullptr };

MONSTERINFO_STAND(eel_stand) (edict_t* self) -> void
{
	self->s.skinnum = 0;
	M_SetAnimation(self, &eel_move_stand);
}

//
// WALK
//

mframe_t eel_frames_walk[] =
{
	{ ai_walk, 6, eel_idle },
	{ ai_walk, 6 },
	{ ai_walk, 6 },
	{ ai_walk, 6 },
	{ ai_walk, 6 },
	{ ai_walk, 6 }
};
MMOVE_T(eel_move_walk) = { FRAME_eelswim1, FRAME_eelswim6, eel_frames_walk, nullptr };

MONSTERINFO_WALK(eel_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &eel_move_walk);
}

//
// RUN
//

mframe_t eel_frames_run[] =
{
	{ ai_run, 10, eel_idle },
	{ ai_run, 10 },
	{ ai_run, 10 },
	{ ai_run, 10 },
	{ ai_run, 10 },
	{ ai_run, 10 }
};
MMOVE_T(eel_move_run) = { FRAME_eelswim1, FRAME_eelswim6, eel_frames_run, nullptr };

MONSTERINFO_RUN(eel_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &eel_move_run);
}

//
// MELEE ATTACK
//

void eel_bite(edict_t* self)
{
	if (!self->enemy || !self->enemy->inuse)
		return;

	vec3_t aim = { MELEE_DISTANCE, 0, 0 };
	if (fire_hit(self, aim, irandom(10, 20), 100))
		gi.sound(self, CHAN_WEAPON, sound_chomp, 1, ATTN_NORM, 0);
}

mframe_t eel_frames_melee[] =
{
	{ ai_charge, 8 },
	{ ai_charge, 8 },
	{ ai_charge, 8 },
	{ ai_charge, 8 },
	{ ai_charge, 8 },
	{ ai_charge, 8, eel_bite }
};
MMOVE_T(eel_move_melee) = { FRAME_eelswim1, FRAME_eelswim6, eel_frames_melee, eel_run };

//
// RANGED ATTACK
//

static void eel_skin_charge(edict_t* self)
{
	if (self->s.skinnum < 5)
	{
		self->s.skinnum += 1;
		if (self->s.skinnum >= 4)
			self->s.effects |= EF_HYPERBLASTER;
	}
}

void eel_shoot(edict_t* self)
{
	vec3_t start, forward, right;
	vec3_t aim;
	vec3_t offset = { 20, 0, -2 };

	if (!self->enemy || !self->enemy->inuse)
		return;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);
	PredictAim(self, self->enemy, start, 800, false, 0.f, &aim, nullptr);

	monster_fire_blaster(self, start, aim, 15, 800, MZ2_SOLDIER_BLASTER_1, EF_BLASTER);

	self->s.skinnum = 0;
	self->s.effects &= ~EF_HYPERBLASTER;
}

mframe_t eel_frames_attack[] =
{
	{ ai_charge, 8, eel_skin_charge },
	{ ai_charge, 8, eel_skin_charge },
	{ ai_charge, 8, eel_skin_charge },
	{ ai_charge, 8, eel_skin_charge },
	{ ai_charge, 8, eel_skin_charge },
	{ ai_charge, 8, eel_shoot }
};
MMOVE_T(eel_move_attack) = { FRAME_eelswim1, FRAME_eelswim6, eel_frames_attack, eel_run };

MONSTERINFO_MELEE(eel_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &eel_move_melee);
}

MONSTERINFO_ATTACK(eel_attack) (edict_t* self) -> void
{
	gi.sound(self, CHAN_WEAPON, sound_attack, 1, ATTN_NORM, 0);
	M_SetAnimation(self, &eel_move_attack);
}

//
// PAIN
//

mframe_t eel_frames_pain[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(eel_move_pain) = { FRAME_eeldth1, FRAME_eeldth7, eel_frames_pain, eel_run };

PAIN(eel_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	self->s.skinnum = 0;
	self->s.effects &= ~EF_COLOR_SHELL;

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 1_sec;

	if (!M_ShouldReactToPain(self, mod))
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	M_SetAnimation(self, &eel_move_pain);
}

//
// DEATH
//

void eel_dead(edict_t* self)
{
	self->mins = { -16, -16, -8 };
	self->maxs = { 16, 16, 8 };
	monster_dead(self);
}

static void eel_skin_fade(edict_t* self)
{
	if (self->s.skinnum > 0)
		self->s.skinnum -= 1;
}

mframe_t eel_frames_death[] =
{
	{ ai_move },
	{ ai_move, 0, eel_skin_fade },
	{ ai_move },
	{ ai_move, 0, eel_skin_fade },
	{ ai_move },
	{ ai_move, 0, eel_skin_fade },
	{ ai_move },
	{ ai_move, 0, eel_skin_fade },
	{ ai_move },
	{ ai_move, 0, eel_skin_fade },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(eel_move_death) = { FRAME_eeldth1, FRAME_eeldth15, eel_frames_death, eel_dead };

MONSTERINFO_SIGHT(eel_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
}

DIE(eel_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	self->s.skinnum = 0;
	self->s.effects &= ~EF_COLOR_SHELL;

	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);
		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/monsters/eel/gibs/head.md2", GIB_HEAD }
			});
		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	// Sink slowly in water like AD version
	if (self->waterlevel)
		self->gravityVector[2] = 0.25f;

	M_SetAnimation(self, &eel_move_death);
}

static void eel_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 30.f;
	self->monsterinfo.fly_speed = 110.f;
	self->monsterinfo.fly_min_distance = 80.f;
	self->monsterinfo.fly_max_distance = 200.f;
}

/*QUAKED monster_eel (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
Electric eel from Quake Mission Pack 1.
Prefers ranged attacks but will bite in melee.

model="models/monsters/eel/tris.md2"
*/
void SP_monster_eel(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_death.assign("eel/death.wav");
	sound_chomp.assign("eel/bite.wav");
	sound_idle.assign("eel/idle.wav");
	sound_attack.assign("eel/attack.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex("models/monsters/eel/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	self->health = 60 * st.health_multiplier;
	self->gib_health = -50;
	self->mass = 100;

	self->pain = eel_pain;
	self->die = eel_die;

	self->monsterinfo.stand = eel_stand;
	self->monsterinfo.walk = eel_walk;
	self->monsterinfo.run = eel_run;
	self->monsterinfo.attack = eel_attack;
	self->monsterinfo.melee = eel_melee;
	self->monsterinfo.sight = eel_sight;

	gi.linkentity(self);

	M_SetAnimation(self, &eel_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	eel_set_fly_parameters(self);

	swimmonster_start(self);
}