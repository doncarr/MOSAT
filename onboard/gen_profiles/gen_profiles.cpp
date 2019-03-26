
/************************************************************
 * gen_profiles.cpp
 *
 * This program is designed to automatically generate the 
 * piecewise linear profiles for light rail passenger service,
 * based on a list of velocity restrictions for curves, at grade
 * crossings, passenger stations, contstruction, etc. 
 *
 * This assumes that the trains have embedded control for a fixed
 * aceleration / deceleration of for instance 0.5 m/s² or 1.0 m/s².
 * This program must determine when restrictions overlap and 
 * generate the transitions between them. There are many little 
 * details that must be handled, such as that you can not switch
 * immediately from acceleration to deceleration. Also, if distances
 * are short enough between restrictions, it may not be worth the
 * electricity to accelerate to maximum speed between restrictions.
 *
 * This is designed for the rules and restrictions at the light rail
 * system in Guadalajar Mexico, though we plan to adapt it to other 
 * light rail systems.
 *
 * This will also be used to generate the informattion needed for 
 * driver cues such as telling them when to decelerate.
 *
 * **********************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "rtcommon.h"
#include "restriction_reader.h"

#define RT_MAX_PROFILE_POINTS (500)
#define GEN_DIST (20.0)

class gen_data_t
{
private:
  int last_end;
  char station[50];
  double start;
  double end;
  double distance_to_next;
  int n_items;
  double x[RT_MAX_PROFILE_POINTS];
  double desired[RT_MAX_PROFILE_POINTS];
  double low[RT_MAX_PROFILE_POINTS];
  double high[RT_MAX_PROFILE_POINTS];
  double very_high[RT_MAX_PROFILE_POINTS];
  int plot_n;
public:
  gen_data_t(void);
  void set_station(const char *name) {safe_strcpy(station, name, sizeof(station));};
  void set_distance_to_next(double d) {distance_to_next = d;};
  void set_start(double d) {start = d;};
  void set_end(double d) {end = d;set_distance_to_next(end - start);};
  void gen_start(void);
  void gen_end(void);
  void gen_decel(double startv, double endv, double end_distance, double accel);
  void gen_decel_to_station(double startv, double end_distance, double accel);
  void gen_decel_to_station2(double startv, double end_distance, double accel);
  void gen_accel(double startv, double endv, double start_distance, double accel);
  void print(void);
  void plot(bool window);
  void gen_curve(double start_d, double end_d, double max_v);
  void gen_restriction( const restriction_def_t *res);
  int find_intersection(double *val);
  void check_intersect(void);
  void reset(void);
};

/***************************************************************/

void gen_data_t::print(void)
{
  printf("%s|%0.1lf|\n", station, distance_to_next);
  for (int i=0; i < n_items; i++)
  {
    printf("%0.1lf|", x[i]);
  }
  printf("\n");
  for (int i=0; i < n_items; i++)
  {
    printf("%0.1lf|", desired[i] * 3.6);
  }
  printf("\n");
}

/***************************************************************/

double total_time = 0.0;

void gen_data_t::plot(bool window)
{
  double time = 0.0;
  FILE *fp = fopen("plot.txt", "w");
  for (int i=0; i < n_items; i++)
  {
    fprintf(fp, "%0.1lf\t%0.1lf\n", x[i], desired[i] * 3.6);
    if (i>0)
    {
      double delta_x = x[i] - x[i-1];
      double avg_spd = (desired[i] + desired[i-1])/2.0;
      time += delta_x / avg_spd; 
    }
  }
  fclose(fp);
  total_time += time;
  printf("----------- Time this segment = %lf\n", time);

  const char *base_name = "plot";
  char gtitle[100];
  double avg_spd =  ((x[n_items-1] - x[0]) / time) * 3.6;
  snprintf(gtitle, sizeof(gtitle), "%s, d = %0.0lf, t = %0.0lf, v = %0.1lf", station, x[n_items-1] - x[0], time, avg_spd);
  
  const char *fname = "gnuplotoptions.txt";
  fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }

  fprintf(fp, "set   autoscale\n");
  fprintf(fp, "unset log\n");
  fprintf(fp, "unset label\n");
  fprintf(fp, "set xtic auto\n");
  fprintf(fp, "set ytic auto\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set xlabel \"meters \"\n");
  fprintf(fp, "set ylabel \"km/h\"\n");
  fprintf(fp, "set title \"%s\"\n", gtitle);

  if (!window)
  {
    fprintf(fp, "set terminal png size 1000,720\n"); 
  }
  fprintf(fp, "%s\n", "set output\n");
  fprintf(fp, "plot \"%s.txt\" using 1:2 with lines lw 2 title \"deseable\"", base_name);
  //fprintf(fp, ", \"%s.txt\" using 1:3 with lines lw 2 title \"deseable\"", base_name);
  //fprintf(fp, ", \"%s.txt\" using 1:4 with lines lw 2 title \"low\"", base_name);
  //fprintf(fp, ", \"%s.txt\" using 1:5 with lines lw 2 title \"high\"", base_name);
  //fprintf(fp, ", \"%s.txt\" using 1:6 with lines lw 2 title \"vhigh\"", base_name);
  //fprintf(fp, "\n");

  fclose(fp);
  char command[500];
  if (window)
  {
    snprintf(command, sizeof(command), "gnuplot -persist %s", fname);
  }
  else
  {
    snprintf(command, sizeof(command), "gnuplot %s > plot/p%02d.png", fname, plot_n);
    plot_n++;
  }
  system(command);
}

