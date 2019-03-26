
function discrete_t(objs)
{
  this.objs = objs;
  console.log("*** discrete_t: objs:" + this.objs);
  //this.objs = JSON.parse(objs);
  for (var i=0; i < this.objs.length;  i++)
  {
    console.log("name: " + this.objs[i].name + ", property: " + this.objs[i].property + ", on_val: " + this.objs[i].on_val + ", off_val: " + this.objs[i].off_val); 
  }
  for (var i=0; i < this.objs.length;  i++)
  {
    console.log("name " + this.objs[i].name);
    console.log("property " + this.objs[i].property);
    console.log("on_val " + this.objs[i].on_val);
    console.log("off_val " + this.objs[i].off_val);
    this.objs[i].obj=document.getElementById(this.objs[i].name);
    console.log("obj: " + this.objs[i].obj);
  }
}

function discrete_init_f(val)
{
  this.lo_desc = val.lo_desc;
  this.hi_desc = val.hi_desc;
}

discrete_t.prototype.init=discrete_init_f;

function discrete_update_f(pv)
{
  //console.log("pv = " + pv + ", objs = " + this.objs);
  for (var i=0; i < this.objs.length;  i++)
  {
    //console.log("obj: " + this.objs[i].obj + ", property: " + this.objs[i].property + ", on_val: " + this.objs[i].on_val + ", off_val: " + this.objs[i].off_val); 
    if (pv)
    {
      //console.log("TRUE!!  obj: " + this.objs[i].obj + ", property: " + this.objs[i].property + ", on_val: " + this.objs[i].on_val + ", off_val: " + this.objs[i].off_val); 
      this.objs[i].obj.setAttribute(this.objs[i].property, this.objs[i].on_val);
      //this.pv_obj.textContent = this.lo_desc; 
    }
    else 
    {
      //console.log("FALSE!!  obj: " + this.objs[i].obj + ", property: " + this.objs[i].property + ", on_val: " + this.objs[i].on_val + ", off_val: " + this.objs[i].off_val); 
      this.objs[i].obj.setAttribute(this.objs[i].property, this.objs[i].off_val);
      //this.pv_obj.textContent = this.hi_desc; 
    }
  }
}
discrete_t.prototype.update=discrete_update_f;

