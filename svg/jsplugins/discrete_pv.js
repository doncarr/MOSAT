
function discrete_pv_t(text_name)
{
  console.log("text_name " + text_name);
  this.text_obj=document.getElementById(text_name);
  this.lo_desc = "LO not set";
  this.hi_desc = "HI not set";
  console.log("text_obj: " + this.text_obj);
}

function discrete_pv_init_f(val)
{
  this.lo_desc = val.lo_desc;
  this.hi_desc = val.hi_desc;
  console.log("lo_desc: " + this.lo_desc);
  console.log("hi_desc: " + this.hi_desc);
}
discrete_pv_t.prototype.init=discrete_pv_init_f;

function discrete_pv_update_f(pv)
{
  if (pv)
  {
    this.text_obj.textContent = this.hi_desc;
  }
  else
  {
    this.text_obj.textContent = this.lo_desc;
  }
  //console.log("pv: " + pv);
}
discrete_pv_t.prototype.update=discrete_pv_update_f;

