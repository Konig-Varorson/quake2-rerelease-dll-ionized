// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

#if 0
/*
===============================================================================

BOBBING

===============================================================================
*/


/*QUAKED func_bobbing (0 .5 .8) ? X_AXIS Y_AXIS
Normally bobs on the Z axis
"model2"	.md3 model to also draw
"height"	amplitude of bob (32 default)
"speed"		seconds to complete a bob cycle (4 default)
"phase"		the 0.0 to 1.0 offset in the cycle to start at
"dmg"		damage to inflict when blocked (2 default)
"color"		constantLight color
"light"		constantLight radius
*/
void SP_func_bobbing(edict_t* ent)
{

	if (!ent->speed)
		ent->speed = 4;
	if (!ent->height)
		ent->height = 32;
	if (!ent->dmg)
		ent->dmg = 2;
	if (!ent->phase)
		ent->phase = 0;

	ent->solid = SOLID_BSP;
	gi.setmodel(ent, ent->model);

	VectorCopy(ent->s.origin, ent->s.pos.trBase);
	VectorCopy(ent->s.origin, ent->r.currentOrigin);

	ent->s.pos.trDuration = ent->speed * 1000;
	ent->s.pos.trTime = ent->s.pos.trDuration * ent->phase;
	ent->s.pos.trType = TR_SINE;

	// set the axis of bobbing
	if (ent->spawnflags.has(1_spawnflag)) {
		ent->s.pos.trDelta[0] = ent->height;
	}
	else if (ent->spawnflags.has(2_spawnflag)) {
		ent->s.pos.trDelta[1] = ent->height;
	}
	else {
		ent->s.pos.trDelta[2] = ent->height;
	}
}

/*
===============================================================================

PENDULUM

===============================================================================
*/


/*QUAKED func_pendulum (0 .5 .8) ?
You need to have an origin brush as part of this entity.
Pendulums always swing north / south on unrotated models.  Add an angles field to the model to allow rotation in other directions.
Pendulum frequency is a physical constant based on the length of the beam and gravity.
"model2"	.md3 model to also draw
"speed"		the number of degrees each way the pendulum swings, (30 default)
"phase"		the 0.0 to 1.0 offset in the cycle to start at
"dmg"		damage to inflict when blocked (2 default)
"color"		constantLight color
"light"		constantLight radius
*/
void SP_func_pendulum(edict_t* ent)
{
	float freq, length;

	if (!ent->speed)
		ent->speed = 30;
	if (!ent->dmg)
		ent->dmg = 2;
	if (!ent->phase)
		ent->phase = 0;

	ent->solid = SOLID_BSP;
	gi.setmodel(ent, ent->model);

	// find pendulum length
	length = fabs(ent->mins[2]);
	if (length < 8)
		length = 8;

	freq = 1 / (PI * 2) * sqrt(g_gravity->value / (3 * length));

	ent->s.pos.trDuration = (1000 / freq);

	//ent->moveinfo.
	InitMover(ent);

	VectorCopy(ent->s.origin, ent->s.pos.trBase);
	VectorCopy(ent->s.origin, ent->r.currentOrigin);

	VectorCopy(ent->s.angles, ent->s.apos.trBase);

	ent->s.apos.trDuration = 1000 / freq;
	ent->s.apos.trTime = ent->s.apos.trDuration * phase;
	ent->s.apos.trType = TR_SINE;
	ent->s.apos.trDelta[2] = speed;

	//Move_Calc(ent, ent->moveinfo.end_origin, 

}
#endif