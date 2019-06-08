/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// r_main.c

#include "r_local.h"
#include "vlights.h"

void R_Clear(void);

viddef_t	vid;

model_t		*r_worldmodel;

float		gldepthmin, gldepthmax;

glconfig_t	glConfig;
glstate_t	glState;
glmedia_t	glMedia;

entity_t	*currententity;
int			r_worldframe; // added for trans animations
model_t		*currentmodel;

cplane_t	frustum[4];

int			r_visframecount;	// bumped when going to a new PVS
int			r_framecount;		// used for dlight push checking

int			c_brush_calls, c_brush_surfs, c_brush_polys, c_alias_polys, c_part_polys;

float		v_blend[4];			// final blending color

int			maxsize;			// Nexus

void GL_Strings_f(void);

//
// view origin
//
vec3_t	vup;
vec3_t	vpn;
vec3_t	vright;
vec3_t	r_origin;

float	r_world_matrix[16];
float	r_base_world_matrix[16];


//
// screen size info
//
refdef_t	r_newrefdef;

int		r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

cvar_t	*gl_allow_software;
cvar_t  *gl_driver;
cvar_t	*gl_clear;

cvar_t	*con_font; // Psychospaz's console font size option
cvar_t	*con_font_size;
cvar_t	*alt_text_color;
cvar_t	*scr_netgraph_pos;

cvar_t	*r_norefresh;
cvar_t	*r_drawentities;
cvar_t	*r_drawworld;
cvar_t	*r_speeds;
cvar_t	*r_fullbright;
cvar_t	*r_novis;
cvar_t	*r_nocull;
cvar_t	*r_lerpmodels;
cvar_t	*r_ignorehwgamma; // hardware gamma
cvar_t	*r_lefthand;
cvar_t	*r_waterwave;	// water waves
cvar_t  *r_caustics;	// Barnes water caustics
cvar_t  *r_glows;		// texture glows

cvar_t	*r_dlights_normal; // lerped dlights on models
cvar_t	*r_model_shading;
cvar_t	*r_model_dlights;

cvar_t	*r_lightlevel;	// FIXME: This is a HACK to get the client's light level

cvar_t	*r_rgbscale; // Vic's RGB brightening

cvar_t	*r_vertex_arrays;

cvar_t	*r_ext_swapinterval;
cvar_t	*r_ext_multitexture;
cvar_t	*r_ext_draw_range_elements;
cvar_t	*r_ext_compiled_vertex_array;
cvar_t	*r_arb_texturenonpoweroftwo;	// Knightmare- non-power-of-two texture support
cvar_t	*r_nonpoweroftwo_mipmaps;		// Knightmare- non-power-of-two texture support
cvar_t	*r_ext_mtexcombine; // Vic's RGB brightening
cvar_t	*r_stencilTwoSide; // Echon's two-sided stenciling
cvar_t	*r_arb_fragment_program;
cvar_t	*r_arb_vertex_program;
cvar_t	*r_arb_vertex_buffer_object;
cvar_t	*r_pixel_shader_warp;	// allow disabling the nVidia water warp
cvar_t	*r_trans_lighting;		// disabling of lightmaps on trans surfaces
cvar_t	*r_warp_lighting;		// allow disabling of lighting on warp surfaces
cvar_t	*r_solidalpha;			// allow disabling of trans33+trans66 surface flag combining
cvar_t	*r_entity_fliproll;		// allow disabling of backwards alias model roll
cvar_t	*r_old_nullmodel;		// allow selection of nullmodel

cvar_t	*r_glass_envmaps; // Psychospaz's envmapping
cvar_t	*r_trans_surf_sorting; // trans bmodel sorting
cvar_t	*r_shelltype; // entity shells: 0 = solid, 1 = warp, 2 = spheremap
cvar_t	*r_ext_texture_compression; // Heffo - ARB Texture Compression
cvar_t	*r_screenshot_format;		// determines screenshot format
cvar_t	*r_screenshot_jpeg_quality;	// Heffo - JPEG Screenshots

//cvar_t	*r_motionblur;				// motionblur
cvar_t	*r_lightcutoff;	//** DMP - allow dynamic light cutoff to be user-settable

cvar_t	*r_dlightshadowmapscale; //mxd. 0 - disabled, 1 - 1 ray per 1 lightmap pixel, 2 - 1 ray per 2x2 lightmap pixels, 4 - 1 ray per 4x4 lightmap pixels etc.
cvar_t	*r_dlightshadowrange;   //mxd. Dynamic lights don't cast shadows when distance between camera and dlight is > r_dlightshadowrange.
cvar_t	*r_dlightnormalmapping; //mxd. Dynamic lights use normalmaps (1) or not (0).

cvar_t	*r_log;
cvar_t	*r_drawbuffer;
cvar_t	*r_lightmap;
cvar_t	*r_shadows;
cvar_t	*r_shadowalpha;
cvar_t	*r_shadowrange;
cvar_t	*r_shadowvolumes;
cvar_t	*r_stencil;
cvar_t	*r_transrendersort; // correct trasparent sorting
cvar_t	*r_particle_lighting;	// particle lighting
cvar_t	*r_particle_min;
cvar_t	*r_particle_max;
cvar_t	*r_particle_mode; //mxd

cvar_t	*r_particledistance;
cvar_t	*r_particle_overdraw;

cvar_t	*r_mode;
cvar_t	*r_dynamic;

cvar_t	*r_modulate;
cvar_t	*r_nobind;
cvar_t	*r_round_down;
cvar_t	*r_picmip;
cvar_t	*r_skymip;
cvar_t	*r_playermip;
cvar_t	*r_showtris;
cvar_t	*r_showbbox;	// show model bounding box
cvar_t	*r_ztrick;
cvar_t	*r_finish;
cvar_t	*r_cull;
cvar_t	*r_polyblend;
cvar_t	*r_flashblend;
cvar_t  *r_saturatelighting;
cvar_t	*r_swapinterval;
cvar_t	*r_texturemode;
cvar_t	*r_texturealphamode;
cvar_t	*r_texturesolidmode;
cvar_t	*r_anisotropic;
cvar_t	*r_anisotropic_avail;
cvar_t	*r_lockpvs;

cvar_t	*vid_fullscreen;
cvar_t	*vid_gamma;
cvar_t	*vid_ref;

cvar_t	*r_bloom; // BLOOMS

cvar_t	*r_skydistance; //Knightmare- variable sky range
cvar_t	*r_fog_skyratio; //Knightmare- variable sky fog ratio
cvar_t	*r_saturation; //** DMP


/*
=================
R_CullBox

Returns true if the box is completely outside the frustom
=================
*/
qboolean R_CullBox(vec3_t mins, vec3_t maxs)
{
	if (r_nocull->value)
		return false;

	for (int i = 0; i < 4; i++)
		if (BOX_ON_PLANE_SIDE(mins, maxs, &frustum[i]) == 2)
			return true;

	return false;
}


/*
============
R_PolyBlend
============
*/
void R_PolyBlend(void)
{
	if (!r_polyblend->value || !v_blend[3])
		return;

	GL_Disable(GL_ALPHA_TEST);
	GL_Enable(GL_BLEND);
	GL_Disable(GL_DEPTH_TEST);
	GL_DisableTexture(0);

    qglLoadIdentity();

	// FIXME: get rid of these
    qglRotatef(-90, 1, 0, 0);	    // put Z going up
    qglRotatef( 90, 0, 0, 1);	    // put Z going up

	rb_vertex = rb_index = 0;
	indexArray[rb_index++] = rb_vertex + 0;
	indexArray[rb_index++] = rb_vertex + 1;
	indexArray[rb_index++] = rb_vertex + 2;
	indexArray[rb_index++] = rb_vertex + 0;
	indexArray[rb_index++] = rb_vertex + 2;
	indexArray[rb_index++] = rb_vertex + 3;

	VA_SetElem3(vertexArray[rb_vertex], 10, 100, 100);
	VA_SetElem4(colorArray[rb_vertex], v_blend[0], v_blend[1], v_blend[2], v_blend[3]);
	rb_vertex++;

	VA_SetElem3(vertexArray[rb_vertex], 10, -100, 100);
	VA_SetElem4(colorArray[rb_vertex], v_blend[0], v_blend[1], v_blend[2], v_blend[3]);
	rb_vertex++;

	VA_SetElem3(vertexArray[rb_vertex], 10, -100, -100);
	VA_SetElem4(colorArray[rb_vertex], v_blend[0], v_blend[1], v_blend[2], v_blend[3]);
	rb_vertex++;

	VA_SetElem3(vertexArray[rb_vertex], 10, 100, -100);
	VA_SetElem4(colorArray[rb_vertex], v_blend[0], v_blend[1], v_blend[2], v_blend[3]);
	rb_vertex++;

	RB_RenderMeshGeneric(false);

	GL_Disable(GL_BLEND);
	GL_EnableTexture(0);
	//GL_Enable (GL_ALPHA_TEST);

	qglColor4f(1, 1, 1, 1);
}

