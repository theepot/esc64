#ifndef TARGET_ESC64
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#else
#include <stdesc.h>
#endif

#include <tanky.h>
#include <utf8codes.h>

static int16_t terrain[FIELD_WIDTH];

static Player player1 =
{
	'1',
	100, 100, 45,
	{ 16, 0 },
	{ INFO_X, INFO_Y }
};

static Player player2 =
{
	'2',
	100, 100, 45,
	{ 60, 0 },
	{ INFO_X + STATS_WIDTH + 1, INFO_Y }
};

static Player* curPlayer;

static Shot shot;

int main(int argc, char** argv)
{
#ifndef TARGET_ESC64
	//disable line-buffering on stdin
	struct termios old_tio, new_tio;
	assert(!tcgetattr(STDIN_FILENO, &old_tio));
	new_tio = old_tio;
	new_tio.c_lflag &= (~ICANON & ~ECHO);
	assert(!tcsetattr(STDIN_FILENO, TCSANOW, &new_tio));
#endif

	for(;;)
	{
		fputs("\x1B[2J\x1B[H\x1B[?25l", stdout); //clear screen, cursor to home, cursor invis
		player1.pos.x = 16;
		player2.pos.x = 60;
		
		//initialize / generate
		gen_terrain();
		init_player(&player1);
		init_player(&player2);
		curPlayer = &player1;
		
		//draw
		draw_static();
		draw_terrain();
		draw_player(&player1);
		draw_player(&player2);
		draw_stats(&player1);
		draw_stats(&player2);
		
		while(player1.hull > 0 && player2.hull > 0)
		{		
			if(player_input())	{ break; }
			TERM_DRAWSTR(MSG_X, MSG_Y + 1, "shot fired");
			shoot();
			
			curPlayer = curPlayer == &player1 ? &player2 : &player1;
		}
	}

#ifndef TARGET_ESC64
	//restore old settings line-buffering settings
	tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
#endif
	
	return 0;
}

void draw_static(void)
{
	static Vector corners[] =
	{
		{ SCREEN_X, SCREEN_Y },
		{ SCREEN_X, SCREEN_Y + FIELD_HEIGHT+ 1 },
		{ SCREEN_X, SCREEN_Y + SCREEN_HEIGHT },
		
		{ SCREEN_X + SCREEN_WIDTH - 1, SCREEN_Y },
		{ SCREEN_X + SCREEN_WIDTH - 1, SCREEN_Y + FIELD_HEIGHT + 1 },
		{ SCREEN_X + SCREEN_WIDTH - 1, SCREEN_Y + SCREEN_HEIGHT },
	};
	
	Vector* p;

	//horizontal lines
	draw_line(SCREEN_X + 1, SCREEN_Y, SCREEN_WIDTH - 2, UTF8_MENU_HOR, LINE_HOR);
	draw_line(SCREEN_X + 1, SCREEN_Y + FIELD_HEIGHT + 1, SCREEN_WIDTH - 2, UTF8_MENU_HOR, LINE_HOR);
	draw_line(SCREEN_X + 1, SCREEN_Y + FIELD_HEIGHT + INFO_HEIGHT + 2, SCREEN_WIDTH - 2, UTF8_MENU_HOR, LINE_HOR);
	
	//vertical lines
	draw_line(SCREEN_X, SCREEN_Y + 1, SCREEN_HEIGHT - 1, UTF8_MENU_VERT, LINE_VERT);
	draw_line(SCREEN_X + SCREEN_WIDTH - 1, SCREEN_Y + 1, SCREEN_HEIGHT - 1, UTF8_MENU_VERT, LINE_VERT);
	
	//corners
	for(p = corners; p < corners + (sizeof corners / sizeof corners[0]); ++p)
	{
		printf("\x1B[%d;%dH%s", p->y, p->x, UTF8_MENU_CROSS);
	}
	
	//menu stuff
	/*TERM_DRAWSTR(INFO_X, INFO_Y + STATS_NAME, "player 1");
	TERM_DRAWSTR(INFO_X, INFO_Y + STATS_HULL, "hull:");
	TERM_DRAWSTR(INFO_X, INFO_Y + STATS_POWER, "power:");
	TERM_DRAWSTR(INFO_X, INFO_Y + STATS_ANGLE, "angle:");*/
}

