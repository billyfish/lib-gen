#include <ctype.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "debugging_utils.h"

#include "parameter.h"
#include "utils.h"



#ifdef _DEBUG
#define PARAMETER_DEBUG (1)
#endif

/**************************/

static BOOL SetParameterValue (char **param_value_ss, const char *start_p, const char *end_p);

/**************************/

struct Parameter *ParseFunctionPointerParameter (const char *start_p, const char *end_p)
{
	struct Parameter *param_p = NULL;
	const char *opening_bracket_p = strchr (start_p, '(');

	ENTER ();


	if (opening_bracket_p)
		{
			const char *name_start_p = opening_bracket_p + 1;

			/* scroll past any whitespace */
			while (isspace (*name_start_p))
				{
					++ name_start_p;
				}

			#if PARAMETER_DEBUG >= 1
			DB (KPRINTF ("%s %ld - at pointer name_start \"%s\"\n", __FILE__, __LINE__, name_start_p));
			#endif

			/* do we have the pointer? */
			if (*name_start_p == '*')
				{
					++ name_start_p;

					if (*name_start_p != '\0')
						{
							/* scroll past any whitespace */
							while (isspace (*name_start_p))
								{
									++ name_start_p;
								}

							#if PARAMETER_DEBUG >= 1
							DB (KPRINTF ("%s %ld - pre name name_start \"%s\"\n", __FILE__, __LINE__, name_start_p));
							#endif

							if ((isalpha (*name_start_p)) || (*name_start_p == '_'))
								{
									const char *name_end_p = name_start_p + 1;

									while ((isalnum (*name_end_p)) || (*name_end_p == '_'))
										{
											++ name_end_p;
										}

									#if PARAMETER_DEBUG >= 1
									DB (KPRINTF ("%s %ld - post name  name_start \"%s\"\n", __FILE__, __LINE__, name_start_p));
									#endif

									if (*name_end_p != '\0')
										{
											const char *closing_bracket_p = name_end_p;

											-- name_end_p;

											/* scroll past any whitespace */
											while (isspace (*closing_bracket_p))
												{
													++ closing_bracket_p;
												}


												#if PARAMETER_DEBUG >= 1
												DB (KPRINTF ("%s %ld - closing bracket \"%s\"\n", __FILE__, __LINE__, closing_bracket_p));
												#endif

											/* have we reached the clsoing bracket of the name? */
											if (*closing_bracket_p == ')')
												{
													/* we have the bounds of the name */
													char *name_s = CopyToNewString (name_start_p, name_end_p, FALSE);

													if (name_s)
														{
															/* Now we need to construct the type */
															char *type_s = NULL;
															size_t l = name_start_p - start_p;
															size_t m = end_p - name_end_p + 1;

															#if PARAMETER_DEBUG >= 1
															DB (KPRINTF ("%s %ld - fp name \"%s\"\n", __FILE__, __LINE__, name_s));
															#endif

															type_s = (char *) IExec->AllocVecTags (l + m + 2, TAG_DONE);

															if (type_s)
																{
																	char *data_p = type_s;

																	IExec->CopyMem (start_p, data_p, l);
																	data_p += l;

																	IExec->CopyMem (name_end_p + 1, data_p, m);
																	data_p += m;
																	*data_p = '\0';

																	#if PARAMETER_DEBUG >= 1
																	DB (KPRINTF ("%s %ld - fp type \"%s\"\n", __FILE__, __LINE__, type_s));
																	#endif

																	param_p = AllocateParameter (type_s, name_s);

																	if (!param_p)
																		{
																			IDOS->Printf ("Failed to allocate function pointer parameter");
																			IExec->FreeVec (type_s);
																		}

																}		/* if (type_s) */
															else
																{
																	IDOS->Printf ("Failed to allocate function parameter type");
																}

															if (!param_p)
																{
																	IExec->FreeVec (name_s);
																}


														}		/* if (name_s) */
													else
														{
															IDOS->Printf ("Failed to allocate function parameter name");
														}


												}		/* if (*closing_bracket_p == ')') */

										}
								}
						}

				}

		}		/* if (opening_bracket_p) */

	LEAVE ();
	return param_p;
}


