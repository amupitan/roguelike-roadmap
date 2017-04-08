#include <string.h>
#include <ncurses.h>
#include "display.h"

void ncurses_init(){
  initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* To get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	start_color();			/* Start color 			*/
	/*Initialize colors*/
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
}


/*Get input in control mode*/
Pair* getInputC(Pair* target){ /*TODO: make void?*/
  // int pos = getch();
  switch(getch()){
    case 'y':
    case '7':
      /*upper-left*/
      target->y--;
      target->x--;
      break;
    case 'k':
    case '8':
    case KEY_UP: /*TODO remove?*/
      /*up*/
      target->y--;
      break;
    case 'u':
    case '9':
      /*upper-right*/
      target->y--;
      target->x++;
      break;
    case 'l':
    case '6':
    case KEY_RIGHT: /*TODO remove?*/
      /*right*/
      target->x++;
      break;
    case 'n':
    case '3':
      /*lower-right*/
      target->y++;
      target->x++;
      break;
    case 'j':
    case '2':
    case KEY_DOWN: /*TODO remove?*/
      /*down*/
      target->y++;
      break;
    case 'b':
    case '1':
      /*lower-left*/
      target->x--;
      target->y++;
      break;
    case 'h':
    case '4':
    case KEY_LEFT: /*TODO remove?*/
      /*left*/
      target->x--;
      break;
    case ' ':
    case '5':
      /*rest*/
      break;
    case '>':
      /*attempt to go downstairs*/
      target->x = target->y = -3;
      break;
    case '<':
      /*attempt to go upstairs*/
      target->x = target->y = -4;
      break;
    case 'L':
      /*Enter look mode*/
      target->x = target->y = -2;
      break;
    case 'Q':
      /*Quit the game*/
      target->x = target->y = -1;
      break;
    case 'S':
      /*Display seed*/
      target->x = target->y = -5;
      break;
    case 'R':
      /*Display number of room*/
      target->x = target->y = -6;
      break;
    default:
      target->x = target->y = -10;
      break;
  }
  return target;
}

Pair* look_mode(Pair *target, int* control_mode){ //TODO: uses hardcoded width/height
  switch(getch()){
    case 'k':
    case '8':
    case KEY_UP: /*TODO remove?*/
      /*move up*/
      target->y -= 10;
      break;
    case 'l':
    case '6':
    case KEY_RIGHT: /*TODO remove?*/
      /*move right*/
      target->x += 40;
      break;
    case 'j':
    case '2':
    case KEY_DOWN: /*TODO remove?*/
      /*move down*/
      target->y += 10;
      break;
    case 'h':
    case '4':
    case KEY_LEFT: /*TODO remove?*/
      /*move left*/
      target->x -= 40;
      break;
    case 'Q':
      /*Quit the game*/
      ungetch('Q');
    case 27:/*ESC*/
      /*go back to control mode*/
      *control_mode = 1;
      break;
    default:
      break;
  }
  return target;
}

void log_message(const char* message){
  int row, col;
  move(0, 0);
  clrtoeol();
  getmaxyx(stdscr, row, col); /*Longindex is passed here but this macro function requires an argument*/
  mvprintw(0/row, (col - strlen(message))/2, message);
  /*TODO: call refresh()?*/
}