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

/*
=================
misc_prox
=================
*/

constexpr gtime_t PROX_TIME_DELAY = 500_ms;
constexpr float	  PROX_DAMAGE_RADIUS = 192;
constexpr int32_t PROX_HEALTH = 20;
constexpr int32_t PROX_DAMAGE = 60;

void Prox_Explode(edict_t* ent);
void prox_die(edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod);

constexpr spawnflags_t SPAWNFLAG_IGNORE_GOODGUY = 1_spawnflag;
constexpr spawnflags_t SPAWNFLAG_START_INACTIVE = 2_spawnflag;
constexpr spawnflags_t SPAWNFLAG_NO_MOVE = 4_spawnflag;

THINK(misc_prox_seek) (edict_t* ent) -> void
{
	edict_t* target = nullptr;
	edict_t* best = nullptr;
	vec3_t	 vec;
	float	 len;
	float	 oldlen = 8000;

	while ((target = findradius(target, ent->s.origin, PROX_DAMAGE_RADIUS)) != nullptr)
	{
		if (target == ent)
			continue;

		if (!target->client && (ent->spawnflags.has(SPAWNFLAG_IGNORE_GOODGUY) || !(target->monsterinfo.aiflags & AI_GOOD_GUY)))
			continue;

		if (target->health <= 0)
			continue;

		if (!visible(ent, target))
			continue;

		vec = ent->s.origin - target->s.origin;
		len = vec.length();

		if (!best)
		{
			best = target;
			oldlen = len;
			continue;
		}
		if (len < oldlen)
		{
			oldlen = len;
			best = target;
		}
	}

	if (best)
	{
		gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/proxwarn.wav"), 1, ATTN_NORM, 0);
		ent->think = Prox_Explode;
		ent->nextthink = level.time + PROX_TIME_DELAY;
		return;
	}

	ent->nextthink = level.time + 10_hz;
}

THINK(misc_prox_activate) (edict_t* ent) -> void
{
	ent->s.frame = 9;
	ent->s.skinnum = 3;
	ent->think = misc_prox_seek;
	ent->nextthink = level.time + 10_hz;
}

USE(misc_prox_use) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	if (self->nextthink)
		return;

	misc_prox_activate(self);
}

void SP_misc_prox(edict_t* ent)
{
	if (deathmatch->integer)
	{
		G_FreeEdict(ent);
		return;
	}

	if (!ent->health)
		ent->health = PROX_HEALTH;

	if (!ent->dmg)
		ent->dmg = PROX_DAMAGE;

	ent->s.modelindex = gi.modelindex("models/weapons/g_prox/tris.md2");
	ent->mins = { -6, -6, -6 };
	ent->maxs = { 6, 6, 6 };
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->takedamage = true;
	ent->die = prox_die;
	ent->classname = "prox_mine";
	ent->flags |= (FL_DAMAGEABLE | FL_TRAP | FL_MECHANICAL);
	ent->s.renderfx |= RF_IR_VISIBLE;

	if (ent->spawnflags.has(SPAWNFLAG_START_INACTIVE))
	{
		ent->use = misc_prox_use;
		ent->s.frame = 0;
	}
	else
	{
		misc_prox_activate(ent);
	}

	gi.linkentity(ent);
}

/*
=================

misc_tesla

=================
*/

constexpr float	  TESLA_DAMAGE_RADIUS = 128;
constexpr int32_t TESLA_DAMAGE = 3;
constexpr int32_t TESLA_KNOCKBACK = 8;

DIE(misc_tesla_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	BecomeExplosion1(self);
}

THINK(misc_tesla_think_active) (edict_t* self) -> void
{
	edict_t* target = nullptr;
	vec3_t start = self->s.origin;
	start[2] += 16;

	while ((target = findradius(target, self->s.origin, TESLA_DAMAGE_RADIUS)) != nullptr)
	{
		if (!target->inuse)
			continue;
		if (target == self)
			continue;
		if (target->health < 1)
			continue;

		bool is_valid_target = false;

		if (target->client)
		{
			is_valid_target = true;
		}
		else if (!self->spawnflags.has(SPAWNFLAG_IGNORE_GOODGUY) && (target->monsterinfo.aiflags & AI_GOOD_GUY))
		{
			is_valid_target = true;
		}

		if (!is_valid_target)
			continue;

		if (!(target->svflags & SVF_MONSTER) && !(target->flags & FL_DAMAGEABLE) && !target->client)
			continue;

		trace_t tr = gi.traceline(start, target->s.origin, self, MASK_PROJECTILE);
		if (tr.fraction == 1 || tr.ent == target)
		{
			vec3_t dir = target->s.origin - start;

			if (self->dmg > TESLA_DAMAGE)
				gi.sound(self, CHAN_ITEM, gi.soundindex("items/damage3.wav"), 1, ATTN_NORM, 0);

			if ((target->svflags & SVF_MONSTER) && !(target->flags & (FL_FLY | FL_SWIM)))
				T_Damage(target, self, self->owner, dir, tr.endpos, tr.plane.normal,
					self->dmg, 0, DAMAGE_NONE, MOD_TESLA);
			else
				T_Damage(target, self, self->owner, dir, tr.endpos, tr.plane.normal,
					self->dmg, TESLA_KNOCKBACK, DAMAGE_NONE, MOD_TESLA);

			gi.WriteByte(svc_temp_entity);
			gi.WriteByte(TE_LIGHTNING);
			gi.WriteEntity(self);
			gi.WriteEntity(target);
			gi.WritePosition(start);
			gi.WritePosition(tr.endpos);
			gi.multicast(start, MULTICAST_PVS, false);
		}
	}

	if (self->inuse)
	{
		self->think = misc_tesla_think_active;
		self->nextthink = level.time + 10_hz;
	}
}

