// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

SAURON

==============================================================================
*/

#include "../g_local.h"
#include "m_unseen_sauron.h"
#include "../m_flash.h"

//TODO
// Sound for firing
// Add guardian rockets alt attack
// Nightmare fires flakshot
// Add sidestep

static cached_soundindex sound_pain1;
static cached_soundindex sound_pain2;
static cached_soundindex sound_death1;
static cached_soundindex sound_death2;
static cached_soundindex sound_sight;
static cached_soundindex sound_search1;
static cached_soundindex sound_search2;

void BossPowerups(edict_t* self);

MONSTERINFO_SIGHT(sauron_sight) (edict_t *self, edict_t *other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(sauron_search) (edict_t *self) -> void
{
	if (frandom() < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_search1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_search2, 1, ATTN_NORM, 0);
}

void sauron_run(edict_t *self);
void sauron_dead(edict_t *self);
void sauron_attack(edict_t *self);
void sauron_reattack(edict_t *self);
void sauron_fire_nails(edict_t *self);

mframe_t sauron_frames_stand[] = {
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
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(sauron_move_stand) = { FRAME_stand1, FRAME_stand40, sauron_frames_stand, nullptr };

mframe_t sauron_frames_pain1[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move, 2 },
	{ ai_move, -8 },
	{ ai_move, -4 },
	{ ai_move, -6 },
	{ ai_move, -4 },
	{ ai_move, -3 },
	{ ai_move, 1 },
	{ ai_move },

	{ ai_move },
	{ ai_move }
};
MMOVE_T(sauron_move_pain1) = { FRAME_pain1, FRAME_pain12, sauron_frames_pain1, sauron_run };

mframe_t sauron_frames_walk[] = {
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 },
	{ ai_walk, 4 }
};
MMOVE_T(sauron_move_walk) = { FRAME_run1, FRAME_run6, sauron_frames_walk, nullptr };

mframe_t sauron_frames_run[] = {
	{ ai_run, 10 },
	{ ai_run, 10 },
	{ ai_run, 10 },
	{ ai_run, 10 },
	{ ai_run, 10 },
	{ ai_run, 10 }
};
MMOVE_T(sauron_move_run) = { FRAME_run1, FRAME_run6, sauron_frames_run, nullptr };

static void sauron_gib(edict_t *self)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1_BIG);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->s.sound = 0;
	self->s.skinnum /= 2;

	self->gravityVector.z = -1.0f;

	ThrowGibs(self, 150, {
		{ 2, "models/objects/gibs/sm_meat/tris.md2" },
		{ 2, "models/objects/gibs/sm_metal/tris.md2", GIB_METALLIC },
		{ "models/objects/gibs/chest/tris.md2" },
		{ "models/objects/gibs/head2/tris.md2", GIB_HEAD },
		});

}

mframe_t sauron_frames_death1[] = {
	{ ai_move, 0, BossExplode },
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
MMOVE_T(sauron_move_death1) = { FRAME_death1, FRAME_death20, sauron_frames_death1, sauron_dead };

mframe_t sauron_frames_start_attack[] = {
	{ ai_charge, 1 },
	{ ai_charge, 1 },
	{ ai_charge, 1 }
};
MMOVE_T(sauron_move_start_attack) = { FRAME_attak1, FRAME_attak3, sauron_frames_start_attack, sauron_attack };

mframe_t sauron_frames_attack1[] = {
	{ ai_charge, -10, sauron_fire_nails },
	{ ai_charge, -10, sauron_fire_nails },
	{ ai_charge, 0, sauron_reattack },
};
MMOVE_T(sauron_move_attack1) = { FRAME_attak4, FRAME_attak6, sauron_frames_attack1, nullptr };

mframe_t sauron_frames_end_attack[] = {
	{ ai_charge, 1 },
	{ ai_charge, 1 }
};
MMOVE_T(sauron_move_end_attack) = { FRAME_attak7, FRAME_attak8, sauron_frames_end_attack, sauron_run };

mframe_t sauron_frames_attack2[] = {
	{ ai_charge, 10, sauron_fire_nails },
	{ ai_charge, 10, sauron_fire_nails },
	{ ai_charge, 10, sauron_reattack },
};
MMOVE_T(sauron_move_attack2) = { FRAME_attak4, FRAME_attak6, sauron_frames_attack2, nullptr };

void sauron_reattack(edict_t *self)
{
	if (self->enemy->health > 0)
		if (visible(self, self->enemy))
			if (frandom() <= 0.6f)
			{
				if (self->monsterinfo.attack_state == AS_STRAIGHT)
				{
					M_SetAnimation(self, &sauron_move_attack1);
					return;
				}
				else if (self->monsterinfo.attack_state == AS_SLIDING)
				{
					M_SetAnimation(self, &sauron_move_attack2);
					return;
				}
				else
					gi.Com_PrintFmt("sauron_reattack: unexpected state {}\n", (int32_t) self->monsterinfo.attack_state);
			}
	M_SetAnimation(self, &sauron_move_end_attack);
}

void sauron_fire_nails(edict_t *self)
{
	vec3_t	  start;
	vec3_t	  forward, right;
	vec3_t	  end;
	vec3_t	  dir;
	int dmg = 10;
	float n = 0.2f;
	int	rocketSpeed = 650;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[MZ2_ACTOR_MACHINEGUN_1], forward, right);

	end = self->enemy->s.origin;
	end[2] += self->enemy->viewheight;
	dir = end - start;
	dir.normalize();

	monster_fire_flechette(self, start, dir, dmg, 6000, MZ2_ACTOR_MACHINEGUN_1);
}

