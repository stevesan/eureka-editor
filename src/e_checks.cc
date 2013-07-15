//------------------------------------------------------------------------
//  INTEGRITY CHECKS
//------------------------------------------------------------------------
//
//  Eureka DOOM Editor
//
//  Copyright (C) 2001-2013 Andrew Apted
//  Copyright (C) 1997-2003 Andr� Majorel et al
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//------------------------------------------------------------------------
//
//  Based on Yadex which incorporated code from DEU 5.21 that was put
//  in the public domain in 1994 by Rapha�l Quinet and Brendon Wyber.
//
//------------------------------------------------------------------------

#include "main.h"

#include <algorithm>

#include "e_checks.h"
#include "e_path.h"
#include "e_vertex.h"
#include "editloop.h"
#include "m_dialog.h"
#include "m_game.h"
#include "levels.h"
#include "objects.h"
#include "selectn.h"
#include "w_rawdef.h"
#include "ui_window.h"
#include "x_hover.h"


#define   ERROR_MSG_COLOR	FL_RED
#define WARNING_MSG_COLOR	FL_BLUE


static char check_message[MSG_BUF_LEN];


static void CheckingObjects ();

// TODO: REMOVE THIS
#define Expert  true


/*
 *  CheckLevel
 *  check the level consistency
 *  FIXME should be integrated into editloop.cc
 */
void CheckLevel (int x0, int y0)
{
#if 0 // TODO: CheckLevel

	const char *line5 = 0;

	if (Registered)
	{
		/* Commented out AYM 19971130 - wouldn't work with Doom 2
		   if (! FindMasterDir (MasterDir, "TEXTURE2"))
		   NumVertexes--;
		   else
		   */
		line5 = "Check texture names";
	}
	switch (vDisplayMenu (x0, y0, "Check level consistency",
				"Number of objects",   YK_, 0,
				"Check if all Sectors are closed", YK_, 0,
				"Check all cross-references",  YK_, 0,
				"Check for missing textures",  YK_, 0,
				line5,       YK_, 0,
				NULL))
	{
		case 1:
			Statistics ();
			break;
		case 2:
			CheckSectors ();
			break;
		case 3:
			CheckCrossReferences ();
			break;
		case 4:
			CheckTextures ();
			break;
		case 5:
			CheckTextureNames ();
			break;
	}
#endif
}


/*
   display a message while the user is waiting...
   */

static void CheckingObjects ()
{
	//DisplayMessage (-1, -1, "Grinding...");
}


/*
   display a message, then ask if the check should continue (prompt2 may be NULL)
   */

bool CheckFailed (int x0, int y0, char *prompt1, char *prompt2, bool fatal,
		bool &first_time)
{
#if 0
	int key;
	int maxlen;

	if (fatal)
		maxlen = 44;
	else
		maxlen = 27;
	if (strlen (prompt1) > maxlen)
		maxlen = strlen (prompt1);
	if (prompt2 && strlen (prompt2) > maxlen)
		maxlen = strlen (prompt2);
	int width = 2 * BOX_BORDER + 2 * WIDE_HSPACING + FONTW * maxlen;
	int height = 2 * BOX_BORDER + 2 * WIDE_VSPACING + FONTH * (prompt2 ? 6 : 5);
	if (x0 < 0)
		x0 = (ScrMaxX - width) / 2;
	if (y0 < 0)
		y0 = (ScrMaxY - height) / 2;
	int text_x0 = x0 + BOX_BORDER + WIDE_HSPACING;
	int text_y0 = y0 + BOX_BORDER + WIDE_VSPACING;
	int cur_y = text_y0;
	DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
	set_colour (LIGHTRED);
	DrawScreenText (text_x0, cur_y, "Verification failed:");
	if (first_time)
		Beep ();
	set_colour (WHITE);
	DrawScreenText (text_x0, cur_y += FONTH, prompt1);
	LogPrintf("\t%s\n", prompt1);
	if (prompt2)
	{
		DrawScreenText (text_x0, cur_y += FONTH, prompt2);
		LogPrintf("\t%s\n", prompt2);
	}
	if (fatal)
	{
		DrawScreenText (text_x0, cur_y += 2 * FONTH,
				"The game will crash if you play with this level.");
		set_colour (WINTITLE);
		DrawScreenText (text_x0, cur_y += FONTH,
				"Press any key to see the object");
		LogPrintf("\n");
	}
	else
	{
		set_colour (WINTITLE);
		DrawScreenText (text_x0, cur_y += 2 * FONTH,
				"Press Esc to see the object,");
		DrawScreenText (text_x0, cur_y += FONTH,
				"or any other key to continue");
	}
	key = FL_Escape; //!!!! get_key ();
	if (key != FL_Escape)
	{
		DrawScreenBox3D (x0, y0, x0 + width - 1, y0 + height - 1);
		//   DrawScreenText (x0 + 10 + 4 * (maxlen - 26), y0 + 28,
		//      "Verifying other objects...");
	}
	first_time = false;
	return (key == FL_Escape);
#else
	return false;
#endif
}


/*
   check if all sectors are closed
   */

void CheckSectors ()
{
#if 0  // TODO
	int        s, n, sd;
	char *ends;
	char       msg1[80], msg2[80];
	bool       first_time = true;

	CheckingObjects ();
	LogPrintf("\nVerifying Sectors...\n");

	ends = (char *) GetMemory (NumVertices);
	for (s = 0 ; s < NumSectors ; s++)
	{
		/* clear the "ends" array */
		for (n = 0 ; n < NumVertices ; n++)
			ends[n] = 0;
		/* for each sidedef bound to the Sector, store a "1" in the "ends" */
		/* array for its starting Vertex, and a "2" for its ending Vertex  */
		for (n = 0 ; n < NumLineDefs ; n++)
		{
			sd = LineDefs[n].side_R;
			if (sd >= 0 && SideDefs[sd].sector == s)
			{
				ends[LineDefs[n].start] |= 1;
				ends[LineDefs[n].end] |= 2;
			}
			sd = LineDefs[n].side_L;
			if (sd >= 0 && SideDefs[sd].sector == s)
			{
				ends[LineDefs[n].end] |= 1;
				ends[LineDefs[n].start] |= 2;
			}
		}
		/* every entry in the "ends" array should be "0" or "3" */
		for (n = 0 ; n < NumVertices ; n++)
		{
			if (ends[n] == 1)
			{
				sprintf (msg1, "Sector #%d is not closed!", s);
				sprintf (msg2, "There is no sidedef ending at Vertex #%d", n);
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_VERTICES, n));
					return;
				}
			}
			if (ends[n] == 2)
			{
				sprintf (msg1, "Sector #%d is not closed!", s);
				sprintf (msg2, "There is no sidedef starting at Vertex #%d", n);
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_VERTICES, n));
					return;
				}
			}
		}
	}
	FreeMemory (ends);

	/*
	   Note from RQ:
	   This is a very simple idea, but it works!  The first test (above)
	   checks that all Sectors are closed.  But if a closed set of LineDefs
	   is moved out of a Sector and has all its "external" SideDefs pointing
	   to that Sector instead of the new one, then we need a second test.
	   That's why I check if the SideDefs facing each other are bound to
	   the same Sector.

	   Other note from RQ:
	   Nowadays, what makes the power of a good editor is its automatic tests.
	   So, if you are writing another Doom editor, you will probably want
	   to do the same kind of tests in your program.  Fine, but if you use
	   these ideas, don't forget to credit DEU...  Just a reminder... :-)
	   */

	/* now check if all SideDefs are facing a sidedef with the same Sector number */
	for (n = 0 ; n < NumLineDefs ; n++)
	{
		sd = LineDefs[n].side_R;
		if (sd >= 0)
		{
			s = GetOppositeSector (n, 1);

			if (s < 0 || SideDefs[sd].sector != s)
			{
				if (s < 0)
				{
					sprintf (msg1, "Sector #%d is not closed!", SideDefs[sd].sector);
					sprintf (msg2, "Check linedef #%d (first sidedef: #%d)", n, sd);
				}
				else
				{
					sprintf (msg1, "Sectors #%d and #%d are not closed!",
							SideDefs[sd].sector, s);
					sprintf (msg2, "Check linedef #%d (first sidedef: #%d)"
							" and the one facing it", n, sd);
				}
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_LINEDEFS, n));
					return;
				}
			}
		}

		sd = LineDefs[n].side_L;
		if (sd >= 0)
		{
			s = GetOppositeSector (n, 0);

			if (s < 0 || SideDefs[sd].sector != s)
			{
				if (s < 0)
				{
					sprintf (msg1, "Sector #%d is not closed!", SideDefs[sd].sector);
					sprintf (msg2, "Check linedef #%d (second sidedef: #%d)", n, sd);
				}
				else
				{
					sprintf (msg1, "Sectors #%d and #%d are not closed!",
							SideDefs[sd].sector, s);
					sprintf (msg2, "Check linedef #%d (second sidedef: #%d)"
							" and the one facing it", n, sd);
				}
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_LINEDEFS, n));
					return;
				}
			}
		}
	}
