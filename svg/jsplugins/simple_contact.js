
function simple_contact_t(rect_name, line_name, on_color, off_color, on_y2, off_y2)
{
  this.on_color = on_color;
  this.off_color = off_color;
  this.on_y2 = on_y2;
  this.off_y2 = off_y2;
  this.name = rect_name;
  this.last_pv = false;
  console.log("on_color " + this.on_color);
  console.log("off_color " + this.off_color);
  console.log("on_y2 " + this.on_y2);
  console.log("off_y2 " + this.off_y2);
  this.rect_obj=document.getElementById(rect_name);
  this.line_obj=document.getElementById(line_name);
  console.log("rect_obj: " + this.rect_obj);
  console.log("line_obj: " + this.line_obj);
}

function simple_contact_init_f(val)
{
}

simple_contact_t.prototype.init=simple_contact_init_f;

function simple_contact_update_f(pv)
{
  //console.log("pv = " + pv + ", rect obj = " + this.rect_obj);
  if (pv)
  {
    this.rect_obj.setAttribute("fill", this.on_color);
    this.line_obj.setAttribute("y2", this.on_y2);
  }
  else 
  {
    this.rect_obj.setAttribute("fill", this.off_color);
    this.line_obj.setAttribute("y2", this.off_y2);
  }
  if (this.last_pv != pv)
  {
    console.log("new value for" + this.name + ": " + pv);
  }
  this.last_pv = pv;
}
simple_contact_t.prototype.update=simple_contact_update_f;