THINK(misc_tesla_activate) (edict_t* ent) -> void
{
	ent->s.frame = 14;
	ent->s.skinnum = 3;
	ent->think = misc_tesla_think_active;
	ent->nextthink = level.time + 10_hz;
}

USE(misc_tesla_use) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	if (self->nextthink)
		return;

	misc_tesla_activate(self);
}

void SP_misc_tesla(edict_t* ent)
{
	if (deathmatch->integer)
	{
		G_FreeEdict(ent);
		return;
	}

	if (!ent->health)
		ent->health = 20;

	if (!ent->dmg)
		ent->dmg = TESLA_DAMAGE;

	ent->s.modelindex = gi.modelindex("models/weapons/g_tesla/tris.md2");
	ent->mins = { -12, -12, 0 };
	ent->maxs = { 12, 12, 20 };
	if (ent->spawnflags.has(SPAWNFLAG_NO_MOVE))
		ent->movetype = MOVETYPE_NONE;
	else
		ent->movetype = MOVETYPE_TOSS;
	ent->solid = SOLID_BBOX;
	ent->takedamage = true;
	ent->die = misc_tesla_die;
	ent->classname = "tesla_mine";
	ent->owner = ent;
	ent->flags |= (FL_DAMAGEABLE | FL_TRAP | FL_MECHANICAL);
	ent->s.renderfx |= RF_IR_VISIBLE;
	ent->s.effects |= EF_GRENADE;

	if (ent->spawnflags.has(SPAWNFLAG_START_INACTIVE))
	{
		ent->use = misc_tesla_use;
		ent->s.frame = 0;
	}
	else
	{
		misc_tesla_activate(ent);
	}

	gi.linkentity(ent);
}

/*
=================

misc_foodcube_trap

=================
*/

void Trap_Think(edict_t* self);
void SP_item_foodcube(edict_t* self);
void Trap_Gib_Think(edict_t* self);
void trap_die(edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod);

USE(misc_foodcube_trap_use) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	if (self->s.frame >= 4)
		return;

	self->s.frame = 4;
	self->think = Trap_Think;
	self->nextthink = level.time + 10_hz;
}

