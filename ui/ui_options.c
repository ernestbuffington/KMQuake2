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

// ui_options.c -- the options menu

#include <ctype.h>
#include "../client/client.h"
#include "ui_local.h"

static menuframework_s	s_options_menu;
static menuaction_s		s_options_sound_section;
static menuaction_s		s_options_controls_section;
static menuaction_s		s_options_screen_section;
static menuaction_s		s_options_effects_section;
static menuaction_s		s_options_interface_section;
static menuaction_s		s_options_back_action;

#pragma region ======================= Menu item callbacks

static void MenuSoundFunc(void *unused)
{
	M_Menu_Options_Sound_f();
}

static void MenuControlsFunc(void *unused)
{
	M_Menu_Options_Controls_f();
}

static void MenuScreenFunc(void *unused)
{
	M_Menu_Options_Screen_f();
}

static void MenuEffectsFunc(void *unused)
{
	M_Menu_Options_Effects_f();
}

static void MenuInterfaceFunc(void *unused)
{
	M_Menu_Options_Interface_f();
}

#pragma endregion

void Options_MenuInit(void)
{
	const int x = -MENU_FONT_SIZE * 4; //mxd
	int y = 0; //mxd
	
	s_options_menu.x = SCREEN_WIDTH * 0.5f;
	s_options_menu.nitems = 0;

	s_options_sound_section.generic.type		= MTYPE_ACTION;
	s_options_sound_section.generic.flags		= QMF_LEFT_JUSTIFY;
	s_options_sound_section.generic.name		= "Sound";
	s_options_sound_section.generic.x			= x;
	s_options_sound_section.generic.y			= y;
	s_options_sound_section.generic.callback	= MenuSoundFunc;
	
	s_options_controls_section.generic.type		= MTYPE_ACTION;
	s_options_controls_section.generic.flags	= QMF_LEFT_JUSTIFY;
	s_options_controls_section.generic.name		= "Controls";
	s_options_controls_section.generic.x		= x;
	s_options_controls_section.generic.y		= y += 2 * MENU_FONT_SIZE;
	s_options_controls_section.generic.callback = MenuControlsFunc;
	
	s_options_screen_section.generic.type		= MTYPE_ACTION;
	s_options_screen_section.generic.flags		= QMF_LEFT_JUSTIFY;
	s_options_screen_section.generic.name		= "Screen";
	s_options_screen_section.generic.x			= x;
	s_options_screen_section.generic.y			= y += 2 * MENU_FONT_SIZE;
	s_options_screen_section.generic.callback	= MenuScreenFunc;

	s_options_effects_section.generic.type		= MTYPE_ACTION;
	s_options_effects_section.generic.flags		= QMF_LEFT_JUSTIFY;
	s_options_effects_section.generic.name		= "Effects";
	s_options_effects_section.generic.x			= x;
	s_options_effects_section.generic.y			= y += 2 * MENU_FONT_SIZE;
	s_options_effects_section.generic.callback	= MenuEffectsFunc;

	s_options_interface_section.generic.type	= MTYPE_ACTION;
	s_options_interface_section.generic.flags	= QMF_LEFT_JUSTIFY;
	s_options_interface_section.generic.name	= "Interface";
	s_options_interface_section.generic.x		= x;
	s_options_interface_section.generic.y		= y += 2 * MENU_FONT_SIZE;
	s_options_interface_section.generic.callback = MenuInterfaceFunc;

	s_options_back_action.generic.type			= MTYPE_ACTION;
	s_options_back_action.generic.flags			= QMF_LEFT_JUSTIFY;
	s_options_back_action.generic.name			= (UI_MenuDepth() == 0 ? MENU_BACK_CLOSE : MENU_BACK_TO_MAIN); //mxd
	s_options_back_action.generic.x				= UI_CenteredX(&s_options_back_action.generic, s_options_menu.x); //mxd. Was 0
	s_options_back_action.generic.y				= y += 3 * MENU_FONT_SIZE;
	s_options_back_action.generic.callback		= UI_BackMenu;

	Menu_AddItem(&s_options_menu, (void *)&s_options_sound_section);
	Menu_AddItem(&s_options_menu, (void *)&s_options_controls_section);
	Menu_AddItem(&s_options_menu, (void *)&s_options_screen_section);
	Menu_AddItem(&s_options_menu, (void *)&s_options_effects_section);
	Menu_AddItem(&s_options_menu, (void *)&s_options_interface_section);
	Menu_AddItem(&s_options_menu, (void *)&s_options_back_action);

	Menu_Center(&s_options_menu); //mxd
}

void Options_MenuDraw(void)
{
	Menu_DrawBanner("m_banner_options");

	Menu_AdjustCursor(&s_options_menu, 1);
	Menu_Draw(&s_options_menu);
}

const char *Options_MenuKey(int key)
{
	return Default_MenuKey(&s_options_menu, key);
}

void M_Menu_Options_f(void)
{
	Options_MenuInit();
	UI_PushMenu(Options_MenuDraw, Options_MenuKey);
}