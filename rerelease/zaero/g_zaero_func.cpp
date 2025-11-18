// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

THINK(barrier_think) (edict_t* self) -> void
{
	if (!self)
	{
		return;
	}

	if (self->timeout > level.time)
	{
		self->svflags &= ~SVF_NOCLIENT;
	}
	else
	{
		self->svflags |= SVF_NOCLIENT;
	}

	self->nextthink = level.time + FRAME_TIME_S;
}

PAIN(barrier_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (!self)
	{
		return;
	}

	self->timeout = level.time + FRAME_TIME_S * 2;
	if (self->damage_debounce_time < level.time)
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/lashit.wav"), 1, ATTN_NORM, 0);
		self->damage_debounce_time = level.time + FRAME_TIME_S * 2;
	}
}

TOUCH(barrier_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (!self || !other)
	{
		return;
	}

	if (other == world)
		return;

	self->timeout = level.time + FRAME_TIME_S * 2;
	if (self->touch_debounce_time < level.time)
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/lashit.wav"), 1, ATTN_NORM, 0);
		self->touch_debounce_time = level.time + FRAME_TIME_S * 2;
	}

}

void SP_func_barrier(edict_t* self)
{
	if (!self)
	{
		return;
	}

	self->solid = SOLID_BBOX;
	self->movetype = MOVETYPE_NONE;
	self->s.modelindex = gi.modelindex("models/objects/wall/tris.md2");
	self->svflags = SVF_NOCLIENT;
	self->s.effects = EF_BFG;

	self->think = barrier_think;
	self->nextthink = level.time + FRAME_TIME_S;
	self->touch = barrier_touch;
	self->health = 1;
	self->takedamage = false;
	self->pain = barrier_pain;

	gi.linkentity(self);
}