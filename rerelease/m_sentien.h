// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
// G:\quake2\baseq2\models/monsters/sentien

enum
{
	FRAME_idle101,
	FRAME_idle102,
	FRAME_idle103,
	FRAME_idle104,
	FRAME_idle105,
	FRAME_idle106,
	FRAME_idle107,
	FRAME_idle108,
	FRAME_idle109,
	FRAME_idle110,
	FRAME_idle111,
	FRAME_idle112,
	FRAME_idle113,
	FRAME_idle114,
	FRAME_idle115,
	FRAME_idle116,
	FRAME_idle117,
	FRAME_idle118,
	FRAME_idle119,
	FRAME_idle120,
	FRAME_idle121,
	FRAME_idle122,
	FRAME_idle123,
	FRAME_idle124,
	FRAME_idle125,
	FRAME_idle126,
	FRAME_idle127,
	FRAME_idle128,
	FRAME_idle129,
	FRAME_idle201,
	FRAME_idle202,
	FRAME_idle203,
	FRAME_idle204,
	FRAME_idle205,
	FRAME_idle206,
	FRAME_idle207,
	FRAME_idle208,
	FRAME_idle209,
	FRAME_idle210,
	FRAME_idle211,
	FRAME_idle212,
	FRAME_idle213,
	FRAME_idle214,
	FRAME_idle215,
	FRAME_idle216,
	FRAME_idle217,
	FRAME_idle218,
	FRAME_idle219,
	FRAME_idle220,
	FRAME_idle301,
	FRAME_idle302,
	FRAME_idle303,
	FRAME_idle304,
	FRAME_idle305,
	FRAME_idle306,
	FRAME_idle307,
	FRAME_idle308,
	FRAME_idle309,
	FRAME_idle310,
	FRAME_idle311,
	FRAME_idle312,
	FRAME_idle313,
	FRAME_idle314,
	FRAME_idle315,
	FRAME_idle316,
	FRAME_idle317,
	FRAME_idle318,
	FRAME_idle319,
	FRAME_idle320,
	FRAME_idle321,
	FRAME_idle322,
	FRAME_idle323,
	FRAME_idle324,
	FRAME_idle325,
	FRAME_idle326,
	FRAME_idle327,
	FRAME_idle328,
	FRAME_idle329,
	FRAME_idle330,
	FRAME_idle331,
	FRAME_walk01,
	FRAME_walk02,
	FRAME_walk03,
	FRAME_walk04,
	FRAME_walk05,
	FRAME_walk06,
	FRAME_walk07,
	FRAME_walk08,
	FRAME_walk09,
	FRAME_walk10,
	FRAME_walk11,
	FRAME_walk12,
	FRAME_walk13,
	FRAME_walk14,
	FRAME_walk15,
	FRAME_walk16,
	FRAME_walk17,
	FRAME_walk18,
	FRAME_walk19,
	FRAME_walk20,
	FRAME_walk21,
	FRAME_walk22,
	FRAME_walk23,
	FRAME_walk24,
	FRAME_walk25,
	FRAME_walk26,
	FRAME_walk27,
	FRAME_walk28,
	FRAME_walk29,
	FRAME_walk30,
	FRAME_walk31,
	FRAME_walk32,
	FRAME_attak101,
	FRAME_attak102,
	FRAME_attak103,
	FRAME_attak104,
	FRAME_attak105,
	FRAME_attak106,
	FRAME_attak107,
	FRAME_attak108,
	FRAME_attak109,
	FRAME_attak110,
	FRAME_attak111,
	FRAME_attak112,
	FRAME_attak113,
	FRAME_attak114,
	FRAME_attak201,
	FRAME_attak202,
	FRAME_attak203,
	FRAME_attak204,
	FRAME_attak205,
	FRAME_attak206,
	FRAME_attak207,
	FRAME_attak208,
	FRAME_attak209,
	FRAME_attak210,
	FRAME_attak211,
	FRAME_attak212,
	FRAME_attak213,
	FRAME_attak214,
	FRAME_attak215,
	FRAME_attak216,
	FRAME_attak217,
	FRAME_attak218,
	FRAME_attak219,
	FRAME_attak220,
	FRAME_attak301,
	FRAME_attak302,
	FRAME_attak303,
	FRAME_attak304,
	FRAME_attak305,
	FRAME_attak306,
	FRAME_attak307,
	FRAME_attak308,
	FRAME_attak309,
	FRAME_attak310,
	FRAME_attak311,
	FRAME_attak312,
	FRAME_attak313,
	FRAME_attak314,
	FRAME_attak315,
	FRAME_attak316,
	FRAME_attak317,
	FRAME_attak318,
	FRAME_attak319,
	FRAME_attak320,
	FRAME_attak321,
	FRAME_attak322,
	FRAME_fend01,
	FRAME_fend02,
	FRAME_fend03,
	FRAME_fend04,
	FRAME_fend05,
	FRAME_fend06,
	FRAME_fend07,
	FRAME_fend08,
	FRAME_fend09,
	FRAME_fend10,
	FRAME_fend11,
	FRAME_fend12,
	FRAME_fend13,
	FRAME_fend14,
	FRAME_fend15,
	FRAME_pain101,
	FRAME_pain102,
	FRAME_pain103,
	FRAME_pain104,
	FRAME_pain201,
	FRAME_pain202,
	FRAME_pain203,
	FRAME_pain204,
	FRAME_pain205,
	FRAME_pain206,
	FRAME_pain301,
	FRAME_pain302,
	FRAME_pain303,
	FRAME_pain304,
	FRAME_pain305,
	FRAME_pain306,
	FRAME_pain307,
	FRAME_pain308,
	FRAME_pain309,
	FRAME_pain310,
	FRAME_pain311,
	FRAME_pain312,
	FRAME_pain313,
	FRAME_pain314,
	FRAME_pain315,
	FRAME_pain316,
	FRAME_pain317,
	FRAME_pain318,
	FRAME_pain319,
	FRAME_pain320,
	FRAME_pain321,
	FRAME_death101,
	FRAME_death102,
	FRAME_death103,
	FRAME_death104,
	FRAME_death105,
	FRAME_death106,
	FRAME_death107,
	FRAME_death108,
	FRAME_death109,
	FRAME_death110,
	FRAME_death111,
	FRAME_death112,
	FRAME_death113,
	FRAME_death114,
	FRAME_death115,
	FRAME_death116,
	FRAME_death117,
	FRAME_death118,
	FRAME_death119,
	FRAME_death120,
	FRAME_death121,
	FRAME_death122,
	FRAME_death123,
	FRAME_death124,
	FRAME_death125,
	FRAME_death126,
	FRAME_death127,
	FRAME_death128,
	FRAME_death201,
	FRAME_death202,
	FRAME_death203,
	FRAME_death204,
	FRAME_death205,
	FRAME_death206,
	FRAME_death207,
	FRAME_death208,
	FRAME_death209,
	FRAME_death210,
	FRAME_death211,
	FRAME_death212,
	FRAME_death213,
	FRAME_death214,
	FRAME_death215,
	FRAME_death216,
	FRAME_death217,
	FRAME_death218,
	FRAME_death219,
	FRAME_death220,
	FRAME_death221,
	FRAME_death222,
	FRAME_death223,
	FRAME_death224,
	FRAME_death225,
	FRAME_death226,
	FRAME_death227,
	FRAME_death228,
	FRAME_death229,
};