MONSTERINFO_STAND(sauron_stand) (edict_t *self) -> void
{
	M_SetAnimation(self, &sauron_move_stand);
}

MONSTERINFO_RUN(sauron_run) (edict_t *self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &sauron_move_stand);
	else
		M_SetAnimation(self, &sauron_move_run);
}

MONSTERINFO_WALK(sauron_walk) (edict_t *self) -> void
{
	M_SetAnimation(self, &sauron_move_walk);
}

MONSTERINFO_ATTACK(sauron_start_attack) (edict_t *self) -> void
{
	M_SetAnimation(self, &sauron_move_start_attack);
}

void sauron_attack(edict_t *self)
{
	float chance = 0.6f;

	if (frandom() > chance)
	{
		M_SetAnimation(self, &sauron_move_attack1);
		self->monsterinfo.attack_state = AS_STRAIGHT;
	}
	else // circle strafe
	{
		if (frandom() <= 0.5f) // switch directions
			self->monsterinfo.lefty = !self->monsterinfo.lefty;
		M_SetAnimation(self, &sauron_move_attack2);
		self->monsterinfo.attack_state = AS_SLIDING;
	}
}

PAIN(sauron_pain) (edict_t *self, edict_t *other, float kick, int damage, const mod_t &mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	float r = frandom();

	//====
	if (r < 0.5f)
	{
			gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	}
	else
	{
			gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
	}
	// PGM
	//====

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	M_SetAnimation(self, &sauron_move_pain1);
}

MONSTERINFO_CHECKATTACK(Sauron_CheckAttack) (edict_t* self) -> bool
{
	if (!self->enemy)
		return false;

	BossPowerups(self);

	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.6f, 0.7f, 0.85f, 0.f);
}

MONSTERINFO_SETSKIN(sauron_setskin) (edict_t *self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1; // PGM support for skins 2 & 3.
	else
		self->s.skinnum &= ~1; // PGM support for skins 2 & 3.
}

void sauron_dead(edict_t *self)
{
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		self->deadflag = false;
		self->takedamage = true;
		return;
	}

	sauron_gib(self);
}

DIE(sauron_die) (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, const vec3_t &point, const mod_t &mod) -> void
{
	self->s.modelindex2 = 0;

	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		// check for gib
		if (M_CheckGib(self, mod))
		{
			sauron_gib(self);
			self->deadflag = true;
			return;
		}

		if (self->deadflag)
			return;
	}
	else
	{
		ThrowGib(self, "models/monsters/sauron/weapon.md2", damage, GIB_METALLIC, self->s.scale);

		if (frandom() < 0.5f)
			gi.sound(self, CHAN_VOICE, sound_death1, 1, ATTN_NORM, 0);
		else
			gi.sound(self, CHAN_VOICE, sound_death2, 1, ATTN_NORM, 0);
		self->deadflag = true;
		self->takedamage = false;
		self->count = 0;
		self->velocity = {};
		self->gravityVector.z *= 0.30f;
	}

	M_SetAnimation(self, &sauron_move_death1);
}

static void sauron_set_fly_parameters(edict_t *self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 20.f;
	self->monsterinfo.fly_speed = 120.f;
	self->monsterinfo.fly_min_distance = 250.f;
	self->monsterinfo.fly_max_distance = 450.f;
}

/*QUAKED monster_sauron (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
 */
void SP_monster_sauron(edict_t *self)
{
	const spawn_temp_t &st = ED_GetSpawnTemp();

	if ( !M_AllowSpawn( self ) ) {
		G_FreeEdict( self );
		return;
	}

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/sauron/tris.md2");
	self->s.modelindex2 = gi.modelindex("models/monsters/sauron/weapon.md2");

	self->mins = { -24, -24, -24 };
	self->maxs = { 24, 24, 32 };

	self->health = max(1500, 1500 + 1000 * (skill->integer - 1)) * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(350, 350 + 100 * (skill->integer - 1));
	if (coop->integer)
	{
		self->health += (250 * skill->integer * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (100 * skill->integer * (CountPlayers() - 1));
	}
	self->gib_health = -500;
	self->mass = 450;

	self->pain = sauron_pain;
	self->die = sauron_die;

	self->monsterinfo.stand = sauron_stand;
	self->monsterinfo.walk = sauron_walk;
	self->monsterinfo.run = sauron_run;
	self->monsterinfo.attack = sauron_start_attack;
	self->monsterinfo.sight = sauron_sight;
	self->monsterinfo.search = sauron_search;
	self->monsterinfo.checkattack = Sauron_CheckAttack;
	self->monsterinfo.setskin = sauron_setskin;

	self->yaw_speed = 23;
	sound_pain1.assign("sauron/pain1.wav");
	sound_pain2.assign("sauron/pain2.wav");
	sound_death1.assign("sauron/deth1.wav");
	sound_death2.assign("sauron/deth2.wav");
	sound_sight.assign("sauron/sght1.wav");
	sound_search1.assign("sauron/srch1.wav");
	sound_search2.assign("sauron/srch2.wav");
	gi.soundindex("sauron/atck1.wav");

	self->monsterinfo.engine_sound = gi.soundindex("sauron/hovidle1.wav");

	gi.linkentity(self);

	M_SetAnimation(self, &sauron_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	flymonster_start(self);

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	sauron_set_fly_parameters(self);
}
