#include <stdio.h>
#include <SDL.h>
#include "xsdl.h"

void XSDL_SetWindowIcon(SDL_Window *window) 
{
	// Declare an SDL_Surface to be filled in with pixel data from an image file
	SDL_Surface *surface;

	Uint16 pixels[16*16] = {
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0aab, 0x0789, 0x0bcc, 0x0eee, 0x09aa, 0x099a, 0x0ddd,
		0x0fff, 0x0eee, 0x0899, 0x0fff, 0x0fff, 0x1fff, 0x0dde, 0x0dee,
		0x0fff, 0xabbc, 0xf779, 0x8cdd, 0x3fff, 0x9bbc, 0xaaab, 0x6fff,
		0x0fff, 0x3fff, 0xbaab, 0x0fff, 0x0fff, 0x6689, 0x6fff, 0x0dee,
		0xe678, 0xf134, 0x8abb, 0xf235, 0xf678, 0xf013, 0xf568, 0xf001,
		0xd889, 0x7abc, 0xf001, 0x0fff, 0x0fff, 0x0bcc, 0x9124, 0x5fff,
		0xf124, 0xf356, 0x3eee, 0x0fff, 0x7bbc, 0xf124, 0x0789, 0x2fff,
		0xf002, 0xd789, 0xf024, 0x0fff, 0x0fff, 0x0002, 0x0134, 0xd79a,
		0x1fff, 0xf023, 0xf000, 0xf124, 0xc99a, 0xf024, 0x0567, 0x0fff,
		0xf002, 0xe678, 0xf013, 0x0fff, 0x0ddd, 0x0fff, 0x0fff, 0xb689,
		0x8abb, 0x0fff, 0x0fff, 0xf001, 0xf235, 0xf013, 0x0fff, 0xd789,
		0xf002, 0x9899, 0xf001, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0xe789,
		0xf023, 0xf000, 0xf001, 0xe456, 0x8bcc, 0xf013, 0xf002, 0xf012,
		0x1767, 0x5aaa, 0xf013, 0xf001, 0xf000, 0x0fff, 0x7fff, 0xf124,
		0x0fff, 0x089a, 0x0578, 0x0fff, 0x089a, 0x0013, 0x0245, 0x0eff,
		0x0223, 0x0dde, 0x0135, 0x0789, 0x0ddd, 0xbbbc, 0xf346, 0x0467,
		0x0fff, 0x4eee, 0x3ddd, 0x0edd, 0x0dee, 0x0fff, 0x0fff, 0x0dee,
		0x0def, 0x08ab, 0x0fff, 0x7fff, 0xfabc, 0xf356, 0x0457, 0x0467,
		0x0fff, 0x0bcd, 0x4bde, 0x9bcc, 0x8dee, 0x8eff, 0x8fff, 0x9fff,
		0xadee, 0xeccd, 0xf689, 0xc357, 0x2356, 0x0356, 0x0467, 0x0467,
		0x0fff, 0x0ccd, 0x0bdd, 0x0cdd, 0x0aaa, 0x2234, 0x4135, 0x4346,
		0x5356, 0x2246, 0x0346, 0x0356, 0x0467, 0x0356, 0x0467, 0x0467,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
		0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff
	};
	surface = SDL_CreateRGBSurfaceFrom(pixels,16,16,16,16*2,0x0f00,0x00f0,0x000f,0xf000);

	// The icon is attached to the window pointer
	SDL_SetWindowIcon(window, surface);

	// ...and the surface containing the icon pixel data is no longer required.
	SDL_FreeSurface(surface);
}

/*
 * Create a new scene, by allocating memory for
 * the scene-struct and resettig the counter to 0.
 *
 * Returns: A pointer to the created scene or NULL
*/
XSDL_Scene *XSDL_CreateScene() 
{
	int len = sizeof(XSDL_Scene);
	XSDL_Scene *ptr = (XSDL_Scene *)malloc(len);
	if(ptr == NULL) return (NULL);

	ptr->count = 0;
	return(ptr);
}

/*
 * Delete a scene-struct and free the reserved memory.
 *
 * @scn: A pointer to the scene to delete
*/
void XSDL_DeleteScene(XSDL_Scene *scn) 
{
	free(scn);
}

/*
 * Create a new button and attach it to the specified scene.
 *
 * @scn: A pointer to the scene, to attach the button to
 * @x: The x-position of the button
 * @y: The y-position of the button
 * @w: The width of the button
 * @h: The height of the button
 * @ptr: The callback-function, for when the button is pressed
 *
 * Returns: 1 on success and 0 if some error occurred
*/
int XSDL_CreateButton(XSDL_Scene *scn, int x, int y, int w, int h, void (*ptr)()) 
{
	return(1);
}

/*
 * Render a button on the screen.
 *
 * @render: The renderer used to display the button
 * @btn: The button-struct to display
 */
void XSDL_RenderButton(SDL_Renderer *renderer, XSDL_Button *btn) 
{
	SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
	SDL_RenderFillRect(renderer, &btn->body);
}
