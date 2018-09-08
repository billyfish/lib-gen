/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: libgen
**
** File: gui.c
**
** Date: 08-09-2018 00:32:53
**
************************************************************

*/

#include <classes/window.h>
#include <gadgets/layout.h>

/*
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>
*/

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/layout.h>
#include <proto/checkbox.h>
#include <proto/window.h>

#include "debugging_utils.h"
#include "gui.h"

enum
{
	GID_MAIN = 0,
	GID_RECURSE_CHECKBOX,
	GID_LAST
};




BOOL OpenPrefsGUI (LibGenPrefs *prefs_p)
{
	BOOL success_flag = FALSE;

    // Make sure class files were loaded.
    if (WindowBase == NULL || LayoutBase == NULL || CheckBoxBase == NULL)
    	{
        DB (KPRINTF ("%s %ld - Failed to open gui libs\n", __FILE__, __LINE__));	
        return success_flag;
    	}


	struct MsgPort *app_port_p = IExec -> AllocSysObjectTags (ASOT_PORT, TAG_DONE);

	if (app_port_p)
		{
			struct Gadget *gadgets_p [GID_LAST];
		
			Object *main_obj_p = IIntuition -> NewObject (NULL, "window.class",
				WA_Title, "LibGen",
				WA_Activate, TRUE,
				WA_SizeGadget, TRUE,
				WA_DepthGadget, TRUE,
				WA_DragBar, TRUE,
				WA_CloseGadget, TRUE,
				WINDOW_IconifyGadget,  TRUE,
				WINDOW_Position,       WPOS_CENTERSCREEN,
				WINDOW_AppPort, app_port_p,
        WINDOW_Layout, IIntuition -> NewObject (NULL, "layout.gadget",
        	LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
					LAYOUT_SpaceOuter, TRUE,
					LAYOUT_DeferLayout, TRUE,
					LAYOUT_AddChild, gadgets_p [GID_RECURSE_CHECKBOX] = (struct Gadget *) IIntuition -> NewObject (NULL, "checkbox.gadget",
						GA_Text, "Recurse",
					TAG_DONE),
		
				TAG_DONE),
				
			TAG_DONE);	

	 	//  Object creation sucessful?
		if (main_obj_p)
			{
				//  Open the window.
				struct Window *window_p = (struct Window *) IIntuition -> IDoMethod  (main_obj_p, WM_OPEN);

				if (window_p)
					{
						ULONG wait, signal, app = (1L << app_port_p -> mp_SigBit);
						ULONG done = FALSE;
						ULONG result;
						UWORD code;
		
					 	// Obtain the window wait signal mask.
						IIntuition -> GetAttr (WINDOW_SigMask, main_obj_p, &signal);

						// Input Event Loop		
						while (!done)
							{
								wait = IExec->Wait (signal | SIGBREAKF_CTRL_C | app);
		
								if ( wait & SIGBREAKF_CTRL_C )
									{
										done = TRUE;
									}
								else
									{
										while ((result = IIntuition -> IDoMethod (main_obj_p, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
											{
												switch (result & WMHI_CLASSMASK)
													{
														case WMHI_CLOSEWINDOW:
															window_p = NULL;
															done = TRUE;
															break;
			
														case WMHI_GADGETUP:
															switch (result & WMHI_GADGETMASK)
																{

																}
															break;
						
														case WMHI_ICONIFY:
															IIntuition -> IDoMethod (main_obj_p, WM_ICONIFY);
															window_p = NULL;
															break;
						
														case WMHI_UNICONIFY:
															window_p = (struct Window *) IIntuition -> IDoMethod (main_obj_p, WM_OPEN);
	
															if (window_p)
																{
																	IIntuition -> GetAttr (WINDOW_SigMask, main_obj_p, &signal);
																}
															else
																{
																	done = TRUE;	// error re-opening window!
																}
												 			break;
													}
											}
									}
							}
					}
				else
					{	
						DB (KPRINTF ("%s %ld - Failed to open window\n", __FILE__, __LINE__));			
					}
				
				/* Disposing of the window object will also close the window if it is
				 * already opened, and it will dispose of the layout object attached to it.
				 */
				IIntuition->DisposeObject (main_obj_p);
			}
		else
			{
				DB (KPRINTF ("%s %ld - Failed to create main object\n", __FILE__, __LINE__));			
			}
				
			IExec->FreeSysObject (ASOT_PORT, app_port_p);
		}		/* if (app_port_p) */
	else
		{
			DB (KPRINTF ("%s %ld - Failed to open app port\n", __FILE__, __LINE__));			
		}
		
	return success_flag;
}