#endif
}


/*
   check cross-references and delete unused objects
   */

void CheckCrossReferences ()
{
// FIXME !!!!  CheckCrossReferences
#if 0

	char   msg[80];
	int    n, m;
	SelPtr cur;
	bool   first_time = true;

	CheckingObjects ();
	LogPrintf("\nVerifying cross-references...\n");

	for (n = 0 ; n < NumLineDefs ; n++)
	{
		/* Check for missing first sidedefs */
		if (LineDefs[n].side_R < 0)
		{
			sprintf (msg, "ERROR: linedef #%d has no first sidedef!", n);
			CheckFailed (-1, -1, msg, 0, 1, first_time);
			GoToObject (Objid (OBJ_LINEDEFS, n));
			return;
		}

		// FIXME should make this a mere warning
#ifdef OLD  /* ifdef'd out AYM 19970725, I do it and it works */
		/* Check for sidedefs used twice in the same linedef */
		if (LineDefs[n].sidedef1 == LineDefs[n].sidedef2)
		{
			sprintf (msg, "ERROR: linedef #%d uses the same sidedef twice (#%d)",
					n, LineDefs[n].sidedef1);
			CheckFailed (-1, -1, msg, 0, 1, first_time);
			GoToObject (Objid (OBJ_LINEDEFS, n));
			return;
		}
#endif

		/* Check for vertices used twice in the same linedef */
		if (LineDefs[n].start == LineDefs[n].end)
		{
			sprintf (msg, "ERROR: linedef #%d uses the same vertex twice (#%d)",
					n, LineDefs[n].start);
			CheckFailed (-1, -1, msg, 0, 1, first_time);
			GoToObject (Objid (OBJ_LINEDEFS, n));
			return;
		}
	}

	/* check if there aren't two linedefs between the same Vertices */
	cur = 0;
	/* AYM 19980203 FIXME should use new algorithm */
	for (n = NumLineDefs - 1 ; n >= 1 ; n--)
	{
		for (m = n - 1 ; m >= 0 ; m--)
			if ((LineDefs[n].start == LineDefs[m].start
						&& LineDefs[n].end   == LineDefs[m].end)
					|| (LineDefs[n].start == LineDefs[m].end
						&& LineDefs[n].end   == LineDefs[m].start))
			{
				SelectObject (&cur, n);
				break;
			}
	}
	if (cur
			&& (Expert
				|| Confirm (-1, -1, "There are multiple linedefs between the same vertices",
					"Do you want to delete the redundant linedefs?")))
		DeleteObjects (OBJ_LINEDEFS, &cur);
	else
		{ delete cur; cur = NULL; }

	CheckingObjects ();
	/* check for invalid flags in the linedefs */
	for (n = 0 ; n < NumLineDefs ; n++)
		if ((LineDefs[n].flags & 0x01) == 0 && LineDefs[n].side_L < 0)
			SelectObject (&cur, n);
	if (cur && (Expert
				|| Confirm (-1, -1, "Some linedefs have only one side but their I flag is"
					" not set",
					"Do you want to set the 'Impassible' flag?")))
		while (cur)
		{
			LogPrintf("Check: 1-sided linedef without I flag: %d", cur->objnum);
			LineDefs[cur->objnum].flags |= 0x01;
			UnSelectObject (&cur, cur->objnum);
		}
	else
		{ delete cur; cur = NULL; }

	CheckingObjects ();
	for (n = 0 ; n < NumLineDefs ; n++)
		if ((LineDefs[n].flags & 0x04) != 0 && LineDefs[n].side_L < 0)
			SelectObject (&cur, n);
	if (cur
			&& (Expert
				|| Confirm (-1, -1, "Some linedefs have only one side but their 2 flag"
					" is set",
					"Do you want to clear the 'two-sided' flag?")))
	{
		while (cur)
		{
			LogPrintf("Check: 1-sided linedef with 2s bit: %d", cur->objnum);
			LineDefs[cur->objnum].flags &= ~0x04;
			UnSelectObject (&cur, cur->objnum);
		}
	}
	else
		{ delete cur; cur = NULL; }

	CheckingObjects ();
	for (n = 0 ; n < NumLineDefs ; n++)
		if ((LineDefs[n].flags & 0x04) == 0 && LineDefs[n].side_L >= 0)
			SelectObject (&cur, n);
	if (cur
			&& (Expert
				|| Confirm (-1, -1,
					"Some linedefs have two sides but their 2S bit is not set",
					"Do you want to set the 'two-sided' flag?")))
	{
		while (cur)
		{
			LineDefs[cur->objnum].flags |= 0x04;
			UnSelectObject (&cur, cur->objnum);
		}
	}
	else
		{ delete cur; cur = NULL; }

	CheckingObjects ();
	/* select all Vertices */
	for (n = 0 ; n < NumVertices ; n++)
		SelectObject (&cur, n);
	/* unselect Vertices used in a LineDef */
	for (n = 0 ; n < NumLineDefs ; n++)
	{
		m = LineDefs[n].start;
		if (cur && m >= 0)
			UnSelectObject (&cur, m);
		m = LineDefs[n].end;
		if (cur && m >= 0)
			UnSelectObject (&cur, m);
		continue;
	}
	/* check if there are any Vertices left */
	if (cur
			&& (Expert
				|| Confirm (-1, -1, "Some vertices are not bound to any linedef",
					"Do you want to delete these unused Vertices?")))
	{
		DeleteObjects (OBJ_VERTICES, &cur);

	}
	else
		{ delete cur; cur = NULL; }

	CheckingObjects ();
	/* select all SideDefs */
	for (n = 0 ; n < NumSideDefs ; n++)
		SelectObject (&cur, n);
	/* unselect SideDefs bound to a LineDef */
	for (n = 0 ; n < NumLineDefs ; n++)
	{
		m = LineDefs[n].side_R;
		if (cur && m >= 0)
			UnSelectObject (&cur, m);
		m = LineDefs[n].side_L;
		if (cur && m >= 0)
			UnSelectObject (&cur, m);
		continue;
	}
	/* check if there are any SideDefs left */
	if (cur
			&& (Expert
				|| Confirm (-1, -1, "Some sidedefs are not bound to any linedef",
					"Do you want to delete these unused sidedefs?")))
		DeleteObjects (OBJ_SIDEDEFS, &cur);
	else
		{ delete cur; cur = NULL; }

	CheckingObjects ();
	/* select all Sectors */
	for (n = 0 ; n < NumSectors ; n++)
		SelectObject (&cur, n);
	/* unselect Sectors bound to a sidedef */
	for (n = 0 ; n < NumLineDefs ; n++)
	{

		m = LineDefs[n].side_L;

		if (cur && m >= 0 /* && SideDefs[m].sector >= 0 AYM 1998-06-13 */)
			UnSelectObject (&cur, SideDefs[m].sector);

		m = LineDefs[n].side_R;

		if (cur && m >= 0 /* && SideDefs[m].sector >= 0 AYM 1998-06-13 */)
			UnSelectObject (&cur, SideDefs[m].sector);
		continue;
	}
	/* check if there are any Sectors left */
	if (cur
			&& (Expert
				|| Confirm (-1, -1, "Some sectors are not bound to any sidedef",
					"Do you want to delete these unused sectors?")))
		DeleteObjects (OBJ_SECTORS, &cur);
	else
		{ delete cur; cur = NULL; }
#endif
}


