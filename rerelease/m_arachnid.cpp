// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

TANK

==============================================================================
*/

#include "g_local.h"
#include "m_arachnid.h"
#include "m_flash.h"

static cached_soundindex sound_pain;
static cached_soundindex sound_death;
static cached_soundindex sound_sight;

MONSTERINFO_SIGHT(arachnid_sight) (edict_t *self, edict_t *other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
// stand
//

mframe_t arachnid_frames_stand[] = {
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
MMOVE_T(arachnid_move_stand) = { FRAME_idle1, FRAME_idle13, arachnid_frames_stand, nullptr };

MONSTERINFO_STAND(arachnid_stand) (edict_t *self) -> void
{
	M_SetAnimation(self, &arachnid_move_stand);
}

//
// walk
//

static cached_soundindex sound_step;

void arachnid_footstep(edict_t *self)
{
	gi.sound(self, CHAN_BODY, sound_step, 0.5f, ATTN_IDLE, 0.0f);
}

mframe_t arachnid_frames_walk[] = {
	{ ai_walk, 8, arachnid_footstep },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8, arachnid_footstep },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 }
};
MMOVE_T(arachnid_move_walk) = { FRAME_walk1, FRAME_walk10, arachnid_frames_walk, nullptr };

MONSTERINFO_WALK(arachnid_walk) (edict_t *self) -> void
{
	M_SetAnimation(self, &arachnid_move_walk);
}

//
// run
//

mframe_t arachnid_frames_run[] = {
	{ ai_run, 8, arachnid_footstep },
	{ ai_run, 8 },
	{ ai_run, 8 },
	{ ai_run, 8 },
	{ ai_run, 8 },
	{ ai_run, 8, arachnid_footstep },
	{ ai_run, 8 },
	{ ai_run, 8 },
	{ ai_run, 8 },
	{ ai_run, 8 }
};
MMOVE_T(arachnid_move_run) = { FRAME_walk1, FRAME_walk10, arachnid_frames_run, nullptr };

MONSTERINFO_RUN(arachnid_run) (edict_t *self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &arachnid_move_stand);
		return;
	}

	M_SetAnimation(self, &arachnid_move_run);
}

//
// pain
//

