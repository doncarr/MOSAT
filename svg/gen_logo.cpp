
#include <stdio.h>

/**********************************************************/

void gen_imasa_logo(FILE *fp, double x, double y, double height, double width)
{
  double halfy = height / 2.0;
  double halfx = width / 2.0;
  const double ftopa = 0.53; 
  const double ftopb = 0.59; 
  const double fbota = 0.38; 
  const double fbotb = 0.46; 
  const double ftop2a = 0.47; 
  const double ftop2b = 0.54; 
  const double fbot2a = 0.35;
  const double fbot2b = 0.44;
  const double fmxa = 0.51;
  const double fmya = 0.67;

  const double fmxb = 0.52;
  const double fmyb = 0.79;

  const double fmxl = 0.30;
  const double fmyl = 0.36;

  const double fmxr = 0.72;
  const double fmyr = 0.36;
  
  fprintf(fp, "\n");
  fprintf(fp, "<!-- START of IMASA logo -->\n");
      // First, we have to define the gradients to use
  fprintf(fp, "  <defs>\n");
  fprintf(fp, "    <linearGradient id=\"imasaLinearBlue\" x1=\"0%%\" y1=\"0%%\" x2=\"0%%\" y2=\"100%%\" spreadMethod=\"pad\" gradientUnits=\"objectBoundingBox\">\n");
  fprintf(fp, "      <stop offset=\"0%%\" stop-color=\"mediumblue\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "      <stop offset=\"100%%\" stop-color=\"lightblue\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "    </linearGradient>\n");
  fprintf(fp, "    <linearGradient id=\"imasaLinearGrey\" x1=\"0%%\" y1=\"0%%\" x2=\"0%%\" y2=\"100%%\" spreadMethod=\"pad\" gradientUnits=\"objectBoundingBox\">\n");
  fprintf(fp, "      <stop offset=\"0%%\" stop-color=\"lightgrey\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "      <stop offset=\"100%%\" stop-color=\"grey\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "    </linearGradient>\n");
  fprintf(fp, "    <linearGradient id=\"imasaLinearWhite\" x1=\"0%%\" y1=\"0%%\" x2=\"0%%\" y2=\"100%%\" spreadMethod=\"pad\" gradientUnits=\"objectBoundingBox\">\n");
  fprintf(fp, "      <stop offset=\"0%%\" stop-color=\"lightgrey\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "      <stop offset=\"50%%\" stop-color=\"white\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "      <stop offset=\"100%%\" stop-color=\"lightgrey\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "    </linearGradient>\n");
  fprintf(fp, "  </defs>\n");

      // Now, draw the upper polygon in gradient of blue 
  fprintf(fp, "<polygon points=\"%lf,%lf %lf,%lf %lf,%lf %lf,%lf\"\n",
       x + 0, y + halfy, x + halfx, y + 0, 
       x + width, y + halfy, x + halfx, y + height);
  fprintf(fp, "  style=\"fill:url(#imasaLinearBlue);stroke:none;stroke-width:0\"/>\n");

      // Now, draw the lower polygon in gradient of gray 
  fprintf(fp, "<polygon points=\"%lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf\"\n",
           x + (halfx * fbotb), 
           y + halfy + (halfy * fbotb),

           x + (width * fmxl), 
           y + (height * fmyl), 

           x + (width * fmxb), 
           y + (height * fmyb),

           x + (width * fmxr), 
           y + (height * fmyr),

           x + width - (halfx * fbot2b), 
           y + halfy + (halfy * fbot2b),

           x + halfx, 
           y + height);
  fprintf(fp, "  style=\"fill:url(#imasaLinearGrey);stroke:none;stroke-width:0\"/>\n");

      // Now, draw the IMASA 'M' in gradient of white 
  fprintf(fp, "<polygon points=\"%lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf %lf,%lf\"\n",
           x + (halfx * fbotb), 
           y + halfy + (halfy * fbotb),

           x + (width * fmxl), 
           y + (height * fmyl), 

           x + (width * fmxb), 
           y + (height * fmyb),

           x + (width * fmxr), 
           y + (height * fmyr),

           x + width - (halfx * fbot2b), 
           y + halfy + (halfy * fbot2b),

           x + width - (halfx * fbot2a), 
           y + halfy + (halfy * fbot2a),

           x + width - (halfx * ftop2a), 
           y + (halfy * (1.0-ftop2a)), 

           x + width - (halfx * ftop2b), 
           y + (halfy * (1.0-ftop2b)), 

           x + (width * fmxa), 
           y + (height * fmya),

           x + (halfx * ftopb), 
           y + (halfy * (1.0-ftopb)), 

           x + (halfx * ftopa), 
           y + (halfy * (1.0-ftopa)), 

           x + (halfx * fbota), 
           y + halfy + (halfy * fbota) );
  fprintf(fp, "  style=\"fill:url(#imasaLinearWhite);stroke:none;stroke-width:0\"/>\n");
  fprintf(fp, "<!-- END of IMASA logo -->\n");
  fprintf(fp, "\n");
}
 
/**********************************************************/