/***************************************************************/

void gen_data_t::reset(void)
{
  n_items = 0;
  last_end = 0;
  station[0] = '\0';
  distance_to_next = 0.0;
}

/***************************************************************/

gen_data_t::gen_data_t(void)
{
  plot_n = 0;
  reset();
}

/***************************************************************/
static const double start_coast_vel = (20.0 / 3.6);  // m/s
static const double meters_coasting = 20; // meters 
static const double coast_decel = 0.1; // m/s^2

// The following assumes 1 coasting phase while approaching the station
// You decel to 35, then coast, and hit the brakes when the time is right.
// We think the driver can not do this - it is too hard to tell when to 
//    brake to reach zero at the correct place when traveling at 35 kmh.
//
// Would it work to brake to say 20, then coast????

void gen_data_t::gen_decel_to_station(double startv, double end_distance, double accel)
{
  double final_coast_vel = sqrt(
                  pow(start_coast_vel, 2.0) - 
                          (2.0 * coast_decel * meters_coasting) 
                           );  
  double stop_dist_from_coast = pow(final_coast_vel, 2.0) / (2 * accel);
  gen_decel(70.0 / 3.6, start_coast_vel, 
           end_distance - meters_coasting - stop_dist_from_coast, accel);
  gen_decel(start_coast_vel, final_coast_vel, 
            end_distance - stop_dist_from_coast, coast_decel);
  gen_decel(final_coast_vel, 0, end_distance, accel);
}

/***************************************************************/

static const double start_coast_vel_1 = (35.0 / 3.6);  // m/s
static const double meters_coasting_1 = 30; // meters 

static const double start_coast_vel_2 = (15.0 / 3.6);  // m/s
static const double meters_coasting_2 = 20; // meters 

// The following assumes 2 coasting phases while approaching the station.
// First you decel to 35, then coast, decel to 15, coast, decel to zero.
// The coast speeds can be adjusted.

