// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

SHARK

==============================================================================
*/

#include "../g_local.h"
#include "m_ionized_shark.h"

static cached_soundindex sound_chomp;
static cached_soundindex sound_attack;
static cached_soundindex sound_pain1;
static cached_soundindex sound_pain2;
static cached_soundindex sound_death;
static cached_soundindex sound_idle;
static cached_soundindex sound_search;
static cached_soundindex sound_sight;

mframe_t shark_frames_stand[] = {
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
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};

MMOVE_T(shark_move_stand) = { FRAME_amba001, FRAME_amba020, shark_frames_stand, nullptr };

MONSTERINFO_STAND(shark_stand) (edict_t *self) -> void
{
	M_SetAnimation(self, &shark_move_stand);
}

mframe_t shark_frames_run[] = {
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 }, // 10

	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 }, // 20

	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 } // 30
};
MMOVE_T(shark_move_run) = { FRAME_swima001, FRAME_swima030, shark_frames_run, nullptr };

MONSTERINFO_RUN(shark_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &shark_move_run);
}

/* Standard Swimming */
mframe_t shark_frames_walk[] = {
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },

	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },

	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 }
};
MMOVE_T(shark_move_walk) = { FRAME_swima001, FRAME_swima030, shark_frames_walk, nullptr };

MONSTERINFO_WALK(shark_walk) (edict_t *self) -> void
{
	M_SetAnimation(self, &shark_move_walk);
}

mframe_t shark_frames_pain[] = {
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
MMOVE_T(shark_move_pain) = { FRAME_hita001, FRAME_hita015, shark_frames_pain, shark_run };

void shark_bite(edict_t *self)
{
	vec3_t aim = { MELEE_DISTANCE, 0, 0 };
	fire_hit(self, aim, 25, 0);
}

void shark_preattack(edict_t *self)
{
	gi.sound(self, CHAN_WEAPON, sound_chomp, 1, ATTN_NORM, 0);
}

mframe_t shark_frames_attack1[] = {
	{ ai_charge, 0, shark_preattack },
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
	{ ai_charge, 0, shark_bite },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, shark_bite },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(shark_move_attack1) = { FRAME_ataka001, FRAME_ataka021, shark_frames_attack1, shark_run };

mframe_t shark_frames_attack2[] = {
	{ ai_charge, 0, shark_preattack },
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
	{ ai_charge, 0, shark_bite },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, shark_bite },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(shark_move_attack2) = { FRAME_atakb001, FRAME_atakb021, shark_frames_attack2, shark_run };

mframe_t shark_frames_attack3[] = {
	{ ai_charge, 0, shark_preattack },
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
	{ ai_charge, 0, shark_bite },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, shark_bite },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(shark_move_attack3) = { FRAME_atakc001, FRAME_atakc025, shark_frames_attack3, shark_run };

MONSTERINFO_MELEE(shark_melee) (edict_t *self) -> void
{
	M_SetAnimation(self, &shark_move_attack1);
}

PAIN(shark_pain) (edict_t *self, edict_t *other, float kick, int damage, const mod_t &mod) -> void
{
	int n;

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;
	n = brandom();

	if (n == 0)
		gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	M_SetAnimation(self, &shark_move_pain);
}

MONSTERINFO_SETSKIN(shark_setskin) (edict_t *self) -> void
{
	/* KONIG - allow multiple skins */
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

void shark_dead(edict_t *self)
{
	self->mins = { -16, -16, -8 };
	self->maxs = { 16, 16, 8 };
	monster_dead(self);
}

mframe_t shark_frames_death1[] = {
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
	{ ai_move },
	{ ai_move },

	{ ai_move }
};
MMOVE_T(shark_move_death1) = { FRAME_diea001, FRAME_diea021, shark_frames_death1, shark_dead };

mframe_t shark_frames_death2[] = {
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
MMOVE_T(shark_move_death2) = { FRAME_dieb001, FRAME_dieb015, shark_frames_death2, shark_dead };

MONSTERINFO_SIGHT(shark_sight) (edict_t *self, edict_t *other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

DIE(shark_die) (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, const vec3_t &point, const mod_t &mod) -> void
{
	// check for gib
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);
		ThrowGibs(self, damage, {
			{ 4, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/objects/gibs/head2/tris.md2", GIB_HEAD }
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
	if (frandom() > 0.5f)
		M_SetAnimation(self, &shark_move_death1);
	else
		M_SetAnimation(self, &shark_move_death2);

}

static void shark_set_fly_parameters(edict_t *self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 30.f;
	self->monsterinfo.fly_speed = 110.f;
	// only melee, so get in close
	self->monsterinfo.fly_min_distance = 10.f;
	self->monsterinfo.fly_max_distance = 10.f;
}

/*QUAKED monster_shark (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
 */
void SP_monster_shark(edict_t *self)
{
	const spawn_temp_t &st = ED_GetSpawnTemp();

	if ( !M_AllowSpawn( self ) ) {
		G_FreeEdict( self );
		return;
	}

	sound_pain1.assign("shark/flppain1.wav");
	sound_pain2.assign("shark/flppain2.wav");
	sound_death.assign("shark/flpdeth1.wav");
	sound_chomp.assign("shark/flpatck1.wav");
	sound_attack.assign("shark/flpatck2.wav");
	sound_idle.assign("shark/flpidle1.wav");
	sound_search.assign("shark/flpsrch1.wav");
	sound_sight.assign("shark/flpsght1.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/shark/tris.md2");
	self->mins = { -32, -32, -8 };
	self->maxs = { 32, 32, 20 };

	self->health = 300 * st.health_multiplier;
	self->gib_health = -200;
	self->mass = 400;

	self->pain = shark_pain;
	self->die = shark_die;

	self->monsterinfo.stand = shark_stand;
	self->monsterinfo.walk = shark_walk;
	self->monsterinfo.run = shark_run;
	self->monsterinfo.melee = shark_melee;
	self->monsterinfo.sight = shark_sight;
	self->monsterinfo.setskin = shark_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &shark_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	shark_set_fly_parameters(self);

	swimmonster_start(self);
}
