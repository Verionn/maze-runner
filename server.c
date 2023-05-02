#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h> 
#include <semaphore.h>
#include <fcntl.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <ncurses.h>

#include "server.h"

struct player_t player[MAX_PLAYERS];
struct server_t server;
struct beast_t bestia[MAX_BEASTS];  


int server_socket[MAX_PLAYERS+2];
int moves[MAX_PLAYERS];
int beast_moves[MAX_BEASTS];
char **mapa;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void print_msg_string(char* s)
{
	FILE *wsk = fopen("/dev/pts/3", "wr");
	fprintf(wsk, "%s", s);
	fclose(wsk);
}
void print_msg_int(int liczba)
{
	FILE *wsk = fopen("/dev/pts/3", "wr");
	fprintf(wsk, "%d", liczba);
	fclose(wsk);
}
void print_msg_char(char znak)
{
	FILE *wsk = fopen("/dev/pts/3", "wr");
	fprintf(wsk, "%c", znak);
	fclose(wsk);
}
void Clear_Sockets()
{
	for(int i =0; i<4; i++)
	{
		server_socket[i+1] = 0;
	}
}
void Clear_Moves()
{
	for(int i =0; i<4; i++)
	{
		moves[i] = 0;
	}
}
void Clear_Beast_moves()
{
	for(int i = 0; i<server.beasts; i++)
	{
		beast_moves[i] = 0;
	}
}
void Print_Alive_Objects()
{
	attron(COLOR_PAIR(1));
	for(int i = 0; i<MAX_PLAYERS;i++)
	{
		if(player[i].is_active)
		{
			mvprintw(player[i].y, player[i].x, "%c", player[i].number+'0');	
		}
	}
	attroff(COLOR_PAIR(1));
	
	attron(COLOR_PAIR(4));
	for(int i = 0; i<server.beasts;i++)
	{
		mvprintw(bestia[i].y, bestia[i].x, "*");
	}
	attroff(COLOR_PAIR(4));
	refresh();
}
void Print_Server_Stats()
{
	refresh();
	mvprintw(0,WIDTH+2, "Server's PID: %d", getpid());
	mvprintw(1,WIDTH+3, "Campsite X/Y: %d/%d", 11, 25);
	mvprintw(2,WIDTH+3, "Round number = %d", server.round);
	
	mvprintw(4, WIDTH+2, "Parameter: Player1 Player2 Player3 Player4" );
	mvprintw(5, WIDTH+3, "PID: %d %d %d %d", player[0].number,player[1].number,player[2].number,player[3].number);
	mvprintw(6, WIDTH+3, "                                               ");
	mvprintw(6, WIDTH+3, "TYPE: %s %s %s %s", player[0].type, player[1].type, player[2].type, player[3].type);
	mvprintw(7, WIDTH+3, "                                               ");
	mvprintw(7, WIDTH+3, "Curr X/Y: %d/%d %d/%d %d/%d %d/%d", player[0].x, player[0].y,player[1].x, player[1].y,player[2].x, player[2].y,player[3].x, player[3].y);
	mvprintw(8, WIDTH+3, "Deaths: %d %d %d %d",player[0].deaths,player[1].deaths,player[2].deaths,player[3].deaths);
	
	mvprintw(10, WIDTH+2, "Coins");
	mvprintw(11, WIDTH+5, "                                    ");
	mvprintw(11, WIDTH+5, "carried: %d %d %d %d", player[0].money,player[1].money,player[2].money,player[3].money);
	mvprintw(12, WIDTH+5, "brought: %d %d %d %d", player[0].money_in_base,player[1].money_in_base,player[2].money_in_base,player[3].money_in_base);
	
	mvprintw(15, WIDTH+2, "Legend:");
    attron(COLOR_PAIR(1));
    mvprintw(16,WIDTH+3,"1234");
    attroff(COLOR_PAIR(1)); 
	mvprintw(16, WIDTH+8, "- players ");
	
	mvaddch(17,WIDTH+3,ACS_CKBOARD);
	mvprintw(17, WIDTH+4, " - wall");
	
	mvprintw(18, WIDTH+3, "# - bushes (slow down)");
	
	attron(COLOR_PAIR(4));
	mvprintw(19, WIDTH+3, "*");
	attroff(COLOR_PAIR(4));
	mvprintw(19, WIDTH+5," - wild beast");
	
	attron(COLOR_PAIR(2));
	mvprintw(20,WIDTH+3,"c");
	mvprintw(20,WIDTH+21, "D");
	attroff(COLOR_PAIR(2));
	mvprintw(20, WIDTH+5, "- one coin");
	mvprintw(20, WIDTH+23, "- dropped treasure");
	
	attron(COLOR_PAIR(2));
	mvprintw(21,WIDTH+3,"t");
	mvprintw(22,WIDTH+3,"T");
	attroff(COLOR_PAIR(2));
	mvprintw(21, WIDTH+5, "- treasue (10 coins)");
	mvprintw(22, WIDTH+5, "- large treasure (50 coins)");	
	
	attron(COLOR_PAIR(3));
	mvprintw(23,WIDTH+3,"A");
	attroff(COLOR_PAIR(3));
	mvprintw(23, WIDTH+5, "- campsite");
	refresh();
	
}
void Print_Server_Map()
{
	initscr();
    if(has_colors() == FALSE)
	{
		printf("You won't be able to see colors because ur terminal does not support colors!\n");
	}
	noecho();
	use_default_colors();
	start_color();
	init_pair( 1, COLOR_WHITE, COLOR_MAGENTA );
	init_pair( 2, COLOR_BLACK, COLOR_YELLOW );
	init_pair( 3, COLOR_YELLOW, COLOR_GREEN );
	init_pair( 4, COLOR_RED, COLOR_BLACK );
	curs_set(0);
	for(int i =0; i<HEIGHT; i++)
	{
		for(int j =0; j<WIDTH; j++)
		{
			if(mapa[i][j] == '@')
			{
				mvaddch(i,j,ACS_CKBOARD);
			}
			else if(mapa[i][j] == 'c' || mapa[i][j] == 't' || mapa[i][j] == 'T' || mapa[i][j] == 'D')
			{
				attron(COLOR_PAIR(2));
				mvprintw(i,j,"%c", mapa[i][j]);
				attroff(COLOR_PAIR(2));
			}
			else if(mapa[i][j] == 'A')
			{
				attron(COLOR_PAIR(3));
				mvprintw(i,j,"%c", mapa[i][j]);
				attroff(COLOR_PAIR(3));
			}
			else
			{
				mvprintw(i,j,"%c", mapa[i][j]);
			}
		}
	}
	Print_Server_Stats();
	refresh();
	return;
}
struct player_t Create_Player(int number)
{
	struct player_t gracz;
	while(1)
	{
		int x = (rand() % 51);
		int y = (rand() % 25);
		if(mapa[y][x] != '@' && mapa[y][x] != '#' && mapa[y][x] != 't' && mapa[y][x] != 'T' && mapa[y][x] != 'c')
		{
			refresh();
			gracz.x = x;
			gracz.y = y;
			gracz.money = 0;
			gracz.money_in_base = 0;
			gracz.deaths = 0;
			gracz.bush = 0;
			gracz.base_x = x;
			gracz.base_y = y;
			gracz.number = number;
			gracz.is_active = 1;
			gracz.server_pid = getpid();
			break;
		}
	}
	return gracz;
	
}
void Player_View(int x, int y, char *player_pos)
{
	memset(player_pos, 0, 35);
	int counter = 0;
	
	for(int i = y-VIEW_RANGE;i <= y+VIEW_RANGE;i++)
	{
		for(int j =x-VIEW_RANGE;j<=x+VIEW_RANGE; j++)
		{
			if(i >= 0 && j >= 0 && i < HEIGHT && j< WIDTH)
			{
				player_pos[counter] = mapa[i][j];
				counter++;
			}
			else
			{
				player_pos[counter] = ' ';
				counter++;
			}
			for(int k = 0; k< MAX_PLAYERS; k++)
			{
				if(player[k].x == j && player[k].y == i)
				{
					player_pos[counter-1] = player[k].number+'0';
					break;
				}
			}
			for(int k = 0; k< server.beasts; k++)
			{
				if(bestia[k].x == j && bestia[k].y == i)
				{
					player_pos[counter-1] = '*';
					break;
				}
			}
			
			
		}
		player_pos[counter] = '\n';
		counter++;
	}
	return;
}
void Move_Beast(struct beast_t* bestia, int move)
{
	if(move == KEY_UP && mapa[bestia->y-1][bestia->x] != '@')
	{
		bestia->y--;
		Print_Server_Map();
		Print_Alive_Objects();
		attron(COLOR_PAIR(4));
		mvprintw(bestia->y,bestia->x,"*");
		attroff(COLOR_PAIR(4));
		refresh();
	}
	else if(move == KEY_LEFT && mapa[bestia->y][bestia->x-1] != '@')
	{
		bestia->x--;
		Print_Server_Map();
		Print_Alive_Objects();
		attron(COLOR_PAIR(4));
		mvprintw(bestia->y,bestia->x,"*");
		attroff(COLOR_PAIR(4));
		refresh();
	}
	else if(move == KEY_DOWN && mapa[bestia->y+1][bestia->x] != '@')
	{
		bestia->y++;
		Print_Server_Map();
		Print_Alive_Objects();
		attron(COLOR_PAIR(4));
		mvprintw(bestia->y,bestia->x,"*");
		attroff(COLOR_PAIR(4));
		refresh();
	}
	else if(move == KEY_RIGHT && mapa[bestia->y][bestia->x+1] != '@')
	{
		bestia->x++;
		Print_Server_Map();
		Print_Alive_Objects();
		attron(COLOR_PAIR(4));
		mvprintw(bestia->y,bestia->x,"*");
		attroff(COLOR_PAIR(4));
		refresh();
	}
}
int Move_Generator()
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
int Beast_Hunting(struct beast_t* bestia_ptr)
{
	int Start_Hunting = Check_Area(bestia_ptr->x, bestia_ptr->y);
	if(Start_Hunting != -1)
	{
		unsigned int x = player[Start_Hunting].x;
		unsigned int y = player[Start_Hunting].y;
		int dir = Show_Me_Direction(x, y, bestia_ptr->x, bestia_ptr->y);
		switch(dir)
		{
			case DIR_UP:
				return KEY_UP;
			case DIR_DOWN:
				return KEY_DOWN;
			case DIR_LEFT:
				return KEY_LEFT;
			case DIR_RIGHT:
				return KEY_RIGHT;
			default:
				return 0;
		}
	}
	else
	{
		return 0;
	}
}
int Check_Area(unsigned int x, unsigned int y)
{
	
	for(int i =y; i<= y+VIEW_RANGE;i++)
	{
		if(i >= 0 && i < HEIGHT)
		{
			for(int k = 0; k< MAX_PLAYERS; k++)
			{
				if(player[k].is_active == 1)
				{
					if(player[k].x == x && player[k].y == i)
					{
						return k;
					}
				}
			}
			if(mapa[i][x] == '@')
			{
				break;
			}
		}
	}
	
	for(int i = y-1;i >= y-VIEW_RANGE;i--)
	{
		if(i >= 0 && i < HEIGHT)
		{
			for(int k = 0; k< MAX_PLAYERS; k++)
			{
				if(player[k].is_active == 1)
				{
					if(player[k].x == x && player[k].y == i)
					{
						return k;
					}
				}
			}
			if(mapa[i][x] == '@')
			{
				break;
			}
		}
	}
	
	for(int i = x; i<= x+VIEW_RANGE;i++)
	{
		if(i >= 0 && i < WIDTH)
		{
			for(int k = 0; k< MAX_PLAYERS; k++)
			{
				if(player[k].is_active == 1)
				{
					if(player[k].x == i && player[k].y == y)
					{
						return k;
					}
				}
			}
			if(mapa[y][i] == '@')
			{
				break;
			}
		}
	}
	
	for(int i = x-1;i >= x-VIEW_RANGE;i--)
	{
		if(i >= 0 && i < WIDTH)
		{
			for(int k = 0; k< MAX_PLAYERS; k++)
			{
				if(player[k].is_active == 1)
				{
					if(player[k].x == i && player[k].y == y)
					{
						return k;
					}
				}
			}
			if(mapa[y][i] == '@')
			{
				break;
			}
		}
	}
	return -1;
}
int Show_Me_Direction(unsigned int px, unsigned int py, unsigned int bx, unsigned int by)
{
	if(px == bx)
	{
		if(py < by)
		{
			return DIR_UP;
		}
		else
		{
			return DIR_DOWN;
		}
	}
	if(py == by)
	{
		if(px < bx)
		{
			return DIR_LEFT;
		}
		else
		{
			return DIR_RIGHT;
		}
	}
	
}
void* Spawn_Beast(void *arg)
{
	int index = *(int*)arg;
	while(1)
	{
		int x = (rand() % 51);
		int y = (rand() % 25);
		
		if(mapa[y][x] == ' ')
		{
			bestia[index].x = x;
			bestia[index].y = y;
			attron(COLOR_PAIR(4));
			mvprintw(y, x, "*");
			attroff(COLOR_PAIR(4));
			refresh();
			break;
		}
	}
    fd_set fds;
	int znak;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    while(1)
    {
		struct timeval tp = { .tv_sec = 0, .tv_usec = 400000 };
		switch( select(STDIN_FILENO + 1, &fds, NULL, NULL, &tp) )
		{
			case 0:
				pthread_mutex_lock(&lock);
				int res = Beast_Hunting(&bestia[index]);
				if(res == 0)
				{
					beast_moves[index] = Move_Generator();
				}
				else
				{
					beast_moves[index] = res;
				}
				pthread_mutex_unlock(&lock);
				
				FD_ZERO(&fds);
				FD_SET(STDIN_FILENO, &fds);
				break;
			default:
				break;
		}
		if(server.is_active == 0)
		{
			break;
		}
	}
}
void Spawn_Treasure(int size)
{
	while(1)
	{
		int x = (rand() % 51);
		int y = (rand() % 25);
		if(mapa[y][x] != '@' && mapa[y][x] != '#' && mapa[y][x] != 'A')
		{
			if(size == 'c')
			{
				attron(COLOR_PAIR(2));
				mvprintw(y, x, "c");
				attroff(COLOR_PAIR(2));
				mapa[y][x] = 'c';
				refresh();
				break;
			}
			if(size == 't')
			{
				attron(COLOR_PAIR(2));
				mvprintw(y, x, "t");
				attroff(COLOR_PAIR(2));
				mapa[y][x] = 't';
				refresh();
				break;
			}
			if(size == 'T')
			{
				attron(COLOR_PAIR(2));
				mvprintw(y, x, "T");
				attroff(COLOR_PAIR(2));
				mapa[y][x] = 'T';
				refresh();
				break;
			}
		}
	}
	
}
void Load_Map_And_Settings()
{
	server.players = 0;
	server.beasts = 0;
	server.round = 1;
	server.amount_of_dropped_money = 0;
	Clear_Sockets();
	server.max_drop = MAX_DROPPED_MONEY;
	mapa = Read_Map("mapa.txt");
}
void Close_Everything()
{
	pthread_mutex_destroy(&lock);
	Free_Map();
	close(server_socket[0]); 
	close(server_socket[1]); 
	close(server_socket[2]); 
	close(server_socket[3]); 
	close(server_socket[4]); 
	close(server_socket[5]); 
	endwin();
	printf("\n\n\n\n\n					The server has been shut down!\n\n\n\n");
}
void Free_Map()
{
	for(int i = 0; i<HEIGHT; i++)
	{
		free(*(mapa+i));
	}
	free(mapa);
}
char **Read_Map(char *filename)
{
	if(filename == NULL)
	{
		return NULL;
	}
	FILE *wsk = fopen(filename, "r");
	if(wsk == NULL)
	{
		return NULL;
	}
	char linia[51];
	char **tab = (char **)calloc(HEIGHT, sizeof(char *));
    for (int i = 0 ; i < 25; i++)
    {
        *(tab+i) = (char *)calloc(WIDTH, sizeof(char));
    }
	int counter = 0;
	while(counter != 25)
	{
		fscanf(wsk, "%51[^\n]\n", linia);
		strcpy(*(tab+counter), linia);
		counter++;
	}
    return tab;
}
struct dropped_money* Find_Money(unsigned int x, unsigned int y)
{
	for(int i =0; i<server.amount_of_dropped_money; i++)
	{
		if(server.drop[i].x == x && server.drop[i].y == y)
		{
			return &server.drop[i];
		}
	}
	return NULL;
}
void Add_Coins_To_Base(struct player_t* gracz)
{
	if(mapa[gracz->y][gracz->x] == 'A')
	{
		gracz->money_in_base += gracz->money;
		gracz->money = 0;
	}
}
void Add_Coins_To_Player(struct player_t* gracz)
{
	if(mapa[gracz->y][gracz->x] == 'c')
	{
		gracz->money += 1;
		mapa[gracz->y][gracz->x] = ' ';
	}
	else if(mapa[gracz->y][gracz->x] == 't')
	{
		gracz->money += 10;
		mapa[gracz->y][gracz->x] = ' ';
	}
	else if(mapa[gracz->y][gracz->x] == 'T')
	{
		gracz->money += 50;
		mapa[gracz->y][gracz->x] = ' ';
	}
	else if(mapa[gracz->y][gracz->x] == 'D')
	{
		struct dropped_money *bigmoney = Find_Money(gracz->x, gracz->y);
		gracz->money += bigmoney->amount;
		mapa[gracz->y][gracz->x] = ' ';
	}
}
void Move_Player(struct player_t* gracz, int move)
{
	if(gracz->bush == 1)
	{
		gracz->bush = 0;
		return;
	}
	if(move == KEY_UP && mapa[gracz->y-1][gracz->x] != '@')
	{
		gracz->y--;
		Print_Server_Map();
		Print_Alive_Objects();
		switch(mapa[gracz->y][gracz->x])
		{
			case 'c':
				Add_Coins_To_Player(gracz);
				break;
			case 't':
				Add_Coins_To_Player(gracz);
				break;
			case 'T':
				Add_Coins_To_Player(gracz);
				break;
			case 'D':
				Add_Coins_To_Player(gracz);
				break;
			case 'A':
				Add_Coins_To_Base(gracz);
				break;
			case '#':
				gracz->bush = 1;
				break;
			default:
				break;
					
		}
		attron(COLOR_PAIR(1));
		mvprintw(gracz->y,gracz->x,"%c", gracz->number+'0');
		attroff(COLOR_PAIR(1));
		refresh();
	}
	else if(move == KEY_LEFT && mapa[gracz->y][gracz->x-1] != '@')
	{
		gracz->x--;
		Print_Server_Map();
		Print_Alive_Objects();
		switch(mapa[gracz->y][gracz->x])
		{
			case 'c':
				Add_Coins_To_Player(gracz);
				break;
			case 't':
				Add_Coins_To_Player(gracz);
				break;
			case 'T':
				Add_Coins_To_Player(gracz);
				break;
			case 'D':
				Add_Coins_To_Player(gracz);
				break;
			case 'A':
				Add_Coins_To_Base(gracz);
				break;
			case '#':
				gracz->bush = 1;
				break;
			default:
				break;
					
		}
		attron(COLOR_PAIR(1));
		mvprintw(gracz->y,gracz->x,"%c", gracz->number+'0');
		attroff(COLOR_PAIR(1));
		refresh();
	}
	else if(move == KEY_DOWN && mapa[gracz->y+1][gracz->x] != '@')
	{
		gracz->y++;
		Print_Server_Map();
		Print_Alive_Objects();
		switch(mapa[gracz->y][gracz->x])
		{
			case 'c':
				Add_Coins_To_Player(gracz);
				break;
			case 't':
				Add_Coins_To_Player(gracz);
				break;
			case 'T':
				Add_Coins_To_Player(gracz);
				break;
			case 'D':
				Add_Coins_To_Player(gracz);
				break;
			case 'A':
				Add_Coins_To_Base(gracz);
				break;
			case '#':
				gracz->bush = 1;
				break;
			default:
				break;
					
		}
		attron(COLOR_PAIR(1));
		mvprintw(gracz->y,gracz->x,"%c", gracz->number+'0');
		attroff(COLOR_PAIR(1));
		refresh();
	}
	else if(move == KEY_RIGHT && mapa[gracz->y][gracz->x+1] != '@')
	{
		gracz->x++;
		Print_Server_Map();
		Print_Alive_Objects();
		switch(mapa[gracz->y][gracz->x])
		{
			case 'c':
				Add_Coins_To_Player(gracz);
				break;
			case 't':
				Add_Coins_To_Player(gracz);
				break;
			case 'T':
				Add_Coins_To_Player(gracz);
				break;
			case 'D':
				Add_Coins_To_Player(gracz);
				break;
			case 'A':
				Add_Coins_To_Base(gracz);
				break;
			case '#':
				gracz->bush = 1;
				break;
			default:
				break;
					
		}
		attron(COLOR_PAIR(1));
		mvprintw(gracz->y,gracz->x,"%c", gracz->number+'0');
		attroff(COLOR_PAIR(1));
		refresh();
	}
}
int Find_Free_Player_Slot()
{
	for(int i =1; i< MAX_PLAYERS+1; i++)
	{
		if(server_socket[i] <= 0)
		{
			return i;
		}
	}
	return -1;
}
struct dropped_money *Find_Free_Money_Slot()
{
	for(int i = 0; i<server.amount_of_dropped_money; i++)
	{
		if(server.drop[i].amount == 0)
		{
			return &server.drop[i];
		}
	}
	return NULL;
}
void Am_I_Dead(struct player_t *gracz, struct beast_t *bestia_ptr)
{
	if(bestia_ptr == NULL)
	{
		int number = gracz->number-1;
		for(int i = 0; i<server.players; i++)
		{
			if(i != number)
			{
				if(player[i].x == gracz->x && player[i].y == gracz->y) // czy sa na tych samych pozycjach
				{
					if(player[i].money != 0 || gracz->money != 0) // czy maja jakies siano
					{
						attron(COLOR_PAIR(2));
						mvprintw(gracz->y, gracz->x, "D");
						mapa[gracz->y][gracz->x] = 'D';
						attroff(COLOR_PAIR(2));
						
						struct dropped_money *free_slot = Find_Free_Money_Slot();
						if(free_slot == NULL)
						{
							server.drop[server.amount_of_dropped_money].amount = gracz->money;
							server.drop[server.amount_of_dropped_money].amount += player[i].money;
							server.drop[server.amount_of_dropped_money].x = gracz->x;
							server.drop[server.amount_of_dropped_money].y = gracz->y;
							server.amount_of_dropped_money++;
						}
						else
						{
							free_slot->amount = gracz->money;
							free_slot->amount += player[i].money;
							free_slot->x = gracz->x;
							free_slot->y = gracz->y;
						}
					}
					else
					{
						mvprintw(gracz->y, gracz->x, " ");
					}	
					player[i].x = player[i].base_x;
					player[i].y = player[i].base_y;
					player[i].deaths++;
					player[i].money = 0;
					
					gracz->x = gracz->base_x;
					gracz->y = gracz->base_y;
					gracz->money = 0;
					gracz->deaths++;
					
					attron(COLOR_PAIR(1));
					mvprintw(player[i].y,player[i].x,"%c", player[i].number+'0');
					attroff(COLOR_PAIR(1));
					
					attron(COLOR_PAIR(1));
					mvprintw(gracz->y, gracz->x,"%c", gracz->number+'0');
					attroff(COLOR_PAIR(1));
					
					return;
				}
			}
		}
		for(int i =0; i<server.beasts; i++)
		{
			if(gracz->x == bestia[i].x && gracz->y == bestia[i].y) // czy sa w tym samym miejscu
			{
				if(gracz->money != 0) // czy gracz ma jakies siano
				{
					attron(COLOR_PAIR(2));
					mvprintw(gracz->y, gracz->x, "D");
					mapa[gracz->y][gracz->x] = 'D';
					attroff(COLOR_PAIR(2));
					
					struct dropped_money *free_slot = Find_Free_Money_Slot();
					if(free_slot == NULL)
					{
						server.drop[server.amount_of_dropped_money].amount = gracz->money;
						server.drop[server.amount_of_dropped_money].x = gracz->x;
						server.drop[server.amount_of_dropped_money].y = gracz->y;
						server.amount_of_dropped_money++;
					}
					else
					{
						free_slot->amount = gracz->money;
						free_slot->x = gracz->x;
						free_slot->y = gracz->y;
					}
				}
				else
				{
					mvprintw(gracz->y, gracz->x, " ");
				}
				gracz->x = gracz->base_x;
				gracz->y = gracz->base_y;
				gracz->money = 0;
				gracz->deaths++;
				
				attron(COLOR_PAIR(1));
				mvprintw(gracz->base_y,gracz->base_x,"%c", gracz->number+'0');
				attroff(COLOR_PAIR(1));
				return;
				
			}
		}
	}
	if(gracz == NULL)
	{
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			if(player[i].is_active == 1)
			{
				if(player[i].x == bestia_ptr->x && player[i].y == bestia_ptr->y)
				{
					if(player[i].money != 0)
					{
						attron(COLOR_PAIR(2));
						mvprintw(player[i].y, player[i].x, "D");
						mapa[player[i].y][player[i].x] = 'D';
						attroff(COLOR_PAIR(2));
						
						struct dropped_money *free_slot = Find_Free_Money_Slot();
						if(free_slot == NULL)
						{
							server.drop[server.amount_of_dropped_money].amount = player[i].money;
							server.drop[server.amount_of_dropped_money].x = player[i].x;
							server.drop[server.amount_of_dropped_money].y = player[i].y;
							server.amount_of_dropped_money++;
						}
						else
						{
							free_slot->amount = player[i].money;
							free_slot->x = player[i].x;
							free_slot->y = player[i].y;
						}
					}
					else
					{
						mvprintw(player[i].y, player[i].x, " ");
					}
					player[i].x = player[i].base_x;
					player[i].y = player[i].base_y;
					player[i].money = 0;
					player[i].deaths++;
					
					attron(COLOR_PAIR(1));
					mvprintw(player[i].base_y,player[i].base_x,"%c", player[i].number+'0');
					attroff(COLOR_PAIR(1));
					return;
					}
			}
		}
	}
}
void Player_Delete(struct player_t *gracz)
{
	memset(gracz->mapa,0,35);
	gracz->x = 0;
	gracz->y = 0;
	gracz->money = 0;
	gracz->money_in_base = 0;
	gracz->deaths = 0;
	gracz->base_x = 0;
	gracz->base_y = 0;
	close(server_socket[gracz->number]);
	server_socket[gracz->number] = 0;
	gracz->number = 0;
	gracz->is_active = 0;
	strcpy(gracz->type, "-");
}
void Player_Leave(struct player_t *gracz)
{
	if(gracz->money != 0)
	{
		attron(COLOR_PAIR(2));
		mvprintw(gracz->y, gracz->x, "D");
		attroff(COLOR_PAIR(2));
		struct dropped_money *free_slot = Find_Free_Money_Slot();
		if(free_slot == NULL)
		{
			server.drop[server.amount_of_dropped_money].amount = gracz->money;
			server.drop[server.amount_of_dropped_money].x = gracz->x;
			server.drop[server.amount_of_dropped_money].y = gracz->y;
			server.amount_of_dropped_money++;
		}
		else
		{
			free_slot->amount = gracz->money;
			free_slot->x = gracz->x;
			free_slot->y = gracz->y;
		}
	}
	else
	{
		mvprintw(gracz->y, gracz->x, " ");	
	}
	Player_Delete(gracz);
	Print_Server_Stats();
	server.players--;
	refresh();
}
void *Gameplay(void* arg)
{
	int trash;
	while(1)
	{
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			if(player[i].is_active == 1)
			{
				int res = recv(server_socket[i+1], &moves[i], sizeof(int),0);
				if(res <= 0)
				{
					Player_Leave(&player[i]);
					print_msg_string("Player ");
					print_msg_int(player[i].number);
					print_msg_string(" Left\n");
				}
			}
		}
		for(int i = 0; i<MAX_PLAYERS; i++)
		{
			if(player[i].is_active == 1)
			{
				if(moves[i] == KEY_UP || moves[i] == KEY_DOWN || moves[i] == KEY_RIGHT || moves[i] == KEY_LEFT)
				{
					Move_Player(&player[i], moves[i]);
					Am_I_Dead(&player[i], NULL);
				}
				else if(moves[i] == 'q')
				{
					Player_Leave(&player[i]);
					print_msg_string("Player ");
					print_msg_int(player[i].number);
					print_msg_string(" Left\n");
				}
			}
		}
		for(int i = 0; i<server.beasts; i++)
		{
			Move_Beast(&bestia[i], beast_moves[i]);
			Am_I_Dead(NULL, &bestia[i]);
		}
		Clear_Beast_moves();
		Print_Server_Map();
		Print_Server_Stats();
		Print_Alive_Objects();
		for(int i = 0; i<MAX_PLAYERS; i++)
		{
			if(player[i].is_active == 1)
			{
				Player_View(player[i].x, player[i].y, player[i].mapa);
				player[i].round = server.round;
				send(server_socket[i+1], &player[i], sizeof(struct player_t), 0);
			}
		}
		Clear_Moves();
		server.round++;
		usleep(400000);
	}
	
}
void *Connection_Listener(void *arg)
{
	while(server.is_active)
    {
		if(server.players < 4)
		{
			int free_socket = Find_Free_Player_Slot();
			
			server_socket[free_socket]= accept(server_socket[0], NULL, NULL);
			if(server_socket[free_socket] < 0)
			{
				print_msg_string("Failed to accept client\n");
			}
			else
			{
				print_msg_string("New player arrived!\n");
				char Type[6];
				recv(server_socket[free_socket], &Type, sizeof(Type), 0);
				server.players++;
				player[free_socket-1] = Create_Player(free_socket);
				
				strcpy(player[free_socket-1].type, Type);
				char Message[64] = "Welcome on the server!\n";
				send(server_socket[free_socket], &Message, sizeof(Message), 0);
				
				Player_View(player[free_socket-1].x, player[free_socket-1].y, player[free_socket-1].mapa);
				player[free_socket-1].round = server.round;
				send(server_socket[free_socket], &player[free_socket-1], sizeof(struct player_t), 0);
			}
		}
		else
		{
			server_socket[5]= accept(server_socket[0], NULL, NULL);
			if(server_socket[5] < 0)
			{
				print_msg_string("Failed to accept client\n");
			}
			else
			{
				char Message[64] = "Server is full! Try to join later!\n";
				send(server_socket[5], &Message, sizeof(Message), 0);
				close(server_socket[5]);
			}
		}
	}
}
void* Server_Listener(void *arg)
{
	int *a = malloc(sizeof(int));
	while(1)
	{
		int move = getchar();
		if(move == 'q' || move == 'Q')
		{
			print_msg_string("Serwer disabled!\n");
			server.is_active = 0;
			break;
		}
		else if(move == 'b' || move == 'B')
		{
			if(server.beasts == MAX_BEASTS)
			{
				print_msg_string("Server has reached its maximum amount of beasts!\n");
			}
			else
			{
				*a = server.beasts;
				pthread_create(&(server.thread_beasts[server.beasts]), NULL, Spawn_Beast, a);
				server.beasts++;
				print_msg_string("Server spawned new beast!\n");
			}
			
		}
		else if(move == 'c' ||  move == 't' || move == 'T')
		{
			Spawn_Treasure(move);
			print_msg_string("Server spawned new treasure!\n");
		}
	}
	free(a);
}

