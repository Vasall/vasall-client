#include "world.h"
#include "mbasic.h"
#include "world_utils.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Redefine the global world-wrapper */
struct world_wrapper world;


static int twodim(int x, int y, int w) {return y * w + x;}


extern int wld_init(void)
{
	vec2_t del;

	world.size[0] = CHUNK_SIZE;
	world.size[1] = CHUNK_SIZE;

	del[0] = world.size[0] / 2.0;
	del[1] = world.size[1] / 2.0;	

	world.min_pos[0] = -del[0];
	world.min_pos[1] = -del[1];

	world.max_pos[0] = del[0];
	world.max_pos[1] = del[1];

	world.ptnum = (CHUNK_SIZE * CHUNK_SIZE);
	if(!(world.heights = malloc(sizeof(float) * (world.ptnum))))
		return -1;

	memset(world.heights, 0, world.ptnum);
	memset(&world.rot, 0, sizeof(vec3_t));

	if(wld_gen_terrain() < 0) {
		ERR_LOG(("Failed to generate terrain"));	
		free(world.heights);
		return -1;
	}

	return 0;
}


extern void wld_close(void)
{
	if(!world.heights)
		return;

	free(world.heights);
}


extern void wld_render(float interp) 
{
	mat4_t idt;

	if(interp) {/* Prevent warning for not using parameter */}

	mat4_idt(idt);
	mdl_render(world.terrain, idt);
}


extern float wld_get_height(float x, float z)
{
	float ret = 0.0;
	vec2_t rel_pos, coord;
	int2_t map_idx;
	float heights[4];

	rel_pos[0] = _abs(world.min_pos[0] - x);
	rel_pos[1] = _abs(world.min_pos[1] - z);

	map_idx[0] = floor(rel_pos[0]);
	map_idx[1] = floor(rel_pos[1]);

	/* Check if the position is in range */
	if(map_idx[0] < 0 || map_idx[0] >= world.size[0] ||
			map_idx[1] < 0 || map_idx[1] >= world.size[1])
		return 0;

	heights[0] = world.heights[twodim(map_idx[0], 
			map_idx[1], world.size[0])];
	heights[1] = world.heights[twodim(map_idx[0] + 1, 
			map_idx[1], world.size[0])];
	heights[2] = world.heights[twodim(map_idx[0], 
			map_idx[1] + 1, world.size[0])];
	heights[3] = world.heights[twodim(map_idx[0] + 1, 
			map_idx[1] + 1, world.size[0])];

	coord[0] = rel_pos[0] - floor(rel_pos[0]);
	coord[1] = rel_pos[1] - floor(rel_pos[1]);

	if(coord[0] <= (1 - coord[1])) {
		vec3_t v1, v2, v3;

		v1[0] = 0.0;
		v1[1] = heights[0];
		v1[2] = 0.0;

		v2[0] = 1.0;
		v2[1] = heights[1];
		v2[2] = 0.0;

		v3[0] = 0.0;
		v3[1] = heights[2];
		v3[2] = 1.0;

		ret = vec3_barry_centric(v1, v2, v3, coord);
	}
	else {
		vec3_t v1, v2, v3;

		v1[0] = 1.0;
		v1[1] = heights[1];
		v1[2] = 0.0;

		v2[0] = 1.0;
		v2[1] = heights[3];
		v2[2] = 1.0;

		v3[0] = 0.0;
		v3[1] = heights[2];
		v3[2] = 1.0;

		ret = vec3_barry_centric(v1, v2, v3, coord);

	}

	return ret;
}


