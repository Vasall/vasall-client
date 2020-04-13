#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "world.h"

/* Redefine global variables */
struct ht_t *object_table = NULL;

/*
 * Initialize the global object-list.
 *
 * Returns: Either 0 on success or -1
 * 	if an error occurred
 */
int objInit(void)
{
	object_table = htCreate(OBJ_SLOTS);
	if(object_table == NULL) return(-1);

	return(0);
}

/*
 * Clear the object-list, destroy all objects
 * and free the allocated memory.
 */
void objClose(void)
{

}

int objSet(char *key, char* mdl, vec3_t pos)
{
	int r;
	struct object *obj = NULL;

	obj = malloc(sizeof(struct object));
	if(obj == NULL) return(-1);

	memset(obj, 0, sizeof(struct object));

	/* Copy the key of the model */
	strcpy(obj->key, key);

	/* Set the position */
	vec3_cpy(obj->pos, pos);

	/* Set the velocity */
	vec3_set(obj->vel, 0.0, 0.0, 0.0);

	/* Set the direction */
	vec3_set(obj->dir, 1.0, 1.0, 1.0);

	/* Set the scaling-vector */
	vec3_set(obj->scl, 1.0, 1.0, 1.0);

	/* Set the rotation-values */
	vec3_set(obj->rot, 0.0, 0.0, 0.0);

	/* Set the model */
	obj->model = mdlGet(mdl);
	if(obj->model == NULL) goto failed;

	/* Initialize the model-matrix */
	objUpdMatrix(obj);

	/* Insert the object into the object-table */
	r = htSet(object_table, key, (uint8_t *)obj, sizeof(struct object));
	if(r < 0) goto failed;

	return(0);	

failed:
	free(obj);

	return(-1);
}


void objDel(char *key)
{
	if(key) {}
}

/* 
 * Get an object via the object-id.
 *
 * key: The key of this object
 *
 * Returns: Either the object with the given id
 * 	or NULL if an error occurred
*/
struct object *objGet(char *key)
{
	int r;
	struct object *obj;

	r = htGet(object_table, key, (uint8_t **)&obj, NULL);
	if(r < 0) return(NULL);

	return(obj);
}

/*
 * Update an object and use pyhsical calculations
 * to adjust the attributes of the object.
 *
 * @obj: Pointer to the object to update
 * @delt: The time since the last frame
 */
void objUpdate(struct object *obj, float delt)
{
	vec3_t del;
	
	vec3_cpy(del, obj->vel);
	vec3_scl(del, delt, del);

	objAddPos(obj, del);

	obj->pos[1] = wldGetHeight(obj->pos[0], obj->pos[2]) + 2.2;
	
	if(vec3_mag(del) > 0.0) {
		vec3_nrm(del, obj->dir);
		obj->rot[1] = atan2(-obj->dir[2], obj->dir[0]);
	}

	objUpdMatrix(obj);
}

/* 
 * Render the model of an object on 
 * the screen.
 *
 * obj: Pointer to the object to render
*/
void objRender(struct object *obj)
{
	/* Just render the attached model */
	mdlRender(obj->model, obj->matrix);
}

/* =============================================== */
/*                     POSITION                    */
/* =============================================== */

/* 
 * Get the current position of the object
 * and copy the values to the given vector.
 *
 * @obj: Pointer to the object to get the position of
 * @pos: The vector to copy the values into
*/
void objGetPos(struct object *obj, vec3_t pos)
{
	vec3_cpy(pos, obj->pos);
	objUpdMatrix(obj);
}

/* 
 * Set the position of the object and
 * copy the values of the given vector into
 * the position-vector of the object.
 *
 * @obj: Pointer to the object to set the position of
 * @pos: The new position of the object
 */
void objSetPos(struct object *obj, vec3_t pos)
{
	objUpdMatrix(obj);
	vec3_cpy(obj->pos, pos);
}

/* 
 * Add a vector to the position by adding
 * the values to the current position-vector
 * of the object.
 *
 * @obj: Pointer to the object to change the position of
 * @vec: The vector to add to the position-vector
 */
void objAddPos(struct object *obj, vec3_t del)
{
	vec3_add(obj->pos, del, obj->pos);
	objUpdMatrix(obj);
}

/* =============================================== */
/*                     ROTATION                    */
/* =============================================== */

/* 
 * Get the current rotation of the object
 * and copy the values to the given vector.
 *
 * @obj: Pointer to the object to get the rotation of
 * @rot: The vector to write the rotation-values to
*/
void objGetRot(struct object *obj, vec3_t rot)
{
	vec3_cpy(rot, obj->rot);
	objUpdMatrix(obj);
}