void gen_data_t::gen_decel_to_station2(double startv, double end_distance, double accel)
{
  double final_coast_vel_1 = sqrt(
                  pow(start_coast_vel_1, 2.0) -
                          (2.0 * coast_decel * meters_coasting_1)
                           );

  double final_coast_vel_2 = sqrt(
                  pow(start_coast_vel_2, 2.0) -
                          (2.0 * coast_decel * meters_coasting_2)
                          );

  double decel_dist_from_coast_1 = 
     (pow(final_coast_vel_1, 2.0) - pow(start_coast_vel_2, 2.0)) 
                      / (2 * accel);

  double stop_dist_from_coast_2 = pow(final_coast_vel_2, 2.0) / (2 * accel);
 
  printf("final_coast_vel_1: %0.1lf\n", final_coast_vel_1 * 3.6);
  printf("final_coast_vel_2: %0.1lf\n", final_coast_vel_2 * 3.6);
  printf("meters_coasting_1: %0.1lf\ndecel_dist_from_coast_1: %0.1lf\nmeters_coasting_2: %0.1lf\nstop_dist_from_coast_2: %lf\n\n",
                 meters_coasting_1,  
                decel_dist_from_coast_1,
                meters_coasting_2, 
                stop_dist_from_coast_2);

  double gen_end_1 =  end_distance
              - meters_coasting_1
               - decel_dist_from_coast_1
               - meters_coasting_2
               - stop_dist_from_coast_2; 
  double gen_end_2 =  end_distance
               - decel_dist_from_coast_1
               - meters_coasting_2
               - stop_dist_from_coast_2; 
  double gen_end_3 =  end_distance
               - meters_coasting_2
               - stop_dist_from_coast_2; 
  double gen_end_4 =  end_distance
               - stop_dist_from_coast_2; 

  printf("gen_end 1: %0.1lf\n", gen_end_1);
  printf("decel %0.1lf to %0.1lf: %0.1lf\n\n", 70.0, start_coast_vel_1 * 3.6,
     (70.0 / 3.6 - start_coast_vel_1) / (accel));

  printf("gen_end 2: %0.1lf\n", gen_end_2);
  printf("decel %0.1lf to %0.1lf: %0.1lf\n\n", start_coast_vel_1 * 3.6, final_coast_vel_1 * 3.6,
     (start_coast_vel_1 - final_coast_vel_1) / (coast_decel));

  printf("gen_end 3: %0.1lf\n", gen_end_3);
  printf("decel %0.1lf to %0.1lf: %0.1lf\n\n", final_coast_vel_1 * 3.6, start_coast_vel_2 * 3.6,
     (final_coast_vel_1 - start_coast_vel_2) / (accel));

  printf("gen_end 4: %0.1lf\n", gen_end_4);
  printf("decel %0.1lf to %0.1lf: %0.1lf\n\n", start_coast_vel_2 * 3.6, final_coast_vel_2 * 3.6,
     (start_coast_vel_2 - final_coast_vel_2) / (coast_decel));

  printf("gen_end 5: %0.1lf\n", end_distance);
  printf("decel %0.1lf to %0.1lf: %0.1lf\n\n", final_coast_vel_2 * 3.6, 0.0,
     (final_coast_vel_2) / (accel));

  gen_decel(70.0 / 3.6, start_coast_vel_1, gen_end_1, accel);

  gen_decel(start_coast_vel_1, final_coast_vel_1, gen_end_2, coast_decel);

  gen_decel(final_coast_vel_1, start_coast_vel_2, gen_end_3, accel);

  gen_decel(start_coast_vel_2, final_coast_vel_2, gen_end_4, coast_decel);

  gen_decel(final_coast_vel_2, 0, end_distance, accel);
}


/******
void gen_data_t::gen_decel_to_station(double startv, double end_distance, double accel)
{
  double meters_at_35 = 30.0; // Meters before final stop that the train must be at 35.
  double coast_decel = 0.1;
  double v35 = 35.0 / 3.6;
  double v35sqr = v35 * v35;

  double dist_to_35 = fabs(((startv * startv) - (35.0 * 35.0)) / (2.0 * accel));

  double vel2_numer = fabs(meters_at_35 - fabs(v35sqr / (2 * coast_decel)));
  double vel2_denom = fabs(fabs(1.0 / (2.0 * accel)) - fabs(1.0 / (2.0 * coast_decel)));  
  printf("numer: %lf, denom: %lf\n", vel2_numer, vel2_denom);

  double vel2 = sqrt(vel2_numer / vel2_denom); 

  double dist_2 = fabs((v35sqr - (vel2 * vel2)) / (2.0 * coast_decel));
  double dist_3 = fabs((vel2 * vel2) / (2.0 * accel));

  printf("vel2: %lf, dist_2: %lf, dist_3: %lf\n", vel2, dist_2, dist_3);
  gen_decel(70.0 / 3.6, v35, end_distance - dist_2 - dist_3, accel);
  gen_decel(v35, vel2, end_distance - dist_3, coast_decel);
  gen_decel(vel2, 0, end_distance, accel);
}

***************************************************************/

void gen_data_t::gen_decel(double startv, double endv, double end_distance, double accel)
{
  double dist = fabs(((startv * startv) - (endv * endv)) / (2.0 * -accel));
  int idx = this->n_items;
  int n_points = 1 + int(dist / GEN_DIST);
  // Here we generate one point every GEN_DIST meters. 
  for (int i=0; i < n_points; i++)
  {
    double location = double(i) * GEN_DIST;
    double vel = sqrt(((2.0 * -accel) * location) + (startv * startv));
    this->x[idx + i] = end_distance - dist + location;    
    this->desired[idx + i] = vel;
    if (i == 0) printf("First Decel Point Generated: (%lf, %lf)\n", 
                 x[n_items], desired[n_items]);
    this->n_items++;
  } 
  // Now generate one last point at the final point where you reach the velocity.
  this->x[idx + n_points] = end_distance;    
  this->desired[idx + n_points] = endv;
  this->n_items++;
}

