
function attributes_t(x, y, h)
{
  console.log("attributes_t: x: " + x + ", y: " + y + ", h:" + h);
  this.x = x;
  this.y = y;
  this.h = h;
}

function attributes_init_f(val)
{
  if (val == null)
  {
    shape = document.createElementNS(svgNS, "text");
    shape.setAttribute("x", this.x);
    shape.setAttribute("y", this.y);
    shape.setAttribute("font-size", this.h * 2.0);
    //shape.setAttribute("baseline-shift", "-33%");
    shape.setAttribute("text-anchor", "start");
    shape.textContent="Bad Tag";
    //document.documentElement.appendChild(shape);
    reactmainobj.appendChild(shape);
    this.y += (this.h * 1.2); 
    return;
  }
  for(var prop_name in val) 
  {
    if(typeof(val[prop_name]) != "undefined") 
    {
      shape = document.createElementNS(svgNS, "text");
      shape.setAttribute("x", this.x);
      shape.setAttribute("y", this.y);
      shape.setAttribute("font-size", this.h);
      //shape.setAttribute("baseline-shift", "-33%");
      shape.setAttribute("text-anchor", "start");
      shape.textContent=prop_name + ": " + val[prop_name];
      //document.documentElement.appendChild(shape);
      reactmainobj.appendChild(shape);
      this.y += (this.h * 1.2); 
    }
  }
}
attributes_t.prototype.init=attributes_init_f;

function attributes_update_f(pv)
{
}
attributes_t.prototype.update=attributes_update_f;

