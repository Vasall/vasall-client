#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "XSDL/xsdl.h"
#include "vec.h"
#include "mat.h"
#include "list.h"
#include "hashtable.h"
#include "camera.h"
#include "model.h"
#include "model_utils.h"
#include "object.h"
#include "world.h"
#include "thing.h"

/*
 * This is a struct used as a global-wrapper.
 * This has the purpose of making the most
 * important objects and pointers accessable
 * throughout the whole project. The main
 * wrapper should be called core.
 */
typedef struct gloWrapper {
	/*
	 * Pointer to the window-struct,
	 * which represents the created
	 * window itself.
	*/
	XSDL_Window *window;

	/*
	 * Pointer to the OpenGL-context,
	 * which is used for rendering
	 * both by the client and the
	 * XSDL-subsystem.
	*/
	XSDL_GLContext glcontext;

	/*
	 * Pointer to the UI-context
	 * created by the XSDL-subsystem.
	 * This struct is used to manage
	 * the userinterface.
	*/
	XSDL_UIContext *uicontext;

	/*
	 * This is just a shortcut to the
	 * root node in the node-tree of the
	 * previously declared ui-context.
	*/
	XSDL_Node *uiroot;

	/*
	 * The absolute path to the directory
	 * the binary is in.
	*/
	char *bindir;

	/*
	 * Is the game currently running.
	*/
	int8_t running;

	/*
	 * Is fullscreen currently active.
	*/
	int8_t fullscr;

	/*
	 * This is the callback-function
	 * to handle user-inputs. This
	 * function will be set to NULL by
	 * default, but overwriting this
	 * variable will enable running
	 * this function whenever the user
	 * input something.
	*/
	void (*procevt)(XSDL_Event *e);

	/*
	 * This is the callback-function to
	 * handle the update-sequence. This
	 * function will be set to NULL by
	 * default, but overwriting this
	 * varaible will enable running this
	 * function on every update-cycle.
	*/
	void (*update)(void);

	/*
	 * This is the callback-function to
	 * handle the render-sequence. This
	 * function will be set to NULL by
	 * default, but overwriting this
	 * variable will enable running this
	 * function on every render-call.
	*/
	void (*render)(void);

	struct object *obj;
} gloWrapper;

/*
 * These two variables are just defined, as
 * for many things, pointers to a variable
 * containing either one or zero are required.
 * So instead of having to define 1-0-variables
 * again and again, these act as shortcuts.
 */
extern uint8_t one;
extern uint8_t zero;

/*
 * The flags for the window. Note that
 * this variable can be changed any time,
 * to enable things like fullscreen.
 */
extern int g_win_flgs; 

/*
 * The one global-wrapper used to
 * contain all important things in
 * one single instance, to make it
 * easier to use throughout the
 * project.
 */
extern gloWrapper *core;

/*
 * This is the error-buffer for the
 * project which contains the most
 * recent error-message. This buffer
 * should never be set or read manually,
 * but rather through the use of the 
 * glSetError() and glGetError()
 * functions as doing otherwise can lead
 * to undefind behavior.
 */
extern char gloErrBuf[256];

/* Set a new error-message */
void gloSetError(char *err);

/* Get the most recent error-message */
char *gloGetError(void);

/* Initialize the global-wrapper */
int gloInit(int argc, char **argv);

/* Destroy the global-wrapper */
void gloClose(void);

/* Read the include-register and import all resources */
int gloLoad(char *pth);

#endif