struct Parameter *ParseParameter (const char *start_p, const char *end_p)
{
	struct Parameter *param_p = NULL;
	const char *type_start_p = NULL;
	const char *type_end_p = NULL;
	const char *name_start_p = NULL;
	const char *name_end_p = end_p;
	char *name_s = NULL;
	char *type_s = NULL;
	uint8 array_count = 0;
	uint8 loop_flag = TRUE;
	BOOL matched_flag = FALSE;

	ENTER ();

	/*
		Start from the end and work backwards
	*/
	while (loop_flag && !matched_flag)
		{
			const char c = *name_end_p;

			if (isspace (c))
				{

				}
			else if (c == ']')
				{
					/* find the opening array bracket */
//					BOOL matched_flag = FALSE;

					while (loop_flag && !matched_flag)
						{
							-- name_end_p;

							if (*name_end_p == '[')
								{
									++ array_count;
									matched_flag = TRUE;
								}
							else
								{
									loop_flag = (start_p < name_end_p);
								}
						}

					if (matched_flag)
						{
							/* reset the matched flag */
							matched_flag = FALSE;
						}
					else
						{
							// error;
						}
				}
			else if (isalnum (c))
				{
					matched_flag = TRUE;
				}

			if (!matched_flag)
				{
					-- name_end_p;
					loop_flag = (start_p < name_end_p);
				}
		}

	DB (KPRINTF ("%s %ld -  name_end_p to: \"%s\" from \"%s\" %ld\n", __FILE__, __LINE__, name_end_p, start_p, (int) matched_flag));

	/* Have we found the end of the name? */
	if (matched_flag)
		{
			/* Now scroll to the start of the name */
			name_start_p = name_end_p - 1;
			matched_flag = FALSE;
			loop_flag = TRUE;

			DB (KPRINTF ("%s %ld -  start_p=%s\n", __FILE__, __LINE__, start_p));

			while (loop_flag && !matched_flag)
				{
					const char c = *name_start_p;

					#if PARAMETER_DEBUG >= 5
					DB (KPRINTF ("%s %ld - looking for start of name %c\n", __FILE__, __LINE__, c));
					#endif

					if (isspace (c) || (c == '*'))
						{
							matched_flag = TRUE;
							++ name_start_p;

							DB (KPRINTF ("%s %ld - !!!!!!!matched and incrementing start of name %s\n", __FILE__, __LINE__, name_start_p));
						}
					else
						{
							-- name_start_p;
							loop_flag = (start_p < name_start_p);

							DB (KPRINTF ("%s %ld - counting down %ld=%s\n", __FILE__, __LINE__, (int) loop_flag, name_start_p));
						}
				}

			#if PARAMETER_DEBUG >= 1
			DB (KPRINTF ("%s %ld -  name_start_p to: \"%s\" from \"%s\" %ld\n", __FILE__, __LINE__, name_start_p,  start_p, (int) matched_flag));
			#endif

			if (!matched_flag)
				{
					#if PARAMETER_DEBUG >= 2
					DB (KPRINTF ("%s %ld -  checking void match name_start_p: \"%s\" name_end_p: \"%s\" diff %ld\n", __FILE__, __LINE__, name_start_p,  name_end_p, (int) (name_end_p - name_start_p)));
					#endif

					if ((name_end_p - name_start_p == 3) && (strncmp ("void", name_start_p, 4) == 0))
						{
							type_s = CopyToNewString (name_start_p, name_end_p, FALSE);

							matched_flag = (type_s != NULL);
							name_s = NULL;
						}

					#if PARAMETER_DEBUG >= 2
					DB (KPRINTF ("%s %ld -  void match %ld\n", __FILE__, __LINE__, (int) matched_flag));
					#endif
				}
			else
				{
					// success;
					name_s = CopyToNewString (name_start_p, name_end_p, FALSE);

					#if PARAMETER_DEBUG >= 2
					DB (KPRINTF ("%s %ld -  setting param name to: \"%s\" from \"%s\"\n", __FILE__, __LINE__, name_s ? name_s : "NULL", start_p));
					#endif

					if (!name_s)
						{
							IDOS->Printf ("Failed to allocate memory for parameter name starting from \"%s\" to \"%s\"\n", name_start_p, name_end_p);
							matched_flag = FALSE;
						}


					/* Have we found the start of the name? */
					if (matched_flag)
						{
							/* scroll to the end of the type*/
							type_end_p = name_start_p - 1;
							matched_flag = FALSE;
							loop_flag = (type_end_p > start_p);

							while (loop_flag && !matched_flag)
								{
									const char c = *type_end_p;

									if (isspace (c))
										{
											-- type_end_p;
											loop_flag = (type_end_p > start_p);
										}
									else
										{
											matched_flag = TRUE;
										}
								}


							#if PARAMETER_DEBUG >= 1
							DB (KPRINTF ("%s %ld -  type_end_p to: \"%s\" from \"%s\" %ld\n", __FILE__, __LINE__, type_end_p,  start_p, (int) matched_flag));
							#endif

							/* Did we get the end of the type? */
							if (matched_flag)
								{
									/* scroll to the start of the type*/
									type_start_p = start_p;
									matched_flag = FALSE;
									loop_flag = (type_start_p < type_end_p);

									while (loop_flag && !matched_flag)
										{
											const char c = *type_start_p;

											if (isspace (c))
												{
													++ type_start_p;
													loop_flag = (type_start_p < type_end_p);
												}
											else
												{
													matched_flag = TRUE;
												}
										}

									#if PARAMETER_DEBUG >= 1
									DB (KPRINTF ("%s %ld -  type_start_p to: \"%s\" from \"%s\" %ld\n", __FILE__, __LINE__, type_start_p,  start_p, (int) matched_flag));
									#endif

								}

							if (matched_flag)
								{
									type_s = CopyToNewString (type_start_p, type_end_p, FALSE);
								}
						}
				}


			if (matched_flag)
				{
					param_p = AllocateParameter (type_s, name_s);
				}
		}

