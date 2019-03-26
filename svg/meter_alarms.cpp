
#include <stdio.h>
#include <math.h>

void gen_alarm_arc(FILE *fp, double deg1, double deg2, double cx, double cy, 
                double radius, double width, const char *color)
{
  double x1, y1, x2, y2;
  double radians;

  radians = (deg1 - 90.0) * ((2.0 * M_PI) / 360.0);
  x1 = cx - (radius * cos(radians));
  y1 = cy - (radius * sin(radians));

  radians = (deg2 - 90.0) * ((2.0 * M_PI) / 360.0);
  x2 = cx - (radius * cos(radians));
  y2 = cy - (radius * sin(radians));

  fprintf(fp, "<path fill=\"none\" stroke=\"%s\" stroke-width=\"%lf\"\n",
           color, width); 
  //fprintf(fp, "   d=\"M75,7.5 A67.5,67.5 0 0,1 142.5,75\"/>", 
  fprintf(fp, "   d=\"M%lf,%lf A%lf,%lf 0 0,1 %lf,%lf\"/>\n\n", 
         x1, y1, radius, radius, x2, y2);
            
}


int main(int argc, char *argv[])
{
  gen_alarm_arc(stdout, 45.0, 100.0, 75, 75, 67.5, 5.0, "lime");
  gen_alarm_arc(stdout, 100, 220.0, 75, 75, 67.5, 5.0, "lime");

  gen_alarm_arc(stdout, 220.0, 260.0, 75, 75, 67.5, 5.0, "yellow");
  gen_alarm_arc(stdout, 260.0, 315.0, 75, 75, 67.5, 5.0, "red");
}

