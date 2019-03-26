

function rotate_t(obj_name, cx, cy, a1, a2)
{
  console.log("obj_name " + obj_name);
  this.max=200.0;
  this.rotate_obj=document.getElementById(obj_name);
  this.cx=cx;
  this.cy=cy;
  this.a1=a1;
  this.a2=a2;
  console.log("rotate_obj: " + this.rotate_obj);
}

function rotate_init_f(val)
{
  this.max=val.scale_hi;
  this.min=val.scale_lo;
}
rotate_t.prototype.init=rotate_init_f;

function rotate_update_f(pv)
{
  if (pv < this.min) {pv = min;}
  if (pv > this.max) {pv = max;}
  var fraction = (pv - this.min) / (this.max - this.min) 
  var angle = (fraction * (this.a2 - this.a1)) + this.a1; 
  var mystr= "rotate("+ angle + " " + this.cx + " " + this.cy +")";
  this.rotate_obj.setAttribute("transform", mystr);
}
rotate_t.prototype.update=rotate_update_f;