int main(int argc, char const* argv[])
{
    srand(time(0));
    
	server_socket[0] = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket[0] == -1)
	{
		print_msg_string("Couldn't create socket\n");
		return -1;
	}
	else
	{
		print_msg_string("Socket successfully created\n");
	}
	
	struct sockaddr_in server_address[MAX_PLAYERS];
	server_address[0].sin_family = AF_INET;
	server_address[0].sin_port = 7940;
	server_address[0].sin_addr.s_addr = INADDR_ANY;
	
	/*int flags = fcntl(server_socket[0], F_GETFL, 0);
	flags |= O_NONBLOCK;
	int status = fcntl(server_socket[0], F_SETFL, flags);*/
	
	int res = bind(server_socket[0], (struct sockaddr*) &server_address, sizeof(server_address));
	if(res < 0)
	{
		print_msg_string("Socket bind failed\n");
		close(server_socket[0]); 
		return -1;
	}
	else
	{
		print_msg_string("Socket successfully binded\n");
	}
	
	res = listen(server_socket[0], 5);
	if(res != 0)
	{
		print_msg_string("Listen failed\n");
		close(server_socket[0]); 
		return -1;
	}
	else
	{
		server.is_active = 1;
		print_msg_string("Waiting for connections\n");
	}
	
	Load_Map_And_Settings();
	
	Print_Server_Map(mapa);
	
    pthread_create(&(server.thread_server[0]), NULL, Server_Listener, NULL);
    pthread_create(&(server.thread_server[1]), NULL, Connection_Listener, NULL);
    pthread_create(&(server.thread_server[2]), NULL, Gameplay, NULL);
	pthread_join(server.thread_server[0], NULL);
	
	Close_Everything();
	
    return 0;
}