mframe_t arachnid_frames_pain1[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(arachnid_move_pain1) = { FRAME_pain11, FRAME_pain15, arachnid_frames_pain1, arachnid_run };

mframe_t arachnid_frames_pain2[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(arachnid_move_pain2) = { FRAME_pain21, FRAME_pain26, arachnid_frames_pain2, arachnid_run };

PAIN(arachnid_pain) (edict_t *self, edict_t *other, float kick, int damage, const mod_t &mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;
	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	float r = frandom();

	if (r < 0.5f)
		M_SetAnimation(self, &arachnid_move_pain1);
	else
		M_SetAnimation(self, &arachnid_move_pain2);
}
/*KONIG - set pain skin*/
MONSTERINFO_SETSKIN(arachnid_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

static cached_soundindex sound_charge;

void arachnid_charge_rail(edict_t *self)
{
	if (!self->enemy || !self->enemy->inuse)
		return;

	gi.sound(self, CHAN_WEAPON, sound_charge, 1.f, ATTN_NORM, 0.f);
	self->pos1 = self->enemy->s.origin;
	self->pos1[2] += self->enemy->viewheight;
}

void arachnid_rail(edict_t *self)
{
	vec3_t start;
	vec3_t dir;
	vec3_t forward, right;
	monster_muzzleflash_id_t id;

	switch (self->s.frame)
	{
		case FRAME_rails4:
		default:
			id = MZ2_ARACHNID_RAIL1;
			break;
		case FRAME_rails8:
			id = MZ2_ARACHNID_RAIL2;
			break;
		case FRAME_rails_up7:
			id = MZ2_ARACHNID_RAIL_UP1;
			break;
		case FRAME_rails_up11:
			id = MZ2_ARACHNID_RAIL_UP2;
			break;
	}

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[id], forward, right);

	// calc direction to where we targeted
	dir = self->pos1 - start;
	dir.normalize();

	/* KONIG - Increased damage to 50 to match PSX */
	monster_fire_railgun(self, start, dir, 50, 100, id);
}
/*KONIG - Beta attacks; NOTE: 3 rockets, 4-6 blaster shots;
Q4 Stream Protectors also have flamethrower and flak nail volley, consider adding?*/
static void protector_blind_check(edict_t* self)
{
	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
	{
		vec3_t aim = self->monsterinfo.blind_fire_target - self->s.origin;
		self->ideal_yaw = vectoyaw(aim);
	}
}

void protector_rockets(edict_t *self)
{
	vec3_t						start;
	vec3_t						dir;
	vec3_t						forward, right;
	vec3_t						vec;
	monster_muzzleflash_id_t	id;
	int							rocketSpeed;
	vec3_t						target;
	bool						blindfire = self->monsterinfo.aiflags & AI_MANUAL_STEERING;

	switch (self->s.frame)
	{
	case FRAME_rails4:
	default:
		id = MZ2_ARACHNID_RAIL1;
		break;
	case FRAME_rails8:
		id = MZ2_ARACHNID_RAIL2;
		break;
	case FRAME_rails_up7:
		id = MZ2_ARACHNID_RAIL_UP1;
		break;
	case FRAME_rails_up11:
		id = MZ2_ARACHNID_RAIL_UP2;
		break;
	}

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[id], forward, right);

	rocketSpeed = 500;

	if (blindfire)
		target = self->monsterinfo.blind_fire_target;
	else
		target = self->enemy->s.origin;

	if (blindfire)
	{
		vec = target;
		dir = vec - start;
	}
	else if (frandom() < 0.66f || (start[2] < self->enemy->absmin[2]))
	{
		vec = self->enemy->s.origin;
		vec[2] += self->enemy->viewheight;
		dir = vec - start;
	}
	else
	{
		vec = self->enemy->s.origin;
		vec[2] = self->enemy->absmin[2] + 1;
		dir = vec - start;
	}

	if ((!blindfire) && ((frandom() < (0.2f + ((3 - skill->integer) * 0.15f)))))
		PredictAim(self, self->enemy, start, rocketSpeed, false, 0, &dir, &vec);

	dir.normalize();
	if (blindfire)
	{
		if (M_AdjustBlindfireTarget(self, start, vec, right, dir))
		{
			monster_fire_heat(self, start, dir, 75, rocketSpeed, id, self->accel);
		}
	}
	else
	{
		trace_t trace = gi.traceline(start, vec, self, MASK_PROJECTILE);

		if (trace.fraction > 0.5f || trace.ent->solid != SOLID_BSP)
		{
			monster_fire_heat(self, start, dir, 75, rocketSpeed, id, self->accel);
		}
	}
}

mframe_t arachnid_frames_attack1[] = {
	{ ai_charge, 0, arachnid_charge_rail },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_rail },
	{ ai_charge, 0, arachnid_charge_rail },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_rail },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(arachnid_attack1) = { FRAME_rails1, FRAME_rails11, arachnid_frames_attack1, arachnid_run };

mframe_t arachnid_frames_attack_up1[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_charge_rail },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_rail },
	{ ai_charge, 0, arachnid_charge_rail },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_rail },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
};
MMOVE_T(arachnid_attack_up1) = { FRAME_rails_up1, FRAME_rails_up16, arachnid_frames_attack_up1, arachnid_run };
/*KONIG - Beta attack animations*/
mframe_t protector_frames_attack1[] = {
	{ ai_charge, 0, protector_blind_check },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, protector_rockets },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, protector_rockets },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(protector_attack1) = { FRAME_rails1, FRAME_rails11, protector_frames_attack1, arachnid_run };

mframe_t protector_frames_attack_up1[] = {
	{ ai_charge, 0, protector_blind_check },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, protector_rockets },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, protector_rockets },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
};
MMOVE_T(protector_attack_up1) = { FRAME_rails_up1, FRAME_rails_up16, protector_frames_attack_up1, arachnid_run };
static cached_soundindex sound_melee, sound_melee_hit;

void arachnid_melee_charge(edict_t *self)
{
	gi.sound(self, CHAN_WEAPON, sound_melee, 1.f, ATTN_NORM, 0.f);
}

void arachnid_melee_hit(edict_t *self)
{
	/* KONIG - update damage to 20 to match PSX;Beta does 30 meleedamage*/
	if (self->style == 1)
	{
		if (!fire_hit(self, { MELEE_DISTANCE, 0, 0 }, 30, 50))
			self->monsterinfo.melee_debounce_time = level.time + 1000_ms;
	}
	else
	{
		if (!fire_hit(self, { MELEE_DISTANCE, 0, 0 }, 20, 50))
			self->monsterinfo.melee_debounce_time = level.time + 1000_ms;
	}
}

