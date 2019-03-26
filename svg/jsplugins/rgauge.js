

function rg_delete_trailing_zeros(str)
{
  // Deletes any trailing zeros if there is a decimal point.
  // There MUST be better more elegant way to do this, PLEASE HELP
  console.log("delete trailing zeros: " + str);
  var found = false;
  var the_ch;
  for (var i=0; i < str.length; i++)
  {
    the_ch = str.charAt(i);
    console.log("the char at " + i + ": " + the_ch);
    if ('.' == the_ch) found = true; 
  }
  if (!found) return str;

  console.log(str);
  while ('0' == str.charAt(str.length-1)) 
  {
    str = str.substring(0, str.length-1);
    console.log(str);
  }
  if ('.' == str.charAt(str.length-1)) str = str.substring(0, str.length-1);
  return str;
} 

function rgauge_t(pv_name, eu_name, lo_name, hi_name, x, y, width)
{
  console.log("pv_name " + pv_name);
  console.log("eu_name " + eu_name);

  this.min=0.0;
  this.max=200.0;
  this.pv_obj=document.getElementById(pv_name);
  this.eu_obj=document.getElementById(eu_name);
  this.lo_obj=document.getElementById(lo_name);
  this.hi_obj=document.getElementById(hi_name);
  this.eu_label="not set";
  this.x=x;
  this.y=y;
  this.width=width;
  this.cx=this.x + (this.width / 2.0)
  this.cy=this.y + (this.width / 2.0)
  this.r = width / 2;
  this.rcolor_ok = "green";
  this.rcolor_caution = "gold";
  this.rcolor_alarm = "rgb(204,0,0)";
  this.n_blocks = 80;
  this.alarm_n=999;
  this.caution_n=999;
  this.last_n=0;

  console.log("pv_obj: " + this.pv_obj);
  console.log("eu_obj: " + this.eu_obj);
  console.log("lo_obj: " + this.lo_obj);
  console.log("hi_obj: " + this.hi_obj);
}

function rg_gauge_init_f(val)
{
  this.min=val.scale_lo;
  this.max=val.scale_hi;
  this.eu_label=val.eu;
  this.decimal_places = val.decimal_places;
  this.eu_obj.textContent = this.eu_label;
  this.lo_obj.textContent = this.min;
  this.hi_obj.textContent = this.max;
  if (val.hi_caution_enable)
  {
    this.caution_n = Math.round((this.n_blocks-1) * (val.hi_caution / this.max));
  }
  if (val.hi_alarm_enable)
  {
    this.alarm_n =  Math.round((this.n_blocks-1) * (val.hi_alarm / this.max));
  }
  this.create_blocks(30, 300/this.n_blocks, this.n_blocks)
  console.log("max: " + this.max);
  console.log("eu_label: " + this.eu_label);
}
rgauge_t.prototype.init=rg_gauge_init_f;

function rg_set_block_f(n, on)
{
  var len = this.shapes.length;
  if (n < 0) n=0; 
  if (n >= len) n=len-1;
  if(!on) 
  {
    this.shapes[n].setAttribute("fill", "antiquewhite");
    return;
  } 
  if (n > this.alarm_n) 
  {
    this.shapes[n].setAttribute("fill", this.rcolor_alarm);
  }
  if (n > this.caution_n) 
  {
    this.shapes[n].setAttribute("fill", this.rcolor_caution);
  }
  else
  {
    this.shapes[n].setAttribute("fill", this.rcolor_ok);
  }

}
rgauge_t.prototype.set_block=rg_set_block_f;

