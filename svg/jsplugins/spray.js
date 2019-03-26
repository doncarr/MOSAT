
function spray_t(out1_name)
{
  console.log("out1_name " + out1_name);
  this.out1_obj=document.getElementById(out1_name);
  console.log("out1_obj: " + this.out1_obj);
}

function spray_init_f(val)
{
}

spray_t.prototype.init=spray_init_f;

function spray_update_f(pv)
{
  //console.log("pv = " + pv + ", obj = " + this.group_obj);
  //this.out1_obj.setAttribute("y2", pv / 1.3);
  this.out1_obj.setAttribute("stroke-width", pv * 0.01);
  /**
  if (pv)
  {
    //this.out1_obj.setAttribute("visibility", "visible");
    this.out1_obj.setAttribute("stroke-width", 2.0);
  }
  else 
  {
    //this.out1_obj.setAttribute("visibility", "hidden");
    this.out1_obj.setAttribute("stroke-width", 0.3);
  }
  **/
}
spray_t.prototype.update=spray_update_f;