/*
   check for missing textures
   */

void CheckTextures ()
{
#if 0  // TODO
	int  n;
	int  sd1, sd2;
	int  s1, s2;
	char msg1[80], msg2[80];
	bool first_time = true;

	CheckingObjects ();
	LogPrintf("\nVerifying textures...\n");

	for (n = 0 ; n < NumSectors ; n++)
	{
		if (strcmp (Sectors[n].ceil_tex, "-") == 0
				|| strcmp (Sectors[n].ceil_tex, "") == 0
				|| memcmp (Sectors[n].ceil_tex, "        ", 8) == 0)
		{
			sprintf (msg1, "Error: sector #%d has no ceiling texture", n);
			sprintf (msg2, "You probably used a brain-damaged editor to do that...");
			CheckFailed (-1, -1, msg1, msg2, 1, first_time);
			GoToObject (Objid (OBJ_SECTORS, n));
			return;
		}
		if (strcmp (Sectors[n].floor_tex, "-") == 0
				|| strcmp (Sectors[n].floor_tex, "") == 0
				|| memcmp (Sectors[n].floor_tex, "        ", 8) == 0)
		{
			sprintf (msg1, "Error: sector #%d has no floor texture", n);
			sprintf (msg2, "You probably used a brain-damaged editor to do that...");
			CheckFailed (-1, -1, msg1, msg2, 1, first_time);
			GoToObject (Objid (OBJ_SECTORS, n));
			return;
		}
		if (Sectors[n].ceilh < Sectors[n].floorh)
		{
			sprintf (msg1,
					"Error: sector #%d has its ceiling lower than its floor", n);
			sprintf (msg2,
					"The textures will never be displayed if you cannot go there");
			CheckFailed (-1, -1, msg1, msg2, 1, first_time);
			GoToObject (Objid (OBJ_SECTORS, n));
			return;
		}
#if 0  /* AYM 2000-08-13 */
		if (Sectors[n].ceilh - Sectors[n].floorh > 1023)
		{
			sprintf (msg1, "Error: sector #%d has its ceiling too high", n);
			sprintf (msg2, "The maximum difference allowed is 1023 (ceiling - floor)");
			CheckFailed (-1, -1, msg1, msg2, 1, first_time);
			GoToObject (Objid (OBJ_SECTORS, n));
			return;
		}
#endif
	}

	for (n = 0 ; n < NumLineDefs ; n++)
	{

		sd1 = LineDefs[n].side_R;
		sd2 = LineDefs[n].side_L;

		if (sd1 >= 0)
			s1 = SideDefs[sd1].sector;
		else
			s1 = OBJ_NO_NONE;
		if (sd2 >= 0)
			s2 = SideDefs[sd2].sector;
		else
			s2 = OBJ_NO_NONE;
		if (is_obj (s1) && ! is_obj (s2))
		{
			if (SideDefs[sd1].mid_tex[0] == '-' && SideDefs[sd1].mid_tex[1] == '\0')
			{
				sprintf (msg1, "Error in one-sided linedef #%d:"
						" sidedef #%d has no middle texture", n, sd1);
				sprintf (msg2, "Do you want to set the texture to \"%s\""
						" and continue?", default_middle_texture);
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_LINEDEFS, n));
					return;
				}
				strncpy (SideDefs[sd1].mid_tex, default_middle_texture, WAD_TEX_NAME);
				MarkChanges();
				CheckingObjects ();
			}
		}
		if (is_obj (s1) && is_obj (s2) && Sectors[s1].ceilh > Sectors[s2].ceilh)
		{
			if (SideDefs[sd1].upper_tex[0] == '-' && SideDefs[sd1].upper_tex[1] == '\0'
					&& (! is_sky (Sectors[s1].ceil_tex) || ! is_sky (Sectors[s2].ceil_tex)))
			{
				sprintf (msg1, "Error in first sidedef of linedef #%d:"
						" sidedef #%d has no upper texture", n, sd1);
				sprintf (msg2, "Do you want to set the texture to \"%s\""
						" and continue?", default_upper_texture);
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_LINEDEFS, n));
					return;
				}
				strncpy (SideDefs[sd1].upper_tex, default_upper_texture, WAD_TEX_NAME);
				MarkChanges();
				CheckingObjects ();
			}
		}
		if (is_obj (s1) && is_obj (s2) && Sectors[s1].floorh < Sectors[s2].floorh)
		{
			if (SideDefs[sd1].lower_tex[0] == '-' && SideDefs[sd1].lower_tex[1] == '\0')
			{
				sprintf (msg1, "Error in first sidedef of linedef #%d:"
						" sidedef #%d has no lower texture", n, sd1);
				sprintf (msg2, "Do you want to set the texture to \"%s\""
						" and continue?", default_lower_texture);
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_LINEDEFS, n));
					return;
				}
				strncpy (SideDefs[sd1].lower_tex, default_lower_texture, WAD_TEX_NAME);
				MarkChanges();
				CheckingObjects ();
			}
		}
		if (is_obj (s1) && is_obj (s2) && Sectors[s2].ceilh > Sectors[s1].ceilh)
		{
			if (SideDefs[sd2].upper_tex[0] == '-' && SideDefs[sd2].upper_tex[1] == '\0'
					&& (! is_sky (Sectors[s1].ceil_tex) || ! is_sky (Sectors[s2].ceil_tex)))
			{
				sprintf (msg1, "Error in second sidedef of linedef #%d:"
						" sidedef #%d has no upper texture", n, sd2);
				sprintf (msg2, "Do you want to set the texture to \"%s\""
						" and continue?", default_upper_texture);
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_LINEDEFS, n));
					return;
				}
				strncpy (SideDefs[sd2].upper_tex, default_upper_texture, WAD_TEX_NAME);
				MarkChanges();
				CheckingObjects ();
			}
		}
		if (is_obj (s1) && is_obj (s2) && Sectors[s2].floorh < Sectors[s1].floorh)
		{
			if (SideDefs[sd2].lower_tex[0] == '-' && SideDefs[sd2].lower_tex[1] == '\0')
			{
				sprintf (msg1, "Error in second sidedef of linedef #%d:"
						" sidedef #%d has no lower texture", n, sd2);
				sprintf (msg2, "Do you want to set the texture to \"%s\""
						" and continue?", default_lower_texture);
				if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
				{
					GoToObject (Objid (OBJ_LINEDEFS, n));
					return;
				}
				strncpy (SideDefs[sd2].lower_tex, default_lower_texture, WAD_TEX_NAME);
				MarkChanges();
				CheckingObjects ();
			}
		}
	}
#endif
}


/*
   check if a texture name matches one of the elements of a list
   */

bool IsTextureNameInList (char *name, char **list, int numelems)
{
	int n;

	for (n = 0 ; n < numelems ; n++)
		if (! y_strnicmp (name, list[n], WAD_TEX_NAME))
			return true;
	return false;
}


/*
   check for invalid texture names
   */

