
/*
==============================================================================

QUAKE ZOMBIE

==============================================================================
*/


#include "../g_local.h"
#include "m_zombie.h"


static cached_soundindex	sound_idle_cruc;
static cached_soundindex	sound_idle;
static cached_soundindex	sound_idle2;
static cached_soundindex	sound_pain;
static cached_soundindex	sound_pain2;
static cached_soundindex	sound_fall;
static cached_soundindex	sound_gib;
static cached_soundindex	sound_shot;

void q1_fire_gib (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed);
void zombie_down (edict_t *self);
void zombie_fallagain (edict_t *self);
void zombie_pain2 (edict_t *self);
void zombie_idle3 (edict_t *self);
void zombie_run (edict_t *self);

void zombie_idle_cruc (edict_t *self)
{
	if (frandom() < 0.1)
		gi.sound (self, CHAN_VOICE, sound_idle_cruc, 1, ATTN_IDLE, 0); //was ATTN_STATIC
}

mframe_t zombie_frames_pain5 [] =
{
	{ai_move, 0, zombie_pain2 },		//1
	{ai_move, -8},
	{ai_move, -5},
	{ai_move, -3},
	{ai_move, -1},				//5
	{ai_move, -2},
	{ai_move, -1},
	{ai_move, -1},
	{ai_move, -2},
	{ai_move, 0, zombie_down},		//10
	{ai_move},
	{ai_move, 0, zombie_idle3},		//12
	{ai_move, 0, zombie_fallagain}, //13
	{ai_move},  
	{ai_move},				//15
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},				//19- was zombie_takedamage
	{ai_move},				//20
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 5},
	{ai_move, 3},				//25
	{ai_move, 1},
	{ai_move, -1},
	{ai_move},
	{ai_move},
	{ai_move}				//30
};
MMOVE_T(zombie_move_pain5) = { FRAME_paine1, FRAME_paine30, zombie_frames_pain5, zombie_run };

void zombie_idle3 (edict_t *self)
{
	trace_t tr;
	
	self->health = self->max_health;
	self->solid = SOLID_BBOX;
	self->count = 0;
	gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0); //was ATTN_STATIC
	// Check if player is standing on top
	tr = gi.trace (self->s.origin, self->mins, self->maxs, self->s.origin, self, CONTENTS_PLAYERCLIP|CONTENTS_MONSTER);
	if (tr.ent && tr.ent->solid != SOLID_BSP)
	{
		self->solid = SOLID_NOT;
		self->count = 1;
		self->s.frame = FRAME_paine11;
		self->nextthink += 5_ms;
		self->pain_debounce_time = level.time + 3_ms;
	}
}

void zombie_idle (edict_t *self)
{	
	if (frandom() > 0.8)
		gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0); //was ATTN_STATIC

	if(self->solid == SOLID_NOT)
		self->solid = SOLID_BBOX;
}

void zombie_idle2 (edict_t *self)
{
	if (frandom() > 0.2)
		gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0); //was ATTN_STATIC
	else
		gi.sound (self, CHAN_VOICE, sound_idle2, 1, ATTN_IDLE, 0); //was ATTN_STATIC
}

void zombie_pain1(edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0); //was ATTN_IDLE
	self->health = self->max_health;
}

void zombie_pain2(edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0); //was ATTN_IDLE
	self->health = self->max_health;
}

void zombie_fall(edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_fall, 1, ATTN_NORM, 0); //was ATTN_IDLE
	self->health = self->max_health;
}


// STAND

void zombie_stand (edict_t *self);

mframe_t zombie_frames_stand [] =
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
MMOVE_T(zombie_move_stand) = { FRAME_stand1, FRAME_stand15, zombie_frames_stand, zombie_stand};

void zombie_stand (edict_t *self)
{
	self->solid = SOLID_BBOX;
	self->count = 0;
	M_SetAnimation(self, &zombie_move_stand);
}


//Crucified
void zombie_crucify (edict_t *self);

mframe_t zombie_frames_cruc [] =
{
	{ai_stand},
	{ai_stand, 0, zombie_idle_cruc},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(zombie_crucified) = { FRAME_cruc_1, FRAME_cruc_6, zombie_frames_cruc, zombie_crucify};

void zombie_crucify (edict_t *self)
{
	M_SetAnimation(self, &zombie_crucified);
}

//
// WALK
//

mframe_t zombie_frames_walk [] =
{
	{ai_walk},
	{ai_walk, 2},
	{ai_walk, 3},
	{ai_walk, 2},
	{ai_walk, 1},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 1},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk, 0,  zombie_idle}
};
MMOVE_T(zombie_move_walk) = { FRAME_walk1, FRAME_walk19, zombie_frames_walk, nullptr };

void zombie_walk (edict_t *self)
{
	self->solid = SOLID_BBOX;
	self->count = 0;
	M_SetAnimation(self, &zombie_move_walk);
}