void gen_terrain(void)
{
	terrain[0] = 11 + 10;
	terrain[1] = 12 + 10;
	terrain[2] = 13 + 10;
	terrain[3] = 10 + 10;
	terrain[4] = 8 + 10;
	terrain[5] = 7 + 10;
	terrain[6] = 6 + 10;
	terrain[7] = 6 + 10;
	terrain[8] = 6 + 10;
	terrain[9] = 6 + 10;
	terrain[10] = 6 + 10;
	terrain[11] = 6 + 10;
	terrain[12] = 6 + 10;
	terrain[13] = 7 + 10;
	terrain[14] = 8 + 10;
	terrain[15] = 10 + 10;
	terrain[16] = 11 + 10;
	terrain[17] = 11 + 10;
	terrain[18] = 12 + 10;
	terrain[19] = 12 + 10;
	terrain[20] = 13 + 10;
	terrain[21] = 14 + 10;
	terrain[22] = 11 + 10;
	terrain[23] = 9 + 10;
	terrain[24] = 8 + 10;
	terrain[25] = 8 + 10;
	terrain[26] = 8 + 10;
	terrain[27] = 8 + 10;
	terrain[28] = 8 + 10;
	terrain[29] = 8 + 10;
	terrain[30] = 8 + 10;
	terrain[31] = 9 + 10;
	terrain[32] = 9 + 10;
	terrain[33] = 9 + 10;
	terrain[34] = 9 + 10;
	terrain[35] = 9 + 10;
	terrain[36] = 11 + 10;
	terrain[37] = 13 + 10;
	terrain[38] = 11 + 10;
	terrain[39] = 11 + 10;
	terrain[40] = 10 + 10;
	terrain[41] = 9 + 10;
	terrain[42] = 8 + 10;
	terrain[43] = 8 + 10;
	terrain[44] = 8 + 10;
	terrain[45] = 10 + 10;
	terrain[46] = 11 + 10;
	terrain[47] = 11 + 10;
	terrain[48] = 12 + 10;
	terrain[49] = 12 + 10;
	terrain[50] = 8 + 10;
	terrain[51] = 9 + 10;
	terrain[52] = 9 + 10;
	terrain[53] = 9 + 10;
	terrain[54] = 9 + 10;
	terrain[55] = 9 + 10;
	terrain[56] = 11 + 10;
	terrain[57] = 13 + 10;
	terrain[58] = 11 + 10;
	terrain[59] = 11 + 10;
	terrain[60] = 11 + 10;
	terrain[61] = 12 + 10;
	terrain[62] = 13 + 10;
	terrain[63] = 10 + 10;
	terrain[64] = 8 + 10;
	terrain[65] = 7 + 10;
	terrain[66] = 6 + 10;
	terrain[67] = 6 + 10;
	terrain[68] = 6 + 10;
	terrain[69] = 6 + 10;	
	terrain[70] = 11 + 10;
	terrain[71] = 13 + 10;
	terrain[72] = 11 + 10;
	terrain[73] = 11 + 10;
	terrain[74] = 10 + 10;
	terrain[75] = 9 + 10;
	terrain[76] = 8 + 10;
	terrain[77] = 8 + 10;
	terrain[78] = 8 + 10;
	terrain[79] = 10 + 10;
}

void init_player(Player* player)
{
	player->hull = 100;
	player->power = 100;
	player->angle = 45;
	place_player(player, player->pos.x); //TODO random position
}

void draw_stats(Player* player)
{
	int16_t x, y;
	x = player->stats.x;
	y = player->stats.y;
	
	fillrect(x, y, STATS_WIDTH, STATS_HEIGHT, ' ');
	printf(
		"\x1B[%d;%dHplayer %c"
		"\x1B[%d;%dHhull:  %d%%"
		"\x1B[%d;%dHpower: %d%%"
		"\x1B[%d;%dHangle: %d" UTF8_DEGREES,
		y, x, player->id,
		y + STATS_HULL, x, player->hull,
		y + STATS_POWER, x, player->power,
		y + STATS_ANGLE, x, player->angle);
		
}

int16_t player_input(void)
{
	int16_t ch;
	
	TERM_DRAWSTR(MSG_X, MSG_Y, "press <ENTER> to start your turn");
	while(getchar() != '\n');
	CLEAR_MSG;
	
	for(;;)
	{
		ch = getchar();
		switch(ch)
		{
		case 'w':
			++curPlayer->angle;
			break;
		case 's':
			--curPlayer->angle;
			break;
		case 'd':
			++curPlayer->power;
			break;
		case 'a':
			--curPlayer->power;
			break;
		
		case ' ':	return 0;
		case 'q':	return -1;
		default:	break;
		}
		
		curPlayer->angle = clamp(curPlayer->angle, -90, 90);
		curPlayer->power = clamp(curPlayer->power, 0, 100);
		draw_stats(curPlayer);
	}
}

void shoot(void)
{
	init_shot();
	while(!shot_collision())
	{
		//clear last shot graphic
		draw_shot(1);
		
		//calculate effect of gravity and wind
		shot.velo.y += GRAVITY;
		if(shot.velo.y > SHOT_MAX_SPEED)	{ shot.velo.y = SHOT_MAX_SPEED; }
		//TODO wind
		
		//apply velocity
		shot.pos.x += shot.velo.x;
		shot.pos.y += shot.velo.y;
		
		//draw shot
		draw_shot(0);
		
#ifndef TARGET_ESC64
		fflush(stdout);
#endif
		
		//delay
		delayms(SHOT_DELAY);
	}
}

