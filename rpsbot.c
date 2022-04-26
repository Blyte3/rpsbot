
#define HASHSIZE 10000
#define PATHLIM 29

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "rpsbot.h"
#include "interface.h"

char names[3][10]={"rock","paper","scissors"};
int wintable[3]={2,0,1};
int losetable[3]={1,2,0};
int playermovelist[HASHSIZE];
int botmovelist[HASHSIZE];
int p1wins;
int p2wins;
int movenumber;

struct match playertable[HASHSIZE];

struct match bottable[HASHSIZE];

int pow(int number,int power){
	
	int result;
	
	result=1;
	
	int r;
	
	for(r=0;r<power;r++){
		
		result*=number;
	}
	
	return result;
}

unsigned long getpath(int* path,int pathlen,int totallen){
	
	unsigned long result;
	int index;
	int lastmove;
	int toplace;
	
	index=totallen-pathlen;
	lastmove=path[index];
	result=0b11;

	int r;
	
	for(r=1;r<pathlen;r++){
		
		toplace=getmovetype(lastmove,path[index+r]);
		
		result+=toplace<<(2*r);
		
		lastmove=path[index+r];
	}
	
	result+=3<<(r*2);
}

int getmovetype(int lastmove,int thismove){
	
	int result;
	
	if(lastmove == wintable[thismove]) result=0b10;
	else if(lastmove == losetable[thismove]) result=0b01;
	else if(lastmove == thismove) result=0b00;
	
	return result;
}

//returns the index of the item, or where it would be if it existed

unsigned int tablelookup(struct match *table,unsigned long movepath){
	
	unsigned int hashin;
	
	hashin=movepath;
	
	for(;;hashin++){
		
		hashin%=HASHSIZE;
		
		if(table[hashin].used==0) return hashin;
		
		if(table[hashin].path != movepath) continue;
		
		return hashin;
	}
}

struct evaluation evaluate(struct match move,int depth){
	
	struct evaluation result;

	int moves[3];
	
	moves[0]=move.repeats;
	moves[1]=move.counters;
	moves[2]=move.losers;
	
	result.eval=-100000000;
	
	int temp;
	unsigned int r;
	
	for(r=0;r<3;r++){
		
		temp=moves[r];
		temp+=2*moves[wintable[r]];
		temp-=2*moves[losetable[r]];
		
		if(temp>result.eval){
			
			result.eval=temp;
			result.move=r;
		}
	}
	
	result.eval=pow(result.eval,depth);
	return result;
}

struct extra getbest(struct match* table,int* movelist,int movenumber){
		
	struct extra result;
	
	result.stats.used=0;
	
	int besteval;
	int samplesize;
	int index;
	
	besteval=-1;
	
	int r;
	
	if(movenumber>PATHLIM) samplesize=PATHLIM;
	else samplesize=movenumber;
	
	for(;samplesize>0;samplesize--){
		
		index=tablelookup(table,getpath(movelist,samplesize,movenumber));
		
		if(table[index].used==1){
			
			struct evaluation temp;
				
			temp=evaluate(table[index],samplesize);
				
			if(temp.eval>besteval){
					
				besteval=temp.eval;
				result.stats=table[index];
				result.depth=samplesize;
			}
		}
	}
	
	return result;
}

int aimove(int movenumber,int lastplayermove){
	
	struct extra playerresult;
	struct extra botresult;
	struct match finalresult;
	struct evaluation move;
	
	playerresult=getbest(playertable,playermovelist,movenumber);
	botresult=getbest(bottable,botmovelist,movenumber);
	
	finalresult.repeats=0;
	finalresult.counters=0;
	finalresult.losers=0;
	
	if(botresult.stats.used==1){
		
		finalresult.repeats+=pow(botresult.stats.repeats,botresult.depth);
		finalresult.counters+=pow(botresult.stats.counters,botresult.depth);
		finalresult.losers+=pow(botresult.stats.losers,botresult.depth);
	}	
	
	if(playerresult.stats.used==1){
		
		finalresult.repeats+=pow(playerresult.stats.repeats,botresult.depth);
		finalresult.counters+=pow(playerresult.stats.counters,botresult.depth);
		finalresult.losers+=pow(playerresult.stats.losers,botresult.depth);
	}
	
	if(finalresult.repeats == finalresult.counters && finalresult.counters == finalresult.losers){
		
		//wprintw(mainscreen,"rand");
		//wgetch(mainscreen);
		return rand()%3;
	}
	
	move=evaluate(finalresult,1);
	
	if(move.move==0) return lastplayermove;
	if(move.move==1) return losetable[lastplayermove];
	if(move.move==2) return wintable[lastplayermove];
}

void init(){

	initscr();
	srand(time(0));
	movenumber=0;
	p1wins=0;
	p2wins=0;
	
	mainscreen=newwin(40,40,1,4);
	
	start_color();
	
	init_color(COLOR_YELLOW,850,850,100);
	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(2,COLOR_BLACK,COLOR_YELLOW);
	
	cbreak();
	keypad(mainscreen,TRUE);
	noecho();
		
	int r;

	for(r=0;r<HASHSIZE;r++){
		
		playertable[r].used=0;
		bottable[r].used=0;
	}
}

void tableplace(struct match* table,int* movelist,int movenumber,int movetype){
		
	int samplesize;
	int index;
	int r;
	
	if(movenumber>PATHLIM) samplesize=PATHLIM;
	else samplesize=movenumber;
		
	for(;samplesize>0;samplesize--){
		
		index=tablelookup(table,getpath(movelist,samplesize,movenumber));
			
		if(table[index].used==0){
				
			//make new table entry
				
			table[index].used=1;
			table[index].repeats=0;
			table[index].counters=0;
			table[index].losers=0;
			
			table[index].path=getpath(movelist,samplesize,movenumber);
		}
					
		if(movetype==0b00) table[index].repeats++;
		else if(movetype==0b10) table[index].counters++;
		else table[index].losers++;
	}
}

void main(){
	
	init();

	int p1move;
	int p2move;
	int movenumber;
	int playermovetype;
	
	while(1){

		p1move=getinput();
		p2move=aimove(movenumber,(movenumber>0)?playermovelist[movenumber-1]:0);
		
		if(p1move==-1){ endwin(); return;}
				
		updateresults(p1move,p2move);
		
		playermovelist[movenumber]=p1move;
		botmovelist[movenumber]=p2move;
		
		if(movenumber>0) playermovetype=getmovetype(playermovelist[movenumber-1],playermovelist[movenumber]);
		
		tableplace(playertable,playermovelist,movenumber,playermovetype);
		tableplace(bottable,botmovelist,movenumber,playermovetype);
		
		movenumber++;	
		
		wgetch(mainscreen);
		wclear(mainscreen);
		wprintw(mainscreen,"wins:%d\nlosses:%d\ndraws:%d\ntotal games:%d",p1wins,p2wins,movenumber-p1wins-p2wins,movenumber);
		wgetch(mainscreen);
		wclear(mainscreen);
	}

	endwin();
}
