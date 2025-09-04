// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

void barrel_touch(edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self);
void barrel_start(edict_t* self);

void setupCrate(edict_t* self)
{
	if (deathmatch->integer)
	{
		G_FreeEdict(self);
		return;
	}

	self->solid = SOLID_BBOX;
	self->movetype = MOVETYPE_FALLFLOAT;

	if (!self->mass)
		self->mass = 400;

	self->touch = barrel_touch;
	self->think = barrel_start;
	self->nextthink = level.time + FRAME_TIME_S;

	gi.linkentity(self);
}

/*QUAKED misc_crate (1 .5 0) ( -32 -32 0) (32 32 64 )
model="models/objects/crate/crate64.md2"
*/
void SP_misc_crate(edict_t* self)
{
	if (!self->mass)
		self->mass = 400;

	self->model = "models/objects/crate/crate64.md2";
	self->s.modelindex = gi.modelindex(self->model);
	self->mins = { -32, -32, 0 };
	self->maxs = { 32, 32, 64 };

	setupCrate(self);
}

/*QUAKED misc_crate_medium (1 .5 0) ( -24 -24 0) (24 24 48 )
model="models/objects/crate/crate48.md2"
*/
void SP_misc_crate_medium(edict_t* self)
{
	if (!self->mass)
		self->mass = 150;

	self->model = "models/objects/crate/crate48.md2";
	self->s.modelindex = gi.modelindex(self->model);
	self->mins = { -24, -24, 0 };
	self->maxs = { 24, 24, 48 };

	setupCrate(self);
}

/*QUAKED misc_crate_small (1 .5 0) ( -16 -16 0) (16 16 32 )
model="models/objects/crate/crate32.md2"
*/
void SP_misc_crate_small(edict_t* self)
{
	if (!self->mass)
		self->mass = 50;

	self->model = "models/objects/crate/crate32.md2";
	self->s.modelindex = gi.modelindex(self->model);
	self->mins = { -16, -16, 0 };
	self->maxs = { 16, 16, 32 };

	setupCrate(self);
}

/*QUAKED misc_crate_seat (1 .5 0) ( -16 -16 0) (16 16 40 )
model="models/objects/seat/tris.md2"
*/
void SP_misc_seat(edict_t* self)
{
	if (deathmatch->integer)
	{
		G_FreeEdict(self);
		return;
	}

	self->solid = SOLID_BBOX;
	self->movetype = MOVETYPE_STEP;

	if (!self->mass)
		self->mass = 50;

	self->model = "models/objects/seat/tris.md2";
	self->s.modelindex = gi.modelindex(self->model);
	self->mins = { -16, -16, 0 };
	self->maxs = { 16, 16, 40 };

	self->touch = barrel_touch;
	self->think = barrel_start;
	self->nextthink = level.time + FRAME_TIME_S;

	gi.linkentity(self);
}

/*QUAKED misc_commdish (0 .5 .8) (-16 -16 0) (16 16 40)
model="models/objects/commdish/tris.md2"
*/
THINK(commdish_think) (edict_t* ent) -> void
{
	ent->s.frame++;

	if (ent->s.frame >= 98)
	{
		ent->s.frame = 98;
	}
	else
	{
		ent->nextthink = level.time + FRAME_TIME_S;
	}
}

THINK(commdish_start) (edict_t* self) -> void
{
	M_droptofloor(self);
	self->nextthink = level.time + FRAME_TIME_S;
}

USE(commdish_use) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	self->think = commdish_think;
	self->use = nullptr;
	gi.sound(self, CHAN_AUTO, gi.soundindex("misc/commdish.wav"), 1, ATTN_NORM, 0);
	self->nextthink = level.time + FRAME_TIME_S;
}

void SP_misc_commdish(edict_t* ent)
{
	if (deathmatch->value)
	{	// auto-remove for deathmatch
		G_FreeEdict(ent);
		return;
	}

	ent->solid = SOLID_BBOX;
	ent->movetype = MOVETYPE_STEP;

	ent->mins = { -100, -100, 0 };
	ent->maxs = { 100, 100, 275 };
	ent->s.modelindex = gi.modelindex("models/objects/satdish/tris.md2");

	ent->monsterinfo.aiflags = AI_NOSTEP;

	ent->think = commdish_start;
	ent->nextthink = level.time + 20_hz;
	ent->use = commdish_use;

	gi.linkentity(ent);
}

//ZAERO CUT

/*QUAKED misc_bulldog_s (1 .5 0) (-116 -56 -32) (40 56 16)
This is a ship from ZAERO for the flybys.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast it should fly
*/
USE(misc_bulldog_use) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	self->svflags &= ~SVF_NOCLIENT;
	self->use = train_use;
	train_use(self, other, activator);
}

void SP_misc_bulldog_s(edict_t* ent)
{
	if (!ent->target)
	{
		gi.Com_PrintFmt("{} without a target\n", *ent);
		G_FreeEdict(ent);
		return;
	}

	if (!ent->speed)
		ent->speed = 300;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_NOT;
	ent->s.modelindex = gi.modelindex("models/ships/bulldog_s/main/tris.md2");
	ent->s.modelindex2 = gi.modelindex("models/ships/bulldog_s/left/tris.md2");
	ent->s.modelindex3 = gi.modelindex("models/ships/bulldog_s/right/tris.md2");
	ent->mins = { -116, -56, -32 };
	ent->maxs = { 40, 56, 16 };

	ent->think = func_train_find;
	ent->nextthink = level.time + 10_hz;
	ent->use = misc_bulldog_use;
	ent->svflags |= SVF_NOCLIENT;
	ent->moveinfo.accel = ent->moveinfo.decel = ent->moveinfo.speed = ent->speed;

	gi.linkentity(ent);
}

/*QUAKED misc_bulldog_, (1 .5 0) (-224 -112 -32) (80 112 64)
This is a larger ship from ZAERO for the flybys.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast it should fly
*/
void SP_misc_bulldog_m(edict_t* ent)
{
	if (!ent->target)
	{
		gi.Com_PrintFmt("{} without a target\n", *ent);
		G_FreeEdict(ent);
		return;
	}

	if (!ent->speed)
		ent->speed = 300;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_NOT;
	ent->s.modelindex = gi.modelindex("models/ships/bulldog_m/main/tris.md2");
	ent->s.modelindex2 = gi.modelindex("models/ships/bulldog_m/left/tris.md2");
	ent->s.modelindex3 = gi.modelindex("models/ships/bulldog_m/right/tris.md2");
	ent->mins = { -224, -112, -32 };
	ent->maxs = { 80, 112, 64 };

	ent->think = func_train_find;
	ent->nextthink = level.time + 10_hz;
	ent->use = misc_bulldog_use;
	ent->svflags |= SVF_NOCLIENT;
	ent->moveinfo.accel = ent->moveinfo.decel = ent->moveinfo.speed = ent->speed;

	gi.linkentity(ent);
}


/*QUAKED misc_bulldog_l (1 .5 0) (-432 -120 -32) (144 120 96)
This is a large stationary bulldog
*/
void SP_misc_bulldog_l(edict_t* ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->mins = { -432, -120, -32 };
	ent->maxs = { 144, 120, 96 };
	ent->s.modelindex = gi.modelindex("models/ships/bulldog_l/main/tris.md2");
	ent->s.modelindex2 = gi.modelindex("models/ships/bulldog_l/left/tris.md2");
	ent->s.modelindex3 = gi.modelindex("models/ships/bulldog_l/right/tris.md2");
	gi.linkentity(ent);
}