extern int wld_gen_terrain(void)
{
	int vtx_num, x, z, w, count, i, j, idx_num, *idx = NULL;
	float *heights = NULL, xpos, zpos;
	vec3_t *vtx = NULL, *last_row = NULL, *nrm = NULL, *col = NULL;
	int foo = 1;

	w = world.size[0];

	/* Calculate the amount of vertices */
	vtx_num = calcVertexNum(w) + 1;

	/* Initialize the vertex-array */
	if(!(vtx = calloc(vtx_num, VEC3_SIZE)))
		return -1;

	/* Create an array for all vertice-colors */
	if(!(col = calloc(vtx_num, VEC3_SIZE)))
		goto err_free;

	if(!(nrm = calloc(vtx_num, VEC3_SIZE)))
		goto err_free;

	world.size[0] = CHUNK_SIZE;
	world.size[1] = CHUNK_SIZE;

	heights = world.heights;
	for(x = 0; x < world.size[0]; x++) {
		for(z = 0; z < world.size[1]; z++) {
			i = twodim(z, x, world.size[1]);
			heights[i] = 0;
		}
	}

	if(!(last_row = malloc(((int)world.size[0] - 1) * VEC3_SIZE * 2)))
		goto err_free;

	/* Iterate over all points in the heightmap */
	count = 0;
	for(z = 0; z < world.size[1] - 1; z++) {
		for(x = 0; x < world.size[0] - 1; x++) {
			vec3_t ctl, ctr, cbl, cbr;

			xpos = x;
			zpos = z;

			ctl[0] = xpos;
			ctl[1] = heights[twodim(x, z, world.size[0])];
			ctl[2] = zpos;

			ctr[0] = xpos + 1.0;
			ctr[1] = heights[twodim(x + 1, z, world.size[0])];
			ctr[2] = zpos;

			cbl[0] = xpos;
			cbl[1] = heights[twodim(x, z + 1, world.size[0])];
			cbl[2] = zpos + 1.0;

			cbr[0] = xpos + 1.0;
			cbr[1] = heights[twodim(x + 1, z + 1, world.size[0])];
			cbr[2] = zpos + 1.0;

			vec3_cpy(vtx[count], ctl);
			count++;

			if(z != world.size[1] - 2 || x == world.size[0] - 2) {
				vec3_cpy(vtx[count], ctr);
				count++;
			}

			if(z == world.size[1] - 2) {
				vec3_cpy(last_row[x * 2], cbl);
				vec3_cpy(last_row[x * 2 + 1], cbr);
			}
		}
	}

	for(j = 0; j < world.size[0] - 1; j++) {
		vec3_t left, right;
		vec3_cpy(left, last_row[j * 2]);
		vec3_cpy(right, last_row[j * 2 + 1]);

		if(left[0] == 0 || right[0] == 1) {
			vec3_cpy(vtx[count], left);
			count++;
		}

		vec3_cpy(vtx[count], right);
		count++;
	}

	if(!(idx = (int *)genIndexBuf(world.size[0], &idx_num)))
		goto err_free;

	/* Calculate the colors for the vertices */
	for(j = 0; j < idx_num - 2; j += 3) {
		vec3_t col_tmp;

		foo++;
		if(foo % 2 == 0) {
			col_tmp[0] = 0.0;
			col_tmp[1] = 0.0;
			col_tmp[2] = 0.0;
		}
		else {
			col_tmp[0] = 1.0;
			col_tmp[1] = 1.0;
			col_tmp[2] = 1.0;
		}

		memcpy(col[idx[j]], col_tmp, VEC3_SIZE);
	}

	/* Calculate the normal-vectors */
	for(i = 0; i < idx_num - 2; i += 3) {
		vec3_t v1, v2, v3, del1, del2, nrm_tmp;

		vec3_cpy(v1, vtx[idx[i]]);
		vec3_cpy(v2, vtx[idx[i + 1]]);
		vec3_cpy(v3, vtx[idx[i + 2]]);

		vec3_sub(v2, v1, del1);
		vec3_sub(v2, v3, del2);

		vec3_cross(del1, del2, nrm_tmp);
		vec3_nrm(nrm_tmp, nrm_tmp);
		vec3_scl(nrm_tmp, -1, nrm_tmp);

		memcpy(nrm[idx[i]], nrm_tmp, VEC3_SIZE);
	}

	/* Allocate memory for the model-struct */
	if((world.terrain = mdl_set("wld_")) < 0)
		goto err_free;

	mdl_set_shader(world.terrain, shd_get("col"));
	mdl_set_mesh(world.terrain, idx_num, idx, vtx_num, vtx, nrm, col, 0);

	if(vtx) free(vtx);
	if(col) free(col);
	if(nrm) free(nrm);
	if(last_row) free(last_row);
	if(idx) free(idx);
	return 0;

err_free:
	if(vtx) free(vtx);
	if(col) free(col);
	if(nrm) free(nrm);
	if(last_row) free(last_row);
	if(idx) free(idx);
	return -1;
}
