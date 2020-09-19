#include "model.h"
#include "error.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct model *models[MDL_SLOTS];


static short mdl_get_slot(void)
{
	short i;

	for(i = 0; i < MDL_SLOTS; i++) {
		if(!models[i])
			return i;
	}

	return -1;
}


static int mdl_check_slot(short slot)
{
	if(slot < 0 || slot >= MDL_SLOTS)
		return 1;

	return 0;
}


static void mdl_set_status(short slot, uint8_t status)
{
	struct model *mdl;

	if(mdl_check_slot(slot))
		return;

	if(!(mdl = models[slot]))
		return;

	mdl->status = status;
}


extern int mdl_init(void)
{
	int i;

	for(i = 0; i < MDL_SLOTS; i++)
		models[i] = NULL;

	return 0;
}


extern void mdl_close(void)
{
	int i;
	struct model *mdl;

	for(i = 0; i < MDL_SLOTS; i++) {
		if(!(mdl = models[i]))
			continue;

		/* Destroy buffer-array-objects */
		if(mdl->vtx_bao)
			glDeleteBuffers(1, &mdl->vtx_bao);

		if(mdl->idx_bao)
			glDeleteBuffers(1, &mdl->idx_bao);

		/* Destroy vertex-array-object */
		if(mdl->vao)
			glDeleteVertexArrays(1, &mdl->vao);

		/* Free buffers */
		if(mdl->idx_buf)
			free(mdl->idx_buf);

		if(mdl->vtx_buf)
			free(mdl->vtx_buf);

		free(mdl);
	}
}


extern short mdl_set(char *name)
{
	short slot;
	struct model *mdl;

	/* Check if the given key is valid */
	if(!name || strlen(name) >= MDL_NAME_MAX)
		return -1;

	if((slot = mdl_get_slot()) < 0) {
		ERR_LOG(("The model-table is already full"));
		return -1;
	}

	if(!(mdl = malloc(sizeof(struct model))))
		return -1;

	/* Copy the array-slot for the model */
	mdl->slot = slot;

	/* Copy the key for this model */
	strcpy(mdl->name, name);

	/* Set type of the model */
	mdl->type = MDL_BARE;

	/* Initialize model-attributes */
	mdl->vao = 0;
	mdl->idx_bao = 0;
	mdl->idx_buf = NULL;
	mdl->idx_num = 0;	
	mdl->vtx_bao = 0;
	mdl->vtx_buf = NULL;
	mdl->vtx_num = 0;
	
	mdl->tex = -1;
	mdl->shd = -1;

	/* Initialize joint-arrays */
	mdl->jnt_buf = NULL;
	mdl->jnt_mat = NULL;
		
	/* Initialize animation-attributes */
	mdl->anim_buf = NULL;

	mdl->status = MDL_OK;

	/* Generate a new vao */
	glGenVertexArrays(1, &mdl->vao);

	models[slot] = mdl;
	return slot;
}


extern short mdl_get(char *name)
{
	int i;

	for(i = 0; i < MDL_SLOTS; i++) {
		if(!models[i])
			continue;

		if(!strcmp(models[i]->name, name))
			return i;
	}

	return -1;
}

extern void mdl_del(short slot)
{
	struct model *mdl;
	int i;
	int j;

	if(mdl_check_slot(slot))
		return;

	if(!(mdl = models[slot]))
		return;

	if(mdl->idx_bao)
		glDeleteBuffers(1, &mdl->idx_bao);

	if(mdl->vtx_bao)
		glDeleteBuffers(1, &mdl->vtx_bao);

	if(mdl->vao)
		glDeleteVertexArrays(1, &mdl->vao);

	if(mdl->idx_buf)
		free(mdl->idx_buf);

	if(mdl->vtx_buf)
		free(mdl->vtx_buf);

	/* Free the joint-buffer */
	if(mdl->jnt_buf)
		free(mdl->jnt_buf);

	/* Free the joint-matrices */
	if(mdl->jnt_mat)
		free(mdl->jnt_mat);

	/* Free the animation-buffer and keyframes */
	if(mdl->anim_buf) {
		for(i = 0; i < mdl->anim_num; i++) {
			for(j = 0; j < mdl->anim_buf[i].keyfr_num; j++) {
				free(mdl->anim_buf[i].keyfr_buf[j].pos);
				free(mdl->anim_buf[i].keyfr_buf[j].rot);
			}

			free(mdl->anim_buf[i].keyfr_buf);
		}

		free(mdl->anim_buf);
	}

	free(mdl);
	models[slot] = NULL;
}


