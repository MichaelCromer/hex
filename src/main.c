#include <ncurses.h>

#include "include/draw.h"
#include "include/state.h"


void initialise(void)
{
    initscr();                  /* init the lib                                     */
    cbreak();                   /* send by char (raw ignores ssigs! use cbreak)     */
    noecho();                   /* don't echo user input                            */
    keypad(stdscr, TRUE);       /* capture special keys                             */
    intrflush(stdscr, FALSE);   /* interpret screen changes from ssigs correctly    */
    curs_set(0);                /* disable cursor                                   */
    ESCDELAY = 10;
}


void cleanup(void)
{
    erase();
    endwin();
}


int main(void)
{
    initialise();

    struct State *s = state_create();
    state_initialise(s, stdscr);

    while (!state_quit(s)) {
        erase();
        draw_state(s);
        refresh();

        state_update(s);
    }

    state_destroy(s);

    cleanup();
    return 0;
}