//
// RUN
//

mframe_t zombie_frames_run [] =
{
	{ai_run, 1},
	{ai_run, 1},
	{ai_run},
	{ai_run, 1},
	{ai_run, 2},
	{ai_run, 3},
	{ai_run, 4},
	{ai_run, 4},
	{ai_run, 2},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run, 2},
	{ai_run, 4},
	{ai_run, 6},
	{ai_run, 7},
	{ai_run, 3},
	{ai_run, 8, zombie_idle2}
};
MMOVE_T(zombie_move_run) = { FRAME_run1, FRAME_run18, zombie_frames_run, zombie_run };

void zombie_run (edict_t *self)
{
	self->solid = SOLID_BBOX;
	self->count = 0;

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &zombie_move_stand);
		return;
	}
	else
		M_SetAnimation(self, &zombie_move_run);
}


//
// ATTACK
//
void zombie_firegib (edict_t *self, vec3_t offset)
{
	vec3_t	start;
	vec3_t	forward, right;
	vec3_t	target;
	vec3_t	aim;

	AngleVectors(self->s.angles, forward, right, nullptr);
	G_ProjectSource2(self->s.origin, offset, forward, right, start);

	// project enemy back a bit and target there
	VectorCopy (self->enemy->s.origin, target);
	
	switch(range (self,self->enemy))
	{
	case RANGE_MELEE:
		target[2] += self->enemy->viewheight;
		break;
	case RANGE_NEAR:
		VectorMA (target, -0.04, self->enemy->velocity, target);
		target[2] += (self->enemy->viewheight * 0.9);
		break;
	case RANGE_MID:
		VectorMA (target, -0.08, self->enemy->velocity, target);
		target[2] += (self->enemy->viewheight * 0.8);
		break;
	default:
		VectorMA (target,-0.1, self->enemy->velocity, target);
		target[2] += (rand() & self->enemy->viewheight);
		break;
	}

	VectorSubtract (target, start, aim);
	VectorNormalize (aim);
	
	gi.sound (self, CHAN_WEAPON|CHAN_RELIABLE, sound_shot, 1.0, ATTN_NORM, 0);
	q1_fire_gib (self, start, aim, 10, 600);

}

void zombie_firegib1(edict_t *self)
{
	vec3_t offset;
	offset[0]=-8;
	offset[1]=-18;
	offset[2]=30;

	zombie_firegib(self,offset);
}

void zombie_firegib2(edict_t *self)
{
	vec3_t offset;
	offset[0]=-8;
	offset[1]=-18;
	offset[2]=30;
	zombie_firegib(self,offset);
}

void zombie_firegib3(edict_t *self)
{
	vec3_t offset;
	offset[0]=-8;
	offset[1]=22;
	offset[2]=30;
	zombie_firegib(self,offset);
}

mframe_t zombie_frames_attack1[] = {
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, zombie_firegib1}
};
MMOVE_T(zombie_move_attack1) = { FRAME_atta1, FRAME_atta13, zombie_frames_attack1, zombie_run };

mframe_t zombie_frames_attack2[] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, zombie_firegib2}
};
MMOVE_T(zombie_move_attack2) = { FRAME_attb1, FRAME_attb14, zombie_frames_attack2, zombie_run };


mframe_t zombie_frames_attack3[] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, zombie_firegib3}
};
MMOVE_T(zombie_move_attack3) = { FRAME_attc1, FRAME_attc12, zombie_frames_attack3, zombie_run };


void zombie_attack(edict_t *self)
{
	float r = frandom();

	if (self->style)
		return;

	self->solid = SOLID_BBOX;
	self->count = 0;

	if (r < 0.3)
		M_SetAnimation(self, &zombie_move_attack1);
	else if (r < 0.6)
		M_SetAnimation(self, &zombie_move_attack2);
	else
		M_SetAnimation(self, &zombie_move_attack3);
}


//
// PAIN
//

mframe_t zombie_frames_pain1 [] =
{
	{ai_move, 0, zombie_pain1},
	{ai_move, 3},
	{ai_move, 1},
	{ai_move, -1},
	{ai_move, -3},
	{ai_move, -1},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(zombie_move_pain1) = { FRAME_paina1, FRAME_paina12, zombie_frames_pain1, zombie_run };

mframe_t zombie_frames_pain2 [] =
{
	{ai_move, 0, zombie_pain2},
	{ai_move, -2},
	{ai_move, -8},
	{ai_move, -6},
	{ai_move, -2},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0, zombie_fall},
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
	{ai_move, 1},
	{ai_move},
	{ai_move}
};
MMOVE_T(zombie_move_pain2) = { FRAME_painb1, FRAME_painb28, zombie_frames_pain2, zombie_run };

