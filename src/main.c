#include <ncurses.h>

#include "include/draw.h"
#include "include/state.h"

struct State *state = NULL;

void initialise(void)
{
    initscr();                  /* init the lib                                     */
    cbreak();                   /* send by char (raw ignores ssigs! use cbreak)     */
    noecho();                   /* don't echo user input                            */
    keypad(stdscr, TRUE);       /* capture special keys                             */
    intrflush(stdscr, FALSE);   /* interpret screen changes from ssigs correctly    */
    curs_set(0);                /* disable cursor                                   */
    ESCDELAY = 10;

    colour_initialise();
    state = state_create();
    state_initialise(state, stdscr);
}

void terminate(void)
{
    erase();
    endwin();

    state_destroy(state);
}

int main(void)
{
    initialise();

    while (!state_quit(state)) {
        erase();
        draw_state(state);
        refresh();

        state_update(state);
    }

    terminate();
    return 0;
}
