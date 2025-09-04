/*
==============================================================================

QUAKE SCRAG

==============================================================================
*/

#include "../g_local.h"
#include "m_scrag.h"

static cached_soundindex	sound_sight;
static cached_soundindex	sound_idle1;
static cached_soundindex	sound_idle2;
static cached_soundindex	sound_attack;
static cached_soundindex	sound_die;
static cached_soundindex	sound_pain;
static cached_soundindex	sound_hit;

MONSTERINFO_SIGHT(scrag_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

void scrag_attack_sound (edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_attack, 1, ATTN_NORM, 0); // WAS ATTN_IDLE
}

MONSTERINFO_SEARCH(scrag_search) (edict_t* self) -> void
{
	if (frandom() < 0.5f)
		gi.sound (self, CHAN_VOICE, sound_idle1, 1, ATTN_IDLE, 0);
	else
		gi.sound (self, CHAN_VOICE, sound_idle2, 1, ATTN_IDLE, 0);
}

mframe_t scrag_frames_stand [] =
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
MMOVE_T(scrag_move_stand) = { FRAME_hover1, FRAME_hover15, scrag_frames_stand, nullptr };

MONSTERINFO_STAND(scrag_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &scrag_move_stand);
}

mframe_t scrag_frames_walk [] =
{
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6}
};
MMOVE_T(scrag_move_walk) = { FRAME_hover1, FRAME_hover15, scrag_frames_walk, nullptr };

MONSTERINFO_WALK(scrag_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &scrag_move_walk);
}

mframe_t scrag_frames_run [] =
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
	{ai_run, 12}
};
MMOVE_T(scrag_move_run) = { FRAME_fly1, FRAME_fly14, scrag_frames_run, nullptr };

MONSTERINFO_RUN(scrag_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &scrag_move_run);
}

void ScragTeleport(edict_t* self)
{
	if (skill->integer <= 3 && frandom() >= 0.5f)
		return;

	if (!TryRandomTeleportPosition(self, 128.0f)) {
		return;
	}
}

MONSTERINFO_DODGE(scrag_dodge) (edict_t* self, edict_t* attacker, gtime_t eta, trace_t* tr, bool gravity) -> void
{
	if (self->health <= 0)
		return;

	if (!self->enemy)
	{
		self->enemy = attacker;
		FoundTarget(self);
		return;
	}

	if ((eta < FRAME_TIME_MS) || (eta > 5_sec))
		return;

	if (self->timestamp > level.time)
		return;

	self->timestamp = level.time + random_time(1_sec, 5_sec);

	ScragTeleport(self);
}

mframe_t scrag_frames_pain [] =
{	
		{ai_move},
		{ai_move},
		{ai_move},
		{ai_move}
};
MMOVE_T(scrag_move_pain) = { FRAME_pain1, FRAME_pain4, scrag_frames_pain, scrag_run };

PAIN(scrag_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 2_sec;
	
	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare
	
	gi.sound (self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	M_SetAnimation(self, &scrag_move_pain);
}

void scrag_dead (edict_t * self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

static void scrag_shrink(edict_t* self)
{
	self->maxs[2] = -4;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t scrag_frames_die [] =
{	
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0, scrag_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(scrag_move_die) = { FRAME_death1, FRAME_death8, scrag_frames_die, scrag_dead };

DIE(scrag_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 4, "models/objects/gibs/sm_meat/tris.md2" },
			//{ "models/monsters/scrag/gibs/tail.md2", GIB_SKINNED },
			//{ "models/monsters/scrag/gibs/chest.md2", GIB_SKINNED },
			{ "models/objects/gibs/chest/tris.md2" },
			{ "models/monsters/scrag/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_die, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &scrag_move_die);
}

MONSTERINFO_SETSKIN(scrag_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

void scrag_fire (edict_t *self)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		end;
	vec3_t		dir;
	vec3_t		o = { 0, 0, 30 };

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, o, forward, right);

	end = self->enemy->s.origin;
	end[2] += self->enemy->viewheight;
	dir = end - start;
	dir.normalize();

	fire_acid(self,start, dir, 10, 500);
}

mframe_t scrag_frames_attack [] = {
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, scrag_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge, -1,scrag_fire},
	{ai_charge, -2},
	{ai_charge, -3},
	{ai_charge, -2}, // scrag_fire
	{ai_charge, -1},
	{ai_charge, 0, scrag_fire},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(scrag_move_attack) = { FRAME_magatt1, FRAME_magatt13, scrag_frames_attack, scrag_run };

MONSTERINFO_ATTACK(scrag_attack) (edict_t* self) -> void
{
	M_SetAnimation(self, &scrag_move_attack);
}

static void scrag_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 20.f;
	self->monsterinfo.fly_speed = 120.f;
	self->monsterinfo.fly_min_distance = 200.f;
	self->monsterinfo.fly_max_distance = 400.f;
}

/*QUAKED monster_scrag (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight
model="models/monsters/scrag/tris.md2"
*/
void SP_monster_scrag (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_sight.assign("scrag/wsight.wav");
	sound_idle1.assign("scrag/widle1.wav");
	sound_idle2.assign("scrag/widle2.wav");
	sound_attack.assign("scrag/wattack.wav");
	sound_die.assign("scrag/wdeath.wav");
	sound_pain.assign("scrag/wpain.wav");
	sound_hit.assign("scrag/hit.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/scrag/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 40 };

	self->health = 80 * st.health_multiplier;
	self->gib_health = -40;
	self->mass = 50;

	self->pain = scrag_pain;
	self->die = scrag_die;

	self->monsterinfo.stand = scrag_stand;
	self->monsterinfo.walk = scrag_walk;
	self->monsterinfo.run = scrag_run;
	self->monsterinfo.attack = scrag_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.dodge = scrag_dodge;
	self->monsterinfo.sight = scrag_sight;
	self->monsterinfo.search = scrag_search;
	self->monsterinfo.setskin = scrag_setskin;

	gi.linkentity (self);

	M_SetAnimation(self, &scrag_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;
	
	self->flags |= FL_FLY;
	if (!self->yaw_speed)
		self->yaw_speed = 10;
	self->viewheight = 10;

	flymonster_start (self);

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	scrag_set_fly_parameters(self);
}