/* 
 * Set the rotation of the object and
 * copy the values of the given vector to
 * the rotation-vector of the object.
 *
 * @obj: Pointer to the object to set the rotation of
 * @rot: The new rotation of the object
 */
void objSetRot(struct object *obj, vec3_t rot)
{
	vec3_cpy(obj->rot, rot);
	objUpdMatrix(obj);
}

/*
 * Add a vector to the rotation by adding 
 * the values to the current rotation-vector
 * of the object.
 *
 * @obj: Pointer to the object to change the rotation of
 * @del: The vector to add to the rotation
*/
void objAddRot(struct object *obj, vec3_t del)
{
	vec3_add(obj->rot, del, obj->rot);
	objUpdMatrix(obj);
}

/* =============================================== */
/*                     VELOCITY                    */
/* =============================================== */

/* 
 * Get the current velocity of the object
 * and copy the values to the given vector.
 *
 * @obj: Pointer to the object to get the velocity of
 * @vel: The vector to write the velocity-values to
*/
void objGetVel(struct object *obj, vec3_t vel)
{
	vec3_cpy(vel, obj->vel);
}

/* 
 * Set the velocity of the object and
 * copy the values of the given vector to
 * the velocity-vector of the object.
 *
 * @obj: Pointer to the object to set the velocity of
 * @rot: The new velocity of the object
 */
void objSetVel(struct object *obj, vec3_t vel)
{
	vec3_cpy(obj->vel, vel);
}

/*
 * Add a vector to the velocity by adding 
 * the values to the current velocity-vector
 * of the object.
 *
 * @obj: Pointer to the object to change the velocity of
 * @del: The vector to add to the velocity
*/
void objAddVel(struct object *obj, vec3_t del)
{
	vec3_add(obj->vel, del, obj->vel);
}

/* =============================================== */
/*                     DIRECTION                   */
/* =============================================== */

/* 
 * Set the facing-direction of the object and copy 
 * the given vector-values in the direction-vector.
 *
 * @obj: Pointer to the object
 * @dir: The new direction-vector
 */
void objSetDir(struct object *obj, vec3_t dir)
{
	vec3_cpy(obj->dir, dir);
	vec3_nrm(obj->dir, obj->dir);
}

/* 
 * Get the facing-direction of the object and copy
 * the values into the given vector.
 * 
 * @obj: Pointer to the object
 * @dir: A vector to copy the values into
 */
void objGetDir(struct object *obj, vec3_t dir)
{
	vec3_cpy(dir, obj->dir);
}

/* =============================================== */
/*                       MODEL                     */
/* =============================================== */

/* 
 * Set the model of the object, by replacing
 * the current model-pointer with a new one.
 *
 * @obj: Pointer to the object to set the model of
 * @mdl: The key of the model to attach
 */
void objSetModel(struct object *obj, char *mdl)
{
	obj->model = mdlGet(mdl);
}

/*
 * Get the modelmatrix for rendering.
 * This function will combine all 3
 * model-matrices and return the result
 * as a new matrix.
 *
 * @obj: Pointer to the model
 * @mat: The matrix to write the model-matrix to
 */
void objGetMatrix(struct object *obj, mat4_t mat)
{
	mat4_cpy(mat, obj->matrix);
}

/*
 * Update the model-matrix of a
 * given model.
 *
 * @obj: The model to update the mode-matrix for
 */
void objUpdMatrix(struct object *obj)
{
	vec3_t rot;

	vec3_cpy(rot, obj->rot);

	mat4_idt(obj->matrix);

	obj->matrix[0x0] =  cos(rot[1]);
	obj->matrix[0x2] = -sin(rot[1]);
	obj->matrix[0x8] =  sin(rot[1]);
	obj->matrix[0xa] =  cos(rot[1]);

	obj->matrix[0xc] = obj->pos[0];
	obj->matrix[0xd] = obj->pos[1];
	obj->matrix[0xe] = obj->pos[2];
}

/* 
 * Output info about the object in the terminal
 *
 * @obj: Pointer to the object to display data about
*/
void objPrint(struct object *obj)
{
	printf("Key: %s\n", obj->key);
	printf("Pos: "); vec3_dump(obj->pos); printf("\n");
	printf("Rot: "); vec3_dump(obj->rot); printf("\n");
	printf("Vel: "); vec3_dump(obj->vel); printf("\n");
}
