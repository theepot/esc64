#ifndef TANKY_INCLUDED
#define TANKY_INCLUDED

#include <stdint.h>

//terminal control macro's
#define TERM_CURSOR_INVIS		(printf("\x1B[?25l"))
#define TERM_CURSOR_VIS			(printf("\x1B[?25h"))
#define TERM_SETCURSOR(x, y)	(printf("\x1B[%d;%dH", (y), (x)))
#define TERM_DRAWCH(x, y, ch)	(printf("\x1B[%d;%dH%c", (y), (x), (ch)))
#define TERM_DRAWSTR(x, y, s)	(printf("\x1B[%d;%dH%s", (y), (x), (s)))

//graphic positions
#define SCREEN_X		1
#define SCREEN_Y		1

#define FIELD_X			(SCREEN_X + 1)
#define FIELD_Y			(SCREEN_Y + 1)
#define FIELD_HEIGHT	25
#define FIELD_WIDTH		80

#define INFO_X			FIELD_X
#define INFO_Y			(FIELD_Y + FIELD_HEIGHT + 1)
#define INFO_WIDTH		FIELD_WIDTH
#define INFO_HEIGHT		4

#define STATS_WIDTH		15
#define STATS_HEIGHT	INFO_HEIGHT

#define STATS_NAME		0
#define STATS_HULL		(STATS_NAME + 1)
#define STATS_POWER		(STATS_HULL + 1)
#define STATS_ANGLE		(STATS_POWER + 1)

#define STATS_INSET		7

#define MSG_X			(INFO_X + (STATS_WIDTH + 1) * 2)
#define MSG_Y			INFO_Y
#define MSG_WIDTH		(INFO_WIDTH - (STATS_WIDTH + 1) * 2)
#define MSG_HEIGHT		INFO_HEIGHT

#define SCREEN_WIDTH	(FIELD_WIDTH + 2)
#define SCREEN_HEIGHT	(FIELD_HEIGHT + 1 + INFO_HEIGHT + 1)

//units per character-cell
#define UNITS_X_SHIFT	8
#define UNITS_Y_SHIFT	9

#define UNITS_X			(1 << UNITS_X_SHIFT)
#define UNITS_Y			(1 << UNITS_Y_SHIFT)

//some game constants
#define SHOT_MAX_SPEED	UNITS_X
#define GRAVITY			3
#define SHOT_DAMAGE		34

#ifdef TARGET_ESC64
#define SHOT_DELAY		10
#else
#define SHOT_DELAY		40
#endif

//line draw types
#define LINE_HOR	0
#define LINE_VERT	1

//collision types
#define COLLISION_BORDER	1
#define COLLISION_PLAYER	2
#define COLLISION_TERRAIN	3

//types
typedef struct Vector_
{
	int16_t x, y;
} Vector;

typedef struct Player_
{
	char id;
	int16_t hull, power, angle;
	Vector pos;
	Vector stats;
} Player;

typedef struct Shot_
{
	Vector pos, velo;
} Shot;

extern Vector angle2vec[91];

//macro's
#define CLEAR_MSG	(fillrect(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT, ' '))

#ifdef TARGET_ESC64
#define delayms(ms)		delay((ms), 24)
#define printch(ch)		io_out(SERIAL_IO_DEV, (ch))
#define printstr(str)	io_outs(SERIAL_IO_DEV, (str))
#else
#define delayms(ms)		usleep((ms) * 1000)
#define printch(ch)		putchar(ch)
#define printstr(str)	fputs((str), stdout)
#endif

//routines
void draw_static(void);
void gen_terrain(void);
void init_player(Player* player);
void draw_stats(Player* player);
int16_t player_input(void);
void shoot(void);
void draw_line(int16_t x, int16_t y, int16_t len, const char* str, uint16_t type);
void init_shot(void);
int16_t clamp(int16_t v, int16_t a, int16_t b);
void draw_shot(int16_t ch);
//void draw_error(const char* msg);
int16_t shot_collision(void);
void draw_player(const Player* player);
void place_player(Player* player, int16_t x);
void draw_terrain(void);
int16_t player_hit(Player* player, int16_t x, int16_t y);
void explosion(int16_t x, int16_t y);
void fillrect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t ch);

#endif
