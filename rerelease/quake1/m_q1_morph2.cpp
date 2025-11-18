/*// Licensed under the GNU General Public License 2.0.

==============================================================================

QUAKE GUARDIAN (Child)

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_morph.h"
#include "../m_flash.h"

static cached_soundindex sound_pain;
static cached_soundindex sound_death;
static cached_soundindex sound_sight;
static cached_soundindex sound_attack;

void priest_stand(edict_t* self);
void priest_run(edict_t* self);
void priest_walk(edict_t* self);
void priest_attack(edict_t* self);
void priest_melee(edict_t* self);
void SP_monster_priest(edict_t* self);

//
// HELPER FUNCTIONS
//

void priest_launch_laser(edict_t* self, vec3_t start, vec3_t dir)
{
    fire_flame(self, start, dir, 15, 1000);
}

void priest_chooseskin(edict_t* self)
{
    if (self->style == 0)
        self->s.skinnum = 1;
    else if (self->style == 1)
        self->s.skinnum = 4;
    else if (self->style == 2)
        self->s.skinnum = 7;
}

void priest_skinminus(edict_t* self)
{
    self->s.skinnum = 0;
}

void priest_skinplus(edict_t* self)
{
    self->s.skinnum += 1;
}

//
// SOUNDS
//

MONSTERINFO_SIGHT(priest_sight) (edict_t* self, edict_t* other) -> void
{
    gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
// STANDING
//

mframe_t priest_frames_stand[] = {
    { ai_stand }
};
MMOVE_T(priest_move_stand) = { FRAME_mfstand1, FRAME_mfstand1, priest_frames_stand, nullptr };

MONSTERINFO_STAND(priest_stand) (edict_t* self) -> void
{
    M_SetAnimation(self, &priest_move_stand);
}

//
// WALKING
//

mframe_t priest_frames_walk[] = {
    { ai_walk, 3 },
    { ai_walk, 2 },
    { ai_walk, 4 },
    { ai_walk, 5 },
    { ai_walk, 5 },
    { ai_walk, 4 },
    { ai_walk, 2 },
    { ai_walk, 3 },
    { ai_walk, 4 },
    { ai_walk, 5 },
    { ai_walk, 5 },
    { ai_walk, 4 },
    { ai_walk, 3 }
};
MMOVE_T(priest_move_walk) = { FRAME_mfwlk01, FRAME_mfwlk13, priest_frames_walk, nullptr };

MONSTERINFO_WALK(priest_walk) (edict_t* self) -> void
{
    M_SetAnimation(self, &priest_move_walk);
}

//
// RUNNING
//

mframe_t priest_frames_run[] = {
    { ai_run, 7 },
    { ai_run, 11 },
    { ai_run, 16 },
    { ai_run, 16 },
    { ai_run, 11 },
    { ai_run, 7 },
    { ai_run, 11 },
    { ai_run, 15 },
    { ai_run, 19 },
    { ai_run, 15 },
    { ai_run, 11 }
};
MMOVE_T(priest_move_run) = { FRAME_mfrun01, FRAME_mfrun11, priest_frames_run, nullptr };

MONSTERINFO_RUN(priest_run) (edict_t* self) -> void
{
    if (self->monsterinfo.aiflags & AI_STAND_GROUND)
        M_SetAnimation(self, &priest_move_stand);
    else
        M_SetAnimation(self, &priest_move_run);
}

//
// MELEE ATTACKS
//

void priest_melee_strike(edict_t* self)
{
    vec3_t start, dir, forward, right;
    float dist;

    if (!self->enemy || !self->enemy->inuse)
        return;

    AngleVectors(self->s.angles, forward, right, nullptr);
    start = self->s.origin;
    start[2] += self->viewheight;

    dir = self->enemy->s.origin - self->s.origin;
    dist = dir.length();
    dir.normalize();

    if (dist <= 90)
    {
        gi.sound(self, CHAN_WEAPON, sound_attack, 1, ATTN_NORM, 0);
        int damage = 20 + irandom(10);
        T_Damage(self->enemy, self, self, forward, self->enemy->s.origin, vec3_origin, damage, 0, DAMAGE_NONE, MOD_UNKNOWN);
    }
    else
    {
        vec3_t offset = self->s.origin + (forward * 80.f) + (right * 4.f);
        offset[2] += 4;

        priest_launch_laser(self, offset, dir);

        if (dist > 400)
        {
            vec3_t spread_dir = dir + (right * 0.04f);
            spread_dir.normalize();
            priest_launch_laser(self, offset, spread_dir);

            spread_dir = dir - (right * 0.04f);
            spread_dir.normalize();
            priest_launch_laser(self, offset, spread_dir);
        }
        else
        {
            vec3_t spread_dir = dir + (right * 0.10f);
            spread_dir.normalize();
            priest_launch_laser(self, offset, spread_dir);

            spread_dir = dir - (right * 0.10f);
            spread_dir.normalize();
            priest_launch_laser(self, offset, spread_dir);
        }
    }
}

void priest_knockback_strike(edict_t* self)
{
    vec3_t delta, forward;

    if (!self->enemy || !self->enemy->inuse)
        return;

    delta = self->enemy->s.origin - self->s.origin;
    float dist = delta.length();

    if (dist > 100)
        return;

    int damage = 10 + irandom(10);
    AngleVectors(self->s.angles, forward, nullptr, nullptr);

    T_Damage(self->enemy, self, self, forward, self->enemy->s.origin, vec3_origin, damage, 100, DAMAGE_NONE, MOD_UNKNOWN);

    self->enemy->velocity = forward * 100.f;
    self->enemy->velocity[2] = 100;
}

mframe_t priest_frames_attack[] = {
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge, 0, priest_melee_strike },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge }
};
MMOVE_T(priest_move_attack) = { FRAME_mfatb01, FRAME_mfatb11, priest_frames_attack, priest_run };

mframe_t priest_frames_bigattack[] = {
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge, 0, priest_melee_strike },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge }
};
MMOVE_T(priest_move_bigattack) = { FRAME_mfatc01, FRAME_mfatc16, priest_frames_bigattack, priest_run };

mframe_t priest_frames_knockback[] = {
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge, 0, priest_knockback_strike },
    { ai_charge },
    { ai_charge },
    { ai_charge }
};
MMOVE_T(priest_move_knockback) = { FRAME_mfknck01, FRAME_mfknck12, priest_frames_knockback, priest_run };

MONSTERINFO_MELEE(priest_melee) (edict_t* self) -> void
{
    float choice = frandom();

    if (choice < 0.50f)
        M_SetAnimation(self, &priest_move_bigattack);
    else if (choice < 0.75f)
        M_SetAnimation(self, &priest_move_attack);
    else
        M_SetAnimation(self, &priest_move_knockback);
}

//
// RANGED ATTACK
//

void priest_fire_weapon(edict_t* self)
{
    vec3_t dir, forward, right, offset;

    if (!self->enemy || !self->enemy->inuse)
        return;

    AngleVectors(self->s.angles, forward, right, nullptr);

    offset = self->s.origin + (forward * 30.f) + (right * 8.5f);
    offset[2] += 16;

    dir = self->enemy->s.origin - self->s.origin;
    dir.normalize();

    priest_launch_laser(self, offset, dir);

    float dist = (self->enemy->s.origin - self->s.origin).length();

    if (dist > 400)
    {
        vec3_t spread_dir = dir + (right * 0.04f);
        spread_dir.normalize();
        priest_launch_laser(self, offset, spread_dir);

        spread_dir = dir - (right * 0.04f);
        spread_dir.normalize();
        priest_launch_laser(self, offset, spread_dir);
    }
    else
    {
        vec3_t spread_dir = dir + (right * 0.10f);
        spread_dir.normalize();
        priest_launch_laser(self, offset, spread_dir);

        spread_dir = dir - (right * 0.10f);
        spread_dir.normalize();
        priest_launch_laser(self, offset, spread_dir);
    }
}

mframe_t priest_frames_fire[] = {
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge, 0, priest_fire_weapon },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge },
    { ai_charge }
};
MMOVE_T(priest_move_fire) = { FRAME_mfata01, FRAME_mfata09, priest_frames_fire, priest_run };

MONSTERINFO_ATTACK(priest_attack) (edict_t* self) -> void
{
    M_SetAnimation(self, &priest_move_fire);
}

//
// PAIN
//

void priest_teleport(edict_t* self);

mframe_t priest_frames_painA[] = {
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move, 0, priest_teleport }
};
MMOVE_T(priest_move_painA) = { FRAME_mfpain01, FRAME_mfpain10, priest_frames_painA, priest_run };

mframe_t priest_frames_painB[] = {
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move, 0, priest_teleport }
};
MMOVE_T(priest_move_painB) = { FRAME_mfpbin01, FRAME_mfpbin07, priest_frames_painB, priest_run };

PAIN(priest_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
    if (level.time < self->pain_debounce_time)
        return;

    if (skill->value == 3)
    {
        if (frandom() > 0.5f)
        {
            priest_teleport(self);
            return;
        }
    }

    if (frandom() > 0.25f)
        return;

    self->pain_debounce_time = level.time + 2_sec;

    gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

    if (frandom() > 0.6f)
        M_SetAnimation(self, &priest_move_painB);
    else
        M_SetAnimation(self, &priest_move_painA);
}

MONSTERINFO_SETSKIN(priest_setskin) (edict_t* self) -> void
{
    if (self->health < (self->max_health / 2))
        self->s.skinnum |= 1;
    else
        self->s.skinnum &= ~1;
}

//
// DEATH
//

void priest_dead(edict_t* self)
{
    self->mins = { -16, -16, -24 };
    self->maxs = { 16, 16, -8 };
    monster_dead(self);
}

mframe_t priest_frames_death[] = {
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move, 0, priest_skinminus },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move }
};
MMOVE_T(priest_move_death) = { FRAME_mfdth01, FRAME_mfdth21, priest_frames_death, priest_dead };

DIE(priest_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
    if (M_CheckGib(self, mod))
    {
        gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

        ThrowGibs(self, damage, {
            { 2, "models/objects/gibs/bone/tris.md2" },
            { 2, "models/objects/gibs/sm_meat/tris.md2" },
            { "models/objects/gibs/skull/tris.md2", GIB_HEAD }
            });

        self->deadflag = true;
        return;
    }

    if (self->deadflag)
        return;

    self->deadflag = true;
    self->takedamage = true;

    gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
    M_SetAnimation(self, &priest_move_death);
}

//
// WAKE UP ANIMATION
//

mframe_t priest_frames_wake[] = {
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move, 0, priest_chooseskin },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move },
    { ai_move, 0, priest_skinplus }
};
MMOVE_T(priest_move_wake) = { FRAME_start01, FRAME_start31, priest_frames_wake, priest_run };

void priest_wake_start(edict_t* self)
{
    gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
    self->solid = SOLID_BBOX;
    self->takedamage = true;
    M_SetAnimation(self, &priest_move_wake);
}

//
// TELEPORT / SPAWN
//

bool priest_spawn_child(edict_t* self, float radius)
{
    for (int i = 0; i < 10; ++i)
    {
        vec3_t offset = {
            crandom() * radius,
            crandom() * radius,
            0
        };
        vec3_t target = self->s.origin + offset;

        trace_t tr = gi.trace(self->s.origin, self->mins, self->maxs, target, self, MASK_SOLID);
        if (tr.startsolid)
            continue;

        trace_t occ = gi.trace(target, self->mins, self->maxs, target, self, MASK_MONSTERSOLID);
        if (occ.fraction < 1.0f)
            continue;

        // Spawn child priest
        edict_t* child = G_Spawn();
        child->s.origin = target;
        child->style = self->style;
        child->monsterinfo.commander = self;
        child->enemy = self->enemy;

        SP_monster_priest(child);

        // Teleport effect at spawn location
        gi.WriteByte(svc_temp_entity);
        gi.WriteByte(TE_TELEPORT_EFFECT);
        gi.WritePosition(target);
        gi.multicast(target, MULTICAST_PVS, false);

        self->monsterinfo.monster_slots++;
        return true;
    }
    return false;
}

void priest_teleport(edict_t* self)
{
    if (self->monsterinfo.commander)
        return;

    if (self->monsterinfo.monster_slots >= (1 + (int)skill->value))
        return;

    gi.WriteByte(svc_temp_entity);
    gi.WriteByte(TE_TELEPORT_EFFECT);
    gi.WritePosition(self->s.origin);
    gi.multicast(self->s.origin, MULTICAST_PVS, false);

    priest_spawn_child(self, 256.0f);
}

/*QUAKED monster_priest (1 0 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
Morphing boss from Quake 1. Spawns weaker priests when damaged.

style: 0=Aztec, 1=Egyptian, 2=Greek
*/
void SP_monster_priest(edict_t* self)
{
    if (!M_AllowSpawn(self))
    {
        G_FreeEdict(self);
        return;
    }

    self->s.modelindex = gi.modelindex("models/monsters/priest/tris.md2");

    if (self->style == 0)
        self->s.skinnum = 1;
    else if (self->style == 1)
        self->s.skinnum = 4;
    else if (self->style == 2)
        self->s.skinnum = 7;
    else
        self->s.skinnum = 1;

    sound_attack.assign("infantry/melee1.wav");
    sound_pain.assign("enforcer/pain1.wav");
    sound_death.assign("enforcer/death1.wav");
    sound_sight.assign("enforcer/sight1.wav");

    self->mins = { -16, -16, -24 };
    self->maxs = { 16, 16, 32 };
    self->movetype = MOVETYPE_STEP;
    self->solid = SOLID_BBOX;

    self->health = 2000;
    self->gib_health = -200;
    self->mass = 400;

    self->pain = priest_pain;
    self->die = priest_die;

    self->monsterinfo.stand = priest_stand;
    self->monsterinfo.walk = priest_walk;
    self->monsterinfo.run = priest_run;
    self->monsterinfo.attack = priest_attack;
    self->monsterinfo.melee = priest_melee;
    self->monsterinfo.sight = priest_sight;

    self->monsterinfo.scale = MODEL_SCALE;

    gi.linkentity(self);

    if (self->targetname)
    {
        self->solid = SOLID_NOT;
        self->takedamage = false;
        self->use = monster_use;
        M_SetAnimation(self, &priest_move_stand);
    }
    else
    {
        M_SetAnimation(self, &priest_move_wake);
        walkmonster_start(self);
    }
}