
struct match{

	unsigned int used:1;
	unsigned int repeats:10;
	unsigned int counters:10;
	unsigned int losers:10;
	unsigned long path;
};

struct evaluation{
	
	int eval:30;
	unsigned int move:2;
};

struct extra{
	
	struct match stats;
	int depth;
};

extern char names[3][10];
extern int wintable[3];
extern int losetable[3];
extern int p1wins,p2wins;

struct extra getbest(struct match* table,int* movelist,int movenumber);
struct evaluation evaluate(struct match move,int depth);
int pow(int number,int power);
unsigned long getpath(int* path,int pathlen,int totallen);
int getmovetype(int lastmove,int thismove);
unsigned int tablelookup(struct match *table,unsigned long movepath);
int aimove(int length,int lastplayermove);
void init();
void tableplace(struct match* table,int* movelist,int movenumber,int playermove);