/***************************************************************/

void gen_data_t::gen_accel(double startv, double endv, double start_distance, double accel)
{
  double dist = fabs(((startv * startv) - (endv * endv)) / (2.0 * accel));
  int n_points = 1 + int(dist / GEN_DIST);
  int idx = this->n_items;
  // Here we generate one point every GEN_DIST meters. 
  for (int i=0; i < n_points; i++)
  {
    double location = double(i) * GEN_DIST;
    double vel = sqrt(((2.0 * accel) * location) + (startv * startv));
    this->x[idx + i] = start_distance + location;    
    this->desired[idx + i] = vel;
    if (i == 0) printf("First Decel Point Generated: (%lf, %lf)\n", 
                 x[n_items], desired[n_items]);
    this->n_items++;
  } 
  // Now generate one last point at the final point where you reach the velocity.
  this->x[idx + n_points] = start_distance + dist;    
  this->desired[idx + n_points] = endv;
  this->n_items++;
}

/***************************************************************/

void gen_data_t::gen_curve(double start_d, double end_d, double max_v)
{
  //last_end = n_items-1;
  gen_decel(70.0 / 3.6, max_v / 3.6, start_d, 1.0);
  //check_intersect(); 
  //last_end = n_items-1;
  gen_accel(max_v / 3.6, 70.0 / 3.6, end_d, 1.0);
  //check_intersect(); 
}

/***************************************************************/

void gen_data_t::gen_start(void)
{
  gen_accel(0.0, 70.0 / 3.6, start, 1.0);
}

/***************************************************************/

void gen_data_t::gen_end(void)
{
  last_end = n_items-1;
  //gen_decel(70.0 / 3.6, 0.0, end, 1.0);
  //gen_decel_to_station(70.0 / 3.6, end, 1.0);
  gen_decel_to_station2(70.0 / 3.6, end, 1.0);
  check_intersect(); 
}

/***************************************************************/

void gen_data_t::gen_restriction( const restriction_def_t *res)
{
  printf("generating restriction\n");
  switch (res->type)
  {
    case RESTRICT_OTHER:
    case RESTRICT_CURVE:
    case RESTRICT_CROSSING:
    case RESTRICT_ACCEL_DECEL_CONTROL:
      last_end = n_items-1;
      gen_curve(res->start, res->end, res->max_speed);
      check_intersect(); 
      break;
    default:
      return;
  }
}

/***************************************************************/
struct line_t 
{
  double x1, y1, x2, y2;
};

bool intersect(line_t l1, line_t l2, double *x, double *y) 
{
  /***
  y = l1.y1 + m * (x - l1.x1) = l2.x1 + ((l2.y2 - l2.y2)/(l2.x2 - l2.x1))(x - l2.x1);
  y = m1x + b1
  m1x + b1 = m2x + b2; 
  m1x - m2x = b2 - b1;
  (m1 - m2)x = b2 - b1;
  
  x = (b2 - b1) / (m1 - m2);
  ***/

  double m1 = ((l1.y2 - l1.y1) / (l1.x2 - l1.x1));
  double m2 = ((l2.y2 - l2.y1) / (l2.x2 - l2.x1));
  if (m1 == m2)
  {
    *x = 0.0;
    *y = 0.0;
    return false;
  }
  double b1 = l1.y1 - (m1 * l1.x1);
  double b2 = l2.y1 - (m2 * l2.x1); 
  double x_int = (b2 - b1) / (m1 - m2);

  if ((x_int >= l1.x1) && (x_int <= l1.x2) && (x_int >= l2.x1) && (x_int <= l2.x2))
  {
    *x = x_int;
    *y = l1.y1 + (m1 * (x_int - l1.x1));
    return true;
  }
  else
  {
    *x = 0.0;
    *y = 0.0;
    return false;
  }
}

/***************************************************************/