THINK(misc_trap_think) (edict_t* ent) -> void
{
	edict_t* target = nullptr;
	edict_t* best = nullptr;
	vec3_t	 vec;
	float	 len;
	float	 oldlen = 8000;

	if (ent->timestamp < level.time)
	{
		BecomeExplosion1(ent);
		return;
	}

	ent->nextthink = level.time + 10_hz;

	if (ent->s.frame > 4)
	{
		if (ent->s.frame == 5)
		{
			bool spawn = ent->wait == 64;

			ent->wait -= 2;

			if (spawn)
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/trapdown.wav"), 1, ATTN_IDLE, 0);

			ent->delay += 2.f;

			if (ent->wait < 19)
				ent->s.frame++;

			return;
		}
		ent->s.frame++;
		if (ent->s.frame == 8)
		{
			ent->nextthink = level.time + 1_sec;
			ent->think = G_FreeEdict;
			ent->s.effects &= ~EF_TRAP;

			best = G_Spawn();
			best->count = ent->mass;
			best->s.scale = 1.f + ((ent->accel - 100.f) / 300.f) * 1.0f;
			SP_item_foodcube(best);
			best->s.origin = ent->s.origin;
			best->s.origin[2] += 24 * best->s.scale;
			best->s.angles[YAW] = frandom() * 360;
			best->velocity[2] = 400;
			best->think(best);
			best->nextthink = 0_ms;
			best->s.old_origin = best->s.origin;
			gi.linkentity(best);

			gi.sound(best, CHAN_AUTO, gi.soundindex("misc/fhit3.wav"), 1.f, ATTN_NORM, 0.f);

			return;
		}
		return;
	}

	ent->s.effects &= ~EF_TRAP;
	if (ent->s.frame >= 4)
	{
		ent->s.effects |= EF_TRAP;
	}

	if (ent->s.frame < 4)
	{
		ent->s.frame++;
		return;
	}

	while ((target = findradius(target, ent->s.origin, 256)) != nullptr)
	{
		if (target == ent)
			continue;

		if (target->classname && ((deathmatch->integer &&
			((!strncmp(target->classname, "info_player_", 12)) ||
				(!strcmp(target->classname, "misc_teleporter_dest")) ||
				(!strncmp(target->classname, "item_flag_", 10))))) &&
			(visible(target, ent)))
		{
			BecomeExplosion1(ent);
			return;
		}

		if (!target->client && (ent->spawnflags.has(SPAWNFLAG_IGNORE_GOODGUY) || !(target->monsterinfo.aiflags & AI_GOOD_GUY)))
			continue;

		if (target->health <= 0)
			continue;
		if (!visible(ent, target))
			continue;
		vec = ent->s.origin - target->s.origin;
		len = vec.length();
		if (!best)
		{
			best = target;
			oldlen = len;
			continue;
		}
		if (len < oldlen)
		{
			oldlen = len;
			best = target;
		}
	}

	if (best)
	{
		if (best->groundentity)
		{
			best->s.origin[2] += 1;
			best->groundentity = nullptr;
		}
		vec = ent->s.origin - best->s.origin;
		len = vec.normalize();

		float max_speed = best->client ? 145.f : 75.f;

		best->velocity += (vec * clamp(max_speed - len, 64.f, max_speed));

		ent->s.sound = gi.soundindex("weapons/trapsuck.wav");

		if (len < 48)
		{
			if (best->mass <= 400)
			{
				ent->takedamage = false;
				ent->solid = SOLID_NOT;
				ent->die = nullptr;

				T_Damage(best, ent, ent->owner, vec3_origin, best->s.origin, vec3_origin, 100000, 1, DAMAGE_NONE, MOD_TRAP);

				if (best->svflags & SVF_MONSTER)
					M_ProcessPain(best);

				ent->enemy = best;
				ent->wait = 64;
				ent->s.old_origin = ent->s.origin;
				ent->timestamp = level.time + 30_sec;
				ent->accel = best->mass;
				if (deathmatch->integer)
					ent->mass = best->mass / 4;
				else
					ent->mass = best->mass / 10;
				ent->s.frame = 5;

				for (uint32_t i = 0; i < globals.num_edicts; i++)
				{
					edict_t* e = &g_edicts[i];

					if (!e->inuse)
						continue;
					else if (strcmp(e->classname, "gib"))
						continue;
					else if ((e->s.origin - ent->s.origin).length() > 128.f)
						continue;

					e->movetype = MOVETYPE_NONE;
					e->nextthink = level.time + FRAME_TIME_S;
					e->think = Trap_Gib_Think;
					e->owner = ent;
					Trap_Gib_Think(e);
				}
			}
			else
			{
				BecomeExplosion1(ent);
				return;
			}
		}
	}
}

void SP_misc_foodcube_trap(edict_t* ent)
{
	if (deathmatch->integer)
	{
		G_FreeEdict(ent);
		return;
	}

	if (!ent->health)
		ent->health = 20;

	ent->s.modelindex = gi.modelindex("models/weapons/z_trap/tris.md2");
	ent->mins = { -4, -4, 0 };
	ent->maxs = { 4, 4, 8 };
	if (ent->spawnflags.has(SPAWNFLAG_NO_MOVE))
		ent->movetype = MOVETYPE_NONE;
	else
		ent->movetype = MOVETYPE_TOSS;
	ent->solid = SOLID_BBOX;
	ent->takedamage = true;
	ent->die = trap_die;
	ent->classname = "food_cube_trap";
	ent->owner = ent;
	ent->flags |= (FL_DAMAGEABLE | FL_MECHANICAL | FL_TRAP);
	ent->s.sound = gi.soundindex("weapons/traploop.wav");
	ent->timestamp = level.time + 30_sec;

	if (ent->spawnflags.has(SPAWNFLAG_START_INACTIVE))
	{
		ent->use = misc_foodcube_trap_use;
		ent->s.frame = 0;
	}
	else
	{
		ent->s.frame = 4;
		ent->s.effects |= EF_TRAP;
		ent->think = misc_trap_think;
		ent->nextthink = level.time + 10_hz;
	}

	gi.linkentity(ent);
}