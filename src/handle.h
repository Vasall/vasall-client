#ifndef VASALL_HANDLE_H
#define VASALL_HANDLE_H

#include "wrapper.h"

/* Handle the resize-event */
void handle_resize(ENUD_Event *evt);

/* MENUSCREEN::CALLBACKS */
void menu_procevt(ENUD_Event *evt); 
void menu_update(void);
void menu_render(void);

/* GAMESCREEN::CALLBACKS */
void game_procevt(ENUD_Event *evt);
void game_update(void);
void game_render(void);

#endif
