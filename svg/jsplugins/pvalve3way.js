
function pvalve3way_t(out1_name, out2_name, on_color, off_color, cx, cy, d, x1, x2, y1, y2)
{
  console.log("out1_name " + out1_name);
  console.log("out2_name " + out2_name);
  this.on_color = on_color;
  this.off_color = off_color;
  console.log("on_color " + this.on_color);
  console.log("off_color " + this.off_color);
  this.cx = cx;
  this.cy = cy;
  this.d = d;
  this.x1 = x1;
  this.x2 = x2;
  this.y1 = y1;
  this.y2 = y2;
  this.out1_obj=document.getElementById(out1_name);
  this.out2_obj=document.getElementById(out2_name);
  console.log("out1_obj: " + this.out1_obj);
  console.log("out2_obj: " + this.out2_obj);
}

function pvalve3way_init_f(val)
{
}
pvalve3way_t.prototype.init=pvalve3way_init_f;

function pvalve3way_update_f(pv)
{
  // Must be between 0 and 1
  if (pv > 1) pv = 1;
  if (pv < 0) pv = 0;
  //console.log("pv = " + pv + ", obj = " + this.group_obj);
  var x = this.cx - this.d;
  var y = this.y1 + ((this.y2 - this.y1) * (pv)); 
  var y1 = this.y1 + ((this.y2 - this.y1) * ((1-pv)/2.0)); 
  var y2 = this.y2 - ((this.y2 - this.y1) * ((1-pv)/2.0)); 
  var the_points = "" + this.cx + "," + this.cy + " " + x + ", " + y1 + " " + x + "," + y2;   
//  var the_points = "" + this.cx + "," + this.cy + " " + x + ", " + y + " " + x + "," + this.y1;   

  this.out1_obj.setAttribute("points", the_points);

  y = this.cy + this.d;
  x = this.x1 + ((this.x2 - this.x1) * (1-pv)) 
  var x1 = this.x1 + ((this.x2 - this.x1) * ((pv)/2.0)) 
  var x2 = this.x2 - ((this.x2 - this.x1) * ((pv)/2.0)) 
//  the_points = "" + this.cx + "," + this.cy + " " + this.x1 + ", " + y + " " + x + "," + y;   
  the_points = "" + this.cx + "," + this.cy + " " + x1 + ", " + y + " " + x2 + "," + y;   

  this.out2_obj.setAttribute("points", the_points);

}
pvalve3way_t.prototype.update=pvalve3way_update_f;