//=======================================================================

int SignbitsForPlane(cplane_t *out)
{
	// for fast box on planeside test
	int bits = 0;
	for (int j = 0; j < 3; j++)
		if (out->normal[j] < 0)
			bits |= 1 << j;

	return bits;
}


void R_SetFrustum(void)
{
	// rotate VPN right by FOV_X/2 degrees
	RotatePointAroundVector(frustum[0].normal, vup, vpn, -(90 - r_newrefdef.fov_x / 2 ));
	
	// rotate VPN left by FOV_X/2 degrees
	RotatePointAroundVector(frustum[1].normal, vup, vpn, 90 - r_newrefdef.fov_x / 2);
	
	// rotate VPN up by FOV_X/2 degrees
	RotatePointAroundVector(frustum[2].normal, vright, vpn, 90 - r_newrefdef.fov_y / 2);
	
	// rotate VPN down by FOV_X/2 degrees
	RotatePointAroundVector(frustum[3].normal, vright, vpn, -(90 - r_newrefdef.fov_y / 2));

	for (int i = 0; i < 4; i++)
	{
		frustum[i].type = PLANE_ANYZ;
		frustum[i].dist = DotProduct(r_origin, frustum[i].normal);
		frustum[i].signbits = SignbitsForPlane(&frustum[i]);
	}
}

//=======================================================================