void CheckTextureNames ()
{
#if 0  // TODO
	int  n;
	char msg1[80], msg2[80];
	bool first_time = true;

	CheckingObjects ();
	LogPrintf("\nVerifying texture names...\n");

	// AYM 2000-07-24: could someone explain this one ?
	if (! FindMasterDir (MasterDir, "F2_START"))
		NumThings--;


	for (n = 0 ; n < NumSectors ; n++)
	{
		if (! is_flat_name_in_list (Sectors[n].ceil_tex))
		{
			sprintf (msg1, "Invalid ceiling texture in sector #%d", n);
			sprintf (msg2, "The name \"%.*s\" is not a floor/ceiling texture",
					(int) WAD_FLAT_NAME, Sectors[n].ceil_tex);
			if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
			{
				GoToObject (Objid (OBJ_SECTORS, n));
				return;
			}
			CheckingObjects ();
		}
		if (! is_flat_name_in_list (Sectors[n].floor_tex))
		{
			sprintf (msg1, "Invalid floor texture in sector #%d", n);
			sprintf (msg2, "The name \"%.*s\" is not a floor/ceiling texture",
					(int) WAD_FLAT_NAME, Sectors[n].floor_tex);
			if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
			{
				GoToObject (Objid (OBJ_SECTORS, n));
				return;
			}
			CheckingObjects ();
		}
	}

	for (n = 0 ; n < NumSideDefs ; n++)
	{
		if (! IsTextureNameInList (SideDefs[n].upper_tex, WTexture, NumWTexture))
		{
			sprintf (msg1, "Invalid upper texture in sidedef #%d", n);
			sprintf (msg2, "The name \"%.*s\" is not a wall texture",
					(int) WAD_TEX_NAME, SideDefs[n].upper_tex);
			if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
			{
				GoToObject (Objid (OBJ_SIDEDEFS, n));
				return;
			}
			CheckingObjects ();
		}
		if (! IsTextureNameInList (SideDefs[n].lower_tex, WTexture, NumWTexture))
		{
			sprintf (msg1, "Invalid lower texture in sidedef #%d", n);
			sprintf (msg2, "The name \"%.*s\" is not a wall texture",
					(int) WAD_TEX_NAME, SideDefs[n].lower_tex);
			if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
			{
				GoToObject (Objid (OBJ_SIDEDEFS, n));
				return;
			}
			CheckingObjects ();
		}
		if (! IsTextureNameInList (SideDefs[n].mid_tex, WTexture, NumWTexture))
		{
			sprintf (msg1, "Invalid middle texture in sidedef #%d", n);
			sprintf (msg2, "The name \"%.*s\" is not a wall texture",
					(int) WAD_TEX_NAME, SideDefs[n].mid_tex);
			if (CheckFailed (-1, -1, msg1, msg2, 0, first_time))
			{
				GoToObject (Objid (OBJ_SIDEDEFS, n));
				return;
			}
			CheckingObjects ();
		}
	}
#endif
}


//------------------------------------------------------------------------


struct vertex_X_CMP_pred
{
	inline bool operator() (int A, int B) const
	{
		const Vertex *V1 = Vertices[A];
		const Vertex *V2 = Vertices[B];

		return V1->x < V2->x;
	}
};


void Vertex_FindOverlaps(selection_c& sel, bool one_coord = false)
{
	// the 'one_coord' parameter limits the selection to a single
	// vertex coordinate.

	sel.change_type(OBJ_VERTICES);

	if (NumVertices < 2)
		return;

	// sort the vertices into order of the 'X' value.
	// hence any overlapping vertices will be near each other.

	std::vector<int> sorted_list(NumVertices, 0);

	for (int i = 0 ; i < NumVertices ; i++)
		sorted_list[i] = i;

	std::sort(sorted_list.begin(), sorted_list.end(), vertex_X_CMP_pred());

	bool seen_one = false;
	int last_y = 0;

#define VERT_K  Vertices[sorted_list[k]]
#define VERT_N  Vertices[sorted_list[n]]

	for (int k = 0 ; k < NumVertices ; k++)
	{
		for (int n = k + 1 ; n < NumVertices && VERT_N->x == VERT_K->x ; n++)
		{
			if (VERT_N->y == VERT_K->y)
			{
				if (one_coord && seen_one && VERT_K->y != last_y)
					continue;

				sel.set(sorted_list[k]);
				sel.set(sorted_list[n]);

				seen_one = true; last_y = VERT_K->y;
			}
		}
	}

#undef VERT_K
#undef VERT_N
}


void Vertex_MergeOverlaps()
{
	for (;;)
	{
		selection_c sel;

		Vertex_FindOverlaps(sel, true /* one_coord */);

		if (sel.empty())
			break;

		Vertex_MergeList(&sel);
	}
}


void Vertex_HighlightOverlaps()
{
	if (edit.mode != OBJ_VERTICES)
		Editor_ChangeMode('v');

	Vertex_FindOverlaps(*edit.Selected);

	GoToSelection();

	edit.error_mode = true;
	edit.RedrawMap = 1;
}


void Vertex_FindUnused(selection_c& sel)
{
	sel.change_type(OBJ_VERTICES);

	if (NumVertices == 0)
		return;

	for (int i = 0 ; i < NumLineDefs ; i++)
	{
		sel.set(LineDefs[i]->start);
		sel.set(LineDefs[i]->end);
	}

	sel.frob_range(0, NumVertices - 1, BOP_TOGGLE);
}


void Vertex_RemoveUnused()
{
	selection_c sel;

	Vertex_FindUnused(sel);

	BA_Begin();
	DeleteObjects(&sel);
	BA_End();

//??	Status_Set("Removed %d vertices", sel.count_obj());
}


//------------------------------------------------------------------------


void Sectors_FindUnused(selection_c& sel)
{
	sel.change_type(OBJ_SECTORS);

	if (NumSectors == 0)
		return;

	for (int i = 0 ; i < NumLineDefs ; i++)
	{
		const LineDef *L = LineDefs[i];

		if (L->left >= 0)
			sel.set(L->Left()->sector);

		if (L->right >= 0)
			sel.set(L->Right()->sector);
	}

	sel.frob_range(0, NumSectors - 1, BOP_TOGGLE);
}


void Sectors_RemoveUnused()
{
	selection_c sel;

	Sectors_FindUnused(sel);

	BA_Begin();
	DeleteObjects(&sel);
	BA_End();

//??	Status_Set("Removed %d vertices", sel.count_obj());
}


void Sectors_FindBadCeil(selection_c& sel)
{
	sel.change_type(OBJ_SECTORS);

	if (NumSectors == 0)
		return;

	for (int i = 0 ; i < NumSectors ; i++)
	{
		if (Sectors[i]->ceilh < Sectors[i]->floorh)
			sel.set(i);
	}
}


void Sectors_FixBadCeil()
{
	selection_c sel;

	Sectors_FindBadCeil(sel);

	BA_Begin();

	for (int i = 0 ; i < NumSectors ; i++)
	{
		if (Sectors[i]->ceilh < Sectors[i]->floorh)
		{
			BA_ChangeSEC(i, Sector::F_CEILH, Sectors[i]->floorh);
		}
	}

	BA_End();
}


void Sectors_HighlightBadCeil()
{
	if (edit.mode != OBJ_SECTORS)
		Editor_ChangeMode('s');

	Sectors_FindBadCeil(*edit.Selected);

	GoToSelection();

	edit.error_mode = true;
	edit.RedrawMap = 1;
}


void SideDefs_FindUnused(selection_c& sel)
{
	sel.change_type(OBJ_SIDEDEFS);

	if (NumSideDefs == 0)
		return;

	for (int i = 0 ; i < NumLineDefs ; i++)
	{
		const LineDef *L = LineDefs[i];

		if (L->left  >= 0) sel.set(L->left);
		if (L->right >= 0) sel.set(L->right);
	}

	sel.frob_range(0, NumSideDefs - 1, BOP_TOGGLE);
}


void SideDefs_RemoveUnused()
{
	selection_c sel;

	SideDefs_FindUnused(sel);

	BA_Begin();
	DeleteObjects(&sel);
	BA_End();

//??	Status_Set("Removed %d vertices", sel.count_obj());
}


void SideDefs_FindPacking(selection_c& sides, selection_c& lines)
{
	sides.change_type(OBJ_SIDEDEFS);
	lines.change_type(OBJ_LINEDEFS);

	for (int i = 0 ; i < NumLineDefs ; i++)
	for (int k = 0 ; k < i ; k++)
	{
		const LineDef * A = LineDefs[i];
		const LineDef * B = LineDefs[k];

		bool AA = (A->left  >= 0 && A->left == A->right);

		bool AL = (A->left  >= 0 && (A->left  == B->left || A->left  == B->right));
		bool AR = (A->right >= 0 && (A->right == B->left || A->right == B->right));

		if (AL || AA) sides.set(A->left);
		if (AR)       sides.set(A->right);

		if (AL || AR)
		{
			lines.set(i);
			lines.set(k);
		}
		else if (AA)
		{
			lines.set(i);
		}
	}
}


void SideDefs_HighlightPacked()
{
	if (edit.mode != OBJ_LINEDEFS)
		Editor_ChangeMode('l');

	selection_c sides;

	SideDefs_FindPacking(sides, *edit.Selected);

	GoToSelection();

	edit.error_mode = true;
	edit.RedrawMap = 1;
}


static int Copy_SideDef(int num)
{
	int sd = BA_New(OBJ_SIDEDEFS);

	SideDefs[sd]->RawCopy(SideDefs[num]);

	return sd;
}


