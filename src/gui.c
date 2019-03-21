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

#include <gadgets/checkbox.h>
#include <gadgets/getfile.h>
#include <gadgets/integer.h>
#include <gadgets/layout.h>
#include <gadgets/radiobutton.h>
#include <gadgets/string.h>

#include <images/label.h>

#include <proto/chooser.h>
#include <proto/checkbox.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/getfile.h>
#include <proto/integer.h>
#include <proto/intuition.h>
#include <proto/label.h>
#include <proto/layout.h>
#include <proto/radiobutton.h>
#include <proto/string.h>
#include <proto/window.h>


#include "debugging_utils.h"
#include "gui.h"


enum
{
	GID_MAIN = 0,
	GID_HEADERS_DIR,
	GID_SOURCES_DIR,
	GID_RECURSE_CHECKBOX,
	GID_LIBRARY_NAME,
	GID_HEADER_FILE_PATTERN,
	GID_SOURCES_FILE_PATTERN,
	GID_PROTOTYPE_PATTERN,
	GID_VERSION_INT,
	GID_FLAGS,
	GID_GENERATE_CODE_CHECKBOX,
	GID_VERBOSE,
	GID_NEWLIB,
	GID_ORDERING_FILENAME,
	GID_EXCLUDED_FUNCTIONS_FILENAME,
	GID_PATHS_TO_IGNORE,
	GID_RUN,
	GID_LAST
};


static struct List *GetRadioButtonLabels (void);


