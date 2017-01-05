/*
** Copyright 2014-2016 The Earlham Institute
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * reg_ex.c
 *
 *  Created on: 5 Jan 2017
 *      Author: billy
 *
 * @file
 * @brief
 */
#include <regex.h>

static regex_t s_regex;


void *CompileRegEx (const char *reg_ex_s)
{
	regex_t *reg_exp_p = (regex_t *) malloc (sizeof (regex_t));

	if (reg_exp_p)
		{
			if (regcomp (reg_exp_p, reg_ex_s, 0) != 0)
				{
					return reg_exp_p;
				}

			free (reg_exp_p);
		}

	return NULL;
}


bool GetNextMatch (const char *data_s, char **match_ss)
{
	bool matched_flag = false;

	return matched_flag;
}