mframe_t arachnid_frames_melee[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_melee_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_melee_hit },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_melee_charge },
	{ ai_charge },
	{ ai_charge, 0, arachnid_melee_hit },
	{ ai_charge }
};
MMOVE_T(arachnid_melee) = { FRAME_melee_atk1, FRAME_melee_atk12, arachnid_frames_melee, arachnid_run };

MONSTERINFO_ATTACK(arachnid_attack) (edict_t *self) -> void
{
	if (!self->enemy || !self->enemy->inuse)
		return;
	/*KONIG - Adding beta attacks*/
	if (self->monsterinfo.melee_debounce_time < level.time && range_to(self, self->enemy) < MELEE_DISTANCE)
		M_SetAnimation(self, &arachnid_melee);
	else
	{
		if (self->style == 1)
		{
			if ((self->enemy->s.origin[2] - self->s.origin[2]) > 150.f)
				M_SetAnimation(self, &protector_attack_up1);
			else
				M_SetAnimation(self, &protector_attack1);
		}
		else
		{
			if ((self->enemy->s.origin[2] - self->s.origin[2]) > 150.f)
				M_SetAnimation(self, &arachnid_attack_up1);
			else
				M_SetAnimation(self, &arachnid_attack1);
		}
	}
}

//
// death
//

void arachnid_dead(edict_t *self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	self->movetype = MOVETYPE_TOSS;
	self->svflags |= SVF_DEADMONSTER;
	self->nextthink = 0_ms;
	gi.linkentity(self);
}

mframe_t arachnid_frames_death1[] = {
	{ ai_move, 0 },
	{ ai_move, -1.23f },
	{ ai_move, -1.23f },
	{ ai_move, -1.23f },
	{ ai_move, -1.23f },
	{ ai_move, -1.64f },
	{ ai_move, -1.64f },
	{ ai_move, -2.45f },
	{ ai_move, -8.63f },
	{ ai_move, -4.0f },
	{ ai_move, -4.5f },
	{ ai_move, -6.8f },
	{ ai_move, -8.0f },
	{ ai_move, -5.4f },
	{ ai_move, -3.4f },
	{ ai_move, -1.9f },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(arachnid_move_death) = { FRAME_death1, FRAME_death20, arachnid_frames_death1, arachnid_dead };

DIE(arachnid_die) (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, const vec3_t &point, const mod_t &mod) -> void
{
	// check for gib
	/* KONIG - replaced generic head for gunner head, added gunner chest, added generic gear gib*/
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);
		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 4, "models/objects/gibs/sm_meat/tris.md2" },
			{ 1, "models/objects/gibs/gear/tris.md2" },
			{ "models/monsters/gunner/gibs/chest.md2", GIB_SKINNED },
			{ "models/monsters/gunner/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
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

	M_SetAnimation(self, &arachnid_move_death);
}

//
// monster_arachnid
//

/*QUAKED monster_arachnid (1 .5 0) (-48 -48 -20) (48 48 48) Ambush Trigger_Spawn Sight
 */
void SP_monster_arachnid(edict_t *self)
{
	if ( !M_AllowSpawn( self ) ) {
		G_FreeEdict( self );
		return;
	}

	sound_step.assign("insane/insane11.wav");
	sound_charge.assign("gladiator/railgun.wav");
	sound_melee.assign("gladiator/melee3.wav");
	sound_melee_hit.assign("gladiator/melee2.wav");
	sound_pain.assign("arachnid/pain.wav");
	sound_death.assign("arachnid/death.wav");
	sound_sight.assign("arachnid/sight.wav");

	self->s.modelindex = gi.modelindex("models/monsters/arachnid/tris.md2");
	self->mins = { -48, -48, -20 };
	self->maxs = { 48, 48, 48 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = 1000 * st.health_multiplier;
	self->gib_health = -200;

	self->monsterinfo.scale = MODEL_SCALE;

	self->mass = 450;

	self->pain = arachnid_pain;
	self->die = arachnid_die;
	self->monsterinfo.stand = arachnid_stand;
	self->monsterinfo.walk = arachnid_walk;
	self->monsterinfo.run = arachnid_run;
	self->monsterinfo.attack = arachnid_attack;
	self->monsterinfo.sight = arachnid_sight;
	self->monsterinfo.setskin = arachnid_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &arachnid_move_stand);

	walkmonster_start(self);
}

/*KONIG - Stream Protector from Q4 as Arachnid Beta*/
void SP_monster_protector(edict_t *self)
{
	SP_monster_arachnid(self);

	self->monsterinfo.armor_type = IT_ARMOR_COMBAT;
	self->monsterinfo.armor_power = 100;
	self->style = 1;
	self->s.skinnum = 2;
}