	LEAVE ();
	return param_p;
}


BOOL IsVoidParameter (const struct Parameter *param_p)
{
	ENTER ();

	LEAVE ();
	return ((strcmp (param_p -> pa_type_s, "void") == 0) && (param_p -> pa_type_s == NULL));
}


void FreeParameterList (struct List *params_p)
{
	struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (params_p);
	struct ParameterNode *next_node_p = NULL;

	ENTER ();

	while (curr_node_p)
		{
			next_node_p = (struct ParameterNode *) IExec->GetSucc (& (curr_node_p -> pn_node));

			FreeParameterNode (curr_node_p);

			curr_node_p = next_node_p;
		}

	IExec->FreeSysObject (ASOT_LIST, params_p);
	LEAVE ();
}


struct List *AllocateParameterList (void)
{
	ENTER ();

	struct List *params_p = (struct List *) IExec->AllocSysObjectTags (ASOT_LIST,
		ASOLIST_Type, PT_PARAMETER,
		TAG_DONE);

	LEAVE ();
	return params_p;
}


void FreeParameterNode (struct ParameterNode *node_p)
{
	ENTER ();

	if (node_p -> pn_param_p)
		{
			FreeParameter (node_p -> pn_param_p);
		}

	IExec->FreeSysObject (ASOT_NODE, node_p);

	LEAVE ();
}


struct ParameterNode *AllocateParameterNode (struct Parameter *param_p)
{
	ENTER ();

	struct ParameterNode *node_p = IExec->AllocSysObjectTags (ASOT_NODE,
		ASONODE_Size, sizeof (struct ParameterNode),
		TAG_DONE);


	if (node_p)
		{
			node_p -> pn_param_p = param_p;

			return node_p;
		}

	LEAVE ();
	return NULL;
}



struct Parameter *AllocateParameter (char *type_s, char *name_s)
{
	ENTER ();

	struct Parameter *param_p = (struct Parameter *) IExec->AllocVecTags (sizeof (struct Parameter), TAG_DONE);

	if (param_p)
		{
			param_p -> pa_name_s = name_s;
			param_p -> pa_type_s = type_s;

			return param_p;
		}

	LEAVE ();
	return NULL;
}


void FreeParameter (struct Parameter *param_p)
{
	ENTER ();

	ClearParameter (param_p);
	IExec->FreeVec (param_p);

	LEAVE ();
}


void ClearParameter (struct Parameter *param_p)
{
	ENTER ();

	if (param_p -> pa_name_s)
		{
			IExec->FreeVec (param_p -> pa_name_s);
		}

	if (param_p -> pa_type_s)
		{
			IExec->FreeVec (param_p -> pa_type_s);
		}

	LEAVE ();
}


