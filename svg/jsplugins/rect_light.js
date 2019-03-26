
function rect_light_t(rect_name, pv_name, on_color, off_color)
{
  console.log("rect_name " + rect_name);
  this.on_color = on_color;
  this.off_color = off_color;
  console.log("on_color " + this.on_color);
  console.log("off_color " + this.off_color);
  this.panel_obj=document.getElementById(rect_name);
  if ( pv_name != null )
      this.pv_obj=document.getElementById(pv_name);
  else
      this.pv_obj=null;
  console.log("panel_obj: " + this.panel_obj);
}

function rect_light_init_f(val)
{
  this.lo_desc = val.lo_desc;
  this.hi_desc = val.hi_desc;
}

rect_light_t.prototype.init=rect_light_init_f;

function rect_light_update_f(pv)
{
  //console.log("pv = " + pv + ", obj = " + this.panel_obj);
  if (pv)
  {
    this.panel_obj.setAttribute("fill", this.on_color);
    if (this.pv_obj != null)
    	this.pv_obj.textContent = this.hi_desc; 
  }
  else 
  {
    this.panel_obj.setAttribute("fill", this.off_color);
    if (this.pv_obj != null)
    	this.pv_obj.textContent = this.lo_desc; 
  }
}
rect_light_t.prototype.update=rect_light_update_f;

