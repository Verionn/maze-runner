#ifndef GRA_PLAYER_H
#define GRA_PLAYER_H

#define VIEW_RANGE 2

struct player_t
{
	char mapa[35];
	int server_pid;
	unsigned int x;
	unsigned int y;
	unsigned int bush;
	unsigned int money;
	unsigned int money_in_base;
	unsigned int deaths;
	unsigned int base_x;
	unsigned int base_y;
	unsigned int number;
	unsigned int is_active;
	unsigned int round;
	char type[6];
};

void Print_Info(struct player_t gracz);
void Player_View_Frame();
void Print_Player_View(struct player_t player);

#endif

