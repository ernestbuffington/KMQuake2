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
// vid.h -- video driver defs

#pragma once

typedef struct vrect_s
{
	int x;
	int y;
	int width;
	int height;
} vrect_t;

typedef struct
{
	int width; // Coordinates from main game
	int height;
} viddef_t;

extern viddef_t viddef; // Global video state

// Video module initialisation etc
void VID_Init(void);
void VID_Shutdown(void);
void VID_CheckChanges(void);

// Stuff provided by platform backend.
qboolean GLimp_Init();
void GLimp_GrabInput(qboolean grab);
qboolean GLimp_GetWindowPosition(int *x, int *y);
qboolean GLimp_SetWindowPosition(const int x, const int y);
void GLimp_AppActivate();