extern void mdl_set_data(short slot, int vtxnum, float *vtx, float *tex,
		float *nrm, int *jnt, float *wgt, int idxnum,
		unsigned int *idx)
{
	int i;
	char *ptr;
	int vtx_size;
	struct model *mdl;
	void *p;
	GLenum err;

	if(mdl_check_slot(slot))
		return;

	mdl = models[slot];
	if(!mdl || mdl->status != MDL_OK)
		goto err_set_failed;


	/* Calculate the size of a single vertex in bytes */
	if(jnt && wgt) {
		printf("Rigged!\n");

		/* With joints and weights */
		vtx_size = 12 * sizeof(float) + 4 * sizeof(int);

		/* Set the model-type */
		mdl->type = MDL_RIG; 
	}
	else {
		/* Just the bare mesh(vtx-positions, uv-coord, nrm-vec) */
		vtx_size = 8 * sizeof(float);
	}

	/* Allocate memory for the indices */
	mdl->idx_num = idxnum;
	if(!(mdl->idx_buf = malloc(idxnum * sizeof(unsigned int))))
		goto err_set_failed;

	/* Allocate memory for the vertex-data */
	mdl->vtx_num = vtxnum;
	if(!(mdl->vtx_buf = malloc(vtxnum * vtx_size)))
		goto err_set_failed;

	/* Copy the indices into the allocated index-buffer */
	memcpy(mdl->idx_buf, idx, idxnum * sizeof(int));

	/* Create the vertex array and fill in the vertex-data */
	ptr = mdl->vtx_buf;
	for(i = 0; i < vtxnum; i++) {
		memcpy(ptr, vtx + (i * 3), VEC3_SIZE);
		ptr += VEC3_SIZE;

		memcpy(ptr, tex + (i * 2), VEC2_SIZE);
		ptr += VEC2_SIZE;

		memcpy(ptr, nrm + (i * 3), VEC3_SIZE);
		ptr += VEC3_SIZE;

		if(mdl->type >= MDL_RIG) {
			memcpy(ptr, jnt + (i * 4), INT4_SIZE);
			ptr += INT4_SIZE;

			memcpy(ptr, wgt + (i * 4), VEC4_SIZE);
			ptr += VEC4_SIZE;
		}

	}

	/* Bind vertex-array-object */
	glBindVertexArray(mdl->vao);

	/* Register the vertex-positions */
	glGenBuffers(1, &mdl->vtx_bao);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->vtx_bao);
	glBufferData(GL_ARRAY_BUFFER, vtxnum * vtx_size, mdl->vtx_buf, 
			GL_STATIC_DRAW);

	/* Bind the data to the vertices */
	/* Vertex-Position */
	p = NULL;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vtx_size, p);

	/* Tex-Coordinate */
	p = (void *)(3 * sizeof(float));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vtx_size, p);
	
	/* Normal-Vector */
	p = (void *)(5 * sizeof(float));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vtx_size, p);

	if(mdl->type >= MDL_RIG) {
		/* Joint-Index */
		p = (void *)(8 * sizeof(float));
		glVertexAttribIPointer(3, 4, GL_INT, vtx_size, p);

		/* Joint-Weights */
		p = (void *)(8 * sizeof(float) + 4 * sizeof(int));
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, vtx_size, p);
	}

	/* Register the indices */
	glGenBuffers(1, &mdl->idx_bao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->idx_bao);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxnum * sizeof(int), 
			mdl->idx_buf, GL_STATIC_DRAW);

	/* Unbind the vertex-array-object */
	glBindVertexArray(0);

	if((err = glGetError()) != GL_NO_ERROR) {
		ERR_LOG(("Failed to set model-data"));
	}

	return;

err_set_failed:
	mdl_set_status(slot, MDL_ERR_MESH);
}