function rg_gauge_update_f(pv)
{
  var n;
  //console.log("pv: " + pv + ", needle_obj = " + this.needle_obj);
  this.pv_obj.textContent = pv.toFixed(this.decimal_places);
  if (pv < 0.0) {pv = 0.0;}
  if (pv > this.max) {pv = max;}
  //console.log("max: " + this.max);
  //console.log("length: " + this.length);
  n = Math.round((this.n_blocks-1) * (pv / this.max)); // gives 0-49
  n=n+1;
  if (n > this.last_n)
  {
    for (var i=this.last_n; i < (n); i++)
    {
      this.set_block(i, true);
    }
  }
  else
  {
    // Go in reverse in case it is slow enough to see.
    for (var i=this.last_n-1; i >= n; i--)
    //for (var i=n; i < this.last_n; i++)
    {
      this.set_block(i, false);
    }
  }
  this.last_n = n;
}
rgauge_t.prototype.update=rg_gauge_update_f;

function rg_create_blocks_f(start_angle, degrees, n)
{
  this.w1 = this.r * (3/50);
  this.w2 = this.r * (15/50);

  var rads;
  var shape;
  var x1, x2, x3, x4, lx1, lx2;
  var y1, y2, y3, y4, ly1, ly2;
  var a1 = start_angle - degrees * 0.42; 
  var a2 = start_angle + degrees * 0.42; 
  var rline = this.r*(51/50); 
  var wline = this.r/50;

  console.log("Creating " + n + " blocks, each " + degrees + " degrees, starting at " + start_angle);
  this.shapes = new Array();
  for (var i=0; i < n; i++)
  {
    var ws = this.w1 + ((i/n) * (this.w2-this.w1))
    var we = this.w1 + (((i+1)/n) * (this.w2-this.w1))

    //------------------------------------------
    rads = (a1 - 90.0) * ((2.0 * Math.PI) / 360.0);

    x1 = this.cx - (this.r * Math.cos(rads));
    y1 = this.cy - (this.r * Math.sin(rads));

    x2 = this.cx - ((this.r-ws) * Math.cos(rads));
    y2 = this.cy - ((this.r-ws) * Math.sin(rads));

    lx1 = this.cx - (rline * Math.cos(rads));
    ly1 = this.cy - (rline * Math.sin(rads));
    //------------------------------------------

    //------------------------------------------
    rads = (a2 - 90.0) * ((2.0 * Math.PI) / 360.0);

    x3 = this.cx - ((this.r-we) * Math.cos(rads));
    y3 = this.cy - ((this.r-we) * Math.sin(rads));

    x4 = this.cx - (this.r * Math.cos(rads));
    y4 = this.cy - (this.r * Math.sin(rads));

    lx2 = this.cx - (rline * Math.cos(rads));
    ly2 = this.cy - (rline * Math.sin(rads));
    //------------------------------------------

    shape = document.createElementNS(svgNS, "line");
    //shape.appendChild(reactmainobj);
    shape.setAttribute("x1", lx1);
    shape.setAttribute("y1", ly1);
    shape.setAttribute("x2", lx2);
    shape.setAttribute("y2", ly2);
    shape.setAttribute("stroke-width", wline);
    if (i > this.alarm_n)
    {
      shape.setAttribute("stroke", this.rcolor_alarm);
    }
    else if (i > this.caution_n)
    {
      shape.setAttribute("stroke", this.rcolor_caution);
    }
    else
    {
      shape.setAttribute("stroke", this.rcolor_ok);
    }
    //document.documentElement.appendChild(shape);
    reactmainobj.appendChild(shape);

    shape = document.createElementNS(svgNS, "polygon");
    reactmainobj.appendChild(shape);
    //shape.appendChild(reactmainobj);
    shape.setAttribute("points", x1 + "," + y1 + " " + x2 + "," + y2 + " " + x3 + "," + y3 + " " + x4 + "," + y4);
    shape.setAttribute("stroke-width", this.r / 200);
    shape.setAttribute("stroke", "lightgray");
    shape.setAttribute("fill", "antiquewhite");
    
    //document.documentElement.appendChild(shape);
    reactmainobj.appendChild(shape);
    this.shapes[i]=shape;
    a1 += degrees;
    a2 += degrees;
  }
}
rgauge_t.prototype.create_blocks=rg_create_blocks_f;