/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame(void)
{
	r_framecount++;

// build the transformation matrix for the given view angles
	VectorCopy(r_newrefdef.vieworg, r_origin);

	AngleVectors(r_newrefdef.viewangles, vpn, vright, vup);

// current viewcluster
	if (!(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
	{
		r_oldviewcluster = r_viewcluster;
		r_oldviewcluster2 = r_viewcluster2;
		mleaf_t *leaf = Mod_PointInLeaf (r_origin, r_worldmodel);
		r_viewcluster = r_viewcluster2 = leaf->cluster;

		// check above and below so crossing solid water doesn't draw wrong
		if (!leaf->contents)
		{	
			// look down a bit
			vec3_t	temp;

			VectorCopy(r_origin, temp);
			temp[2] -= 16;
			leaf = Mod_PointInLeaf(temp, r_worldmodel);
			if (!(leaf->contents & CONTENTS_SOLID) && leaf->cluster != r_viewcluster2)
				r_viewcluster2 = leaf->cluster;
		}
		else
		{	
			// look up a bit
			vec3_t	temp;

			VectorCopy(r_origin, temp);
			temp[2] += 16;
			leaf = Mod_PointInLeaf(temp, r_worldmodel);
			if (!(leaf->contents & CONTENTS_SOLID) && leaf->cluster != r_viewcluster2)
				r_viewcluster2 = leaf->cluster;
		}
	}

	for (int i = 0; i < 4; i++)
		v_blend[i] = r_newrefdef.blend[i];

	c_brush_calls = 0;
	c_brush_surfs = 0;
	c_brush_polys = 0;
	c_alias_polys = 0;
	c_part_polys = 0;

	// clear out the portion of the screen that the NOWORLDMODEL defines
	/*if ( r_newrefdef.rdflags & RDF_NOWORLDMODEL )
	{
		GL_Enable( GL_SCISSOR_TEST );
		qglClearColor( 0.3, 0.3, 0.3, 1 );
		qglScissor( r_newrefdef.x, vid.height - r_newrefdef.height - r_newrefdef.y, r_newrefdef.width, r_newrefdef.height );
		qglClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		qglClearColor( 1, 0, 0.5, 0.5 );
		GL_Disable( GL_SCISSOR_TEST );
	}*/
}


void MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	const GLdouble ymax = zNear * tan(fovy * M_PI / 360.0);
	const GLdouble ymin = -ymax;

	GLdouble xmin = ymin * aspect;
	GLdouble xmax = ymax * aspect;

	xmin += -(2 * glState.camera_separation) / zNear;
	xmax += -(2 * glState.camera_separation) / zNear;

	qglFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}


/*
=============
R_SetupGL
=============
*/
void R_SetupGL(void)
{
	//Knightmare- variable sky range
	static GLdouble farz;
	//end Knightmare

	// Knightmare- update r_modulate in real time
	if (r_modulate->modified && r_worldmodel) //Don't do this if no map is loaded
	{
		msurface_t *surf;
		int i;
		
		for (i = 0, surf = r_worldmodel->surfaces; i < r_worldmodel->numsurfaces; i++, surf++)
			surf->cached_light[0] = 0; 

		r_modulate->modified = 0; 
	}

	//
	// set up viewport
	//
	const int x = floor(r_newrefdef.x * vid.width / vid.width);
	const int x2 = ceil((r_newrefdef.x + r_newrefdef.width) * vid.width / vid.width);
	const int y = floor(vid.height - r_newrefdef.y * vid.height / vid.height);
	const int y2 = ceil(vid.height - (r_newrefdef.y + r_newrefdef.height) * vid.height / vid.height);

	const int w = x2 - x;
	const int h = y - y2;

	qglViewport(x, y2, w, h);

	// Knightmare- variable sky range
	// calc farz falue from skybox size
	if (r_skydistance->modified)
	{
		r_skydistance->modified = false;

		GLdouble boxsize = r_skydistance->value;
		boxsize -= 252 * ceil(boxsize / 2300);
		farz = 1.0;
		while (farz < boxsize) //make this a power of 2
		{
			farz *= 2.0;
			if (farz >= 65536) //don't make it larger than this
				break;
		}

		farz *= 2.0; //double since boxsize is distance from camera to edge of skybox, not total size of skybox

		R_UpdateFogVars(); //mxd
	}
	// end Knightmare

	//mxd. Update variable sky fogging ratio.
	if(r_fog_skyratio->modified)
	{
		r_fog_skyratio->modified = false;
		R_UpdateFogVars();
	}

	//
	// set up projection matrix
	//
	const float screenaspect = (float)r_newrefdef.width/r_newrefdef.height;
//	yfov = 2*atan((float)r_newrefdef.height/r_newrefdef.width)*180/M_PI;
	qglMatrixMode(GL_PROJECTION);
    qglLoadIdentity();

	//Knightmare- 12/26/2001- increase back clipping plane distance
    MYgluPerspective(r_newrefdef.fov_y,  screenaspect,  4, farz); //was 4096
	//end Knightmare

	GL_CullFace(GL_FRONT);

	qglMatrixMode(GL_MODELVIEW);
    qglLoadIdentity();

    qglRotatef(-90, 1, 0, 0);	    // put Z going up
    qglRotatef( 90, 0, 0, 1);	    // put Z going up
    qglRotatef(-r_newrefdef.viewangles[2], 1, 0, 0);
    qglRotatef(-r_newrefdef.viewangles[0], 0, 1, 0);
    qglRotatef(-r_newrefdef.viewangles[1], 0, 0, 1);
    qglTranslatef(-r_newrefdef.vieworg[0], -r_newrefdef.vieworg[1], -r_newrefdef.vieworg[2]);

//	if ( glState.camera_separation != 0 && glState.stereo_enabled )
//		qglTranslatef ( glState.camera_separation, 0, 0 );

	qglGetFloatv(GL_MODELVIEW_MATRIX, r_world_matrix);

	//
	// set drawing parms
	//
	if (r_cull->value)
		GL_Enable(GL_CULL_FACE);
	else
		GL_Disable(GL_CULL_FACE);

	GL_Disable(GL_BLEND);
	GL_Disable(GL_ALPHA_TEST);
	GL_Enable(GL_DEPTH_TEST);

	rb_vertex = rb_index = 0;
}


/*
=============
R_Clear
=============
*/
void R_Clear(void)
{
	GLbitfield clearBits = 0;	// bitshifter's consolidation

	if (gl_clear->value)
		clearBits |= GL_COLOR_BUFFER_BIT;

	if (r_ztrick->value)
	{
		static int trickframe;

	//	if (gl_clear->value)
	//		qglClear (GL_COLOR_BUFFER_BIT);

		trickframe++;
		if (trickframe & 1)
		{
			gldepthmin = 0;
			gldepthmax = 0.49999;
			GL_DepthFunc(GL_LEQUAL);
		}
		else
		{
			gldepthmin = 1;
			gldepthmax = 0.5;
			GL_DepthFunc(GL_GEQUAL);
		}
	}
	else
	{
	//	if (gl_clear->value)
	//		qglClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	else
	//		qglClear (GL_DEPTH_BUFFER_BIT);
		clearBits |= GL_DEPTH_BUFFER_BIT;

		gldepthmin = 0;
		gldepthmax = 1;
		GL_DepthFunc(GL_LEQUAL);
	}

	GL_DepthRange(gldepthmin, gldepthmax);

	// added stencil buffer
	if (glConfig.have_stencil)
	{
		if (r_shadows->value == 3) // BeefQuake R6 shadows
			qglClearStencil(0);
		else
			qglClearStencil(1);
	//	qglClear(GL_STENCIL_BUFFER_BIT);
		clearBits |= GL_STENCIL_BUFFER_BIT;
	}
//	GL_DepthRange (gldepthmin, gldepthmax);

	if (clearBits)	// bitshifter's consolidation
		qglClear(clearBits);
}


/*
=============
R_Flash
=============
*/
void R_Flash(void)
{
	R_PolyBlend();
}


/*
=============
R_DrawLastElements
=============
*/
void R_DrawLastElements(void)
{
	//if (parts_prerender)
	//	R_DrawParticles(parts_prerender);
	if (ents_trans)
		R_DrawEntitiesOnList(ents_trans);
}
//============================================


/*
================
R_RenderView

r_newrefdef must be set before the first call
================
*/
void R_RenderView(refdef_t *fd)
{
	if (r_norefresh->value)
		return;

	r_newrefdef = *fd;

	if (!r_worldmodel && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
		VID_Error(ERR_DROP, "R_RenderView: NULL worldmodel");

	if (r_speeds->value)
	{
		c_brush_calls = 0;
		c_brush_surfs = 0;
		c_brush_polys = 0;
		c_alias_polys = 0;
		c_part_polys = 0;
	}

	R_PushDlights();

	if (r_finish->value)
		qglFinish();

	R_SetupFrame();
	R_SetFrustum();
	R_SetupGL();
	R_SetFog();
	R_MarkLeaves();	// done here so we know if we're in water
	R_DrawWorld();

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL) // options menu
	{
		R_SuspendFog();

		R_DrawAllEntities(false);
		R_DrawAllParticles();

		R_ResumeFog();
	}
	else
	{
		GL_Disable(GL_ALPHA_TEST);

		R_RenderDlights();

		if (r_transrendersort->value)
		{
			//R_BuildParticleList();
			R_SortParticlesOnList();
			R_DrawAllDecals();
			//R_DrawAllEntityShadows();
			R_DrawSolidEntities();
			R_DrawEntitiesOnList(ents_trans);
		}
		else
		{
			R_DrawAllDecals();
			//R_DrawAllEntityShadows();
			R_DrawAllEntities(true);
		}

		R_DrawAllParticles();
		R_DrawEntitiesOnList(ents_viewweaps);

		R_ParticleStencil(1);
		R_DrawAlphaSurfaces();
		R_ParticleStencil(2);

		R_ParticleStencil(3);
		if (r_particle_overdraw->value) // redraw over alpha surfaces, those behind are occluded
			R_DrawAllParticles();
		R_ParticleStencil(4);

		// always draw vwep last...
		R_DrawEntitiesOnList(ents_viewweaps_trans);

		R_BloomBlend(fd);	// BLOOMS

		R_Flash();
	}
}


/*
================
R_SetGL2D
================
*/
void Con_DrawString(int x, int y, char *string, int alpha);
float SCR_ScaledVideo(float param);
#define	FONT_SIZE SCR_ScaledVideo(con_font_size->value)

void R_SetGL2D(void)
{
	// set 2D virtual screen size
	qglViewport(0,0, vid.width, vid.height);
	qglMatrixMode(GL_PROJECTION);
    qglLoadIdentity();
	qglOrtho(0, vid.width, vid.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
    qglLoadIdentity();
	GL_Disable(GL_DEPTH_TEST);
	GL_Disable(GL_CULL_FACE);
	GL_Disable(GL_BLEND);
	GL_Enable(GL_ALPHA_TEST);
	qglColor4f(1, 1, 1, 1);

	// Knightmare- draw r_speeds (modified from Echon's tutorial)
	if (r_speeds->value && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL)) // don't do this for options menu
	{
		char line[128];
		const int lines = (glConfig.multitexture ? 5 : 7);

		for (int i = 0; i < lines; i++)
		{
			int len = 0;
			switch (i)
			{
				case 0:	len = sprintf(line, S_COLOR_ALT S_COLOR_SHADOW"%5i wcall", c_brush_calls); break;
				case 1:	len = sprintf(line, S_COLOR_ALT S_COLOR_SHADOW"%5i wsurf", c_brush_surfs); break;
				case 2:	len = sprintf(line, S_COLOR_ALT S_COLOR_SHADOW"%5i wpoly", c_brush_polys); break;
				case 3: len = sprintf(line, S_COLOR_ALT S_COLOR_SHADOW"%5i epoly", c_alias_polys); break;
				case 4: len = sprintf(line, S_COLOR_ALT S_COLOR_SHADOW"%5i ppoly", c_part_polys); break;
				case 5: len = sprintf(line, S_COLOR_ALT S_COLOR_SHADOW"%5i tex  ", c_visible_textures); break;
				case 6: len = sprintf(line, S_COLOR_ALT S_COLOR_SHADOW"%5i lmaps", c_visible_lightmaps); break;
				default: break;
			}

			int x;
			if (scr_netgraph_pos->value)
				x = r_newrefdef.width - (len * FONT_SIZE + FONT_SIZE / 2);
			else
				x = FONT_SIZE / 2;

			const int y = r_newrefdef.height - (lines - i) * (FONT_SIZE + 2);
			Con_DrawString(x, y, line, 255);
		}
	}
}


/*
====================
R_SetLightLevel
====================
*/
void R_SetLightLevel(void)
{
	vec3_t shadelight;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	// save off light value for server to look at (BIG HACK!)
	R_LightPoint(r_newrefdef.vieworg, shadelight, false);

	// pick the greatest component, which should be the same as the mono value returned by software
	r_lightlevel->value = 150 * max(shadelight[0], max(shadelight[1], shadelight[2])); //mxd
}


/*
@@@@@@@@@@@@@@@@@@@@@
R_RenderFrame
@@@@@@@@@@@@@@@@@@@@@
*/
void R_RenderFrame(refdef_t *fd)
{
	R_RenderView(fd);
	R_SetLightLevel();
	R_SetGL2D();
}


void AssertCvarRange(cvar_t *var, float min, float max, qboolean isInteger)
{
	if (!var)
		return;

	if (isInteger && ((int)var->value != var->integer))
	{
		VID_Printf(PRINT_ALL, S_COLOR_YELLOW"Warning: cvar '%s' must be an integer (%f)\n", var->name, var->value);
		Cvar_Set(var->name, va("%d", var->integer));
	}

	if (var->value < min)
	{
		VID_Printf(PRINT_ALL, S_COLOR_YELLOW"Warning: cvar '%s' is out of range (%f < %f)\n", var->name, var->value, min);
		Cvar_Set(var->name, va("%f", min));
	}
	else if (var->value > max)
	{
		VID_Printf(PRINT_ALL, S_COLOR_YELLOW"Warning: cvar '%s' is out of range (%f > %f)\n", var->name, var->value, max);
		Cvar_Set(var->name, va("%f", max));
	}
}


void R_Register(void)
{
	// Added Psychospaz's console font size option
	con_font = Cvar_Get("con_font", "default", CVAR_ARCHIVE);
	con_font_size = Cvar_Get("con_font_size", "8", CVAR_ARCHIVE);
	alt_text_color = Cvar_Get("alt_text_color", "2", CVAR_ARCHIVE);
	scr_netgraph_pos = Cvar_Get("netgraph_pos", "0", CVAR_ARCHIVE);

	gl_driver = Cvar_Get("gl_driver", "opengl32", CVAR_ARCHIVE);
	gl_clear = Cvar_Get("gl_clear", "0", 0);

	r_lefthand = Cvar_Get("hand", "0", CVAR_USERINFO | CVAR_ARCHIVE);
	r_norefresh = Cvar_Get("r_norefresh", "0", CVAR_CHEAT);
	r_fullbright = Cvar_Get("r_fullbright", "0", CVAR_CHEAT);
	r_drawentities = Cvar_Get("r_drawentities", "1", 0);
	r_drawworld = Cvar_Get("r_drawworld", "1", CVAR_CHEAT);
	r_novis = Cvar_Get("r_novis", "0", CVAR_CHEAT);
	r_nocull = Cvar_Get("r_nocull", "0", CVAR_CHEAT);
	r_lerpmodels = Cvar_Get("r_lerpmodels", "1", 0);
	r_speeds = Cvar_Get("r_speeds", "0", 0);
	r_ignorehwgamma = Cvar_Get("r_ignorehwgamma", "0", CVAR_ARCHIVE);	// hardware gamma

	// lerped dlights on models
	r_dlights_normal = Cvar_Get("r_dlights_normal", "1", CVAR_ARCHIVE);
	r_model_shading = Cvar_Get("r_model_shading", "2", CVAR_ARCHIVE);
	r_model_dlights = Cvar_Get("r_model_dlights", "8", CVAR_ARCHIVE);

	r_lightlevel = Cvar_Get("r_lightlevel", "0", 0);
	// added Vic's RGB brightening
	r_rgbscale = Cvar_Get("r_rgbscale", "2", CVAR_ARCHIVE);

	r_waterwave = Cvar_Get("r_waterwave", "0", CVAR_ARCHIVE);
	r_caustics = Cvar_Get("r_caustics", "1", CVAR_ARCHIVE);
	r_glows = Cvar_Get("r_glows", "1", CVAR_ARCHIVE);

	// correct trasparent sorting
	r_transrendersort = Cvar_Get("r_transrendersort", "1", CVAR_ARCHIVE);
	r_particle_lighting = Cvar_Get("r_particle_lighting", "1.0", CVAR_ARCHIVE);
	r_particledistance = Cvar_Get("r_particledistance", "0", CVAR_ARCHIVE);
	r_particle_overdraw = Cvar_Get("r_particle_overdraw", "0", CVAR_ARCHIVE);
	r_particle_min = Cvar_Get("r_particle_min", "0", CVAR_ARCHIVE);
	r_particle_max = Cvar_Get("r_particle_max", "0", CVAR_ARCHIVE);
	r_particle_mode = Cvar_Get("r_particle_mode", "1", CVAR_ARCHIVE); //mxd. 0 - Vanilla, 1 - KMQ2

	r_modulate = Cvar_Get("r_modulate", "1", CVAR_ARCHIVE);
	r_log = Cvar_Get("r_log", "0", 0);
	r_mode = Cvar_Get("r_mode", "3", CVAR_ARCHIVE);
	r_lightmap = Cvar_Get("r_lightmap", "0", 0);
	r_shadows = Cvar_Get("r_shadows", "1", CVAR_ARCHIVE); //mxd. Was 0
	r_shadowalpha = Cvar_Get("r_shadowalpha", "0.4", CVAR_ARCHIVE);
	r_shadowrange = Cvar_Get("r_shadowrange", "768", CVAR_ARCHIVE);
	r_shadowvolumes = Cvar_Get("r_shadowvolumes", "0", CVAR_CHEAT);
	r_stencil = Cvar_Get("r_stencil", "1", CVAR_ARCHIVE);

	r_dynamic = Cvar_Get("r_dynamic", "1", 0);
	r_nobind = Cvar_Get("r_nobind", "0", CVAR_CHEAT);
	r_round_down = Cvar_Get("r_round_down", "1", 0);
	r_picmip = Cvar_Get("r_picmip", "0", 0);
	r_skymip = Cvar_Get("r_skymip", "0", 0);
	r_showtris = Cvar_Get("r_showtris", "0", CVAR_CHEAT);
	r_showbbox = Cvar_Get("r_showbbox", "0", CVAR_CHEAT); // show model bounding box
	r_ztrick = Cvar_Get("r_ztrick", "0", 0);
	r_finish = Cvar_Get("r_finish", "0", CVAR_ARCHIVE);
	r_cull = Cvar_Get("r_cull", "1", 0);
	r_polyblend = Cvar_Get("r_polyblend", "1", 0);
	r_flashblend = Cvar_Get("r_flashblend", "0", 0);
	r_playermip = Cvar_Get("r_playermip", "0", 0);
	r_texturemode = Cvar_Get("r_texturemode", "GL_NEAREST_MIPMAP_LINEAR", CVAR_ARCHIVE); //mxd. Was GL_LINEAR_MIPMAP_NEAREST
	r_texturealphamode = Cvar_Get("r_texturealphamode", "default", CVAR_ARCHIVE);
	r_texturesolidmode = Cvar_Get("r_texturesolidmode", "default", CVAR_ARCHIVE);
	r_anisotropic = Cvar_Get("r_anisotropic", "16", CVAR_ARCHIVE); //mxd. Was 0
	r_anisotropic_avail = Cvar_Get("r_anisotropic_avail", "0", 0);
	r_lockpvs = Cvar_Get("r_lockpvs", "0", 0);

	r_vertex_arrays = Cvar_Get("r_vertex_arrays", "1", CVAR_ARCHIVE);

	r_ext_swapinterval = Cvar_Get("r_ext_swapinterval", "1", CVAR_ARCHIVE);
	r_ext_multitexture = Cvar_Get("r_ext_multitexture", "1", CVAR_ARCHIVE);
	r_ext_draw_range_elements = Cvar_Get("r_ext_draw_range_elements", "1", CVAR_ARCHIVE /*| CVAR_LATCH*/);
	r_ext_compiled_vertex_array = Cvar_Get("r_ext_compiled_vertex_array", "1", CVAR_ARCHIVE);
	r_arb_texturenonpoweroftwo = Cvar_Get("r_arb_texturenonpoweroftwo", "1", CVAR_ARCHIVE /*| CVAR_LATCH*/);
	r_nonpoweroftwo_mipmaps = Cvar_Get("r_nonpoweroftwo_mipmaps", "1", CVAR_ARCHIVE /*| CVAR_LATCH*/);

	r_ext_mtexcombine = Cvar_Get("r_ext_mtexcombine", "1", CVAR_ARCHIVE); // added Vic's RGB brightening

	// Echon's two-sided stenciling
	r_stencilTwoSide = Cvar_Get("r_stencilTwoSide", "0", CVAR_ARCHIVE);

	r_arb_fragment_program = Cvar_Get("r_arb_fragment_program", "1", CVAR_ARCHIVE);
	r_arb_vertex_program = Cvar_Get("_arb_vertex_program", "1", CVAR_ARCHIVE);

	r_arb_vertex_buffer_object = Cvar_Get("r_arb_vertex_buffer_object", "1", CVAR_ARCHIVE);

	// allow disabling the nVidia water warp
	r_pixel_shader_warp = Cvar_Get("r_pixel_shader_warp", "1", CVAR_ARCHIVE);

	// allow disabling of lightmaps on trans surfaces
	r_trans_lighting = Cvar_Get("r_trans_lighting", "2", CVAR_ARCHIVE);

	// allow disabling of lighting on warp surfaces
	r_warp_lighting = Cvar_Get("r_warp_lighting", "1", CVAR_ARCHIVE);

	// allow disabling of trans33+trans66 surface flag combining
	r_solidalpha = Cvar_Get("r_solidalpha", "1", CVAR_ARCHIVE);

	// allow disabling of backwards alias model roll
	r_entity_fliproll = Cvar_Get("r_entity_fliproll", "1", CVAR_ARCHIVE);

	// allow selection of nullmodel
	r_old_nullmodel = Cvar_Get("r_old_nullmodel", "0", CVAR_ARCHIVE);

	// added Psychospaz's envmapping
	r_glass_envmaps = Cvar_Get("r_glass_envmaps", "1", CVAR_ARCHIVE);
	r_trans_surf_sorting = Cvar_Get("r_trans_surf_sorting", "0", CVAR_ARCHIVE);
	r_shelltype = Cvar_Get("r_shelltype", "1", CVAR_ARCHIVE);

	r_ext_texture_compression = Cvar_Get("r_ext_texture_compression", "0", CVAR_ARCHIVE); // Heffo - ARB Texture Compression

	r_screenshot_format = Cvar_Get("r_screenshot_format", "jpg", CVAR_ARCHIVE);	// determines screenshot format
	r_screenshot_jpeg_quality = Cvar_Get("r_screenshot_jpeg_quality", "85", CVAR_ARCHIVE); // Heffo - JPEG Screenshots

	//r_motionblur = Cvar_Get( "r_motionblur", "0", CVAR_ARCHIVE );	// motionblur

	r_drawbuffer = Cvar_Get("r_drawbuffer", "GL_BACK", 0);
	r_swapinterval = Cvar_Get("r_swapinterval", "1", CVAR_ARCHIVE);

	r_saturatelighting = Cvar_Get("r_saturatelighting", "0", 0);

	vid_fullscreen = Cvar_Get("vid_fullscreen", "1", CVAR_ARCHIVE);
	vid_gamma = Cvar_Get("vid_gamma", "0.8", CVAR_ARCHIVE); // was 1.0
	vid_ref = Cvar_Get("vid_ref", "gl", CVAR_ARCHIVE);

	r_bloom = Cvar_Get("r_bloom", "0", CVAR_ARCHIVE);	// BLOOMS

	r_skydistance = Cvar_Get("r_skydistance", "10000", CVAR_ARCHIVE); // variable sky range
	r_fog_skyratio = Cvar_Get("r_fog_skyratio", "10", CVAR_ARCHIVE);  // variable sky fog ratio
	r_saturation = Cvar_Get("r_saturation", "1.0", CVAR_ARCHIVE);	  //** DMP saturation setting (.89 good for nvidia)
	r_lightcutoff = Cvar_Get("r_lightcutoff", "0", CVAR_ARCHIVE);	  //** DMP dynamic light cutoffnow variable

	r_dlightshadowmapscale = Cvar_Get("r_dlightshadowmapscale", "2", CVAR_ARCHIVE); //mxd
	r_dlightshadowrange = Cvar_Get("r_dlightshadowrange", "768", CVAR_ARCHIVE); //mxd
	r_dlightnormalmapping = Cvar_Get("r_dlightnormalmapping", "1", CVAR_ARCHIVE); //mxd

	Cmd_AddCommand("imagelist", R_ImageList_f);
	Cmd_AddCommand("screenshot", R_ScreenShot_f);
	Cmd_AddCommand("screenshot_silent", R_ScreenShot_Silent_f);
	Cmd_AddCommand("modellist", Mod_Modellist_f);
	Cmd_AddCommand("gl_strings", GL_Strings_f);
	//	Cmd_AddCommand ("resetvertexlights", R_ResetVertextLights_f);
}


/*
==================
R_SetMode
==================
*/
qboolean R_SetMode (void)
{
	const qboolean fullscreen = vid_fullscreen->value;
	r_skydistance->modified = true; // skybox size variable

	// don't allow modes 0, 1, or 2
	/*if ((r_mode->value > -1) && (r_mode->value < 3))
		Cvar_SetValue( "r_mode", 3);*/

	vid_fullscreen->modified = false;
	r_mode->modified = false;

	rserr_t err = GLimp_SetMode(&vid.width, &vid.height, r_mode->value, fullscreen);
	if (err == rserr_ok)
	{
		glState.prev_mode = r_mode->value;
	}
	else
	{
		if (err == rserr_invalid_fullscreen)
		{
			Cvar_SetValue("vid_fullscreen", 0);
			vid_fullscreen->modified = false;
			VID_Printf(PRINT_ALL, S_COLOR_YELLOW"%s: fullscreen unavailable in this mode\n", __func__);
			err = GLimp_SetMode(&vid.width, &vid.height, r_mode->value, false);

			if (err == rserr_ok)
				return true;
		}
		else if (err == rserr_invalid_mode)
		{
			Cvar_SetValue("r_mode", glState.prev_mode);
			r_mode->modified = false;
			VID_Printf(PRINT_ALL, S_COLOR_YELLOW"%s: invalid mode\n", __func__);
		}

		// try setting it back to something safe
		err = GLimp_SetMode(&vid.width, &vid.height, glState.prev_mode, false);
		if (err != rserr_ok)
		{
			VID_Printf(PRINT_ALL, S_COLOR_YELLOW"%s: could not revert to safe mode\n", __func__);
			return false;
		}
	}

	return true;
}


/*
===============
StringContainsToken

A non-ambiguous alternative to strstr.
Useful for parsing the GL extension string.
Similar to code in Fruitz of Dojo Quake2 MacOSX Port.
===============
*/
qboolean StringContainsToken(const char *string, const char *findToken)
{
	if (!string || !findToken) 
		return false;

	if (strchr(findToken, ' ') != NULL || findToken[0] == 0)
		return false;

	const char *strPos = string;
	const int tokenLen = strlen(findToken);
	
	while (true)
	{
		char *tokPos = strstr(strPos, findToken);

		if (!tokPos)
			break;

		char *terminatorPos = tokPos + tokenLen;

		if ( (tokPos == strPos || *(tokPos - 1) == ' ') && (*terminatorPos == ' ' || *terminatorPos == 0) )
			return true;

		strPos = terminatorPos;
	}

	return false;
}


/*
===============
R_CheckGLExtensions

Grabs GL extensions
===============
*/
qboolean R_CheckGLExtensions()
{
	// OpenGL multitexture on GL 1.2.1 or later or GL_ARB_multitexture
	glConfig.multitexture = false;
	glConfig.max_texunits = 1;

	qglMultiTexCoord2fARB = (void *)qwglGetProcAddress("glMultiTexCoord2f");
	qglActiveTextureARB = (void *)qwglGetProcAddress("glActiveTexture");
	qglClientActiveTextureARB = (void *)qwglGetProcAddress("glClientActiveTexture");

	if (!qglMultiTexCoord2fARB || !qglActiveTextureARB || !qglClientActiveTextureARB)
	{
		VID_Printf(PRINT_ALL, "...OpenGL multitexture not found, checking for GL_ARB_multitexture\n");
	}
	else
	{
		VID_Printf(PRINT_ALL, "...using OpenGL multitexture\n");
		qglGetIntegerv(GL_MAX_TEXTURE_UNITS, &glConfig.max_texunits);
		VID_Printf(PRINT_ALL, "...GL_MAX_TEXTURE_UNITS: %i\n", glConfig.max_texunits);
		glConfig.multitexture = true;
	}

	if (!glConfig.multitexture && StringContainsToken(glConfig.extensions_string, "GL_ARB_multitexture"))
	{
		if (r_ext_multitexture->value)
		{
			qglMultiTexCoord2fARB = (void *)qwglGetProcAddress("glMultiTexCoord2fARB");
			qglActiveTextureARB = (void *)qwglGetProcAddress("glActiveTextureARB");
			qglClientActiveTextureARB = (void *)qwglGetProcAddress("glClientActiveTextureARB");

			if (!qglMultiTexCoord2fARB || !qglActiveTextureARB || !qglClientActiveTextureARB)
			{
				VID_Printf(PRINT_ALL, "..." S_COLOR_RED "GL_ARB_multitexture not properly supported!\n"S_COLOR_YELLOW"WARNING: glow/caustic texture effects not enabled\n");
			}
			else
			{
				VID_Printf(PRINT_ALL, "...using GL_ARB_multitexture\n");
				qglGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &glConfig.max_texunits);
				VID_Printf(PRINT_ALL, "...GL_MAX_TEXTURE_UNITS_ARB: %i\n", glConfig.max_texunits);
				glConfig.multitexture = true;
			}
		}
		else
		{
			VID_Printf(PRINT_ALL, "...ignoring GL_ARB_multitexture\n"S_COLOR_YELLOW"WARNING: glow/caustic texture effects not enabled\n");
		}
	}

	if (!glConfig.multitexture)
		VID_Printf(PRINT_ALL, "...GL_ARB_multitexture not found\n"S_COLOR_YELLOW"WARNING: glow/caustic texture effects not supported\n" );

	// GL_EXT_compiled_vertex_array
	// GL_SGI_compiled_vertex_array
	glConfig.extCompiledVertArray = false;
	if ( StringContainsToken(glConfig.extensions_string, "GL_EXT_compiled_vertex_array") || 
		 StringContainsToken(glConfig.extensions_string, "GL_SGI_compiled_vertex_array") )
	{
		if (r_ext_compiled_vertex_array->value)
		{
			qglLockArraysEXT = (void *)qwglGetProcAddress("glLockArraysEXT");
			qglUnlockArraysEXT = (void *)qwglGetProcAddress("glUnlockArraysEXT");

			if (!qglLockArraysEXT || !qglUnlockArraysEXT)
			{
				VID_Printf(PRINT_ALL, "..." S_COLOR_RED "GL_EXT/SGI_compiled_vertex_array not properly supported!\n");
				qglLockArraysEXT	= NULL;
				qglUnlockArraysEXT	= NULL;
			}
			else
			{
				VID_Printf(PRINT_ALL, "...enabling GL_EXT/SGI_compiled_vertex_array\n");
				glConfig.extCompiledVertArray = true;
			}
		}
		else
		{
			VID_Printf(PRINT_ALL, "...ignoring GL_EXT/SGI_compiled_vertex_array\n");
		}
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_EXT/SGI_compiled_vertex_array not found\n");
	}

	// glDrawRangeElements on GL 1.2 or higher or GL_EXT_draw_range_elements
	glConfig.drawRangeElements = false;
	if (glConfig.version_major >= 2 || (glConfig.version_major == 1 && glConfig.version_minor >= 2))
	{
		if (r_ext_draw_range_elements->value)
		{
			qglDrawRangeElementsEXT = (void *)qwglGetProcAddress("glDrawRangeElements");
			
			if (!qglDrawRangeElementsEXT)
				qglDrawRangeElementsEXT = (void *)qwglGetProcAddress("glDrawRangeElementsEXT");

			if (!qglDrawRangeElementsEXT)
			{
				VID_Printf(PRINT_ALL, "..." S_COLOR_RED "glDrawRangeElements not properly supported!\n");
			}
			else
			{
				VID_Printf(PRINT_ALL, "...using glDrawRangeElements\n");
				glConfig.drawRangeElements = true;
			}
		}
		else
		{
			VID_Printf(PRINT_ALL, "...ignoring glDrawRangeElements\n");
		}
	}
	else if (StringContainsToken(glConfig.extensions_string, "GL_EXT_draw_range_elements"))
	{
		if (r_ext_draw_range_elements->value)
		{
			qglDrawRangeElementsEXT = (void *)qwglGetProcAddress("glDrawRangeElementsEXT");

			if (!qglDrawRangeElementsEXT)
				qglDrawRangeElementsEXT = (void *)qwglGetProcAddress("glDrawRangeElements");

			if (!qglDrawRangeElementsEXT)
			{
				VID_Printf(PRINT_ALL, "..." S_COLOR_RED "GL_EXT_draw_range_elements not properly supported!\n");
			}
			else
			{
				VID_Printf(PRINT_ALL, "...enabling GL_EXT_draw_range_elements\n");
				glConfig.drawRangeElements = true;
			}
		}
		else
		{
			VID_Printf(PRINT_ALL, "...ignoring GL_EXT_draw_range_elements\n");
		}
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_EXT_draw_range_elements not found\n");
	}

	// GL_ARB_texture_non_power_of_two
	glConfig.arbTextureNonPowerOfTwo = false;
	if (StringContainsToken(glConfig.extensions_string, "GL_ARB_texture_non_power_of_two"))
	{
		if (r_arb_texturenonpoweroftwo->value)
		{
			VID_Printf(PRINT_ALL, "...using GL_ARB_texture_non_power_of_two\n");
			glConfig.arbTextureNonPowerOfTwo = true;
		}
		else
		{
			VID_Printf(PRINT_ALL, "...ignoring GL_ARB_texture_non_power_of_two\n");
		}

	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_ARB_texture_non_power_of_two not found\n");
	}

#ifdef _WIN32
	// WGL_EXT_swap_control
	if (StringContainsToken(glConfig.extensions_string, "WGL_EXT_swap_control"))
	{
		qwglSwapIntervalEXT = (BOOL(WINAPI *)(int))qwglGetProcAddress("wglSwapIntervalEXT");
		VID_Printf(PRINT_ALL, "...enabling WGL_EXT_swap_control\n");
	}
	else
	{
		VID_Printf(PRINT_ALL, "...WGL_EXT_swap_control not found\n");
	}
#endif

	// GL_ARB_texture_env_combine - Vic
	glConfig.mtexcombine = false;
	if (StringContainsToken(glConfig.extensions_string, "GL_ARB_texture_env_combine"))
	{
		if (r_ext_mtexcombine->value)
		{
			VID_Printf(PRINT_ALL, "...using GL_ARB_texture_env_combine\n");
			glConfig.mtexcombine = true;
		}
		else
		{
			VID_Printf(PRINT_ALL, "..ignoring GL_ARB_texture_env_combine\n");
		}
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_ARB_texture_env_combine not found\n");
	}

	// GL_EXT_stencil_wrap
	glConfig.extStencilWrap = false;
	if (StringContainsToken(glConfig.extensions_string, "GL_EXT_stencil_wrap"))
	{
		VID_Printf(PRINT_ALL, "...using GL_EXT_stencil_wrap\n");
		glConfig.extStencilWrap = true;
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_EXT_stencil_wrap not found\n");
	}

	// GL_ATI_separate_stencil - Barnes
	glConfig.atiSeparateStencil = false;
	if (StringContainsToken(glConfig.extensions_string, "GL_ATI_separate_stencil"))
	{
		if (r_stencilTwoSide->value)
		{
			qglStencilOpSeparateATI = (void *)qwglGetProcAddress("glStencilOpSeparateATI");
			qglStencilFuncSeparateATI = (void *)qwglGetProcAddress("glStencilFuncSeparateATI");

			if (!qglStencilOpSeparateATI)
			{
				VID_Printf(PRINT_ALL, "..." S_COLOR_RED "GL_ATI_separate_stencil not properly supported!\n");
				qglStencilOpSeparateATI = NULL;
			}
			else
			{
				VID_Printf(PRINT_ALL, "...using GL_ATI_separate_stencil\n");
				glConfig.atiSeparateStencil = true;
			}
		}
		else
		{
			VID_Printf(PRINT_ALL, "...ignoring GL_ATI_separate_stencil\n");
		}
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_ATI_separate_stencil not found\n");
	}

	// GL_EXT_stencil_two_side - Echon
	glConfig.extStencilTwoSide = false;
	if (StringContainsToken(glConfig.extensions_string, "GL_EXT_stencil_two_side"))
	{
		if (r_stencilTwoSide->value)
		{
			qglActiveStencilFaceEXT = (void *)qwglGetProcAddress("glActiveStencilFaceEXT");

			if (!qglActiveStencilFaceEXT)
			{
				VID_Printf(PRINT_ALL, "..." S_COLOR_RED "GL_EXT_stencil_two_side not properly supported!\n");
				qglActiveStencilFaceEXT = NULL;
			}
			else
			{
				VID_Printf(PRINT_ALL, "...using GL_EXT_stencil_two_side\n");
				glConfig.extStencilTwoSide = true;
			}
		}
		else
		{
			VID_Printf(PRINT_ALL, "...ignoring GL_EXT_stencil_two_side\n");
		}
	}
	else
	{
		Com_Printf("...GL_EXT_stencil_two_side not found\n");
	}

	// GL_ARB_fragment_program
	glConfig.arb_fragment_program = false;
	if (StringContainsToken(glConfig.extensions_string, "GL_ARB_fragment_program"))
	{
		if (r_arb_fragment_program->value)
		{
			qglProgramStringARB = (void *)qwglGetProcAddress("glProgramStringARB");
			qglBindProgramARB = (void *)qwglGetProcAddress("glBindProgramARB");
			qglDeleteProgramsARB = (void *)qwglGetProcAddress("glDeleteProgramsARB");
			qglGenProgramsARB = (void *)qwglGetProcAddress("glGenProgramsARB");
			qglProgramEnvParameter4dARB = (void *)qwglGetProcAddress("glProgramEnvParameter4dARB");
			qglProgramEnvParameter4dvARB = (void *)qwglGetProcAddress("glProgramEnvParameter4dvARB");
			qglProgramEnvParameter4fARB = (void *)qwglGetProcAddress("glProgramEnvParameter4fARB");
			qglProgramEnvParameter4fvARB = (void *)qwglGetProcAddress("glProgramEnvParameter4fvARB");
			qglProgramLocalParameter4dARB = (void *)qwglGetProcAddress("glProgramLocalParameter4dARB");
			qglProgramLocalParameter4dvARB = (void *)qwglGetProcAddress("glProgramLocalParameter4dvARB");
			qglProgramLocalParameter4fARB = (void *)qwglGetProcAddress("glProgramLocalParameter4fARB");
			qglProgramLocalParameter4fvARB = (void *)qwglGetProcAddress("glProgramLocalParameter4fvARB");
			qglGetProgramEnvParameterdvARB = (void *)qwglGetProcAddress("glGetProgramEnvParameterdvARB");
			qglGetProgramEnvParameterfvARB = (void *)qwglGetProcAddress("glGetProgramEnvParameterfvARB");
			qglGetProgramLocalParameterdvARB = (void *)qwglGetProcAddress("glGetProgramLocalParameterdvARB");
			qglGetProgramLocalParameterfvARB = (void *)qwglGetProcAddress("glGetProgramLocalParameterfvARB");
			qglGetProgramivARB = (void *)qwglGetProcAddress("glGetProgramivARB");
			qglGetProgramStringARB = (void *)qwglGetProcAddress("glGetProgramStringARB");
			qglIsProgramARB = (void *)qwglGetProcAddress("glIsProgramARB");

			if (!qglProgramStringARB || !qglBindProgramARB
				|| !qglDeleteProgramsARB || !qglGenProgramsARB
				|| !qglProgramEnvParameter4dARB || !qglProgramEnvParameter4dvARB
				|| !qglProgramEnvParameter4fARB || !qglProgramEnvParameter4fvARB
				|| !qglProgramLocalParameter4dARB || !qglProgramLocalParameter4dvARB
				|| !qglProgramLocalParameter4fARB || !qglProgramLocalParameter4fvARB
				|| !qglGetProgramEnvParameterdvARB || !qglGetProgramEnvParameterfvARB
				|| !qglGetProgramLocalParameterdvARB || !qglGetProgramLocalParameterfvARB
				|| !qglGetProgramivARB || !qglGetProgramStringARB || !qglIsProgramARB)
			{
				VID_Printf(PRINT_ALL, "..." S_COLOR_RED "GL_ARB_fragment_program not properly supported!\n");
			}
			else
			{
				VID_Printf(PRINT_ALL, "...using GL_ARB_fragment_program\n");
				glConfig.arb_fragment_program = true;
			}
		}
		else
		{
			VID_Printf(PRINT_ALL, "...ignoring GL_ARB_fragment_program\n");
		}
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_ARB_fragment_program not found\n");
	}

	// GL_ARB_vertex_program
	glConfig.arb_vertex_program = false;
	if (glConfig.arb_fragment_program)
	{
		if (StringContainsToken(glConfig.extensions_string, "GL_ARB_vertex_program"))
		{
			if (r_arb_vertex_program->value)
			{
				qglGetVertexAttribdvARB = (void *)qwglGetProcAddress("glGetVertexAttribdvARB");
				qglGetVertexAttribfvARB = (void *)qwglGetProcAddress("glGetVertexAttribfvARB");
				qglGetVertexAttribivARB = (void *)qwglGetProcAddress("glGetVertexAttribivARB");
				qglGetVertexAttribPointervARB = (void *)qwglGetProcAddress("glGetVertexAttribPointervARB");

				if (!qglGetVertexAttribdvARB || !qglGetVertexAttribfvARB || !qglGetVertexAttribivARB || !qglGetVertexAttribPointervARB)
				{
					VID_Printf(PRINT_ALL, "..." S_COLOR_RED "GL_ARB_vertex_program not properly supported!\n");
				}
				else 
				{
					VID_Printf(PRINT_ALL, "...using GL_ARB_vertex_program\n");
					glConfig.arb_vertex_program = true;
				}
			}
			else
			{
				VID_Printf(PRINT_ALL, "...ignoring GL_ARB_vertex_program\n");
			}
		}
		else
		{
			VID_Printf(PRINT_ALL, "...GL_ARB_vertex_program not found\n");
		}
	}

	R_Compile_ARB_Programs();

	// GL_NV_texture_shader - MrG
	if (StringContainsToken(glConfig.extensions_string, "GL_NV_texture_shader"))
	{
		VID_Printf(PRINT_ALL, "...using GL_NV_texture_shader\n");
		glConfig.NV_texshaders = true;
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_NV_texture_shader not found\n");
		glConfig.NV_texshaders = false;
	}

	// GL_EXT_texture_filter_anisotropic - NeVo
	glConfig.anisotropic = false;
	if (StringContainsToken(glConfig.extensions_string, "GL_EXT_texture_filter_anisotropic"))
	{
		VID_Printf(PRINT_ALL, "...using GL_EXT_texture_filter_anisotropic\n");
		glConfig.anisotropic = true;
		qglGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glConfig.max_anisotropy);
		Cvar_SetValue("r_anisotropic_avail", glConfig.max_anisotropy);
	}
	else
	{
		VID_Printf(PRINT_ALL, "..GL_EXT_texture_filter_anisotropic not found\n");
		glConfig.anisotropic = false;
		glConfig.max_anisotropy = 0.0;
		Cvar_SetValue("r_anisotropic_avail", 0.0);
	}

	// GL_SGIS_generate_mipmap
	if (StringContainsToken(glConfig.extensions_string, "GL_SGIS_generate_mipmap"))
	{
		VID_Printf(PRINT_ALL, "...using GL_SGIS_generate_mipmap\n");
		glState.sgis_mipmap = true;
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_SGIS_generate_mipmap not found\n");
		glState.sgis_mipmap = false;
	}

	// GL_ARB_texture_compression - Heffo
	if (StringContainsToken(glConfig.extensions_string, "GL_ARB_texture_compression"))
	{
		if (!r_ext_texture_compression->value)
		{
			VID_Printf(PRINT_ALL, "...ignoring GL_ARB_texture_compression\n");
			glState.texture_compression = false;
		}
		else
		{
			VID_Printf(PRINT_ALL, "...using GL_ARB_texture_compression\n");
			glState.texture_compression = true;
		}
	}
	else
	{
		VID_Printf(PRINT_ALL, "...GL_ARB_texture_compression not found\n");
		glState.texture_compression = false;
		Cvar_Set("r_ext_texture_compression", "0");
	}

	return true;
}


