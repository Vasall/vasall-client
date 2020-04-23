#ifndef _WORLD_H
#define _WORLD_H

#include "core.h"
#include "model.h"
#include "object.h"

#define WORLD_SIZE 256

struct world {
	/*
	 * The x-size and the z-size of the world.
	 */
	int2_t size;
	int ptnum;

	/*
	 * The relative position of the terrain.
	 */
	vec3_t pos;

	/*  
	 * Both the absolute min- and max-positions.
	 */
	vec2_t min_pos, max_pos;

	/*
	 * The heightmap, containing the heights of each vertex of
	 * the terrain.
	 */
	float *heights;

	/*
	 * The relative rotation of the terrain, which will be passed to
	 * the model.
	 */
	vec3_t rot;

	/*
	 * The underlying model for the terrain.
	 */
	short terrain;

	/*
	 * The object table for the world.
	 */
	struct object_table *objects;
};


/* The global world-struct */
extern struct world *world;

/*
 * Initialize the global world-struct
 *
 * Returns: Either 0 on success or -1 if an error occurred
 */
int wld_create(void);

/*
 * Delete a world and free allocated memory.
 *
 * @world: Pointer to the world
 * 	to delete
 */
void wld_destroy(void);

/*
 * Render the objects and objects in it.
 */
void wld_update(void);

/*
 * Render the world using OpenGL.
 */
void wld_render(void);

/*
 * Get the height of the world at a given position.
 *
 * @x: The x-position in the world
 * @z: The z-position in the world
 *
 * Returns: The height at the given position
 */
float wld_get_height(float x, float z);

/*
 * Generate a new terrain and write it to the world-struct.
 *
 * Returns: Either 0 on success or -1 if an error occurred
 */
int wld_gen_terrain(void);

#endif