extern void mdl_set_texture(short slot, short tex)
{
	struct model *mdl;

	if(mdl_check_slot(slot))
		return;

	mdl = models[slot];
	if(!mdl || mdl->status != MDL_OK)
		goto err_set_failed;

	mdl->tex = tex;
	return;

err_set_failed:
	mdl_set_status(slot, MDL_ERR_TEXTURE);
}


extern void mdl_set_shader(short slot, short shd)
{
	struct model *mdl;

	if(mdl_check_slot(slot))
		return;

	mdl = models[slot];
	if(!mdl || mdl->status != MDL_OK)
		goto err_set_failed;

	mdl->shd = shd;
	return;

err_set_failed:
	mdl_set_status(slot, MDL_ERR_SHADER);
}


extern short mdl_load(char *name, char *pth, short tex_slot, short shd_slot)
{
	struct amo_model *data;
	struct model *mdl;
	short slot;

	int vtxnum;
	float *vtx;
	float *tex;
	float *nrm;
	int *jnt;
	float *wgt;
	int idxnum;
	unsigned int *idx;

	int i;
	int j;

	/* Helper-variables */
	int tmp;
	struct mdl_anim *anim;
	struct mdl_keyfr *keyfr;
	struct amo_keyfr *amo_keyfr;


	/* Allocate memory for the model-struct */
	if((slot = mdl_set(name)) < 0)
		return -1;

	/* Get pointer to model */
	mdl = models[slot]; 

	/* Load a model from a file and write it to the data-struct */
	if(!(data = amo_load(pth)))
		goto err_del_mdl;

	/* Attach both the texture and shader to the model */
	mdl_set_texture(slot, tex_slot);
	mdl_set_shader(slot, shd_slot);

	/* 
	 * Get an OpenGL-mesh from the returned data-struct. This is required as
	 * OpenGL requires that each vertex contains a position, texture-coord,
	 * and normal-vector, which has to be filled in, as the returned struct
	 * only references the attributes for each vertex via the indices.
	 * Therefore this function sorts the data and writes each attribute-data
	 * for each unique vertex to the dedicated array and returns the final
	 * index-list which can then be used by OpenGL. Note: vtx, tex, nrm all
	 * have the same number of entries.
	 */
	amo_getdata(data, &vtxnum, (void **)&vtx, (void **)&tex, (void **)&nrm,
			(void **)&jnt, (void **)&wgt, &idxnum, &idx);

	/* Attach data to the model */
	mdl_set_data(slot, vtxnum, vtx, tex, nrm, jnt, wgt, idxnum, idx);

	/* Free the conversion-buffers */
	free(vtx);
	free(tex);
	free(nrm);
	free(jnt);
	free(wgt);
	free(idx);

	/* Copy joints */
	if(data->jnt_c > 0) {
		/* Copy number of joints */
		mdl->jnt_num = data->jnt_c;

		/* Allocate memory for joints */
		tmp = mdl->jnt_num * sizeof(struct mdl_joint);
		if(!(mdl->jnt_buf = malloc(tmp)))
			goto err_free_data;

		/* Copy joint-data */
		for(i = 0; i < mdl->jnt_num; i++) {
			/* Copy joint-name */
			strcpy(mdl->jnt_buf[i].name, data->jnt_lst[i].name);

			/* Set parent-joint-index */
			if(data->jnt_lst[i].par)
				tmp = data->jnt_lst[i].par->index;
			else 
				tmp = -1;
			mdl->jnt_buf[i].par = tmp;			
		}

		/* Allocate memory for joint-matrices */
		tmp = mdl->jnt_num * sizeof(float) * 16;
		if(!(mdl->jnt_mat = malloc(tmp)))
			goto err_free_data;

		/* Initialize the vertex-matrices with identity-matrices */
		mdl_anim_clear(slot);
	}

	/* Copy animations */
	if(data->ani_c > 0) {
		/* Copy number of animations */
		mdl->anim_num = data->ani_c;

		/* Allocate memory for animations */
		tmp = mdl->anim_num * sizeof(struct mdl_anim);
		if(!(mdl->anim_buf = malloc(tmp)))
			goto err_free_data;

		/* Initialize keyframe-buffers */
		for(i = 0; i < mdl->anim_num; i++) {
			/* Get shortcut-pointer */
			anim = &mdl->anim_buf[i];

			anim->keyfr_num = 0;
			anim->keyfr_buf = NULL;
		}

		/* Copy animation-keyframes */
		for(i = 0; i < mdl->anim_num; i++) {
			/* Get shortcut-pointer */
			anim = &mdl->anim_buf[i];

			/* Copy number of keyframes */
			anim->keyfr_num = data->ani_lst[i].keyfr_c;

			/* Allocate memory for keyframes */
			tmp = anim->keyfr_num * sizeof(struct mdl_keyfr);
			if(!(anim->keyfr_buf = malloc(tmp)))
				goto err_free_data;

			/* Initialize keyframe-joint-buffers */
			for(j = 0; j < anim->keyfr_num; j++) {
				/* Get shortcut-pointer */
				keyfr = &anim->keyfr_buf[j];

				keyfr->pos = NULL;
				keyfr->rot = NULL;
			}

			/* Copy keyframes */
			for(j = 0; j < anim->keyfr_num; j++) {
				/* Get shortcut-pointers */
				keyfr = &anim->keyfr_buf[j];
				amo_keyfr = &data->ani_lst[i].keyfr_lst[j];

				/* Allocate memory for location-data */
				tmp = mdl->jnt_num * sizeof(float) * 3;
				if(!(keyfr->pos = malloc(tmp)))
					goto err_free_data;

				/* Allocate memory for rotation-data */
				tmp = mdl->jnt_num * sizeof(float) * 4;
				if(!(keyfr->rot = malloc(tmp)))
					goto err_free_data;

				/* Copy timestamp */
				keyfr->ts = data->ani_lst[i].keyfr_lst[j].ts;

				/* Copy location-data */
				tmp = mdl->jnt_num * sizeof(float) * 3;
				memcpy(keyfr->pos, amo_keyfr->pos, tmp);

				/* Copy rotation-data */
				tmp = mdl->jnt_num * sizeof(float) * 4;
				memcpy(keyfr->rot, amo_keyfr->rot, tmp);
			}
		}
	}

	/* Set the status of the model */
	mdl_set_status(slot, MDL_OK);

	/* Return the returned data-struct */
	amo_destroy(data);

	/* Return the slot the model is on */
	return slot;

err_free_data:
	amo_destroy(data);

err_del_mdl:
	mdl_del(slot);
	return -1;
}