void draw_line(int16_t x, int16_t y, int16_t len, const char* str, uint16_t type)
{
	int16_t* d, limit;
	d = type == LINE_HOR ? &x : &y;
	limit = *d + len;
	
	while(*d < limit)
	{
		TERM_DRAWSTR(x, y, str);
		++(*d);
	}
}

void init_shot(void)
{	
	int16_t pow;

	if(curPlayer->angle < 0)
	{
		shot.velo = angle2vec[-curPlayer->angle];
	}
	else
	{
		shot.velo = angle2vec[curPlayer->angle];
		shot.velo.x = -shot.velo.x;
	}
		
	pow = curPlayer->power;
	if(pow == 0)	{ pow = 1; };
	
	shot.velo.x = (shot.velo.x * pow) / 100;
	shot.velo.y = (shot.velo.y * pow) / 100;
		
	shot.pos.x = (curPlayer->pos.x + 1) << UNITS_X_SHIFT;
	shot.pos.y = (curPlayer->pos.y - 1) << UNITS_Y_SHIFT;
}

int16_t clamp(int16_t v, int16_t a, int16_t b)
{
	if(v < a)		{ v = a; }
	else if(v > b)	{ v = b; }
	return v;
}

void draw_shot(int16_t clear)
{
	int16_t x, y;
	const char* str;
	
	x = shot.pos.x >> UNITS_X_SHIFT;
	y = shot.pos.y >> UNITS_Y_SHIFT;
	
	if(	x < FIELD_X || x >= FIELD_X + FIELD_WIDTH
	||	y < FIELD_Y || y >= FIELD_Y + FIELD_HEIGHT)
	{
		return;
	}
	
	if(clear)
	{
		TERM_DRAWCH(x, y, ' ');
	}
	else
	{
		//str = shot.pos.y % UNITS_Y < UNITS_X ? UTF8_UPPER_HALF_BLOCK : UTF8_LOWER_HALF_BLOCK;
		str = (shot.pos.y & (UNITS_Y - 1)) < UNITS_X ? UTF8_UPPER_HALF_BLOCK : UTF8_LOWER_HALF_BLOCK;
		TERM_DRAWSTR(x, y, str);
	}
	
}

/*void draw_error(const char* msg)
{
	TERM_DRAWSTR(1, 1, msg);
	fflush(stdout);
	usleep(2 * 1000000);
}*/

int16_t shot_collision(void)
{
	int16_t x, y;
	x = shot.pos.x >> UNITS_X_SHIFT;
	y = shot.pos.y >> UNITS_Y_SHIFT;
	
	//bottom, left, right
	if(	y >= FIELD_Y + FIELD_HEIGHT
	||	x < FIELD_X || x >= FIELD_X + FIELD_WIDTH)
	{
		return 1;
	}
	
	//players
	if(player_hit(&player1, x, y) || player_hit(&player2, x, y))	{ return 1; }
	
	//terrain
	if(y >= terrain[x - FIELD_X])
	{
		explosion(x, y);
		//TODO update terrain
		return 1;
	}
	
	return 0;
}

void draw_player(const Player* player)
{
	printf(
		"\x1B[%d;%dH"
		UTF8_TRIANGLE_SE UTF8_LOWER_HALF_BLOCK UTF8_TRIANGLE_SW
		"\x1B[%d;%dH"
		UTF8_FULL_BLOCK "%c" UTF8_FULL_BLOCK,
		player->pos.y, player->pos.x, player->pos.y + 1, player->pos.x, player->id);
}

void place_player(Player* player, int16_t x)
{
	int16_t t;
	
	player->pos.x = x + FIELD_X;
	t = terrain[x];
	terrain[++x] = t;
	terrain[++x] = t;
	player->pos.y = t - 2;
}

void draw_terrain(void)
{
	int16_t i;
	for(i = 0; i < sizeof terrain / sizeof terrain[0]; ++i)
	{
		int16_t x, y, len;
		x = FIELD_X + i;
		y = terrain[i];
		len = FIELD_Y + FIELD_HEIGHT - y;
		draw_line(x, y, len, "#", LINE_VERT);
	}
}

int16_t player_hit(Player* player, int16_t x, int16_t y)
{
	if(	x < player->pos.x || x >= player->pos.x + 3
	||	y < player->pos.y || y >= player->pos.y + 2)	{ return 0; }
	
	explosion(x, y);
	draw_player(player);
	player->hull -= SHOT_DAMAGE;
	draw_stats(player);
	printf("\x1B[%d;%dHhit player %c!", MSG_Y + 1, MSG_X, player->id);
	return 1;
}

void explosion(int16_t x, int16_t y)
{
	TERM_DRAWCH(x, y, 'X');
}

void fillrect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t ch)
{
	int16_t i;
	h += y;
	
	for(; y < h; ++y)
	{
		printf("\x1B[%d;%dH", y, x);
		for(i = 0; i < w; ++i)
		{
			putchar(ch);
		}
	}
}











