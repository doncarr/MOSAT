
#include <form.h>

int main()
	{
	FIELD *field[4];
	FORM  *myForm;
	int ch;
	
	initscr();		/* start curses */
	cbreak();
	noecho();		/* don't echo keystrokes */
	keypad(stdscr, TRUE);	/* accept func keys */

	/* CREATE FIELD ARRAY */
	field[0] = new_field
		(
		1,	/* height in rows */
		10,	/* width in characters */
		4,	/* y position in rows, 0 based */
		17,	/* x position in columns, 0 based */
		0,	/* number of offscreen rows */
		0	/* number of working buffers */
		);
	field[1] = new_field
		(
		1,	/* height in rows */
		10,	/* width in characters */
		6,	/* y position in rows, 0 based */
		17,	/* x position in columns, 0 based */
		0,	/* number of offscreen rows */
		0	/* number of working buffers */
		);
	field[2] = new_field
		(
		1,	/* height in rows */
		10,	/* width in characters */
		8,	/* y position in rows, 0 based */
		17,	/* x position in columns, 0 based */
		0,	/* number of offscreen rows */
		0	/* number of working buffers */
		);
	field[3] = NULL;

	/* SET FIELD OPTIONS */
	set_field_back(field[0], WA_REVERSE);	/* field has reverse video text */
	field_opts_off(field[0], O_AUTOSKIP);	/* don't walk off end of field */ 

	/* CREATE FORM */
	myForm = new_form(field);
	post_form(myForm);
	mvprintw(4, 10, "Field: ");
	mvprintw(6, 10, "Field: ");
	mvprintw(8, 10, "Field: ");
	refresh();

	/* KEYSTROKE ACQUISITION LOOP */
	while(true)
        {
          ch = getch();
          if (ch == KEY_F(9))	/* F9 is accept keystroke */
	  {
            break;
	  }
          if (ch == '\t')	/* TAB, go to next field */
	  {
            break;
	  }
          if (ch == '\n')	/* TAB, go to next field */
	  {
            break;
	  }
	  form_driver(myForm, ch);
        }

	/* UNPOST FORM, FREE MEMORY */
	unpost_form(myForm);
	free_form(myForm);
	free_field(field[0]);
	free_field(field[1]); 
	free_field(field[2]); 

	endwin();
	return 0;
	}
