
/************************************************************************

Dcalc.c

The member functions for discrete calculation points.

*************************************************************************/


#include <stdio.h>
#include <malloc.h>

#include "rtcommon.h"

#include "reactpoint.h"
#include "exp.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "error.h"

/************************************************************************/

void dcalc_point_t::evaluate()
{
  /* Evaluate the dcalc point. */

  pv = (discrete_value_t) expression.evaluate();
  //printf("Discrete calc %s: expr: %s, val: %s\n", tag, expr_string, pv ? "TRUE" : "FALSE");
  check_alarms();
  display_pv();
}

/*********************************************************************/

dcalc_point_t **dcalc_point_t::read(int *cnt, const char *home_dir)
{
  int max_points = 50;
  dcalc_point_t **dc_points =
	(dcalc_point_t **) malloc(max_points * sizeof(dcalc_point_t*));
  MALLOC_CHECK(dc_points);

  *cnt = 0;
  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/discrete_calc.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open %s\n", path);
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];
    //Dcalc1|Discrete Calc 1|temp > 50|HI|LO|N|N|
    ltrim(line);
    rtrim(line);

    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }

    else if (argc != 7)
    {
      logfile->vprint("%s: Wrong number of args, line %d\n", path, i+1);
      continue;
    }

    dcalc_point_t *p = new dcalc_point_t;
    logfile->vprint("%s\n", line);

    /*****
    Tag
    Description
    calc expression
    Low Description
    High Description
    Alarm State
    Shutdown State
    ******/

    safe_strcpy(p->tag, (const char*) argv[0], sizeof(p->tag));
    safe_strcpy(p->description, (const char*) argv[1], sizeof(p->description));

    char temp[200];
    safe_strcpy(temp, argv[2], sizeof(temp));
    ltrim(temp);
    rtrim(temp);
    p->expr_string = strdup(temp);
    MALLOC_CHECK(p->expr_string);
    logfile->vprint("Expression: %s\n", p->expr_string);

    safe_strcpy(p->lo_desc, (const char*) argv[3], sizeof(p->lo_desc));
    safe_strcpy(p->hi_desc, (const char*) argv[4], sizeof(p->hi_desc));

    switch (argv[5][0])
    {
      case 'H':
      case 'h':
	p->alarm_state = DISCRETE_HI;
	break;
      case 'L':
      case 'l':
	p->alarm_state = DISCRETE_LO;
	break;
      case 'N':
      case 'n':
	p->alarm_state = DISCRETE_NONE;
	break;
      default:
	p->alarm_state = DISCRETE_NONE;
	break;
    }

    switch (argv[6][0])
    {
      case 'H':
      case 'h':
	p->shutdown_state = DISCRETE_HI;
	break;
      case 'L':
      case 'l':
	p->shutdown_state = DISCRETE_LO;
	break;
      case 'N':
      case 'n':
	p->shutdown_state = DISCRETE_NONE;
	break;
      default:
	p->shutdown_state = DISCRETE_NONE;
	break;
    }

    p->pv_string = p->lo_desc;
    p->pv = false;

    dc_points[count] = p;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      dc_points = (dcalc_point_t **) realloc(dc_points,
		  max_points * sizeof(dcalc_point_t*));
      MALLOC_CHECK(dc_points);
    }
  }

  if (count == 0)
  {
    free(dc_points);
    return NULL;
  }
  *cnt = count;
  return dc_points;

}

/*************************************************************************/


/***********************************************************************/

void dcalc_point_t::parse_expr(void)
{
  /* Parse the expression for the dcalc point. */

  expression.expr = make_expr(expr_string);
  if (expression.expr == NULL)
  {
    logfile->vprint("Discrete calc %s: Bad Expression: %s\n", tag, expr_string);
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = LOGICAL_VAL;
    expression.expr[0].val.logical_val = 0;
    expression.expr[1].token_type = END_EXPR;
  }
}

/***********************************************************************/