static const char *unpack_confirm_message =
	"This map contains shared sidedefs.  It it recommended to unpack\n"
	"them, otherwise it may cause unexpected behavior during editing\n"
	"(such as random walls changing their texture).\n\n"
	"Unpack the sidedefs now?";


void SideDefs_Unpack(bool confirm_it)
{
	selection_c sides;
	selection_c lines;

	SideDefs_FindPacking(sides, lines);

	if (sides.empty())
		return;

	if (confirm_it)
	{
		if (! Confirm(-1, -1, unpack_confirm_message, NULL))
			return;
	}

	BA_Begin();

	for (int sd = 0 ; sd < NumSideDefs ; sd++)
	{
		if (! sides.get(sd))
			continue;

		// find the first linedef which uses this sidedef
		int first;

		for (first = 0 ; first < NumLineDefs ; first++)
		{
			const LineDef *F = LineDefs[first];

			if (F->left == sd || F->right == sd)
				break;
		}

		if (first >= NumLineDefs)
			continue;

		// handle it when first linedef uses sidedef on both sides
		if (LineDefs[first]->left == LineDefs[first]->right)
		{
			BA_ChangeLD(first, LineDef::F_LEFT, Copy_SideDef(sd));
		}

		// duplicate any remaining references
		for (int ld = first + 1 ; ld < NumLineDefs ; ld++)
		{
			if (LineDefs[ld]->left == sd)
				BA_ChangeLD(ld, LineDef::F_LEFT, Copy_SideDef(sd));

			if (LineDefs[ld]->right == sd)
				BA_ChangeLD(ld, LineDef::F_RIGHT, Copy_SideDef(sd));
		}
	}

	BA_End();
}


//------------------------------------------------------------------------


void Things_FindUnknown(selection_c& list)
{
	list.change_type(OBJ_THINGS);

	for (int n = 0 ; n < NumThings ; n++)
	{
		const thingtype_t *info = M_GetThingType(Things[n]->type);

		if (strncmp(info->desc, "UNKNOWN", 7) == 0)
			list.set(n);
	}
}


void Things_HighlightUnknown()
{
	if (edit.mode != OBJ_THINGS)
		Editor_ChangeMode('t');

	Things_FindUnknown(*edit.Selected);

	GoToSelection();

	edit.error_mode = true;
	edit.RedrawMap = 1;
}


// this returns a bitmask : bits 0..3 for players 1..4
int Things_FindStarts(int *dm_num)
{
	*dm_num = 0;

	int mask = 0;

	for (int n = 0 ; n < NumThings ; n++)
	{
		const Thing * T = Things[n];

		// ideally, these type numbers would not be hard-coded....

		switch (T->type)
		{
			case 1: mask |= (1 << 0); break;
			case 2: mask |= (1 << 1); break;
			case 3: mask |= (1 << 2); break;
			case 4: mask |= (1 << 3); break;

			case 11: *dm_num += 1; break;
		}
	}

	return mask;
}


void Things_FindInVoid(selection_c& list)
{
	list.change_type(OBJ_THINGS);

	for (int n = 0 ; n < NumThings ; n++)
	{
		Objid obj;

		GetCurObject(obj, OBJ_SECTORS, Things[n]->x, Things[n]->y);

		if (obj.is_nil())
			list.set(n);
	}
}


void Things_HighlightInVoid()
{
	if (edit.mode != OBJ_THINGS)
		Editor_ChangeMode('t');

	Things_FindInVoid(*edit.Selected);

	GoToSelection();

	edit.error_mode = true;
	edit.RedrawMap = 1;
}


//------------------------------------------------------------------------


void Tags_UsedRange(int *min_tag, int *max_tag)
{
	int i;

	*min_tag = +999999;
	*max_tag = -999999;

	for (i = 0 ; i < NumLineDefs ; i++)
	{
		int tag = LineDefs[i]->tag;

		if (tag > 0)
		{
			*min_tag = MIN(*min_tag, tag);
			*max_tag = MAX(*max_tag, tag);
		}
	}

	for (i = 0 ; i < NumSectors ; i++)
	{
		int tag = Sectors[i]->tag;

		if (tag > 0)
		{
			*min_tag = MIN(*min_tag, tag);
			*max_tag = MAX(*max_tag, tag);
		}
	}

	// none at all?
	if (*min_tag > *max_tag)
	{
		*min_tag = *max_tag = 0;
	}
}


void Tags_ApplyNewValue(int new_tag)
{
	selection_c list;
	selection_iterator_c it;

	if (GetCurrentObjects(&list))
	{
		BA_Begin();

		for (list.begin(&it); !it.at_end(); ++it)
		{
			if (edit.mode == OBJ_LINEDEFS)
				BA_ChangeLD(*it, LineDef::F_TAG, new_tag);
			else if (edit.mode == OBJ_SECTORS)
				BA_ChangeSEC(*it, Sector::F_TAG, new_tag);
		}

		BA_End();
		MarkChanges();
	}
}


void CMD_ApplyTag()
{
	if (! (edit.mode == OBJ_SECTORS || edit.mode == OBJ_LINEDEFS))
	{
		Beep("ApplyTag: wrong mode");
		return;
	}

	bool do_last = false;

	const char *mode = EXEC_Param[0];

	if (mode[0] == 0 || y_stricmp(mode, "fresh") == 0)
	{
		// fresh tag
	}
	else if (y_stricmp(mode, "last") == 0)
	{
		do_last = true;
	}
	else
	{
		Beep("ApplyTag: unknown param '%s'\n", mode); 
		return;
	}


	bool unselect = false;

	if (edit.Selected->empty())
	{
		if (! edit.highlighted())
		{
			Beep("ApplyTag: nothing selected");
			return;
		}

		edit.Selected->set(edit.highlighted.num);
		unselect = true;
	}


	int min_tag, max_tag;

	Tags_UsedRange(&min_tag, &max_tag);

	int new_tag = max_tag + (do_last ? 0 : 1);

	if (new_tag <= 0)
	{
		Beep("No last tag");
		return;
	}
	else if (new_tag > 32767)
	{
		Beep("Out of tag numbers");
		return;
	}

	Tags_ApplyNewValue(new_tag);

	if (unselect)
		edit.Selected->clear_all();
}


//------------------------------------------------------------------------

// the CHECK_xxx functions return the following values:
typedef enum
{
	CKR_OK = 0,            // no issues at all
	CKR_MinorProblem,      // only minor issues
	CKR_MajorProblem,      // some major problems
	CKR_Highlight,         // need to highlight stuff (skip further checks)
	CKR_TookAction         // [internal use : user took some action]

} check_result_e;


class UI_Check_Vertices : public Fl_Double_Window
{
private:
	bool want_close;

	check_result_e user_action;

	Fl_Group  *line_group;
	Fl_Button *ok_but;

	int cy;

public:
	int worst_severity;

public:
	static void close_callback(Fl_Widget *w, void *data)
	{
		UI_Check_Vertices *dialog = (UI_Check_Vertices *)data;

		dialog->want_close = true;
	}

	static void action_merge(Fl_Widget *w, void *data)
	{
		UI_Check_Vertices *dialog = (UI_Check_Vertices *)data;

		Vertex_MergeOverlaps();

		dialog->user_action = CKR_TookAction;
	}

	static void action_highlight(Fl_Widget *w, void *data)
	{
		UI_Check_Vertices *dialog = (UI_Check_Vertices *)data;

		Vertex_HighlightOverlaps();

		dialog->user_action = CKR_Highlight;
	}