/*
===============
R_Init
===============
*/
qboolean R_Init(void *hinstance, void *hWnd, char *reason)
{	
	Draw_GetPalette();
	R_Register();

	// place default error
	memcpy(reason, "Unknown failure on intialization!\0", 34);

	// initialize our QGL dynamic bindings
	if (!QGL_Init(gl_driver->string))
	{
		QGL_Shutdown();
        VID_Printf(PRINT_ALL, "R_Init() - could not load \"%s\"\n", gl_driver->string);
		memcpy(reason, "Init of QGL dynamic bindings Failed!\0", 37);
		return false;
	}

	// initialize OS-specific parts of OpenGL
	if (!GLimp_Init(hinstance, hWnd))
	{
		QGL_Shutdown();
		memcpy(reason, "Init of OS-specific parts of OpenGL Failed!\0", 44);
		return false;
	}

	// set our "safe" modes
	glState.prev_mode = 3;

	// create the window and set up the context
	if (!R_SetMode())
	{
		QGL_Shutdown();
        VID_Printf(PRINT_ALL, "R_Init() - could not R_SetMode()\n");
		memcpy(reason, "Creation of the window/context set-up Failed!\0", 46);
		return false;
	}

	RB_InitBackend(); // init mini-backend


	//
	// get our various GL strings
	//
	glConfig.vendor_string = qglGetString(GL_VENDOR);
	VID_Printf(PRINT_ALL, "GL_VENDOR: %s\n", glConfig.vendor_string);
	glConfig.renderer_string = qglGetString(GL_RENDERER);
	VID_Printf(PRINT_ALL, "GL_RENDERER: %s\n", glConfig.renderer_string);
	glConfig.version_string = qglGetString(GL_VERSION);
	sscanf(glConfig.version_string, "%d.%d.%d", &glConfig.version_major, &glConfig.version_minor, &glConfig.version_release);
	VID_Printf(PRINT_ALL, "GL_VERSION: %s\n", glConfig.version_string);

	//mxd. We need a GPU
	if (!strcmp(glConfig.renderer_string, "gdi generic"))
	{
		QGL_Shutdown();
		memcpy(reason, "No hardware acceleration detected.\nPlease install drivers provided by your video card/GPU vendor.\0", 98);
		return false;
	}

	//mxd. We need at least OpenGL 1.4
	if(glConfig.version_major == 1 && glConfig.version_minor < 4)
	{
		QGL_Shutdown();
		memcpy(reason, "Support for OpenGL 1.4 is not available!\0", 41);
		return false;
	}

	// Knighmare- added max texture size
	qglGetIntegerv(GL_MAX_TEXTURE_SIZE, &glConfig.max_texsize);
	VID_Printf(PRINT_ALL, "GL_MAX_TEXTURE_SIZE: %i\n", glConfig.max_texsize);

	glConfig.extensions_string = (const char*)qglGetString(GL_EXTENSIONS);
	
	if (developer->value > 0)	// print extensions 2 to a line
	{
		unsigned line = 0;

		VID_Printf(PRINT_DEVELOPER, "GL_EXTENSIONS: ");
		char *extString = (char *)glConfig.extensions_string;
		while (true)
		{
			char *extTok = COM_Parse(&extString);
			if (!extTok[0])
				break;

			line++;
			if (line % 2 == 0)
				VID_Printf(PRINT_DEVELOPER, "%s\n", extTok);
			else
				VID_Printf(PRINT_DEVELOPER, "%s ", extTok);
		}

		if (line % 2 != 0)
			VID_Printf(PRINT_DEVELOPER, "\n");
	}

	Cvar_Set("scr_drawall", "0");

#ifdef __linux__
	Cvar_SetValue("r_finish", 1);
#else
	Cvar_SetValue("r_finish", 0);
#endif

	r_swapinterval->modified = true; // force swapinterval update

	//
	// grab extensions
	//
	if (!R_CheckGLExtensions())
	{
		QGL_Shutdown(); //mxd
		return false;
	}

	GL_SetDefaultState();

	R_InitImages();
	Mod_Init();
	R_InitMedia();
	R_DrawInitLocal();

	R_InitDSTTex(); // init shader warp texture
	R_InitFogVars(); // reset fog variables
	VLight_Init(); // Vic's bmodel lights

	const int err = qglGetError();
	if (err != GL_NO_ERROR)
		VID_Printf(PRINT_ALL, "glGetError() = 0x%x\n", err);

	return true;
}