BOOL OpenPrefsGUI (LibGenPrefs *prefs_p)
{
	BOOL success_flag = FALSE;
	struct MsgPort *app_port_p = NULL;

  // Make sure class files were loaded.
  if (WindowBase == NULL || LayoutBase == NULL || CheckBoxBase == NULL || GetFileBase == NULL || LabelBase == NULL || IntegerBase == NULL || StringBase == NULL || RadioButtonBase == NULL)
  	{
      DB (KPRINTF ("%s %ld - Failed to open gui libs\n", __FILE__, __LINE__));	
      return success_flag;
  	}


	app_port_p = IExec -> AllocSysObjectTags (ASOT_PORT, TAG_DONE);

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
//				WA_IDCMP,        IDCMP_GADGETUP | IDCMP_CLOSEWINDOW | IDCMP_MENUPICK,
//				WINDOW_IconifyGadget,  TRUE,
				WINDOW_Position,       WPOS_CENTERSCREEN,
//				WINDOW_AppPort, app_port_p,
				WINDOW_Layout, IIntuition -> NewObject (NULL, "layout.gadget",
					LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
					LAYOUT_SpaceOuter, TRUE,
					LAYOUT_DeferLayout, TRUE,
				
					/* Library Name */
					LAYOUT_AddChild, gadgets_p [GID_LIBRARY_NAME] = (struct Gadget *) IIntuition -> NewObject (NULL, "string.gadget",
						GA_Text, "Library name",
						GA_ID, GID_LIBRARY_NAME,
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Library name", 
					TAG_DONE),	
					
					/* Headers directory */
					LAYOUT_AddChild, gadgets_p [GID_HEADERS_DIR] = (struct Gadget *) IIntuition -> NewObject (NULL, "getfile.gadget",
						GA_ID, GID_HEADERS_DIR,
						GETFILE_TitleText, "Choose Headers Directory",
						GETFILE_DrawersOnly, TRUE,
						GETFILE_Pattern,            "#?",
						GETFILE_DoSaveMode,         FALSE,
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Headers directory", 
					TAG_DONE),					
					
					/* Headers File Pattern  */
					LAYOUT_AddChild, gadgets_p [GID_HEADER_FILE_PATTERN] = (struct Gadget *) IIntuition -> NewObject (NULL, "string.gadget",
						GA_ID, GID_HEADER_FILE_PATTERN,
						STRINGA_TextVal, "#?.h",
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Headers file pattern", 
					TAG_DONE),				
					

					/* Header paths to ignore */
					LAYOUT_AddChild, gadgets_p [GID_PATHS_TO_IGNORE] = (struct Gadget *) IIntuition -> NewObject (NULL, "string.gadget",
						GA_ID, GID_PATHS_TO_IGNORE,
						STRINGA_TextVal, "",
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Header directories to ignore", 
					TAG_DONE),		

					/* Sources directory */
					LAYOUT_AddChild, gadgets_p [GID_SOURCES_DIR] = (struct Gadget *) IIntuition -> NewObject (NULL, "getfile.gadget",
						GA_ID, GID_SOURCES_DIR,
						GETFILE_TitleText, "Choose Sources Directory",
						GETFILE_DrawersOnly, TRUE,
						GETFILE_Pattern,            "#?",
						GETFILE_DoSaveMode,         FALSE,
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Sources directory", 
					TAG_DONE),
	
					
					/* Sources File Pattern  */
					LAYOUT_AddChild, gadgets_p [GID_SOURCES_FILE_PATTERN] = (struct Gadget *) IIntuition -> NewObject (NULL, "string.gadget",
						GA_ID, GID_SOURCES_FILE_PATTERN,
						STRINGA_TextVal, "#?.c",
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Sources file pattern", 
					TAG_DONE),				

					
					/* Prototype Pattern  */
					LAYOUT_AddChild, gadgets_p [GID_PROTOTYPE_PATTERN] = (struct Gadget *) IIntuition -> NewObject (NULL, "string.gadget",
						GA_ID, GID_PROTOTYPE_PATTERN,
						STRINGA_TextVal, "LIB_API {#?};",
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Prototype pattern", 
					TAG_DONE),		
										
					/* Recurse checkbox */
					LAYOUT_AddChild, gadgets_p [GID_RECURSE_CHECKBOX] = (struct Gadget *) IIntuition -> NewObject (NULL, "checkbox.gadget",
						GA_Text, "Recurse",
						GA_ID, GID_RECURSE_CHECKBOX,
						GA_RelVerify, TRUE,						
					TAG_DONE),

					/* Generate code checkbox */
					LAYOUT_AddChild, gadgets_p [GID_GENERATE_CODE_CHECKBOX] = (struct Gadget *) IIntuition -> NewObject (NULL, "checkbox.gadget",
						GA_Text, "Generate code",
						GA_ID, GID_GENERATE_CODE_CHECKBOX,
						GA_RelVerify, TRUE,						
					TAG_DONE),

					/* Generate code for newlib */
					LAYOUT_AddChild, gadgets_p [GID_NEWLIB] = (struct Gadget *) IIntuition -> NewObject (NULL, "checkbox.gadget",
						GA_Text, "Generated code uses NewLib",
						GA_ID, GID_NEWLIB,
						GA_RelVerify, TRUE,						
					TAG_DONE),

					/* Interface version */
					LAYOUT_AddChild, gadgets_p [GID_VERSION_INT] = (struct Gadget *) IIntuition -> NewObject (NULL, "integer.gadget",
						GA_ID, GID_VERSION_INT,
						GA_RelVerify, TRUE,						
						INTEGER_Minimum, 1,
						INTEGER_Number, 1,
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Interface version", 
					TAG_DONE),				
	
					/* Interface flags */
					LAYOUT_AddChild, gadgets_p [GID_FLAGS] = (struct Gadget *) IIntuition -> NewObject (NULL, "radiobutton.gadget",
						GA_ID, GID_FLAGS,
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Interface flags", 
					TAG_DONE),	

					/* Ordering filename */
					LAYOUT_AddChild, gadgets_p [GID_ORDERING_FILENAME] = (struct Gadget *) IIntuition -> NewObject (NULL, "getfile.gadget",
						GA_ID, GID_ORDERING_FILENAME,
						GETFILE_TitleText, "Choose file containing function ordering",
						GETFILE_Pattern,            "#?",
						GETFILE_DoSaveMode,         FALSE,
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Function order filename", 
					TAG_DONE),
			
					/* Excluded functions filename */
					LAYOUT_AddChild, gadgets_p [GID_EXCLUDED_FUNCTIONS_FILENAME] = (struct Gadget *) IIntuition -> NewObject (NULL, "getfile.gadget",
						GA_ID, GID_EXCLUDED_FUNCTIONS_FILENAME,
						GETFILE_TitleText, "Choose file containing excluded functions",
						GETFILE_Pattern,            "#?",
						GETFILE_DoSaveMode,         FALSE,
					TAG_DONE),
					CHILD_Label,  IIntuition -> NewObject (NULL, "label.image",
						LABEL_Text, "Functions to ignore", 
					TAG_DONE),

					/* Verbose */
					LAYOUT_AddChild, gadgets_p [GID_VERBOSE] = (struct Gadget *) IIntuition -> NewObject (NULL, "checkbox.gadget",
						GA_Text, "Verbose",
						GA_ID, GID_VERBOSE,
						GA_RelVerify, TRUE,						
					TAG_DONE),

					/* Run */
					LAYOUT_AddChild, gadgets_p [GID_RUN] = (struct Gadget *) IIntuition -> NewObject (NULL, "button.gadget",
						GA_Text, "Run",
						GA_ID, GID_RUN,
						GA_RelVerify, TRUE,						
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
						uint32 signal; /*, app = (1L << app_port_p -> mp_SigBit) */;
						uint32 done = FALSE;
						uint32 value;
						
					 	// Obtain the window wait signal mask.
						IIntuition -> GetAttr (WINDOW_SigMask, main_obj_p, &signal);

						// Input Event Loop		
						while (!done)
							{
								uint32 wait = IExec->Wait (signal | SIGBREAKF_CTRL_C /* | app*/ );
		
								if (wait & SIGBREAKF_CTRL_C)
									{
										done = TRUE;
									}
								else if (wait & signal)
									{
										uint32 result = WMHI_LASTMSG;
										int16 code = 0;

										while ((result = IIntuition->IDoMethod (main_obj_p, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG)
											{
												DB (KPRINTF ("%s %ld - code %d result %lu mapped %lu\n", __FILE__, __LINE__, code, result, result & WMHI_CLASSMASK));
												                    	IDOS->Printf ("res %lu code %d WMHI_CLASSMASK %lu\n", result, code, WMHI_CLASSMASK);
												                    	
												switch (result & WMHI_CLASSMASK)
													{
														case WMHI_CLOSEWINDOW:
															DB (KPRINTF ("%s %ld - WMHI_CLOSEWINDOW\n", __FILE__, __LINE__));
															window_p = NULL;
															done = TRUE;
															break;


			
														case WMHI_GADGETDOWN:
															DB (KPRINTF ("%s %ld - WMHI_GADGETDOWN result %ld\n", __FILE__, __LINE__, result));
															
															switch (result & WMHI_GADGETMASK)
																{
																	case GID_GENERATE_CODE_CHECKBOX:
																		{
																			uint32 res = IIntuition -> GetAttr (CHECKBOX_Checked, (Object *) gadgets_p [GID_GENERATE_CODE_CHECKBOX], &value);
																			DB (KPRINTF ("%s %ld - GID_GENERATE_CODE_CHECKBOX res %ld val %ld\n", __FILE__, __LINE__, res, value));
																		}
																		break;
							
																	default:
																		break;
																}
															break;
															
			
														case WMHI_GADGETUP:
															DB (KPRINTF ("%s %ld - WMHI_GADGETUP result %ld\n", __FILE__, __LINE__, result));
															
															switch (result & WMHI_GADGETMASK)
																{
																	case GID_GENERATE_CODE_CHECKBOX:
																		{
																			uint32 res = IIntuition -> GetAttr (CHECKBOX_Checked, (Object *) gadgets_p [GID_GENERATE_CODE_CHECKBOX], &value);
																			DB (KPRINTF ("%s %ld - GID_GENERATE_CODE_CHECKBOX res %ld val %ld\n", __FILE__, __LINE__, res, value));
																		}
																		break;
							
																	default:
																		break;
																}
															break;
						
														case WMHI_ICONIFY:
															DB (KPRINTF ("%s %ld - WMHI_ICONIFY\n", __FILE__, __LINE__));
															IIntuition -> IDoMethod (main_obj_p, WM_ICONIFY);
															window_p = NULL;
															break;
						
														case WMHI_UNICONIFY:
															DB (KPRINTF ("%s %ld - WMHI_UNICONIFY\n", __FILE__, __LINE__));															
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
												 			
												 		default:
												 			DB (KPRINTF ("%s %ld - unknown result %ld\n", __FILE__, __LINE__, result));
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


static struct List *GetRadioButtonLabels (void)
{	
	struct List *labels_p = NULL;

	return NULL;
}

