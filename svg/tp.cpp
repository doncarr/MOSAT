

/***
<svg xmlns="http://www.w3.org/2000/svg"
     xmlns:xlink="http://www.w3.org/1999/xlink"
     width="100%" height="100%" viewBox="0 0 320 160" onload="load()">

  <title>Text Alignment Test</title>
  <metadata>author: Don Carr</metadata>

  <line x1="10" y1="20" x2="150" y2="20"
    style="stroke:red;stroke-width:1"/>
<!-- upper left -->
  <text id="eulabel_text" x="50" y="20" font-size="10"
       baseline-shift="-66%" text-anchor="start">12g0</text>

<!-- middle left -->
  <text id="eulabel_text" x="80" y="20" font-size="10"
       baseline-shift="-33%" text-anchor="start">12g0</text>

<!-- lower left -->
  <text id="eulabel_text" x="110" y="20" font-size="10"
       baseline-shift="0%" text-anchor="start">12g0</text>

<!-- ******************************************************** -->

  <line x1="10" y1="40" x2="150" y2="40"
    style="stroke:red;stroke-width:1"/>
<!-- upper center -->
  <text id="eulabel_text" x="50" y="40" font-size="10"
       baseline-shift="-66%" text-anchor="middle">12g0</text>

<!-- middle center -->
  <text id="eulabel_text" x="80" y="40" font-size="10"
       baseline-shift="-33%" text-anchor="middle">12g0</text>

<!-- lower center -->
  <text id="eulabel_text" x="110" y="40" font-size="10"
       baseline-shift="0%" text-anchor="middle">12g0</text>

<!-- ******************************************************** -->

  <line x1="10" y1="60" x2="150" y2="60"
    style="stroke:red;stroke-width:1"/>
<!-- upper right -->
  <text id="eulabel_text" x="50" y="60" font-size="10"
       baseline-shift="-66%" text-anchor="end">12g0</text>

<!-- middle right -->
  <text id="eulabel_text" x="80" y="60" font-size="10"
       baseline-shift="-33%" text-anchor="end">12g0</text>

<!-- lower right -->
  <text id="eulabel_text" x="110" y="60" font-size="10"
       baseline-shift="0%" text-anchor="end">12g0</text>

<!-- ******************************************************** -->

  <line x1="50" y1="0" x2="50" y2="80"
    style="stroke:red;stroke-width:1"/>
  <line x1="80" y1="0" x2="80" y2="80"
    style="stroke:red;stroke-width:1"/>
  <line x1="110" y1="0" x2="110" y2="80"
    style="stroke:red;stroke-width:1"/>


<!--
// text-anchor: start | middle | end 
// baseline-shift: 0-100%
// alignment-baseline="middle"
-->
</svg>
***/
#include <stdio.h>
#include <math.h>

void gen_text(FILE *fp, double angle, double cx, double cy, 
                double radius, double font_size)
{
  double rads, x, y;
  
  rads = (angle - 90.0) * ((2.0 * M_PI) / 360.0);
  x = cx - (radius * 0.78 * cos(rads)); 
  y = cy - (radius * 0.78 * sin(rads)); 
  printf("<!-- Angle = %0.0lf sin = %0.2lf, cos = %0.2lf-->\n", angle,
        sin(rads), cos(rads));
  fprintf(fp, "<text x=\"%lf\" y=\"%lf\" font-size=\"%lf\"\n",
             x, y, font_size);
  fprintf(fp, 
     "   baseline-shift=\"-33%%\" text-anchor=\"middle\">%0.0lf</text>\n",
          angle - 45.0);
}

int main(int argc, char *argv[])
{
  gen_text(stdout, 45, 75.0, 75.0, 65.0, 10.0);
  gen_text(stdout, 90, 75.0, 75.0, 65.0, 10.0);
  gen_text(stdout, 135, 75.0, 75.0, 65.0, 10.0);
  gen_text(stdout, 180, 75.0, 75.0, 65.0, 10.0);
  gen_text(stdout, 225, 75.0, 75.0, 65.0, 10.0);
  gen_text(stdout, 270, 75.0, 75.0, 65.0, 10.0);
  gen_text(stdout, 315, 75.0, 75.0, 65.0, 10.0);
}
// From 45 to 80 degrees
// Lower left corner alignment 


// From 80 100 degrees
// Gradually move from lower left to upper left allignment.
   

// From 100 to 170 degrees
// Upper left corner alignment 

// From 170 to 210
// Gradually move from left of center to right of center alligntment

// From 210 to 260
// Upper right corner alignment

// From 260 to 280
// Gradually move from upper right to lower right allignment

// 280-315
// Lower right corner allignment 