/*
===============
R_ClearState
===============
*/
void R_ClearState(void)
{	
	R_SetFogVars(false, 0, 0, 0, 0, 0, 0, 0); // clear fog effets
	GL_EnableMultitexture(false);
	GL_SetDefaultState();
}


/*
=================
GL_Strings_f
=================
*/
void GL_Strings_f(void)
{
	unsigned line = 0;

	VID_Printf(PRINT_ALL, "GL_VENDOR: %s\n", glConfig.vendor_string);
	VID_Printf(PRINT_ALL, "GL_RENDERER: %s\n", glConfig.renderer_string);
	VID_Printf(PRINT_ALL, "GL_VERSION: %s\n", glConfig.version_string);
	VID_Printf(PRINT_ALL, "GL_MAX_TEXTURE_SIZE: %i\n", glConfig.max_texsize);
	VID_Printf(PRINT_ALL, "GL_EXTENSIONS: ");

	char *extString = (char *)glConfig.extensions_string;
	while (true)
	{
		char *extTok = COM_Parse(&extString);
		if (!extTok[0])
			break;

		line++;
		if ((line % 2) == 0)
			VID_Printf(PRINT_ALL, "%s\n", extTok);
		else
			VID_Printf(PRINT_ALL, "%s ", extTok);
	}

	if ((line % 2) != 0)
		VID_Printf(PRINT_ALL, "\n");
}