extern void mdl_render(short slot, mat4_t mat_pos, mat4_t mat_rot)
{
	char *vars[5] = {"mpos", "mrot", "view", "proj", "jnts"};
	int loc[5];
	mat4_t mpos, mrot, view, proj;
	struct model *mdl;
	int attr;
	int vari;


	if(mdl_check_slot(slot))
		return;

	mdl = models[slot];
	if(!mdl || mdl->status != MDL_OK)
		return;

	attr = (mdl->type >= MDL_RIG) ? (5) : (3);
	vari = (mdl->type >= MDL_RIG) ? (5) : (4);

	mat4_cpy(mpos, mat_pos);
	mat4_cpy(mrot, mat_rot);
	cam_get_view(view);
	cam_get_proj(proj);

	glBindVertexArray(mdl->vao);
	
	shd_use(mdl->shd, attr, vari, vars, loc);
	tex_use(mdl->tex);

	glUniformMatrix4fv(loc[0], 1, GL_FALSE, mpos);
	glUniformMatrix4fv(loc[1], 1, GL_FALSE, mrot);
	glUniformMatrix4fv(loc[2], 1, GL_FALSE, view);
	glUniformMatrix4fv(loc[3], 1, GL_FALSE, proj);
	if(mdl->type >= MDL_RIG)
		glUniformMatrix4fv(loc[4], 1, GL_FALSE, mdl->jnt_mat);

	glDrawElements(GL_TRIANGLES, mdl->idx_num, GL_UNSIGNED_INT, 0);

	tex_unuse();
	shd_unuse();
	glBindVertexArray(0);
}



extern void mdl_anim_clear(short slot)
{
	int i;
	struct model *mdl;

	if(mdl_check_slot(slot))
		return;

	mdl = models[slot];
	if(!mdl || mdl->status != MDL_OK)
		return;

	if(mdl->type < MDL_RIG)
		return;

	for(i = 0; i < mdl->jnt_num; i++)
		mat4_idt(mdl->jnt_mat + (i * 16));
}
