#ifndef _RIG_H
#define _RIG_H

#include "vector.h"
#include "matrix.h"
#include <stdint.h>


#define JOINT_MAX_NUM 100

/*
 * A simple rig-struct containing the necessary data for the
 * animation of an object.
 */
struct model_rig {
	short     model;
	
	short     anim;
	float     prog;
	short     keyfr;
	uint32_t  ts;

	int       jnt_num;
	
	vec3_t    loc_pos[JOINT_MAX_NUM];
	vec4_t    loc_rot[JOINT_MAX_NUM];

	mat4_t    base_mat[JOINT_MAX_NUM];
	mat4_t    trans_mat[JOINT_MAX_NUM];

	/*
	 * Hooks
	 */
	short     hook_num;
	vec3_t    *hook_pos;
	vec3_t    *hook_dir;
	mat4_t    *hook_base_mat;
	mat4_t    *hook_trans_mat;
};


/*
 * Derive a rig from a model and setup the joint-matrices.
 *
 * @slot: The slot in the model-list to derive the rig from
 *
 * Returns: Either a pointer to an animation-runner or NULL if an error occurred 
 */
extern struct model_rig *rig_derive(short slot);


/*
 * Destroy a rig and free the allocated memory.
 *
 * @anim: Pointer to the rig
 */
extern void rig_free(struct model_rig *rig);


/*
 * Update the rig according to the current animation.
 *
 * @rig: Pointer to the rig to update
 */
extern void rig_update(struct model_rig *rig, float p);


/*
 * Multiply the current joint-matrices with the given transformation-matrix.
 *
 * @rig: Pointer to the rig, to modify
 * @m: The transformation-matrix
 */
extern void rig_mult_mat(struct model_rig *rig, mat4_t m);

#endif