/*
===============
R_Shutdown
===============
*/
void R_Shutdown(void)
{	
	Cmd_RemoveCommand("modellist");
	Cmd_RemoveCommand("screenshot");
	Cmd_RemoveCommand("screenshot_silent");
	Cmd_RemoveCommand("imagelist");
	Cmd_RemoveCommand("gl_strings");
//	Cmd_RemoveCommand("resetvertexlights");

	// Knightmare- Free saveshot buffer
	free(saveshotdata);
	saveshotdata = NULL; // Make sure this is null after a vid restart!

	Mod_FreeAll();

	R_ShutdownImages();
	R_ClearDisplayLists();

	// Shut down OS-specific OpenGL stuff like contexts, etc.
	GLimp_Shutdown();

	// Shutdown our QGL subsystem
	QGL_Shutdown();
}



/*
@@@@@@@@@@@@@@@@@@@@@
R_BeginFrame
@@@@@@@@@@@@@@@@@@@@@
*/
void UpdateGammaRamp(void); //Knightmare added
void RefreshFont(void);

void R_BeginFrame(float camera_separation)
{
	glState.camera_separation = camera_separation;

	// Knightmare- added Psychospaz's console font size option
	if (con_font->modified)
		RefreshFont();

	if (con_font_size->modified)
	{
		if (con_font_size->value < 4)
			Cvar_Set("con_font_size", "4");
		else if (con_font_size->value > 24)
			Cvar_Set("con_font_size", "24");

		con_font_size->modified = false;
	}
	// end Knightmare

	// Change modes if necessary
	if (r_mode->modified || vid_fullscreen->modified)
	{
		// FIXME: only restart if CDS is required
		cvar_t *ref = Cvar_Get("vid_ref", "gl", 0);
		ref->modified = true;
	}

	if (r_log->modified)
	{
		GLimp_EnableLogging(r_log->integer);
		r_log->modified = false;
	}

	if (r_log->integer)
		GLimp_LogNewFrame();

	// Update gamma
	if (vid_gamma->modified)
	{
		vid_gamma->modified = false;
		UpdateGammaRamp();
	}

	GLimp_BeginFrame(camera_separation);

	// Go into 2D mode
	qglViewport(0, 0, vid.width, vid.height);
	qglMatrixMode(GL_PROJECTION);
	qglLoadIdentity();
	qglOrtho(0, vid.width, vid.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
	qglLoadIdentity();

	GL_Disable(GL_DEPTH_TEST);
	GL_Disable(GL_CULL_FACE);
	GL_Disable(GL_BLEND);
	GL_Enable(GL_ALPHA_TEST);
	qglColor4f(1, 1, 1, 1);

	// Draw buffer stuff
	if (r_drawbuffer->modified)
	{
		r_drawbuffer->modified = false;

		if (glState.camera_separation == 0 || !glState.stereo_enabled)
		{
			if (Q_stricmp(r_drawbuffer->string, "GL_FRONT") == 0)
				qglDrawBuffer(GL_FRONT);
			else
				qglDrawBuffer(GL_BACK);
		}
	}

	// Texturemode stuff
	if (r_texturemode->modified || (glConfig.anisotropic && r_anisotropic->modified)) //mxd. https://github.com/yquake2/yquake2/blob/1e3135d4fc306b6085d607afdf766a9514235b0b/src/client/refresh/gl1/gl1_main.c#L1705
	{
		GL_TextureMode(r_texturemode->string);
		r_texturemode->modified = false;
		r_anisotropic->modified = false;
	}

	if (r_texturealphamode->modified)
	{
		GL_TextureAlphaMode(r_texturealphamode->string);
		r_texturealphamode->modified = false;
	}

	if (r_texturesolidmode->modified)
	{
		GL_TextureSolidMode(r_texturesolidmode->string);
		r_texturesolidmode->modified = false;
	}

	// Swapinterval stuff
	GL_UpdateSwapInterval();

	// Clear screen if desired
	R_Clear();
}