#include "rpsbot.h"
#include "interface.h"

WINDOW *mainscreen;

int getinput(){
	
	int optioncolor;
		
	optioncolor=1;
	
	while(1){
		
		int ch;
		int done;
		
		wprintw(mainscreen,"\nmake your move\n\n");
		
		int r;
		
		for(r=0;r<3;r++){
			
			if(optioncolor==r) wattron(mainscreen,COLOR_PAIR(2));
			else wattron(mainscreen,COLOR_PAIR(1));
			
			wprintw(mainscreen,"%s",names[r]);
			
			wattron(mainscreen,COLOR_PAIR(1));
			
			wprintw(mainscreen,"  ");
		}
		
		done=0;
		ch=wgetch(mainscreen);
		
		switch(ch){
			
			case 'q':
				
				return -1;
				
			case KEY_LEFT:
				
				optioncolor--;
				if(optioncolor<0) optioncolor=2;
				break;
			
			case KEY_RIGHT:
				
				optioncolor++;
				if(optioncolor>2) optioncolor=0;
				break;
			
			case 10:
				
				wrefresh(mainscreen);
				wclear(mainscreen);
				return optioncolor;
				
			default: break;
		}
		
		wrefresh(mainscreen);
		wclear(mainscreen);
	}
}

void updateresults(int p1move,int p2move){
	
	wprintw(mainscreen,"computer plays %s\n",names[p2move]);
	
	if(p1move==p2move) wprintw(mainscreen,"\ndraw\n");
	else if(wintable[p1move]==p2move){
		
		p1wins++;
		wprintw(mainscreen,"\nyou win\n");
	}
	else{
		
		p2wins++;
		wprintw(mainscreen,"\nyou lose\n");
	}
}
