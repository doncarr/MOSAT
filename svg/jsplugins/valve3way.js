
function valve3way_t(out1_name, out2_name, on_color, off_color)
{
  console.log("out1_name " + out1_name);
  console.log("out2_name " + out2_name);
  this.on_color = on_color;
  this.off_color = off_color;
  console.log("on_color " + this.on_color);
  console.log("off_color " + this.off_color);
  this.out1_obj=document.getElementById(out1_name);
  this.out2_obj=document.getElementById(out2_name);
  console.log("out1_obj: " + this.out1_obj);
  console.log("out2_obj: " + this.out2_obj);
}

function valve3way_init_f(val)
{
  this.lo_desc = val.lo_desc;
  this.hi_desc = val.hi_desc;
}

valve3way_t.prototype.init=valve3way_init_f;

function valve3way_update_f(pv)
{
  //console.log("pv = " + pv + ", obj = " + this.group_obj);
  if (pv)
  {
    this.out1_obj.setAttribute("fill", this.on_color);
    this.out2_obj.setAttribute("fill", this.off_color);
    //this.pv_obj.textContent = this.lo_desc; 
  }
  else 
  {
    this.out1_obj.setAttribute("fill", this.off_color);
    this.out2_obj.setAttribute("fill", this.on_color);
    //this.pv_obj.textContent = this.hi_desc; 
  }
}
valve3way_t.prototype.update=valve3way_update_f;

