/*
==============================================================================

QUAKE GREMLIN

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_gremlin.h"
#include "../m_flash.h"

static cached_soundindex sound_idle;
static cached_soundindex sound_pain1;
static cached_soundindex sound_pain2;
static cached_soundindex sound_pain3;
static cached_soundindex sound_death;
static cached_soundindex sound_sight;
static cached_soundindex sound_attack;

constexpr int32_t MAX_GREMLIN_AMMO = 50;

MONSTERINFO_SEARCH(gremlin_search) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
}

MONSTERINFO_IDLE(gremlin_idle) (edict_t* self) -> void
{
	if (frandom() <= 0.1f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SIGHT(gremlin_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t gremlin_frames_stand[] = {
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
MMOVE_T(gremlin_move_stand) = { FRAME_stand1, FRAME_stand17, gremlin_frames_stand, nullptr };

MONSTERINFO_STAND(gremlin_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &gremlin_move_stand);
}

//
// WALK / RUN
//

mframe_t gremlin_frames_walk[] = {
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 }
};
MMOVE_T(gremlin_move_walk) = { FRAME_walk1, FRAME_walk12, gremlin_frames_walk, nullptr };

MONSTERINFO_WALK(gremlin_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &gremlin_move_walk);
}

mframe_t gremlin_frames_run[] = {
	{ ai_run, 0 },
	{ ai_run, 8 },
	{ ai_run, 12 },
	{ ai_run, 16 },
	{ ai_run, 16 },
	{ ai_run, 12 },
	{ ai_run, 8 },
	{ ai_run, 0 },
	{ ai_run, 8 },
	{ ai_run, 12 },
	{ ai_run, 16 },
	{ ai_run, 16 }
};
MMOVE_T(gremlin_move_run) = { FRAME_run1, FRAME_run12, gremlin_frames_run, nullptr };

MONSTERINFO_RUN(gremlin_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &gremlin_move_stand);
	else
		M_SetAnimation(self, &gremlin_move_run);
}

//
// PAIN
//

mframe_t gremlin_frames_pain[] = {
	{ ai_move, -4 },
	{ ai_move, -4 },
	{ ai_move, -2 },
	{ ai_move }
};
MMOVE_T(gremlin_move_pain) = { FRAME_pain1, FRAME_pain4, gremlin_frames_pain, gremlin_run };

PAIN(gremlin_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return;

	float r = frandom();
	if (r < 0.33f)
		gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	else if (r < 0.66f)
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_pain3, 1, ATTN_NORM, 0);

	M_SetAnimation(self, &gremlin_move_pain);
}

//
// DEATH
//

void gremlin_dead(edict_t* self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

mframe_t gremlin_frames_death[] = {
	{ ai_move },
	{ ai_move, 2 },
	{ ai_move, 1 },
	{ ai_move, 2 },
	{ ai_move, 1 },
	{ ai_move },
	{ ai_move, 2 },
	{ ai_move, 1 },
	{ ai_move, 2 },
	{ ai_move, 1 },
	{ ai_move, 2 },
	{ ai_move }
};
MMOVE_T(gremlin_move_death) = { FRAME_death101, FRAME_death112, gremlin_frames_death, gremlin_dead };

DIE(gremlin_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/objects/gibs/head2/tris.md2", GIB_HEAD }
			});

		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &gremlin_move_death);
}

//
// MELEE ATTACKS
//

void gremlin_melee_hit(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, 0, 0 };

	if (fire_hit(self, aim, 10 + irandom(6), 100))
	{
		gi.sound(self, CHAN_WEAPON, sound_attack, 1, ATTN_NORM, 0);
	}
}

mframe_t gremlin_frames_melee_claw[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, gremlin_melee_hit },
	{ ai_charge, 15 },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(gremlin_move_melee_claw) = { FRAME_attack1, FRAME_attack11, gremlin_frames_melee_claw, gremlin_run };

mframe_t gremlin_frames_melee_lunge[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 15 },
	{ ai_charge, 0, gremlin_melee_hit },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(gremlin_move_melee_lunge) = { FRAME_lunge1, FRAME_lunge11, gremlin_frames_melee_lunge, gremlin_run };

MONSTERINFO_MELEE(gremlin_melee) (edict_t* self) -> void
{
	if (frandom() < 0.5f)
		M_SetAnimation(self, &gremlin_move_melee_claw);
	else
		M_SetAnimation(self, &gremlin_move_melee_lunge);
}

//
// RANGED ATTACK
//

void gremlin_fire_weapon(edict_t* self)
{
	if (!self->client || !self->client->pers.weapon)
		return;

	vec3_t start, forward, right, target, dir;
	vec3_t offset = { 0, 0, 16 };

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);

	target = self->enemy->s.origin;
	target[2] += self->enemy->viewheight;
	dir = target - start;
	dir.normalize();

		fire_shotgun(self, start, dir, 6, 6, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);
}

mframe_t gremlin_frames_attack[] = {
	{ ai_charge, 0, monster_done_dodge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, gremlin_fire_weapon },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(gremlin_move_attack) = { FRAME_gfire1, FRAME_gfire6, gremlin_frames_attack, gremlin_run };

MONSTERINFO_ATTACK(gremlin_attack) (edict_t* self) -> void
{
	M_SetAnimation(self, &gremlin_move_attack);
}

//
// JUMP
//

void gremlin_jump_takeoff(edict_t* self)
{
	vec3_t forward;
	AngleVectors(self->s.angles, forward, nullptr, nullptr);
	self->velocity = forward * 300.f;
	self->velocity[2] = 300.f;
	self->groundentity = nullptr;

	gi.sound(self, CHAN_VOICE, gi.soundindex("demon/djump.wav"), 1, ATTN_NORM, 0);
}

void gremlin_check_landing(edict_t* self)
{
	if (self->groundentity)
	{
		gi.sound(self, CHAN_WEAPON, gi.soundindex("demon/dhit2.wav"), 1, ATTN_NORM, 0);
		M_SetAnimation(self, &gremlin_move_run);
		return;
	}
}

mframe_t gremlin_frames_jump[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, gremlin_jump_takeoff },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, gremlin_check_landing }
};
MMOVE_T(gremlin_move_jump) = { FRAME_jump1, FRAME_jump11, gremlin_frames_jump, gremlin_run };

void gremlin_jump(edict_t* self)
{
	if (!self->groundentity)
		return;

	M_SetAnimation(self, &gremlin_move_jump);
}

MONSTERINFO_CHECKATTACK(gremlin_checkattack) (edict_t* self) -> bool
{
	if (!self->enemy || !self->enemy->inuse)
		return false;

	// Randomly jump toward enemy
	if (self->groundentity && frandom() < 0.05f)
	{
		gremlin_jump(self);
		return true;
	}

	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.8f, 0.8f, 0.f, 0.f);
}

//
// SPAWN
//

/*QUAKED monster_gremlin (1 0 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void SP_monster_gremlin(edict_t* self)
{
	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_idle.assign("grem/idle.wav");
	sound_pain1.assign("grem/pain1.wav");
	sound_pain2.assign("grem/pain2.wav");
	sound_pain3.assign("grem/pain3.wav");
	sound_death.assign("grem/death.wav");
	sound_sight.assign("grem/sight1.wav");
	sound_attack.assign("grem/attack.wav");

	gi.soundindex("demon/djump.wav");
	gi.soundindex("demon/dhit2.wav");
	gi.soundindex("misc/udeath.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/gremlin/tris.md2");

	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	self->health = 100;
	self->gib_health = -35;
	self->mass = 100;

	self->pain = gremlin_pain;
	self->die = gremlin_die;

	self->monsterinfo.stand = gremlin_stand;
	self->monsterinfo.walk = gremlin_walk;
	self->monsterinfo.run = gremlin_run;
	self->monsterinfo.attack = gremlin_attack;
	self->monsterinfo.melee = gremlin_melee;
	self->monsterinfo.sight = gremlin_sight;
	self->monsterinfo.search = gremlin_search;
	self->monsterinfo.idle = gremlin_idle;
	self->monsterinfo.checkattack = gremlin_checkattack;

	self->monsterinfo.scale = MODEL_SCALE;

	gi.linkentity(self);

	M_SetAnimation(self, &gremlin_move_stand);

	walkmonster_start(self);
}