/*
==============================================================================

QUAKE SPIKE MINE

==============================================================================
*/

#include "../g_local.h"
#include "m_spike.h"

static cached_soundindex	sound_sight;
static cached_soundindex	sound_idle;
static cached_soundindex	sound_search;
static cached_soundindex	sound_death;
static cached_soundindex	sound_pain;

//
// SOUNDS
//

MONSTERINFO_SIGHT(spike_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_IDLE(spike_idle) (edict_t* self) -> void
{
	if (frandom() < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SEARCH(spike_search) (edict_t* self) -> void
{
	if (frandom() < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t spike_frames_stand[] =
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
MMOVE_T(spike_move_stand) = { FRAME_spike1, FRAME_spike9, spike_frames_stand, nullptr };

MONSTERINFO_STAND(spike_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &spike_move_stand);
}

//
// WALK
//


mframe_t spike_frames_walk[] =
{
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
MMOVE_T(spike_move_walk) = { FRAME_spike1, FRAME_spike9, spike_frames_walk, nullptr };

MONSTERINFO_WALK(spike_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &spike_move_walk);
}

//
// RUN
//

void spike_explode(edict_t* self);

void spike_check(edict_t* self)
{
	float dist;

	// PMM - this needed because we could have gone away before we get here (blocked code)
	if (!self->inuse)
		return;

	if ((!self->enemy) || (!self->enemy->inuse))
	{
		spike_explode(self);
		return;
	}

	self->s.angles[0] = vectoangles(self->enemy->s.origin - self->s.origin).x;

	self->goalentity = self->enemy;

	dist = realrange(self, self->enemy);

	if (dist < 90)
		spike_explode(self);
}

mframe_t spike_frames_run[] =
{
	{ai_run, 12, spike_check},
	{ai_run, 12, spike_check},
	{ai_run, 12, spike_check},
	{ai_run, 12, spike_check},
	{ai_run, 12, spike_check},

	{ai_run, 12, spike_check},
	{ai_run, 12, spike_check},
	{ai_run, 12, spike_check},
	{ai_run, 12, spike_check}
};
MMOVE_T(spike_move_run) = { FRAME_spike1, FRAME_spike9, spike_frames_run, nullptr };

MONSTERINFO_RUN(spike_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &spike_move_run);
}

//
// ATTACK
//

MONSTERINFO_MELEE(spike_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &spike_move_run);
}

//
// PAIN
//

mframe_t spike_frames_pain[] =
{
		{ai_move, -1},
		{ai_move, -1},
		{ai_move, -1},
		{ai_move, -1},
		{ai_move, -1},

		{ai_move, -1},
		{ai_move, -1},
		{ai_move, -1},
		{ai_move, -1}
};
MMOVE_T(spike_move_pain) = { FRAME_spike1, FRAME_spike9, spike_frames_pain, spike_run };

bool IsNailgun(const mod_t& mod)
{
	switch (mod.id)
	{
	case MOD_ETF_RIFLE:
		return true;
	default:
		return false;
	}
}

PAIN(spike_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (IsNailgun(mod))
	{
		T_Damage(self, self, self, vec3_origin, other->s.origin, vec3_origin, self->health + 10, 0, DAMAGE_NONE, MOD_EXPLOSIVE);
	}

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	M_SetAnimation(self, &spike_move_pain);
}

//
// DEATH
//

void spike_dead(edict_t* self)
{
	T_RadiusDamage(self, self, 120, nullptr, 150, DAMAGE_NONE, MOD_EXPLOSIVE);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->touch = nullptr;

	ThrowGibs(self, 500, {
		{ "models/objects/gibs/sm_meat/tris.md2", GIB_HEAD }
		});
}

mframe_t spike_frames_explode[] =
{
	{ai_move}
};
MMOVE_T(spike_move_explode) = { FRAME_spike1, FRAME_spike1, spike_frames_explode, spike_dead };

void spike_explode(edict_t* self)
{
	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	spike_dead(self);
}

DIE(spike_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &spike_move_explode);
}

//
// SIDESTEP
//

MONSTERINFO_SIDESTEP(spike_sidestep) (edict_t* self) -> bool
{
	if (skill->integer <= 2)
		return false; //don't sidestep if not nightmare

	if (self->monsterinfo.active_move != &spike_move_run)
		M_SetAnimation(self, &spike_move_run);

	return true;
}

static void spike_set_fly_parameters(edict_t* self)
{
self->monsterinfo.fly_pinned = false;
self->monsterinfo.fly_thrusters = true;
self->monsterinfo.fly_position_time = 0_sec;
self->monsterinfo.fly_acceleration = 20.f;
self->monsterinfo.fly_speed = 210.f;
self->monsterinfo.fly_min_distance = 0.f;
self->monsterinfo.fly_max_distance = 10.f;
}

/*QUAKED monster_spike (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight NoJumping Kneeling
model="models/monsters/spike/tris.md2"
*/
void SP_monster_spike(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_search.assign("spike/search.wav");
	sound_death.assign("spike/death.wav");
	sound_pain.assign("spike/pain.wav");
	sound_idle.assign("spike/idle.wav");
	sound_sight.assign("spike/sight.wav");

	gi.modelindex("models/monsters/spike/head/tris.md2");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex("models/monsters/spikeball/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 40 };

	self->health = 200 * st.health_multiplier;
	self->gib_health = -80;
	self->mass = 120;

	self->pain = spike_pain;
	self->die = spike_die;

	self->monsterinfo.aiflags |= AI_IGNORE_SHOTS;

	self->monsterinfo.stand = spike_stand;
	self->monsterinfo.walk = spike_walk;
	self->monsterinfo.run = spike_run;
	self->monsterinfo.sidestep = spike_sidestep;
	self->monsterinfo.attack = nullptr;
	self->monsterinfo.melee = spike_melee;
	self->monsterinfo.sight = spike_sight;
	self->monsterinfo.search = spike_search;

	gi.linkentity(self);

	M_SetAnimation(self, &spike_move_stand);

	self->monsterinfo.combat_style = COMBAT_MELEE;

	self->monsterinfo.scale = MODEL_SCALE;

	flymonster_start(self);

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	spike_set_fly_parameters(self);
}