constexpr float MODEL_SCALE = 1.000000f;

/*
#define FRAME_stand1start	0
#define FRAME_stand1end		28
#define FRAME_stand2start	29
#define FRAME_stand2end		48
#define FRAME_stand3start	49
#define FRAME_stand3end		79
#define FRAME_walkStartStart	80
#define FRAME_walkStartEnd	87
#define FRAME_walkLoopStart	88
#define FRAME_walkLoopEnd	103
#define FRAME_walkEndStart	104
#define FRAME_walkEndEnd	111

#define FRAME_blastPreStart	112
#define FRAME_blastPreEnd	115
#define FRAME_blastStart	116
#define FRAME_blastEnd		121
#define FRAME_blastPostStart	122
#define FRAME_blastPostEnd	125
#define FRAME_laserPreStart	126
#define FRAME_laserPreEnd	130
#define FRAME_laserStart	131
#define FRAME_laserEnd		141
#define FRAME_laserPostStart	142
#define FRAME_laserPostEnd	145
#define FRAME_slashPreStart   146
#define FRAME_slashPreEnd     150
#define FRAME_slashStart      151
#define FRAME_slashEnd        154
#define FRAME_slashPostStart  155
#define FRAME_slashPostEnd    167

#define FRAME_dodgeStart	168
#define FRAME_dodgeHold		173
#define FRAME_dodgeEnd		182
#define FRAME_pain1Start	183
#define FRAME_pain1End		186
#define FRAME_pain2Start	187
#define FRAME_pain2End		192
#define FRAME_pain3Start	193
#define FRAME_pain3End		213
#define FRAME_die1Start		214
#define FRAME_die1End		241
#define FRAME_die2Start		242
#define FRAME_die2End		270
*/