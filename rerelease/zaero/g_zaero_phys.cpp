// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

void SV_Physics_Step(edict_t* ent);

/*
=============
SV_Physics_FallFloat
=============
*/

void SV_Physics_FallFloat(edict_t* ent)
{
	float gravVal;
	bool wasonground = false;
	bool hitsound = false;

	if (!ent)
	{
		return;
	}

	gravVal = ent->gravity * sv_gravity->value * gi.frame_time_s;

	// check velocity
	SV_CheckVelocity(ent);

	wasonground = (ent->groundentity == nullptr);
	if (ent->velocity[2] < sv_gravity->value * -0.1)
		hitsound = true;

	if (!ent->waterlevel)
	{
		vec3_t min, max;
		trace_t tr;
		vec3_t end;
		vec3_t normal;
		vec3_t gravity;

		min = ent->mins;
		max = ent->maxs;

		end = ent->s.origin;
		end[2] -= 0.25; // down 4

		tr = gi.trace(ent->s.origin, min, max, end, ent, MASK_SHOT);
		if (tr.plane.normal[2] > 0.7) // on solid ground
		{
			ent->groundentity = tr.ent;
			ent->s.origin = tr.endpos;
			ent->velocity = { 0, 0, 0 };
		}
		else if (tr.fraction < 1.0 && tr.plane.normal[2] <= 0.7) // on steep slope
		{
			normal = tr.plane.normal;
			gravity = { 0, 0, -gravVal };
			ent->velocity = gravity + (normal * gravVal);
			ent->groundentity = nullptr;
		}
		else // in freefall
		{
			ent->velocity[2] -= gravVal;
			ent->groundentity = nullptr;
		}
	}
	else
	{
		// where's the midpoint? above or below the water?
		const double WATER_MASS = 500.0;
		vec3_t accel;
		float percentBelow = 0.0;
		float massOfObject = 0.0;
		float massOfVolumeWater = 0.0;
		float massOfWater = 0.0;
		float massDiff = 0.0;
		double i = 0.0;
		vec3_t volume;

		// TODO if we're not grounded on the bottom of the lake...

		// calculate massPerCubicMetre
		volume = ent->size * (1.0 / 32.0);
		massOfObject = ent->mass;
		massOfVolumeWater = WATER_MASS * (volume[0] * volume[1] * volume[2]);

		// how much of ourself is actually in the water?
		percentBelow = 1.0;
		for (i = 0.0; i <= 1.0; i += 0.05)
		{
			vec3_t midpoint;
			int watertype;

			midpoint = ent->s.origin + ent->mins;
			midpoint = midpoint + (ent->maxs * i);
			watertype = gi.pointcontents(midpoint);

			if (!(watertype & MASK_WATER))
			{
				percentBelow = i - 0.05;
				break;
			}
		}
		if (percentBelow < 0.05) // safety net
			percentBelow = 0.0;
		massOfWater = percentBelow * massOfVolumeWater;
		massDiff = massOfWater - massOfObject; // difference between
		accel = {};
		accel = { 0, 0, gravVal * (massDiff / massOfVolumeWater) };
		ent->velocity = ent->velocity * 0.7;
		if (accel.length() > 4)
			ent->velocity = accel + ent->velocity;
	}

	if (ent->velocity[0] || ent->velocity[1] || ent->velocity[2])
	{
		bool isinwater = false;
		bool wasinwater = false;
		vec3_t old_origin;
		old_origin = ent->s.origin;

		SV_FlyMove(ent, gi.frame_time_s, MASK_SHOT);

		gi.linkentity(ent);
		G_TouchTriggers(ent);

		if (ent->groundentity)
			if (!wasonground)
				if (hitsound)
					gi.sound(ent, CHAN_AUTO, gi.soundindex("world/land.wav"), 1, 1, 0);

		// check for water transition
		wasinwater = (ent->watertype & MASK_WATER);
		ent->watertype = gi.pointcontents(ent->s.origin);
		isinwater = ent->watertype & MASK_WATER;

		if (isinwater)
			ent->waterlevel = WATER_FEET;
		else
			ent->waterlevel = WATER_NONE;

		if (!wasinwater && isinwater)
			gi.positioned_sound(old_origin, g_edicts, CHAN_AUTO, gi.soundindex("misc/h2ohit1.wav"), 1, 1, 0);
		else if (wasinwater && !isinwater)
			gi.positioned_sound(ent->s.origin, g_edicts, CHAN_AUTO, gi.soundindex("misc/h2ohit1.wav"), 1, 1, 0);
	}

	// relink
	gi.linkentity(ent);

	// regular thinking
	SV_RunThink(ent);
}

/*
=============
SV_Physics_Ride
=============
*/

void adjustRiders(edict_t* ent)
{
	int i = 0;

	if (!ent)
	{
		return;
	}

	// make sure the offsets are constant
	for (i = 0; i < 2; i++)
	{
		if (ent->rideWith[i] != nullptr)
			ent->rideWith[i]->s.origin = ent->s.origin + ent->rideWithOffset[i];
	}
}

void SV_Physics_Ride(edict_t* ent)
{
	if (!ent)
	{
		return;
	}

	// base ourself on the step
	SV_Physics_Step(ent);

	adjustRiders(ent);
}