BOOL SetParameterName (struct Parameter *param_p, const char *start_p, const char *end_p)
{
	ENTER ();

	LEAVE ();
	return SetParameterValue (& (param_p -> pa_name_s), start_p, end_p);
}


BOOL SetParameterType (struct Parameter *param_p, const char *start_p, const char *end_p)
{
	ENTER ();

	LEAVE ();
	return SetParameterValue (& (param_p -> pa_type_s), start_p, end_p);
}


static BOOL SetParameterValue (char **param_value_ss, const char *start_p, const char *end_p)
{
	ENTER ();

	BOOL success_flag = FALSE;
	size_t l;
	char *copy_s = NULL;

	#if PARAMETER_DEBUG >= 2
	DB (KPRINTF ("%s %ld -  setting param value to: \"%s\" - \"%s\"\n", __FILE__, __LINE__, start_p ? start_p : "NULL", end_p ? end_p : "NULL"));
	#endif

	if (end_p)
		{
			l = end_p - start_p;
		}
	else
		{
			l = strlen (start_p);
		}

	while ((start_p != end_p) && (isspace (*start_p)))
		{
			++ start_p;
			-- l;
		}

	if (end_p)
		{
			if (start_p != end_p)
				{
					while ((start_p != end_p) && (isspace (*end_p)))
						{
							-- end_p;
							-- l;
						}
				}
		}

	if (start_p != end_p)
		{
			copy_s = (char *) IExec->AllocVecTags (l + 1, TAG_DONE);

			if (copy_s)
				{
					if (*param_value_ss)
						{
							IExec->FreeVec (*param_value_ss);
						}

					strncpy (copy_s, start_p, l);
					* (copy_s + l) = '\0';

					#if PARAMETER_DEBUG >= 2
					DB (KPRINTF ("%s %ld -  setting param value to: \"%s\"\n", __FILE__, __LINE__, copy_s));
					#endif

					*param_value_ss = copy_s;

					success_flag = TRUE;
				}

		}		/* f (start_p != end_p) */

	LEAVE ();
	return success_flag;
}


BOOL WriteParameterAsSource (BPTR out_p, const struct Parameter * const param_p)
{
	BOOL success_flag = FALSE;

	ENTER ();


	if (IDOS->FPrintf (out_p, "%s", param_p -> pa_type_s) >= 0)
		{
			if (param_p -> pa_name_s)
				{
					if (IDOS->FPrintf (out_p, " %s", param_p -> pa_name_s) >= 0)
						{
							success_flag = TRUE;
						}
				}
			else
				{
					success_flag = TRUE;
				}
		}

	LEAVE ();
	return success_flag;
}


BOOL PrintParameter (BPTR out_p, const struct Parameter * const param_p)
{
	BOOL success_flag = TRUE;

	ENTER ();


	if (param_p)
		{
			if (param_p -> pa_type_s)
				{
					success_flag = (IDOS->FPrintf (out_p, "%s -", param_p -> pa_type_s) >= 0);
				}
			else
				{
					success_flag = (IDOS->FPrintf (out_p, "NULL -") >= 0);
				}

			if (success_flag)
				{
					if (param_p -> pa_name_s)
						{
							success_flag = (IDOS->FPrintf (out_p, " %s", param_p -> pa_name_s) >= 0);
						}
					else
						{
							success_flag = (IDOS->FPrintf (out_p, " NULL") >= 0);
						}
				}
		}

	LEAVE ();
	return success_flag;
}

BOOL PrintParameterList (BPTR out_p, struct List * const params_p)
{
	struct ParameterNode *curr_node_p;
	uint32 i = 0;
			
	ENTER ();

	curr_node_p = (struct ParameterNode *) IExec->GetHead (params_p);

	while (curr_node_p)
		{
			struct ParameterNode *next_node_p = (struct ParameterNode *) IExec->GetSucc (& (curr_node_p -> pn_node));
			
			IDOS->FPrintf (out_p, " %lu: ", i);
			if (PrintParameter (out_p, curr_node_p -> pn_param_p))
				{
					IDOS->FPrintf (out_p, "\n");
					++ i;
				}
			else
				{
					IDOS->Printf ("Failed to print param");
					return FALSE;
				}
			
			curr_node_p = next_node_p;
		}		/* while (curr_node_p) */



	LEAVE ();
	return TRUE;
}

