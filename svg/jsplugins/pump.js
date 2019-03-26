
function pump_t(group_name, on_color, off_color)
{
  console.log("group_name " + group_name);
  this.on_color = on_color;
  this.off_color = off_color;
  console.log("on_color " + this.on_color);
  console.log("off_color " + this.off_color);
  this.group_obj=document.getElementById(group_name);
  console.log("group_obj: " + this.group_obj);
}

function pump_init_f(val)
{
  this.lo_desc = val.lo_desc;
  this.hi_desc = val.hi_desc;
}

pump_t.prototype.init=pump_init_f;

function pump_update_f(pv)
{
  //console.log("pv = " + pv + ", obj = " + this.group_obj);
  if (pv)
  {
    this.group_obj.setAttribute("fill", this.on_color);
    //this.pv_obj.textContent = this.lo_desc; 
  }
  else 
  {
    this.group_obj.setAttribute("fill", this.off_color);
    //this.pv_obj.textContent = this.hi_desc; 
  }
}
pump_t.prototype.update=pump_update_f;