	static void action_remove(Fl_Widget *w, void *data)
	{
		UI_Check_Vertices *dialog = (UI_Check_Vertices *)data;

		Vertex_RemoveUnused();

		dialog->user_action = CKR_TookAction;
	}

public:
	UI_Check_Vertices(bool all_mode) :
		Fl_Double_Window(520, 186, "Check : Vertices"),
		want_close(false), user_action(CKR_OK),
		worst_severity(0)
	{
		cy = 10;

		callback(close_callback, this);

		int ey = h() - 66;

		Fl_Box *title = new Fl_Box(FL_NO_BOX, 10, cy, w() - 20, 30, "Vertex check results");
		title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
		title->labelfont(FL_HELVETICA_BOLD);
		title->labelsize(FL_NORMAL_SIZE + 2);

		cy = 45;

		line_group = new Fl_Group(0, 0, w(), ey);
		line_group->end();

		{ Fl_Group *o = new Fl_Group(0, ey, w(), 66);

		  o->box(FL_FLAT_BOX);
		  o->color(WINDOW_BG, WINDOW_BG);

		  int but_W = all_mode ? 110 : 70;

		  { ok_but = new Fl_Button(w()/2 - but_W/2, ey + 18, but_W, 34,
		                           all_mode ? "Continue" : "OK");
			ok_but->labelfont(1);
			ok_but->callback(close_callback, this);
		  }
		  o->end();
		}

		end();
	}

	void Reset()
	{
		want_close = false;
		user_action = CKR_OK;

		cy = 45;

		line_group->clear();	

		redraw();
	}

	void AddLine(const char *msg, int severity = 0, int W = -1,
	             const char *button1 = NULL, Fl_Callback *cb1 = NULL,
	             const char *button2 = NULL, Fl_Callback *cb2 = NULL,
	             const char *button3 = NULL, Fl_Callback *cb3 = NULL)
	{
		int cx = 25;

		if (W < 0)
			W = w() - 40;

		Fl_Box *box = new Fl_Box(FL_NO_BOX, cx, cy, W, 25, NULL);
		box->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
		box->copy_label(msg);

		if (severity == 2)
		{
			box->labelcolor(ERROR_MSG_COLOR);
			box->labelfont(FL_HELVETICA_BOLD);
		}
		else if (severity == 1)
		{
			box->labelcolor(WARNING_MSG_COLOR);
			box->labelfont(FL_HELVETICA_BOLD);
		}

		line_group->add(box);

		cx += W;

		if (button1)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button1);
			but->callback(cb1, this);

			line_group->add(but);

			cx += but->w() + 10;
		}

		if (button2)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button2);
			but->callback(cb2, this);

			line_group->add(but);

			cx += but->w() + 10;
		}

		if (button3)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button3);
			but->callback(cb3, this);

			line_group->add(but);
		}

		cy = cy + 30;

		if (severity > worst_severity)
			worst_severity = severity;
	}

	void AddGap(int H)
	{
		cy += H;
	}

	check_result_e Run()
	{
		set_modal();

		show();

		while (! (want_close || user_action != CKR_OK))
			Fl::wait(0.2);

		if (user_action != CKR_OK)
			return user_action;

		switch (worst_severity)
		{
			case 0:  return CKR_OK;
			case 1:  return CKR_MinorProblem;
			default: return CKR_MajorProblem;
		}
	}
};


check_result_e CHECK_Vertices(bool all_mode = false)
{
	UI_Check_Vertices *dialog = new UI_Check_Vertices(all_mode);

	selection_c  sel;

	for (;;)
	{
		Vertex_FindOverlaps(sel);

		if (sel.empty())
			dialog->AddLine("No overlapping vertices");
		else
		{
			int approx_num = sel.count_obj() / 2;

			sprintf(check_message, "%d overlapping vertices", approx_num);

			dialog->AddLine(check_message, 2, 210,
			                "Merge", &UI_Check_Vertices::action_merge,
			                "Show",  &UI_Check_Vertices::action_highlight);
		}


		Vertex_FindUnused(sel);

		if (sel.empty())
			dialog->AddLine("No unused vertices");
		else
		{
			sprintf(check_message, "%d unused vertices", sel.count_obj());

			dialog->AddLine(check_message, 1, 170,
			                "Remove", &UI_Check_Vertices::action_remove);
		}


		int worst_severity = dialog->worst_severity;

		// when checking "ALL" stuff, ignore any minor problems
		if (all_mode && worst_severity < 2)
		{
			delete dialog;

			return CKR_OK;
		}

		check_result_e result = dialog->Run();

		if (result == CKR_TookAction)
		{
			// repeat the tests
			dialog->Reset();
			continue;
		}

		delete dialog;

		return result;
	}
}


//------------------------------------------------------------------------

class UI_Check_Sectors : public Fl_Double_Window
{
private:
	bool want_close;

	check_result_e user_action;

	Fl_Group  *line_group;
	Fl_Button *ok_but;

	int cy;

public:
	int worst_severity;

public:
	static void close_callback(Fl_Widget *w, void *data)
	{
		UI_Check_Sectors *dialog = (UI_Check_Sectors *)data;

		dialog->want_close = true;
	}

	static void action_remove(Fl_Widget *w, void *data)
	{
		UI_Check_Sectors *dialog = (UI_Check_Sectors *)data;

		Sectors_RemoveUnused();

		dialog->user_action = CKR_TookAction;
	}

	static void action_remove_sidedefs(Fl_Widget *w, void *data)
	{
		UI_Check_Sectors *dialog = (UI_Check_Sectors *)data;

		SideDefs_RemoveUnused();

		dialog->user_action = CKR_TookAction;
	}

	static void action_fix_ceil(Fl_Widget *w, void *data)
	{
		UI_Check_Sectors *dialog = (UI_Check_Sectors *)data;

		Sectors_FixBadCeil();

		dialog->user_action = CKR_TookAction;
	}

	static void action_show_ceil(Fl_Widget *w, void *data)
	{
		UI_Check_Sectors *dialog = (UI_Check_Sectors *)data;

		Sectors_HighlightBadCeil();

		dialog->user_action = CKR_Highlight;
	}

	static void action_unpack(Fl_Widget *w, void *data)
	{
		UI_Check_Sectors *dialog = (UI_Check_Sectors *)data;

		SideDefs_Unpack(true);

		dialog->user_action = CKR_Highlight;
	}

	static void action_show_packed(Fl_Widget *w, void *data)
	{
		UI_Check_Sectors *dialog = (UI_Check_Sectors *)data;

		SideDefs_HighlightPacked();

		dialog->user_action = CKR_Highlight;
	}

public:
	UI_Check_Sectors(bool all_mode) :
		Fl_Double_Window(520, 286, "Check : Sectors"),
		want_close(false), user_action(CKR_OK),
		worst_severity(0)
	{
		cy = 10;

		callback(close_callback, this);

		int ey = h() - 66;

		Fl_Box *title = new Fl_Box(FL_NO_BOX, 10, cy, w() - 20, 30, "Sector check results");
		title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
		title->labelfont(FL_HELVETICA_BOLD);
		title->labelsize(FL_NORMAL_SIZE + 2);

		cy = 45;

		line_group = new Fl_Group(0, 0, w(), ey);
		line_group->end();

		{ Fl_Group *o = new Fl_Group(0, ey, w(), 66);

		  o->box(FL_FLAT_BOX);
		  o->color(WINDOW_BG, WINDOW_BG);

		  int but_W = all_mode ? 110 : 70;

		  { ok_but = new Fl_Button(w()/2 - but_W/2, ey + 18, but_W, 34,
		                           all_mode ? "Continue" : "OK");
			ok_but->labelfont(1);
			ok_but->callback(close_callback, this);
		  }
		  o->end();
		}

		end();
	}

	void Reset()
	{
		want_close = false;
		user_action = CKR_OK;

		cy = 45;

		line_group->clear();	

		redraw();
	}

	void AddLine(const char *msg, int severity = 0, int W = -1,
	             const char *button1 = NULL, Fl_Callback *cb1 = NULL,
	             const char *button2 = NULL, Fl_Callback *cb2 = NULL,
	             const char *button3 = NULL, Fl_Callback *cb3 = NULL)
	{
		int cx = 25;

		if (W < 0)
			W = w() - 40;

		Fl_Box *box = new Fl_Box(FL_NO_BOX, cx, cy, W, 25, NULL);
		box->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
		box->copy_label(msg);

		if (severity == 2)
		{
			box->labelcolor(ERROR_MSG_COLOR);
			box->labelfont(FL_HELVETICA_BOLD);
		}
		else if (severity == 1)
		{
			box->labelcolor(WARNING_MSG_COLOR);
			box->labelfont(FL_HELVETICA_BOLD);
		}

		line_group->add(box);

		cx += W;

		if (button1)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button1);
			but->callback(cb1, this);

			line_group->add(but);

