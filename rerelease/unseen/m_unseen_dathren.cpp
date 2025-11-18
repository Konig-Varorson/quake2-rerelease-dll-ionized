// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

ASSASSIN

==============================================================================
*/

#include "../g_local.h"
#include "m_unseen_dathren.h"
#include "../m_flash.h"

static cached_soundindex sound_pain100_1;
static cached_soundindex sound_pain100_2;
static cached_soundindex sound_pain75_1;
static cached_soundindex sound_pain75_2;
static cached_soundindex sound_pain50_1;
static cached_soundindex sound_pain50_2;
static cached_soundindex sound_pain25_1;
static cached_soundindex sound_pain25_2;
static cached_soundindex sound_death1;
static cached_soundindex sound_death2;
static cached_soundindex sound_death3;
static cached_soundindex sound_death4;
static cached_soundindex sound_idle;
static cached_soundindex sound_search;
static cached_soundindex sound_sight;

constexpr spawnflags_t SPAWNFLAG_DATHREN_NOJUMPING = 16_spawnflag;

unsigned int dathren_damage_multiplier;

//
//SOUNDS
//

MONSTERINFO_IDLE(dathren_idle) (edict_t* self) -> void
{
	if (frandom() > 0.8f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SEARCH(dathren_search) (edict_t* self) -> void
{
	if (frandom() > 0.8f)
		gi.sound(self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(dathren_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
//STAND
//

mframe_t dathren_frames_stand1[] = {
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
MMOVE_T(dathren_move_stand1) = { FRAME_stand1, FRAME_stand40, dathren_frames_stand1, nullptr };

MONSTERINFO_STAND(dathren_stand) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_DUCKED)
		monster_duck_up(self);

	M_SetAnimation(self, &dathren_move_stand1);
}

//
//RUNWALK
//

mframe_t dathren_frames_walk1[] = {
	{ ai_walk, 4 },
	{ ai_walk, 15, monster_footstep },
	{ ai_walk, 15 },
	{ ai_walk, 8 },
	{ ai_walk, 20, monster_footstep },
	{ ai_walk, 15 }
};
MMOVE_T(dathren_move_walk1) = { FRAME_run1, FRAME_run6, dathren_frames_walk1, nullptr };

MONSTERINFO_WALK(dathren_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &dathren_move_walk1);
}

mframe_t dathren_frames_run[] = {
	{ ai_run, 4 },
	{ ai_run, 15, monster_footstep },
	{ ai_run, 15 },
	{ ai_run, 8 },
	{ ai_run, 20, monster_footstep },
	{ ai_run, 15 }
};
MMOVE_T(dathren_move_run) = { FRAME_run1, FRAME_run6, dathren_frames_run, nullptr };

MONSTERINFO_RUN(dathren_run) (edict_t* self) -> void
{
	monster_done_dodge(self);

	if (self->monsterinfo.aiflags & AI_DUCKED)
		monster_duck_up(self);

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &dathren_move_stand1);
	else
		M_SetAnimation(self, &dathren_move_run);
}

//
//ATTACK
//

void dathren_railgun(edict_t* self)
{

}

void dathren_bfgrenade(edict_t* self)
{

}

mframe_t dathren_frames_attack1[] = {
	{ ai_charge },
	{ ai_charge, -3, dathren_bfgrenade },
	{ ai_charge, 2 },
	{ ai_charge, 1 },
	{ ai_charge },

	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(dathren_move_attack1) = { FRAME_attack101, FRAME_attack108, dathren_frames_attack1, dathren_run };

mframe_t dathren_frames_attack2[] = {
	{ ai_charge },
	{ ai_charge, -1 },
	{ ai_charge, -2, dathren_railgun },
	{ ai_charge, 2 },
	{ ai_charge, -2, dathren_railgun },

	{ ai_charge, 2 },
	{ ai_charge, -2, dathren_railgun },
	{ ai_charge, 2 }
};
MMOVE_T(dathren_move_attack2) = { FRAME_attack201, FRAME_attack208, dathren_frames_attack2, dathren_run };

MONSTERINFO_ATTACK(dathren_attack) (edict_t* self) -> void
{
	float r, chance;

	if (!M_CheckClearShot(self, monster_flash_offset[MZ2_ACTOR_MACHINEGUN_1]))
		return;

	if (self->monsterinfo.aiflags & AI_DUCKED)
		monster_duck_up(self);

	monster_done_dodge(self);

	if (range_to(self, self->enemy) <= MELEE_DISTANCE)
	{
		M_SetAnimation(self, &dathren_move_attack2);
	}
	else if (range_to(self, self->enemy) <= (RANGE_MID / 3))
	{
		M_SetAnimation(self, &dathren_move_attack1);
	}
	else
	{
		M_SetAnimation(self, &dathren_move_attack2);
	}
}

//
//DODGE
//

mframe_t dathren_frames_duck1[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(dathren_move_duck1) = { FRAME_duck1, FRAME_duck8, dathren_frames_duck1, nullptr };

MONSTERINFO_DUCK(dathren_duck) (edict_t* self, gtime_t eta) -> bool
{
	if ((self->monsterinfo.active_move == &dathren_move_attack1) || (self->monsterinfo.active_move == &dathren_move_attack2))
	{
		self->monsterinfo.unduck(self);
		return false;
	}

	M_SetAnimation(self, &dathren_move_duck1);

	monster_duck_down(self);

	return true;
}

MONSTERINFO_BLOCKED(dathren_blocked) (edict_t* self, float dist) -> bool
{
	if (blocked_checkplat(self, dist))
		return true;

	return false;
}

//
//PAIN
//

mframe_t dathren_frames_pain1[] = {
	{ ai_move, -5 },
	{ ai_move, 4 },
	{ ai_move, 1 },
	{ ai_move }
};
MMOVE_T(dathren_move_pain1) = { FRAME_pain101, FRAME_pain104, dathren_frames_pain1, dathren_run };

mframe_t dathren_frames_pain2[] = {
	{ ai_move, -4 },
	{ ai_move, 4 },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(dathren_move_pain2) = { FRAME_pain201, FRAME_pain204, dathren_frames_pain2, dathren_run };

mframe_t dathren_frames_pain3[] = {
	{ ai_move, -1 },
	{ ai_move, 1 },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(dathren_move_pain3) = { FRAME_pain301, FRAME_pain304, dathren_frames_pain3, dathren_run };

PAIN(dathren_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	monster_done_dodge(self);

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	if (self->health > 75)
	{
		if (frandom() > 0.5)
			gi.sound(self, CHAN_VOICE, sound_pain100_1, 1, ATTN_NORM, 0);
		else
			gi.sound(self, CHAN_VOICE, sound_pain100_2, 1, ATTN_NORM, 0);
	}
	else if (self->health > 50)
	{
		if (frandom() > 0.5)
			gi.sound(self, CHAN_VOICE, sound_pain75_1, 1, ATTN_NORM, 0);
		else
			gi.sound(self, CHAN_VOICE, sound_pain75_2, 1, ATTN_NORM, 0);
	}
	else if (self->health > 25)
	{
		if (frandom() > 0.5)
			gi.sound(self, CHAN_VOICE, sound_pain50_1, 1, ATTN_NORM, 0);
		else
			gi.sound(self, CHAN_VOICE, sound_pain50_2, 1, ATTN_NORM, 0);
	}
	else
	{
		if (frandom() > 0.5)
			gi.sound(self, CHAN_VOICE, sound_pain25_1, 1, ATTN_NORM, 0);
		else
			gi.sound(self, CHAN_VOICE, sound_pain25_2, 1, ATTN_NORM, 0);
	}

	if (damage < 35)
		M_SetAnimation(self, &dathren_move_pain1);
	else if (damage < 50)
		M_SetAnimation(self, &dathren_move_pain2);
	else
		M_SetAnimation(self, &dathren_move_pain3);
}

MONSTERINFO_SETSKIN(dathren_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
//DEATH
//

void dathren_dead(edict_t* self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

static void dathren_shrink(edict_t* self)
{
	self->maxs[2] = -4;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t dathren_frames_death1[] = {
	{ ai_move, -13 },
	{ ai_move, 14 },
	{ ai_move, 3 },
	{ ai_move, -2, dathren_shrink },
	{ ai_move, 1 },
	{ ai_move }
};
MMOVE_T(dathren_move_death1) = { FRAME_death101, FRAME_death106, dathren_frames_death1, dathren_dead };

mframe_t dathren_frames_death2[] = {
	{ ai_move, -13 },
	{ ai_move, 14 },
	{ ai_move, 3, dathren_shrink },
	{ ai_move, -2 },
	{ ai_move, 1 },
	{ ai_move }
};
MMOVE_T(dathren_move_death2) = { FRAME_death201, FRAME_death206, dathren_frames_death2, dathren_dead };

mframe_t dathren_frames_death3[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move, 0 , dathren_shrink },
	{ ai_move },
	{ ai_move },
};
MMOVE_T(dathren_move_death3) = { FRAME_death301, FRAME_death308, dathren_frames_death3, dathren_dead };

DIE(dathren_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	self->s.modelindex2 = 0;

	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/arm/tris.md2" },
			{ 2, "models/objects/gibs/leg/tris.md2" },
			{ 2, "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/objects/gibs/sm_metal/tris.md2", GIB_METALLIC },
			{ "models/objects/gibs/chest/tris.md2" },
			{ "models/objects/gibs/head/tris.md2", GIB_HEAD }
			});
		self->deadflag = true;

		return;
	}

	if (self->deadflag)
		return;

	self->deadflag = true;
	self->takedamage = true;

	if (frandom() < 0.25f)
		gi.sound(self, CHAN_VOICE, sound_death1, 1, ATTN_NORM, 0);
	else if (frandom() < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_death2, 1, ATTN_NORM, 0);
	else if (frandom() < 0.75f)
		gi.sound(self, CHAN_VOICE, sound_death3, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_death4, 1, ATTN_NORM, 0);

	if (frandom() < 0.33f)
		M_SetAnimation(self, &dathren_move_death1);
	else if (frandom() < 0.66f)
		M_SetAnimation(self, &dathren_move_death2);
	else
		M_SetAnimation(self, &dathren_move_death3);
}

//
// CHECK ATTAKC
//

void DathrenQuad(edict_t* self, gtime_t time)
{
	self->monsterinfo.quad_time = time;
	dathren_damage_multiplier = 4;
}

void DathrenQuadnDouble(edict_t* self, gtime_t time)
{
	self->monsterinfo.quad_time = time;
	self->monsterinfo.double_time = time;
	dathren_damage_multiplier = 8;
}

void DathrenDouble(edict_t* self, gtime_t time)
{
	self->monsterinfo.double_time = time;
	dathren_damage_multiplier = 2;
}

void DathrenPent(edict_t* self, gtime_t time)
{
	self->monsterinfo.invincible_time = time;
}

void DathrenPowerArmor(edict_t* self)
{
	self->monsterinfo.power_armor_type = IT_ITEM_POWER_SHIELD;
	// I don't like this, but it works
	if (self->monsterinfo.power_armor_power <= 0)
		self->monsterinfo.power_armor_power += 250 * skill->integer;
	if (coop->integer)
		self->monsterinfo.power_armor_power += ((25 * skill->integer) + (25 * (CountPlayers() - 1)));
}

void DathrenRespondPowerup(edict_t* self, edict_t* other)
{
	if (other->s.effects & EF_QUAD & EF_DOUBLE)
	{
		DathrenPowerArmor(self);
		if (skill->integer >= 1)
		{
			DathrenQuadnDouble(self, other->client->quad_time);
		}
	}
	else if (other->s.effects & EF_QUAD)
	{
		DathrenPowerArmor(self);
		if (skill->integer >= 1)
			DathrenQuad(self, other->client->quad_time);
	}
	else if (other->s.effects & EF_DOUBLE)
	{
		DathrenPowerArmor(self);
		if (skill->integer >= 1)
			DathrenDouble(self, other->client->double_time);
	}
	else if (other->s.effects & EF_DUALFIRE)
	{
		DathrenPowerArmor(self);
		if (skill->integer >= 3)
			DathrenDouble(self, other->client->double_time);
	}
	else
		dathren_damage_multiplier = 1;

	if (other->s.effects & EF_PENT)
	{
		if (skill->integer == 1)
			DathrenPowerArmor(self);
		else if (skill->integer >= 2)
			DathrenPent(self, other->client->invincible_time);
	}
}

void DathrenPowerups(edict_t* self)
{
	edict_t* ent;

	if (!coop->integer)
	{
		DathrenRespondPowerup(self, self->enemy);
	}
	else
	{
		// in coop, check for pents, then quads, then doubles
		for (uint32_t player = 1; player <= game.maxclients; player++)
		{
			ent = &g_edicts[player];
			if (!ent->inuse)
				continue;
			if (!ent->client)
				continue;
			if (ent->s.effects & EF_PENT)
			{
				DathrenRespondPowerup(self, ent);
				return;
			}
		}

		for (uint32_t player = 1; player <= game.maxclients; player++)
		{
			ent = &g_edicts[player];
			if (!ent->inuse)
				continue;
			if (!ent->client)
				continue;
			if (ent->s.effects & EF_QUAD)
			{
				DathrenRespondPowerup(self, ent);
				return;
			}
		}

		for (uint32_t player = 1; player <= game.maxclients; player++)
		{
			ent = &g_edicts[player];
			if (!ent->inuse)
				continue;
			if (!ent->client)
				continue;
			if (ent->s.effects & EF_DOUBLE)
			{
				DathrenRespondPowerup(self, ent);
				return;
			}
		}
	}
}

MONSTERINFO_CHECKATTACK(dathren_CheckAttack) (edict_t* self) -> bool
{
	/* KONIG - add powerup copy */
	DathrenPowerups(self);
	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.4f, 0.2f, 0.0f, 0.f);
}

/*QUAKED monster_dathren (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight  NoJumping
*/

void SP_monster_dathren(edict_t *self)
{
	const spawn_temp_t &st = ED_GetSpawnTemp();

	if ( !M_AllowSpawn( self ) ) {
		G_FreeEdict( self );
		return;
	}

	sound_pain100_1.assign("tactical/cyborg/pain100_1.wav");
	sound_pain100_2.assign("tactical/cyborg/pain100_2.wav");
	sound_pain75_1.assign("tactical/cyborg/pain75_1.wav");
	sound_pain75_2.assign("tactical/cyborg/pain75_2.wav");
	sound_pain50_1.assign("tactical/cyborg/pain50_1.wav");
	sound_pain50_2.assign("tactical/cyborg/pain50_2.wav");
	sound_pain25_1.assign("tactical/cyborg/pain25_1.wav");
	sound_pain25_2.assign("tactical/cyborg/pain25_2.wav");
	sound_death1.assign("tactical/cyborg/death1.wav");
	sound_death2.assign("tactical/cyborg/death2.wav");
	sound_death3.assign("tactical/cyborg/death3.wav");
	sound_death4.assign("tactical/cyborg/death4.wav");
	sound_sight.assign("tactical/cyborg/sight.wav");
	sound_search.assign("tactical/cyborg/seach.wav");
	sound_idle.assign("tactical/cyborg/idle.wav");

	self->s.modelindex = gi.modelindex("models/monsters/dathren/tris.md2");
	self->s.modelindex2 = gi.modelindex("models/monsters/dathren/weapon.md2");

	gi.modelindex("models/monsters/dathren/gibs/bodya.md2");
	gi.modelindex("models/monsters/dathren/gibs/bodyb.md2");
	gi.modelindex("models/monsters/dathren/gibs/claw.md2");
	gi.modelindex("models/monsters/dathren/gibs/foot.md2");
	gi.modelindex("models/monsters/dathren/gibs/head.md2");
	gi.modelindex("models/monsters/dathren/gibs/leg.md2");

	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = max(4500, 4500 + 1250 * (skill->integer - 1)) * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(500, 500 + 150 * (skill->integer - 1));
	if (!st.was_key_specified("power_armor_type"))
		self->monsterinfo.power_armor_type = IT_ITEM_POWER_SHIELD;
	if (!st.was_key_specified("power_armor_power"))
		self->monsterinfo.power_armor_power = max(500, 500 + 150 * (skill->integer - 1));
	if (coop->integer)
	{
		self->health += (500 * skill->integer) + (500 * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (250 * skill->integer) + (250 * (CountPlayers() - 1));
		self->monsterinfo.power_armor_power += (250 * skill->integer) + (250 * (CountPlayers() - 1));
	}

	self->pain = dathren_pain;
	self->die = dathren_die;

	self->monsterinfo.stand = dathren_stand;
	self->monsterinfo.walk = dathren_walk;
	self->monsterinfo.run = dathren_run;
	self->monsterinfo.attack = dathren_attack;
	self->monsterinfo.sight = dathren_sight;
	self->monsterinfo.idle = dathren_idle;
	self->monsterinfo.duck = dathren_duck;
	self->monsterinfo.unduck = monster_duck_up;
	self->monsterinfo.blocked = dathren_blocked;
	self->monsterinfo.setskin = dathren_setskin;
	self->monsterinfo.checkattack = dathren_CheckAttack;

	gi.linkentity(self);

	M_SetAnimation(self, &dathren_move_stand1);
	self->monsterinfo.scale = MODEL_SCALE;

	self->monsterinfo.can_jump = !self->spawnflags.has(SPAWNFLAG_DATHREN_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	walkmonster_start(self);
}
