/*
==============================================================================

QUAKE ROTFISH

==============================================================================
*/

#include "../g_local.h"
#include "m_eel.h"

static cached_soundindex	sound_chomp;
static cached_soundindex	sound_death;
static cached_soundindex	sound_idle;

void eel_stand(edict_t* self);

void eel_idle (edict_t *self)
{
	if (frandom() < 0.5)
		gi.sound (self, CHAN_AUTO, sound_idle, 1, ATTN_IDLE, 0);
}


mframe_t eel_frames_stand [] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};	
MMOVE_T(eel_move_stand) = { FRAME_eelswim1, FRAME_eelswim6, eel_frames_stand, nullptr };

MONSTERINFO_STAND(eel_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &eel_move_stand);
}

mframe_t eel_frrun_loop [] =
{
	{ai_run, 9},
	{ai_run, 9},
	{ai_run, 9},
	{ai_run, 9},
	{ai_run, 9},
	{ai_run, 9}
};	
MMOVE_T(eel_move_run_loop) = { FRAME_eelswim1, FRAME_eelswim6, eel_frrun_loop, nullptr };

MONSTERINFO_RUN(eel_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &eel_move_run_loop);
}

/* Standard Swimming */ 
mframe_t eel_walk_loop [] =
{
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6}
};	
MMOVE_T(eel_move_walk_loop) = { FRAME_eelswim1, FRAME_eelswim6, eel_walk_loop, nullptr };

MONSTERINFO_WALK(eel_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &eel_move_walk_loop);
}

mframe_t eel_frames_pain1 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(eel_move_pain1) = { FRAME_eeldth1, FRAME_eeldth4, eel_frames_pain1, eel_run};

static void eel_skin_fire(edict_t* self)
{
	if (self->s.skinnum < 5)
		self->s.skinnum += 1;
	else
		return;
}

void eel_shoot (edict_t *self)
{
	vec3_t					start;
	vec3_t					forward, right;
	vec3_t					aim;
	vec3_t					offset = { 0, 0, 0 };

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);
	PredictAim(self, self->enemy, start, 800, false, frandom() * 0.3f, &aim, nullptr);
	for (int i = 0; i < 3; i++)
		aim[i] += crandom_open() * 0.025f;
	fire_lightning(self, start, aim, 5, 600, EF_PLASMA);

	self->s.skinnum = 0;
}

mframe_t eel_frames_attack1 [] =
{
	{ai_charge, 0, eel_skin_fire},
	{ai_charge, 0, eel_skin_fire},
	{ai_charge, 0, eel_skin_fire},
	{ai_charge, 0, eel_skin_fire},
	{ai_charge, -1, eel_skin_fire},
	{ai_charge, -2, eel_shoot}
};
MMOVE_T(eel_move_attack1) = { FRAME_eelswim1, FRAME_eelswim6, eel_frames_attack1, eel_run};

mframe_t eel_frames_attack2[] =
{
	{ai_charge, 5, eel_skin_fire},
	{ai_charge, 5, eel_skin_fire},
	{ai_charge, 5, eel_skin_fire},
	{ai_charge, 5, eel_skin_fire},
	{ai_charge, 5, eel_skin_fire},
	{ai_charge, 5, eel_shoot}
};
MMOVE_T(eel_move_attack2) = { FRAME_eelswim1, FRAME_eelswim6, eel_frames_attack2, eel_run };

MONSTERINFO_ATTACK(eel_attack) (edict_t* self) -> void
{
	float chance = 0.5f;

	if (frandom() > chance)
	{
		M_SetAnimation(self, &eel_move_attack1);
		self->monsterinfo.attack_state = AS_STRAIGHT;
	}
	else // circle strafe
	{
		if (frandom() <= 0.5f) // switch directions
			self->monsterinfo.lefty = !self->monsterinfo.lefty;
		M_SetAnimation(self, &eel_move_attack2);
		self->monsterinfo.attack_state = AS_SLIDING;
	}
}

PAIN(eel_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 1_sec;
		
	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare
	
	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	M_SetAnimation(self, &eel_move_pain1);
}

void eel_dead(edict_t* self)
{
	self->mins = { -16, -16, -8 };
	self->maxs = { 16, 16, 8 };
	monster_dead(self);
}

static void eel_skin_death(edict_t* self)
{
	if (self->s.skinnum > 0)
		self->s.skinnum -= 1;
	else
		return;
}

mframe_t eel_frames_death [] =
{
	{ai_move},
	{ai_move, 0, eel_skin_death},
	{ai_move},
	{ai_move, 0, eel_skin_death},
	{ai_move},
	{ai_move, 0, eel_skin_death},
	{ai_move},
	{ai_move, 0, eel_skin_death},
	{ai_move},
	{ai_move, 0, eel_skin_death},
	
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(eel_move_death) = { FRAME_eeldth1, FRAME_eeldth15, eel_frames_death, eel_dead};

MONSTERINFO_SIGHT(eel_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
}

DIE(eel_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	// check for gib
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);
		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ "models/objects/gibs/sm_meat/tris.md2" },
			{"models/monsters/eel/gibs/head.md2", GIB_HEAD }
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
	M_SetAnimation(self, &eel_move_death);
}

static void eel_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 30.f;
	self->monsterinfo.fly_speed = 110.f;
	// only melee, so get in close
	self->monsterinfo.fly_min_distance = 10.f;
	self->monsterinfo.fly_max_distance = 10.f;
}

/*QUAKED monster_eel (1 .5 0) (-16 -16 -24) (16 16 24) Ambush Trigger_Spawn Sight
model="models/monsters/eel/tris.md2"
*/
void SP_monster_eel (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_death.assign("eel/death.wav");	
	sound_chomp.assign("eel/bite.wav");
	sound_idle.assign("eel/idle.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/eel/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	self->health = 90 * st.health_multiplier;
	self->gib_health = -50;
	self->mass = 100;

	self->pain = eel_pain;
	self->die = eel_die;

	self->monsterinfo.stand = eel_stand;
	self->monsterinfo.walk = eel_walk;
	self->monsterinfo.run = eel_run;
	self->monsterinfo.attack = eel_attack;
	self->monsterinfo.sight = eel_sight;

	gi.linkentity (self);

	M_SetAnimation(self, &eel_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	eel_set_fly_parameters(self);

	swimmonster_start (self);
}