			cx += but->w() + 10;
		}

		if (button2)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button2);
			but->callback(cb2, this);

			line_group->add(but);

			cx += but->w() + 10;
		}

		if (button3)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button3);
			but->callback(cb3, this);

			line_group->add(but);
		}

		cy = cy + 30;

		if (severity > worst_severity)
			worst_severity = severity;
	}

	void AddGap(int H)
	{
		cy += H;
	}

	check_result_e Run()
	{
		set_modal();

		show();

		while (! (want_close || user_action != CKR_OK))
			Fl::wait(0.2);

		if (user_action != CKR_OK)
			return user_action;

		switch (worst_severity)
		{
			case 0:  return CKR_OK;
			case 1:  return CKR_MinorProblem;
			default: return CKR_MajorProblem;
		}
	}
};


check_result_e CHECK_Sectors(bool all_mode = false)
{
	UI_Check_Sectors *dialog = new UI_Check_Sectors(all_mode);

	selection_c  sel, lines;

	for (;;)
	{
		Sectors_FindBadCeil(sel);

		if (sel.empty())
			dialog->AddLine("No sectors with ceil < floor");
		else
		{
			sprintf(check_message, "%d sectors with ceil < floor", sel.count_obj());

			dialog->AddLine(check_message, 2, 220,
			                "Fix",  &UI_Check_Sectors::action_fix_ceil,
			                "Show", &UI_Check_Sectors::action_show_ceil);
		}


		SideDefs_FindPacking(sel, lines);

		if (sel.empty())
			dialog->AddLine("No shared sidedefs");
		else
		{
			int approx_num = sel.count_obj();

			sprintf(check_message, "%d shared sidedefs", approx_num);

			dialog->AddLine(check_message, 1, 200,
			                "Unpack", &UI_Check_Sectors::action_unpack,
			                "Show",   &UI_Check_Sectors::action_show_packed);
		}


		Sectors_FindUnused(sel);

		if (sel.empty())
			dialog->AddLine("No unused sectors");
		else
		{
			sprintf(check_message, "%d unused sectors", sel.count_obj());

			dialog->AddLine(check_message, 1, 170,
			                "Remove", &UI_Check_Sectors::action_remove);
		}


		SideDefs_FindUnused(sel);

		if (sel.empty())
			dialog->AddLine("No unused sidedefs");
		else
		{
			sprintf(check_message, "%d unused sidedefs", sel.count_obj());

			dialog->AddLine(check_message, 1, 170,
			                "Remove", &UI_Check_Sectors::action_remove_sidedefs);
		}


		int worst_severity = dialog->worst_severity;

		// when checking "ALL" stuff, ignore any minor problems
		if (all_mode && worst_severity < 2)
		{
			delete dialog;

			return CKR_OK;
		}

		check_result_e result = dialog->Run();

		if (result == CKR_TookAction)
		{
			// repeat the tests
			dialog->Reset();
			continue;
		}

		delete dialog;

		return result;
	}
}


//------------------------------------------------------------------------

check_result_e CHECK_LineDefs(bool all_mode = false)
{
	// TODO

	return CKR_OK;
}


//------------------------------------------------------------------------

class UI_Check_Things : public Fl_Double_Window
{
private:
	bool want_close;

	check_result_e user_action;

	Fl_Group  *line_group;
	Fl_Button *ok_but;

	int cy;

public:
	int worst_severity;

public:
	static void close_callback(Fl_Widget *w, void *data)
	{
		UI_Check_Things *dialog = (UI_Check_Things *)data;

		dialog->want_close = true;
	}

	static void action_show_unknown(Fl_Widget *w, void *data)
	{
		UI_Check_Things *dialog = (UI_Check_Things *)data;

		Things_HighlightUnknown();

		dialog->user_action = CKR_Highlight;
	}

	static void action_show_void(Fl_Widget *w, void *data)
	{
		UI_Check_Things *dialog = (UI_Check_Things *)data;

		Things_HighlightInVoid();

		dialog->user_action = CKR_Highlight;
	}

public:
	UI_Check_Things(bool all_mode) :
		Fl_Double_Window(520, 286, "Check : Things"),
		want_close(false), user_action(CKR_OK),
		worst_severity(0)
	{
		cy = 10;

		callback(close_callback, this);

		int ey = h() - 66;

		Fl_Box *title = new Fl_Box(FL_NO_BOX, 10, cy, w() - 20, 30, "Thing check results");
		title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
		title->labelfont(FL_HELVETICA_BOLD);
		title->labelsize(FL_NORMAL_SIZE + 2);

		cy = 45;

		line_group = new Fl_Group(0, 0, w(), ey);
		line_group->end();

		{ Fl_Group *o = new Fl_Group(0, ey, w(), 66);

		  o->box(FL_FLAT_BOX);
		  o->color(WINDOW_BG, WINDOW_BG);

		  int but_W = all_mode ? 110 : 70;

		  { ok_but = new Fl_Button(w()/2 - but_W/2, ey + 18, but_W, 34,
		                           all_mode ? "Continue" : "OK");
			ok_but->labelfont(1);
			ok_but->callback(close_callback, this);
		  }
		  o->end();
		}

		end();
	}

	void Reset()
	{
		want_close = false;
		user_action = CKR_OK;

		cy = 45;

		line_group->clear();	

		redraw();
	}

	void AddLine(const char *msg, int severity = 0, int W = -1,
	             const char *button1 = NULL, Fl_Callback *cb1 = NULL,
	             const char *button2 = NULL, Fl_Callback *cb2 = NULL,
	             const char *button3 = NULL, Fl_Callback *cb3 = NULL)
	{
		int cx = 25;

		if (W < 0)
			W = w() - 40;

		Fl_Box *box = new Fl_Box(FL_NO_BOX, cx, cy, W, 25, NULL);
		box->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
		box->copy_label(msg);

		if (severity == 2)
		{
			box->labelcolor(ERROR_MSG_COLOR);
			box->labelfont(FL_HELVETICA_BOLD);
		}
		else if (severity == 1)
		{
			box->labelcolor(WARNING_MSG_COLOR);
			box->labelfont(FL_HELVETICA_BOLD);
		}

		line_group->add(box);

		cx += W;

		if (button1)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button1);
			but->callback(cb1, this);

			line_group->add(but);

			cx += but->w() + 10;
		}

		if (button2)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button2);
			but->callback(cb2, this);

			line_group->add(but);

			cx += but->w() + 10;
		}

		if (button3)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button3);
			but->callback(cb3, this);

			line_group->add(but);
		}

		cy = cy + 30;

		if (severity > worst_severity)
			worst_severity = severity;
	}

	void AddGap(int H)
	{
		cy += H;
	}

	check_result_e Run()
	{
		set_modal();

		show();

		while (! (want_close || user_action != CKR_OK))
			Fl::wait(0.2);

		if (user_action != CKR_OK)
			return user_action;

		switch (worst_severity)
		{
			case 0:  return CKR_OK;
			case 1:  return CKR_MinorProblem;
			default: return CKR_MajorProblem;
		}
	}
};


check_result_e CHECK_Things(bool all_mode = false)
{
	UI_Check_Things *dialog = new UI_Check_Things(all_mode);

	selection_c  sel;

	for (;;)
	{
		// TODO: check for things stuck in walls / other things
		//       [need a new 'non-solid' flag in the game definition files]


		Things_FindInVoid(sel);

		if (sel.empty())
			dialog->AddLine("No things in the void");
		else
		{
			sprintf(check_message, "%d things in the void", sel.count_obj());

			dialog->AddLine(check_message, 1, 200,
			                "Show",  &UI_Check_Things::action_show_void);
		}


		Things_FindUnknown(sel);

		if (sel.empty())
			dialog->AddLine("No unknown thing types");
		else
		{
			sprintf(check_message, "%d unknown things", sel.count_obj());

			dialog->AddLine(check_message, 1, 210,
			                "Show",  &UI_Check_Things::action_show_unknown);
		}


		int dm_num, mask;

		mask = Things_FindStarts(&dm_num);

		if (! (mask & 1))
			dialog->AddLine("Player 1 start is missing!", 2);
		else if (! (mask & 2))
			dialog->AddLine("Player 2 start is missing", 1);
		else if (! (mask & 4))
			dialog->AddLine("Player 3 start is missing", 1);
		else if (! (mask & 8))
			dialog->AddLine("Player 4 start is missing", 1);
		else
			dialog->AddLine("Player 1..4 starts are present");

		if (dm_num == 0)
			dialog->AddLine("Map is missing deathmatch starts", 1);
		else if (dm_num < DOOM_MIN_DEATHMATCH_STARTS)
		{
			sprintf(check_message, "Map has only %d deathmatch starts -- need at least %d", dm_num,
			        DOOM_MIN_DEATHMATCH_STARTS);
			dialog->AddLine(check_message, 1);
		}
		else
		{
			sprintf(check_message, "Map has %d deathmatch starts", dm_num);
			dialog->AddLine(check_message);
		}


		int worst_severity = dialog->worst_severity;

		// when checking "ALL" stuff, ignore any minor problems
		if (all_mode && worst_severity < 2)
		{
			delete dialog;

			return CKR_OK;
		}

		check_result_e result = dialog->Run();

		if (result == CKR_TookAction)
		{
			// repeat the tests
			dialog->Reset();
			continue;
		}

		delete dialog;

		return result;
	}
}


