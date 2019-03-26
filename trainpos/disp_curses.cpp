
#include <stdio.h>
#include <string.h>

#include <ncurses.h>
#include <unistd.h>
#include <math.h>

class rt_curses_display_t 
{
private:
  int sz_platform;
  int n_stations;
  int n_segments;
  int n_cols;
  int n_line_cols;
  int n_squares;
  int text_overhang;
  int max_for_line;
  int sz_segment;
  int left_start;
  int top_line;
  int bottom_line;
  char platforms[200];
  char upper_stations_1[200];
  char upper_stations_2[200];
  char lower_stations_1[200];
  char lower_stations_2[200];
  char via_1[200];
  char via_2[200];
  char station_labels_1[50][6];
  char station_labels_2[50][6];
public:
  void put_string(char *dest, int size_dest, const char *src, int location);
  void create_stations(void);
  void do_calcs(void);
  void show(void);
  void set_station_labels(void);
  void init_screen(void);
  void redraw_all();
  void update(double pct);
  ~rt_curses_display_t(void);
};

/*****************************************************/

rt_curses_display_t::~rt_curses_display_t(void)
{
  endwin();
}

/*****************************************************/

void rt_curses_display_t::init_screen(void)
{
  initscr();
  start_color();
  use_default_colors();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);

  short fg, bg;
  pair_content(COLOR_PAIR(0), &fg, &bg);

  init_pair(1, COLOR_RED, bg);
  init_pair(2, COLOR_GREEN, bg);
  init_pair(3, COLOR_BLACK, COLOR_YELLOW);
  init_pair(4, COLOR_BLUE, bg);
  init_pair(5, COLOR_MAGENTA, bg);
  init_pair(6, COLOR_CYAN, bg);
  init_pair(7, COLOR_BLACK, COLOR_WHITE);
//             COLOR_YELLOW
//             //             COLOR_BLUE
//             //             COLOR_MAGENTA
//             //             COLOR_CYAN
//             //             COLOR_WHITE
//

  timeout(0);
};


/*****************************************************/

void rt_curses_display_t::redraw_all()
{
  erase();
  box(stdscr, 0, 0);

  top_line = 2;
  bottom_line = 2 + 14 + 1;
  mvhline(top_line, 0, 0, COLS);
  mvhline(bottom_line, 0, 0, COLS);
  attron(A_ALTCHARSET);
  mvprintw(top_line,0,"%c", ACS_LTEE);
  mvprintw(top_line,COLS-1,"%c", ACS_RTEE);
  mvprintw(bottom_line,0,"%c", ACS_LTEE);
  mvprintw(bottom_line,COLS-1,"%c", ACS_RTEE);
  attroff(A_ALTCHARSET);
  //mvhline(9, 2, 0, 4);
  //mvhline(9, COLS-7, 0, 4);
}

/*****************************************************/


/*****************************************************/

void rt_curses_display_t::update(double pct)
{
  if (COLS != n_cols)
  {
    n_cols = COLS;
    do_calcs();
    redraw_all();
    set_station_labels();
    create_stations();
    show();
  }  
  int add;
  int draw_col;
  int row;
  const char *arrow;
  double dcols = double(n_line_cols);
  double ddc;
  pct /= 100.0;
  if (pct < 0.5)
  {
    ddc = (pct * double(2.0)) * dcols;
    draw_col = int(ddc);
    add = 1;
    arrow = ">";
    row = 9;
  }
  else
  {
    draw_col = n_line_cols - int((pct - 0.5) * dcols * 2.0);
    add = -1;
    arrow = "<";
    row = 6;
  }
  if (draw_col >= (n_line_cols-1)) draw_col = n_line_cols - 1;
  memset(via_2, '-', sizeof(via_2));
  via_2[n_line_cols] = '\0';
  mvprintw(top_line + 9, left_start,"%s", via_2);
  memset(via_2, '-', sizeof(via_1));
  via_2[n_line_cols] = '\0';
  mvprintw(top_line + 6, left_start,"%s", via_1);
  color_set(4, NULL);
  attron(A_ALTCHARSET);
  mvprintw(top_line + row, left_start+draw_col,"a");
  attroff(A_ALTCHARSET);
  if (!(((add == -1) && (draw_col == 0)) || ((add == 1) && draw_col >= (n_line_cols-1))))
  {
    mvprintw(top_line + row, left_start+draw_col+add,arrow);
  }
  color_set(0, NULL);
  refresh(); 
}

/*****************************************************/