int gen_data_t::find_intersection(double *val)
{
//bool intersect(line_t l1, line_t l2, double *x, double *y) 
  printf("Finding Intersection\n");
  line_t l1;
  line_t l2;
  bool done = false;
  for (int i= (last_end + 1); (i < (n_items-1)) && !done; i++)
  {
    l1.x1 = x[i]; 
    l1.x2 = x[i+1];
    l1.y1 = desired[i];
    l1.y2 = desired[i+1];
    for (int j= last_end; (j > 0) && !done; j--)
    {
      l2.x1 = x[j-1]; 
      l2.x2 = x[j];
      l2.y1 = desired[j-1]; 
      l2.y2 = desired[j];
      printf("(%lf, %lf)(%lf, %lf) (%lf, %lf)(%lf, %lf)\n", 
         l1.x1, l1.y1 * 3.6, l1.x2, l1.y2 * 3.6, 
         l2.x1, l2.y1 * 3.6, l2.x2, l2.y2 * 3.6);
//bool intersect(line_t l1, line_t l2, double *x, double *y) 
     double xi, yi;
     if (intersect(l1, l2, &xi, &yi)) 
     {
       printf("**** Wow they intersect at (%lf, %lf)\n", xi, yi * 3.6); 
       printf("**** n = %d, i = %d, i+1 = %d, j = %d, n, j-1 = %d\n", n_items, i, i + 1, j, j-1);
        /**/
       x[j] = xi;
       desired[j] = yi;
       int k, m;
       for (k=j+1, m = i+1; m < (n_items); k++, m++)
       {
         printf("Copy x[%d] to x[%d]\n", m, k);
         x[k] = x[m];
         desired[k] = desired[m];
       }
       n_items -= (i-j);
       printf("reducing n_items by %d\n", i-j);
       done = true;
       /***/
     }
    }
  }
  
  *val = 0.0;
  return 0;
}

/***************************************************************/

void gen_data_t::check_intersect(void)
{
  printf("Checking Intersection\n");

  if (last_end <= 0)
  {
    printf("last end = 0, returning\n");
    return;
  }

  printf("last end: %d +0: %lf +1:%lf", last_end, x[last_end], x[last_end+1]);
  if (x[last_end] <= x[last_end + 1])
  {
    printf("no intersection, returning\n");
    return;
  }

  printf("They intersect - fixing \n");

  double tmp;
  find_intersection(&tmp);
  // Ok, we have overlap, must fix it.
}

/***************************************************************/

int main(int argc, char *argv[])
{
  restriction_reader_t rr;
  gen_data_t gd;

  const char *restrict_fname = "restrictions.txt";
  if (argc > 1)
  {
    restrict_fname = argv[1];
  }
  const char *fname = "profile_out.txt";
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    exit(0);
  }

  rr.read_restrictions(restrict_fname);

/**/
  if (argc > 2)
  {
    gd.gen_decel_to_station2(35.0/3.6, 300, 1.0);
    gd.print();
    gd.plot(false);
    gd.plot(true);
    return 0;
  }
/**/

  int n_stations = 0; 
  for (int i=0; i < rr.get_n_restrictions(); i++)
  {
    const restriction_def_t *res = rr.get_restriction(i);
    if (res->type == RESTRICT_STATION)
    {
      if (i !=0)
      {
        gd.set_end(res->normal_stop_point);
        printf("------ End[%d] = %lf, %s, %d\n", n_stations, res->normal_stop_point, res->description, i);
        gd.gen_end();
        gd.print();
        gd.plot(true);
        gd.plot(false);
        n_stations++;
        //if (n_stations > 0) exit(0);  /* comment this out */
      }
      gd.reset();
      gd.set_station(res->description);
      gd.set_start(res->normal_stop_point);
      printf("----- Start[%d] = %lf, %s, %d\n", n_stations, res->normal_stop_point, res->description, i);
      gd.gen_start();
    }
    else
    {
      printf("Restriction[%d]: %s\n", i, rr.restriction_string(res->type));
      gd.gen_restriction(res);
    }
  }
  int tt = int(total_time);
  printf("Total time = %d, %d:%d\n", tt, tt/60, tt%60);
  /***
  gd.reset();
  gd.set_station("PerSur");
  gd.set_distance_to_next(1099.0);
  gd.gen_accel(0.0, 70.0 / 3.6, 0.0, 0.5);
  gd.gen_decel(70.0 / 3.6, 50.0 / 3.6, 320.0, 1.0);
  gd.gen_accel(50.0 / 3.6, 70.0 / 3.6, 650, 1.0);
  gd.gen_decel(70.0 / 3.6, 0.0 / 3.6, 1099.0, 1.0);
  gd.print();
  gd.plot();
  ***/
}

/***************************************************************/

