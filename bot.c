#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h> 

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define VIEW_RANGE 2

#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_RIGHT 3
#define DIR_LEFT 4

struct player_t
{
	char mapa[35];
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
};
void Print_Info(struct player_t gracz)
{
	mvprintw(0,20, "Server's PID: %d", 324); // do zrobienia
	mvprintw(1,21, "Campsite X/Y: %d/%d", gracz.base_x, gracz.base_y);
	mvprintw(2,21, "Round number = %d", 1);
	mvprintw(4,20, "Player: ");
	mvprintw(5,21, "Number: %d", gracz.number);
	mvprintw(6,21, "Type: HUMAN");
	mvprintw(7,21, "                 ");
	mvprintw(7,21, "Curr X/Y %d/%d", gracz.x, gracz.y);
	mvprintw(8,21, "Deaths: %d", gracz.deaths);
	mvprintw(10,21, "                       ");
	mvprintw(10,21, "Coins found %d", gracz.money);
	mvprintw(11,21, "Coins brought %d", gracz.money_in_base);
	
	mvprintw(15, 20, "Legend: ");
	
    attron(COLOR_PAIR(1));
    mvprintw(16,21,"1234");
    attroff(COLOR_PAIR(1)); 
	mvprintw(16, 26, "- players ");
	
	mvaddch(17,21,ACS_CKBOARD);
	mvprintw(17, 22, " - wall");
	
	mvprintw(18, 21, "# - bushes (slow down)");
	attron(COLOR_PAIR(4));
	mvprintw(19, 21, "*");
	attroff(COLOR_PAIR(4));
	mvprintw(19, 23," - wild beast");
	attron(COLOR_PAIR(2));
	mvprintw(20,21,"c");
	mvprintw(20,39, "D");
	attroff(COLOR_PAIR(2));
	mvprintw(20, 23, "- one coin");
	mvprintw(20, 41, "- dropped treasure");
	attron(COLOR_PAIR(2));
	mvprintw(21,21,"t");
	mvprintw(22,21,"T");
	attroff(COLOR_PAIR(2));
	mvprintw(21, 23, "- treasue (10 coins)");
	mvprintw(22, 23, "- large treasure (50 coins)");
	attron(COLOR_PAIR(3));
	mvprintw(23,21,"A");
	attroff(COLOR_PAIR(3));
	mvprintw(23, 23, "- campsite");
	refresh();
}