void rt_curses_display_t::show(void)
{

  color_set(5, NULL);
  mvprintw(top_line + 1, left_start - text_overhang,"%s", upper_stations_1);
  mvprintw(top_line + 2, left_start - text_overhang,"%s", upper_stations_2);
  mvprintw(top_line + 13, left_start - text_overhang,"%s",lower_stations_1);
  mvprintw(top_line + 14, left_start - text_overhang,"%s", lower_stations_2);
  color_set(0, NULL);

  attron(A_ALTCHARSET);
  mvprintw(top_line + 5, left_start,"%s",platforms);
  mvprintw(top_line + 10, left_start,"%s",platforms);
  attroff(A_ALTCHARSET);

  mvprintw(top_line + 6, left_start,"%s", via_1);
  mvprintw(top_line + 9, left_start,"%s", via_2);
}

/*****************************************************/

void rt_curses_display_t::put_string(char *dest, int size_dest, const char *src, int location)
{
  for (int i=0; (src[i] != '\0') && ((location + i) < (size_dest - 1)); i++)
  {
    dest[location + i] = src[i];
  }
}

/*****************************************************/

void rt_curses_display_t::create_stations(void)
{
  memset(platforms, ' ', sizeof(platforms));
  memset(upper_stations_1, ' ', sizeof(upper_stations_1));
  memset(upper_stations_2, ' ', sizeof(upper_stations_2));
  memset(lower_stations_1, ' ', sizeof(lower_stations_1));
  memset(lower_stations_2, ' ', sizeof(lower_stations_2));
  memset(via_1, '-', sizeof(via_1));
  memset(via_2, '-', sizeof(via_2));
  int loc = 0;
  for (int i=0; i < n_stations; i++)
  {
    loc = (i * (sz_platform + sz_segment)) ;//+ left_start;
    //printf("loc = %d, i = %d, szp = %d, scs = %d\n", loc, i, sz_platform, sz_segment);
    for (int j=0; j < sz_platform; j++) put_string(platforms, sizeof(platforms), "a", loc + j); 
    if (0 == (i % 2))
    {
      put_string(upper_stations_1, sizeof(upper_stations_1), station_labels_1[i], loc - 1 + text_overhang);
      put_string(upper_stations_2, sizeof(upper_stations_2), station_labels_2[i], loc - 1 + text_overhang);
    }
    else
    {
      put_string(lower_stations_1, sizeof(lower_stations_1), station_labels_1[i], loc - 1 + text_overhang);
      put_string(lower_stations_2, sizeof(lower_stations_2), station_labels_2[i], loc - 1 + text_overhang);
    }
  }
  platforms[n_line_cols] = '\0';
  upper_stations_1[n_line_cols + (2 * text_overhang)] = '\0';
  upper_stations_2[n_line_cols + (2 * text_overhang)] = '\0';
  lower_stations_1[n_line_cols + (2 * text_overhang)] = '\0';
  lower_stations_2[n_line_cols + (2 * text_overhang)] = '\0';
  via_1[n_line_cols] = '\0';
  via_2[n_line_cols] = '\0';
}

/*****************************************************/

void  rt_curses_display_t::set_station_labels(void)
{
  for (int i = 0; i < n_stations; i++)
  {
    snprintf(station_labels_1[i], sizeof(station_labels_1[0]), "STA%2d", i+1);
    snprintf(station_labels_2[i], sizeof(station_labels_2[0]), "Line2");
  }
}

/*****************************************************/

void  rt_curses_display_t::do_calcs(void)
{
  n_cols = COLS;
  sz_platform = 1;
  n_stations = 19;
  n_segments = n_stations - 1;
  text_overhang = 3;
  max_for_line = n_cols - (2 * text_overhang) - 2; // substract for overhanging text, borders
  
  n_squares = max_for_line - (sz_platform * n_stations);
  //printf("n_squares: %d, n_segments: %d\n", n_squares, n_segments);
  sz_segment = n_squares / n_segments; 
  left_start = text_overhang + ((n_squares % n_segments) / 2); // center for the extra columns.
  n_line_cols = (sz_segment * n_segments) + (sz_platform * n_stations); 
  //printf("sz segment = %d\n", sz_segment);
}

/*****************************************************/

int main(int argc, char *argv[])
{
  rt_curses_display_t disp;

  disp.init_screen();
  disp.do_calcs();
  disp.redraw_all();
  disp.set_station_labels();
  disp.create_stations();
  disp.show();
  double pct = 0.0;
  while(true)
  {
    disp.update(pct);
    pct += 0.5;
    if (pct > 100.0) pct = 0.0;
    if ('q' == getch())
    {
      break;
    }
    usleep(80000);
  }
}
