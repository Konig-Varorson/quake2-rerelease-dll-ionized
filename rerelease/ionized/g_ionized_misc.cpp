// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

/*QUAKED misc_gib_head2 (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_head2(edict_t* ent)
{
	gi.setmodel(ent, "models/objects/gibs/head2/tris.md2");
	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);
}

/*QUAKED misc_gib_chest (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_chest(edict_t* ent)
{
	gi.setmodel(ent, "models/objects/gibs/chest/tris.md2");
	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);
}

/*QUAKED misc_gib_gekk(1 0 0) (-8 - 8 - 8) (8 8 8)
*/
void SP_misc_gib_gekk(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GREENGIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(6);

	if (r == 0)
		gi.setmodel(ent, "models/objects/gekkgibs/arm/tris.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/objects/gekkgibs/claw/tris.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/objects/gekkgibs/head/tris.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/objects/gekkgibs/torso/tris.md2");
	else if (r == 4)
		gi.setmodel(ent, "models/objects/gekkgibs/pelvis/tris.md2");
	else if (r == 5)
		gi.setmodel(ent, "models/objects/gekkgibs/leg/tris.md2");
}

/*QUAKED misc_gib_berserk(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_berserk(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/berserk/gibs/hammer.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/berserk/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/berserk/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/berserk/gibs/thigh.md2");
}

/*QUAKED misc_gib_chick(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_chick(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/bitch/gibs/arm.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/bitch/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/bitch/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/bitch/gibs/foot.md2");
}

/*QUAKED misc_gib_brain(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_brain(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/brain/gibs/arm.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/brain/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/brain/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/brain/gibs/boot.md2");
}

/*QUAKED misc_gib_glad(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_glad(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/gladiatr/gibs/larm.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/gladiatr/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/gladiatr/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/gladiatr/gibs/thigh.md2");
}

/*QUAKED misc_gib_gunner(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_gunner(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/gunner/gibs/garm.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/gunner/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/gunner/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/gunner/gibs/foot.md2");
}

/*QUAKED misc_gib_hover(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_hover(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/hover/gibs/ring.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/hover/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/hover/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/hover/gibs/foot.md2");
}

/*QUAKED misc_gib_infantry(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_infantry(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/infantry/gibs/arm.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/infantry/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/infantry/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/infantry/gibs/foot.md2");
}

/*QUAKED misc_gib_medic(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_medic(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/medic/gibs/hook.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/medic/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/medic/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/medic/gibs/leg.md2");
}

/*QUAKED misc_gib_parasite(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_parasite(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/parasite/gibs/fleg.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/parasite/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/parasite/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/parasite/gibs/bleg.md2");
}

/*QUAKED misc_gib_soldier(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_soldier(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/soldier/gibs/arm.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/soldier/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/soldier/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/soldier/gibs/gun.md2");
}

/*QUAKED misc_gib_stalker(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_stalker(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/stalker/gibs/claw.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/stalker/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/stalker/gibs/bodya.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/stalker/gibs/leg.md2");
}

/*QUAKED misc_gib_tank(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_tank(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/tank/gibs/barm.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/tank/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/tank/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/tank/gibs/thigh.md2");
}

/*QUAKED misc_gib_mutant(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_mutant(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/mutant/gibs/hand.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/mutant/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/mutant/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/mutant/gibs/foot.md2");
}

/*QUAKED misc_gib_boss1(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
Boss gibs
*/
void SP_misc_gib_boss1(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/boss1/gibs/cgun.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/boss1/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/boss1/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/boss1/gibs/ltread.md2");
}

/*QUAKED misc_gib_boss2(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
Boss gibs
*/
void SP_misc_gib_boss2(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/boss2/gibs/larm.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/boss2/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/boss2/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/boss2/gibs/wing.md2");
}

/*QUAKED misc_gib_boss3(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
Boss gibs
*/
void SP_misc_gib_boss3(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/boss3/jorg/gibs/gun.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/boss3/jorg/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/boss3/jorg/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/boss3/jorg/gibs/thigh.md2");
}

/*QUAKED misc_gib_carrier(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
Boss gibs
*/
void SP_misc_gib_carrier(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/carrier/gibs/rwing.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/carrier/gibs/head.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/carrier/gibs/chest.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/carrier/gibs/thigh.md2");
}

/*QUAKED misc_gib_strider(1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_strider(edict_t* ent)
{
	int r;

	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	r = irandom(4);

	if (r == 0)
		gi.setmodel(ent, "models/monsters/guardian/gib5.md2");
	else if (r == 1)
		gi.setmodel(ent, "models/monsters/guardian/gib3.md2");
	else if (r == 2)
		gi.setmodel(ent, "models/monsters/guardian/gib4.md2");
	else if (r == 3)
		gi.setmodel(ent, "models/monsters/guardian/gib6.md2");
}

/* KONIG - TO DO; other Quake 1 gibs */
/*QUAKED misc_gib_shambler (1 0 0) (-8 - 8 - 8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_shambler(edict_t* ent)
{
	ent->solid = SOLID_NOT;
	ent->s.effects |= EF_GIB;
	ent->takedamage = true;
	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->deadflag = true;
	ent->avelocity[0] = frandom(200);
	ent->avelocity[1] = frandom(200);
	ent->avelocity[2] = frandom(200);
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 10_sec;
	gi.linkentity(ent);

	gi.setmodel(ent, "models/monsters/shambler/gibs/head.md2");
}