//------------------------------------------------------------------------

class UI_Check_Tags : public Fl_Double_Window
{
private:
	bool want_close;

	check_result_e user_action;

	Fl_Group  *line_group;
	Fl_Button *ok_but;

	int cy;

public:
	int worst_severity;
	int fresh_tag;

public:
	static void close_callback(Fl_Widget *w, void *data)
	{
		UI_Check_Tags *dialog = (UI_Check_Tags *)data;

		dialog->want_close = true;
	}

	static void action_fresh_tag(Fl_Widget *w, void *data)
	{
		UI_Check_Tags *dialog = (UI_Check_Tags *)data;

		// fresh_tag is set externally
		Tags_ApplyNewValue(dialog->fresh_tag);

		dialog->want_close = true;
	}

public:
	UI_Check_Tags(bool all_mode) :
		Fl_Double_Window(520, 226, "Check : Tags"),
		want_close(false), user_action(CKR_OK),
		worst_severity(0)
	{
		cy = 10;

		callback(close_callback, this);

		int ey = h() - 66;

		Fl_Box *title = new Fl_Box(FL_NO_BOX, 10, cy, w() - 20, 30, "Tag check results");
		title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
		title->labelfont(FL_HELVETICA_BOLD);
		title->labelsize(FL_NORMAL_SIZE + 2);

		cy = 45;

		line_group = new Fl_Group(0, 0, w(), ey);
		line_group->end();

		{ Fl_Group *o = new Fl_Group(0, ey, w(), 66);

		  o->box(FL_FLAT_BOX);
		  o->color(WINDOW_BG, WINDOW_BG);

		  int but_W = all_mode ? 110 : 70;

		  { ok_but = new Fl_Button(w()/2 - but_W/2, ey + 18, but_W, 34,
		                           all_mode ? "Continue" : "OK");
			ok_but->labelfont(1);
			ok_but->callback(close_callback, this);
		  }
		  o->end();
		}

		end();
	}

	void Reset()
	{
		want_close = false;
		user_action = CKR_OK;

		cy = 45;

		line_group->clear();	

		redraw();
	}

	void AddLine(const char *msg, int severity = 0, int W = -1,
	             const char *button1 = NULL, Fl_Callback *cb1 = NULL,
	             const char *button2 = NULL, Fl_Callback *cb2 = NULL,
	             const char *button3 = NULL, Fl_Callback *cb3 = NULL)
	{
		int cx = 25;

		if (W < 0)
			W = w() - 40;

		Fl_Box *box = new Fl_Box(FL_NO_BOX, cx, cy, W, 25, NULL);
		box->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
		box->copy_label(msg);

		if (severity == 2)
		{
			box->labelcolor(ERROR_MSG_COLOR);
			box->labelfont(FL_HELVETICA_BOLD);
		}
		else if (severity == 1)
		{
			box->labelcolor(WARNING_MSG_COLOR);
			box->labelfont(FL_HELVETICA_BOLD);
		}

		line_group->add(box);

		cx += W;

		if (button1)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button1);
			but->callback(cb1, this);

			line_group->add(but);

			cx += but->w() + 10;
		}

		if (button2)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button2);
			but->callback(cb2, this);

			line_group->add(but);

			cx += but->w() + 10;
		}

		if (button3)
		{
			Fl_Button *but = new Fl_Button(cx, cy, 80, 25, button3);
			but->callback(cb3, this);

			line_group->add(but);
		}

		cy = cy + 30;

		if (severity > worst_severity)
			worst_severity = severity;
	}

	void AddGap(int H)
	{
		cy += H;
	}

	check_result_e Run()
	{
		set_modal();

		show();

		while (! (want_close || user_action != CKR_OK))
			Fl::wait(0.2);

		if (user_action != CKR_OK)
			return user_action;

		switch (worst_severity)
		{
			case 0:  return CKR_OK;
			case 1:  return CKR_MinorProblem;
			default: return CKR_MajorProblem;
		}
	}
};


check_result_e CHECK_Tags()
{
	UI_Check_Tags *dialog = new UI_Check_Tags(false);

	selection_c  sel;

	for (;;)
	{
		int min_tag, max_tag;

		Tags_UsedRange(&min_tag, &max_tag);

		if (max_tag <= 0)
			dialog->AddLine("No tags are in use");
		else
		{
			sprintf(check_message, "Lowest  tag: %d", min_tag);
			dialog->AddLine(check_message);

			sprintf(check_message, "Highest tag: %d", max_tag);
			dialog->AddLine(check_message);
		}

		if ((edit.mode == OBJ_LINEDEFS || edit.mode == OBJ_SECTORS) &&
		    edit.Selected->notempty())
		{
			dialog->fresh_tag = max_tag + 1;

			dialog->AddGap(10);
			dialog->AddLine("Apply fresh tag to selection :", 0, 215, "Apply",
			                &UI_Check_Tags::action_fresh_tag);
		}

		check_result_e result = dialog->Run();

		if (result == CKR_TookAction)
		{
			// repeat the tests
			dialog->Reset();
			continue;
		}

		delete dialog;

		return result;
	}
}


//------------------------------------------------------------------------

void CHECK_All()
{
	bool no_worries = true;

	check_result_e result;

	result = CHECK_Vertices(true);
	if (result == CKR_Highlight) return;
	if (result != CKR_OK) no_worries = false;

	result = CHECK_Sectors(true);
	if (result == CKR_Highlight) return;
	if (result != CKR_OK) no_worries = false;

	result = CHECK_LineDefs(true);
	if (result == CKR_Highlight) return;
	if (result != CKR_OK) no_worries = false;

	result = CHECK_Things(true);
	if (result == CKR_Highlight) return;
	if (result != CKR_OK) no_worries = false;

	if (no_worries)
	{
		Notify(-1, -1, "No major problems.", NULL);
	}
}


//------------------------------------------------------------------------


void CMD_CheckMap()
{
	const char *what = EXEC_Param[0];

	// TODO "textures" and "tags"

	if (! what[0] || y_stricmp(what, "all") == 0)
	{
		CHECK_All();
	}
	else if (y_stricmp(what, "vertices") == 0)
	{
		CHECK_Vertices();
	}
	else if (y_stricmp(what, "sectors") == 0)
	{
		CHECK_Sectors();
	}
	else if (y_stricmp(what, "linedefs") == 0)
	{
		CHECK_LineDefs();
	}
	else if (y_stricmp(what, "things") == 0)
	{
		CHECK_Things();
	}
	else if (y_stricmp(what, "current") == 0)  // current editing mode
	{
		switch (edit.mode)
		{
			case OBJ_VERTICES:
				CHECK_Vertices();
				break;

			case OBJ_SECTORS:
				CHECK_Sectors();
				break;

			case OBJ_LINEDEFS:
				CHECK_LineDefs();
				break;

			case OBJ_THINGS:
				CHECK_Things();
				break;

			default:
				Beep("Nothing to check");
				break;
		}
	}
	else if (y_stricmp(what, "tags") == 0)
	{
		CHECK_Tags();
	}
	else
	{
		Beep("Check: unknown keyword '%s'\n", what);
	}
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
