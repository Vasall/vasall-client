#include "input.h"
#include "network.h"

#include <stdlib.h>


/* Redefine the external input-wrapper */
struct input_wrapper input;


extern int inp_init(void)
{
	int i;

	/* Clear the device-list */
	for(i = 0; i < DEVICE_NUM; i++)
		input.mask[i] = 0;

	/* Load all available input-devices */
	for(i = 0; i < SDL_NumJoysticks(); i++) {
		if(inp_add_device(i) < 0)
			goto err_close_input;
	}

	/* Initialize the share-buffer */
	input.share.num = 0;
	input.share.obj = 0;

	/* Clear the share-buffer */
	for(i = 0; i < INPUT_SLOTS; i++)
		input.share.mask[i] = 0;

	/* Clear the input-buffers */
	vec2_clr(input.mov);
	vec2_clr(input.mov_old);

	return 0;

err_close_input:
	inp_close();
	return -1;
}


extern void inp_close(void)
{
	int i;
	SDL_GameController *contr;

	for(i = 0; i < DEVICE_NUM; i++) {
		if(input.mask[i] == 0)
			continue;

		if((contr = SDL_GameControllerFromInstanceID(input.id[i])))
			SDL_GameControllerClose(contr);
	}
}


static short inp_get_slot(void)
{
	short i;

	for(i = 0; i < DEVICE_NUM; i++) {
		if(input.mask[i] == 0)
			return i;
	}

	return -1;
}


extern int inp_add_device(int idx)
{
	int32_t id;
	short slot;
	SDL_Joystick *joy;
	SDL_GameController *contr;

	if((slot = inp_get_slot()) < 0)
		return -1;

	/* Only register if the device is controller */
	if(SDL_IsGameController(idx)) {
		if(!(contr = SDL_GameControllerOpen(idx)))
			return -1;

		joy = SDL_GameControllerGetJoystick(contr);
		id = SDL_JoystickInstanceID(joy);

		input.mask[slot] = 1;
		input.id[slot] = id;
		input.type[slot] = 0;
		strcpy(input.name[slot], SDL_GameControllerName(contr));
		input.ptr[slot] = contr;	
		return 0;
	}

	return -1;
}


static short inp_get_id(int id)
{
	short i;

	for(i = 0; i < DEVICE_NUM; i++) {
		if(input.mask[i] == 0)
			continue;

		if(input.id[i] == id)
			return i;
	}

	return -1;
}


extern void inp_remv_device(int id)
{
	short slot;

	if((slot = inp_get_id(id)) < 0)
		return;

	SDL_GameControllerClose(input.ptr[slot]);
	input.mask[slot] = 0;
}


extern int inp_col_share(char *buf)
{
	short i;
	short num;
	char *ptr;
	uint32_t mask;
	int written = 0;
	uint32_t base_ts = 0;
	uint8_t del_ts;
	uint32_t tmp_ts;

	if((num = input.share.num) < 1)
		return 0;

	tmp_ts = base_ts = input.share.ts[0];

	memcpy(buf, &base_ts, 4);
	memcpy(buf + 4, &input.share.obj, 4);
	memcpy(buf + 8, &num, 2);

	written += 10;
	ptr = buf + 10;

	for(i = 0; i < num; i++) {
		mask = input.share.mask[i];
		memcpy(ptr, &mask, 4);
		ptr += 4;
		written += 4;

		del_ts = ((input.share.ts[i] - base_ts) - (tmp_ts - base_ts));
		tmp_ts = input.share.ts[i];

		memcpy(ptr, &del_ts, 1);
		ptr += 1;
		written += 1;

		if(mask & INP_M_MOV) {
			vec2_cpy((float *)ptr, input.share.mov[i]);
			ptr += VEC2_SIZE;
			written += VEC2_SIZE;
		}

		input.share.mask[i] = 0;
		memset(input.share.mov[i], 0, VEC2_SIZE);
	}

	input.share.num = 0;
	input.share.obj = 0;
	return written;
}
