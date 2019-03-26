
function trasladar_t(rect_name, x1, x2)
{
  console.log("rect_name " + rect_name);
  console.log("x1 " + x1);
  console.log("x2 " + x2);
  this.max=200.0;
  this.x1=x1;
  this.x2=x2;
  this.rect_obj=document.getElementById(rect_name);
  console.log("rect_obj: " + this.rect_obj);
}

function trasladar_init_f(val)
{
  this.max=val.scale_hi;
  console.log("max: " + this.max);
}
trasladar_t.prototype.init=trasladar_init_f;

function trasladar_update_f(pv)
{
  if (pv < 0.0) {pv = 0.0;}
  //console.log("max: " + this.max);
  //console.log("length: " + this.length);
  this.rect_obj.setAttribute("x", this.x1 + ((pv / this.max) * (this.x2 - this.x1)));
}
trasladar_t.prototype.update=trasladar_update_f;