mframe_t zombie_frames_pain3 [] =
{
	{ai_move, 0, zombie_pain2},
	{ai_move},
	{ai_move, -3},
	{ai_move, -1},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 1},
	{ai_move, 1},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(zombie_move_pain3) = { FRAME_painc1, FRAME_painc18, zombie_frames_pain3, zombie_run };

mframe_t zombie_frames_pain4 [] =
{
	{ai_move, 0, zombie_pain1},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, -1},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(zombie_move_pain4) = { FRAME_paind1, FRAME_paind13, zombie_frames_pain4, zombie_run };

// fall and stand up functions

void zombie_down( edict_t *self)
{
	self->solid = SOLID_NOT;
	zombie_fall(self);
	self->nextthink += 5;
}

void zombie_pain (edict_t *self, edict_t *other, float kick, int damage)
{
	float	r;
	
	self->health = self->max_health;
	if (self->wait > level.time)
	{
		self->dmg += damage;
		if (self->dmg > 30)
		{
			M_SetAnimation(self, &zombie_move_pain5);
			self->pain_debounce_time = level.time + 3;
			self->dmg = 0;
			self->wait = 0;	
		//	self->solid = SOLID_NOT;
			self->count = 1;
			return;
		}
	}
	else
	{
		self->wait = level.time + 1;
		self->dmg = damage;
	}

	if (damage < 15)
		return;

	if (damage >= 25)
	{
		M_SetAnimation(self, &zombie_move_pain5);
		self->pain_debounce_time = level.time + 3;
	//	self->solid = SOLID_NOT;
		self->count = 1;
		return;
	}

	if (self->pain_debounce_time > level.time)
		return;
	
	r = frandom();

	if (r < 0.25)
	{
		self->pain_debounce_time = level.time + 1.0;
		gi.sound (self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zombie_move_pain1);
	}
	else if (r < 0.5)
	{
		self->pain_debounce_time = level.time + 1.5;
		gi.sound (self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zombie_move_pain2);
	}
	else if (r < 0.75)
	{
		self->pain_debounce_time = level.time + 1.1;
		gi.sound (self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zombie_move_pain3);
	}
	else
	{
		self->pain_debounce_time = level.time + 1.0;
		M_SetAnimation(self, &zombie_move_pain4);
	}
}

void zombie_fallagain ( edict_t *self)
{
	if (self->pain_debounce_time > level.time)
	{
		M_SetAnimation(self, &zombie_move_pain5);
	}
}

//
// SIGHT
//

void zombie_sight(edict_t *self, edict_t *other)
{
	gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
	
	if (!self->style && (skill->value > 0) && (range(self, self->enemy) >= RANGE_MID))
		zombie_attack(self);
}


//
// DEATH
//

void zombie_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	int		n;

	// always gib
	gi.sound (self, CHAN_VOICE, sound_gib, 1, ATTN_NORM, 0);
	for (n = 0; n < 2; n++)
		ThrowGib (self, "models/objects/q1gibs/q1gib1/tris.md2", 0, 0, damage, GIB_ORGANIC);
	ThrowGib (self, "models/objects/q1gibs/q1gib2/tris.md2", 0, 0, damage, GIB_ORGANIC);
	for (n = 0; n < 2; n++)
		ThrowGib (self, "models/objects/q1gibs/q1gib3/tris.md2", 0, 0, damage, GIB_ORGANIC);
	ThrowHead (self, "models/monsters/zombie/head/tris.md2", 0, 0, damage*2, GIB_ORGANIC);

	// no regular death
	self->deadflag = DEAD_DEAD;
	self->takedamage = DAMAGE_NO;
	return;
}


//
// SPAWN
//

/*QUAKED monster_q1_zombie (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/zombie/tris.md2"
"sounds"  1 makes the zombie crucified
*/
void SP_monster_q1_zombie (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_idle_cruc.assign("zombie/idle_w2.wav");
	sound_idle.assign("zombie/z_idle.wav");
	sound_idle2.assign("zombie/z_idle1.wav");
	sound_pain.assign("zombie/z_pain.wav");
	sound_pain2.assign("zombie/z_pain1.wav");
	sound_fall.assign("zombie/z_fall.wav");
	sound_gib.assign("zombie/z_gib.wav");
	sound_shot.assign("zombie/z_shot1.wav");

	self->s.modelindex = gi.modelindex ("models/monsters/zombie/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = 60;
	self->gib_health = 0;
	self->mass = 100;

	self->count = 0;
	self->max_health = self->health;	// save custom health

	self->pain = zombie_pain;
	self->die = zombie_die;

	self->monsterinfo.stand = zombie_stand;
	self->monsterinfo.walk = zombie_walk;
	self->monsterinfo.run = zombie_run;
	self->monsterinfo.attack = zombie_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = zombie_sight;

	self->monsterinfo.scale = MODEL_SCALE;

	gi.linkentity (self);

	M_SetAnimation(self, &zombie_move_stand);
	walkmonster_start (self);
}