void Player_View_Frame()
{
	attron(A_REVERSE);
	for(int i = 0; i< 7; i++)
	{
		mvprintw(0, i, " ");
	}
	for(int i = 1; i<6; i++)
	{
		mvprintw(i, 0, " ");
		mvprintw(i, 6, " ");
	}
	for(int i = 0; i< 7; i++)
	{
		mvprintw(6, i, " ");
	}
	attroff(A_REVERSE);
	refresh();
}
void Print_Player_View(struct player_t player)
{
	int counter = 0;
	Player_View_Frame();
	for(int i = 0; i <= VIEW_RANGE*2;i++)
	{
		for(int j = 0; j<= VIEW_RANGE*2; j++)
		{
			if(player.mapa[counter] == '@')
			{
				mvaddch(i+1,j+1,ACS_CKBOARD);
			}
			else if(player.mapa[counter] == 'c' || player.mapa[counter] == 't' || player.mapa[counter] == 'T' || player.mapa[counter] == 'D')
			{
				attron(COLOR_PAIR(2));
				mvprintw(i+1,j+1,"%c", player.mapa[counter]);
				attroff(COLOR_PAIR(2));
			}
			else if(player.mapa[counter] == 'A')
			{
				attron(COLOR_PAIR(3));
				mvprintw(i+1,j+1,"%c", player.mapa[counter]);
				attroff(COLOR_PAIR(3));
			}
			else if(player.mapa[counter] == '*')
			{
				attron(COLOR_PAIR(4));
				mvprintw(i+1,j+1,"%c", player.mapa[counter]);
				attroff(COLOR_PAIR(4));
			}
			else if(player.mapa[counter] == '1' || player.mapa[counter] == '2' || player.mapa[counter] == '3' || player.mapa[counter] == '4')
			{
				attron(COLOR_PAIR(1));
				mvprintw(i+1,j+1,"%c", player.mapa[counter]);
				attroff(COLOR_PAIR(1));
			}
			else
			{
				mvprintw(i+1, j+1, "%c", player.mapa[counter]);
			}	
			counter++;
		}
		counter++;
	}
	attron(COLOR_PAIR(1));
	mvprintw(3,3, "%c", player.number + '0');
	attroff(COLOR_PAIR(1));
/*
 * // func returnign move which bot should go when see a beast
 */
}
int Check_Area(struct player_t gracz)
{
	
	for(int i =gracz.y; i<= gracz.y+VIEW_RANGE;i++)
	{
		if(i >= 0 && i < HEIGHT)
		{
			if(gracz.mapa[i][gracz.x] == '*')
			{
				return DIR_DOWN;
			}
			else if(gracz.mapa[i][gracz.x] == '@')
			{
				break;
			}
		}
	}
	
	for(int i = gracz.y-1;i >= gracz.y-VIEW_RANGE;i--)
	{
		if(i >= 0 && i < HEIGHT)
		{
			if(gracz.mapa[i][gracz.x] == '*')
			{
				return DIR_UP;
			}
			if(gracz.mapa[i][gracz.x] == '@')
			{
				break;
			}
		}
	}
	
	for(int i = gracz.x; i<= gracz.x+VIEW_RANGE;i++)
	{
		if(i >= 0)
		{
			if(gracz.mapa[i] == '*')
			{
				return DIR_LEFT;
			}
			if(gracz.mapa[gracz.y][i] == '@')
			{
				break;
			}
		}
	}
	
	for(int i = gracz.x-1;i >= gracz.x-VIEW_RANGE;i--)
	{
		if(i >= 0)
		{
			if(gracz.mapa[gracz.y][i] == '*')
			{
				return DIR_RIGHT;
			}
			if(gracz.mapa[gracz.y][i] == '@')
			{
				break;
			}
		}
	}
	return -1;
}
int move_generator()
{
	int move = (rand() % 4);
	if(move == 0)
	{
		return 258;
	}
	else if(move == 1)
	{
		return 259;
	}
	else if(move == 2)
	{
		return 260;
	}
	else if(move == 3)
	{
		return 261;
	}
	return 0;
}


int main(int argc, char const* argv[])
{
	int game_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = 7940;
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	int connection_status = connect(game_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	if(connection_status == -1)
	{
		printf("Error when making a connection to socket\n\n");
		return -1;
	}
	
	struct player_t player;

	initscr();
    if(has_colors() == FALSE)
	{
		printf("You won't be able to see colors because ur terminal does not support colors!\n");
	}
	
	keypad(stdscr, TRUE);
	use_default_colors();
	
	start_color();
	init_pair( 1, COLOR_WHITE, COLOR_MAGENTA );
	init_pair( 2, COLOR_BLACK, COLOR_YELLOW );
	init_pair( 3, COLOR_YELLOW, COLOR_GREEN );
	init_pair( 4, COLOR_RED, COLOR_BLACK );
	noecho();
	curs_set(0);
	int znak;
	while(1)
	{
		recv(game_socket, &player, sizeof(struct player_t), 0);
		Print_Player_View(player);
		Print_Info(player);
		refresh();
		int beast_in_sight = Check_Area(player);
		switch(beast_in_sight)
		{
			case -1:
				znak = move_generator();
				break;
			case DIR_UP:
				znak = 259;
				break;
			case DIR_DOWN:
				znak = 258;
				break;
			case DIR_LEFT:
				znak = 260;
				break;
			case DIR_RIGHT:
				znak = 261;
				break;
		}
		send(game_socket, &znak, sizeof(int), 0);
	}
	endwin();
	close(game_socket);
	printf("\n\nzamknalem soketa i spierdalalm\n");
    return 0;
}
