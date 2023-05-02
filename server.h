#ifndef GRA_SERVER_H
#define GRA_SERVER_H

#define HEIGHT 25
#define MAX_PLAYERS 4
#define MAX_BEASTS 4
#define WIDTH 52
#define VIEW_RANGE 2
#define MAX_DROPPED_MONEY 30
#define VIEW_RANGE 2

#define MESSAGE_HEIGHT 27
#define MESSAGE_WIDTH 3

#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_RIGHT 3
#define DIR_LEFT 4

struct beast_t
{
	unsigned int x;
	unsigned int y;
};
struct dropped_money{
	unsigned int x;
	unsigned int y;
	unsigned int amount;
};

struct server_t
{
	unsigned int round;
	unsigned int beasts;
	unsigned int players;
	unsigned int is_active;
	
	pthread_t thread_beasts[MAX_BEASTS];
	pthread_t thread_server[3];
	pthread_t thread_players[MAX_PLAYERS];
	
	unsigned int amount_of_dropped_money;
	struct dropped_money drop[MAX_DROPPED_MONEY];
	unsigned int max_drop;
	
};

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
void Load_Map_And_Settings();
void Close_Everything();
void Free_Map();
void print_msg_string(char* s);
void print_msg_int(int liczba);
void print_msg_char(char znak);
void Clear_Sockets();
void Clear_Moves();
void Clear_Beast_moves();
void Print_Alive_Objects();
void Print_Server_Stats();
void Print_Server_Map();
struct player_t Create_Player(int number);
void Player_View(int x, int y, char *player_pos);
int Move_Generator();
void Spawn_Treasure(int size);
void Move_Beast(struct beast_t* bestia, int move);
int Beast_Hunting(struct beast_t* bestia_ptr);
int Check_Area(unsigned int x, unsigned int y);
int Show_Me_Direction(unsigned int px, unsigned int py, unsigned int bx, unsigned int by);
void* Spawn_Beast(void *arg);
char **Read_Map(char *filename);
struct dropped_money* Find_Money(unsigned int x, unsigned int y);
void Add_Coins_To_Base(struct player_t* gracz);
void Add_Coins_To_Player(struct player_t* gracz);
void Move_Player(struct player_t* gracz, int move);
int Find_Free_Player_Slot();
struct dropped_money *Find_Free_Money_Slot();
void Am_I_Dead(struct player_t *gracz, struct beast_t *bestia_ptr);
void Player_Delete(struct player_t *gracz);
void Player_Leave(struct player_t *gracz);
void* Gameplay(void* arg);
void* Connection_Listener(void *arg);
void* Server_Listener(void *arg);

#endif

