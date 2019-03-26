

function delete_trailing_zeros(str)
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

function gauge_create_band(angle1, angle2, cx, cy, r1, r2, color)
{
  var x1;
  var y1;
  var x1;
  var y1;
  var shape;
  var stroke_width = r2 - r1;
  var r = (r1 + r2) / 2;

  var rads1 = (angle1 - 90.0) * ((2.0 * Math.PI) / 360.0);
  var rads2 = (angle2 - 90.0) * ((2.0 * Math.PI) / 360.0);

  var x1 = cx - (r * Math.cos(rads1));
  var y1 = cy - (r * Math.sin(rads1));
  var x2 = cx - (r * Math.cos(rads2));
  var y2 = cy - (r * Math.sin(rads2));
  var shape = document.createElementNS(svgNS, "path");
  shape.setAttribute("fill", "none");
  shape.setAttribute("stroke", color);
  shape.setAttribute("stroke-width", stroke_width);
  var dstr = "M" + x1 + " " + y1 + " A" + r + " " + r + " 0 0 1 " + x2 + " " + y2;
  console.log("dstr: " + dstr);
  shape.setAttribute("d", dstr);
  //document.documentElement.appendChild(shape);
  reactmainobj.appendChild(shape);
}

function gauge_create_text(val, angle, cx, cy, radius, font_size, dec)
{
  var rads;
  var x;
  var y;
  var shape;

  rads = (angle - 90.0) * ((2.0 * Math.PI) / 360.0);
  x = cx - (radius * Math.cos(rads));
  y = cy - (radius * Math.sin(rads));
  console.log("vals- x:" + x + ", y:" + y + ", rads:" + rads);
  shape = document.createElementNS(svgNS, "text");
  shape.setAttribute("x", x);
  shape.setAttribute("y", y);
  shape.setAttribute("font-size", font_size);
  shape.setAttribute("baseline-shift", "-33%");
  shape.setAttribute("text-anchor", "middle");
  shape.textContent=delete_trailing_zeros(val.toFixed(dec));
  //document.documentElement.appendChild(shape);
  reactmainobj.appendChild(shape);
}


function gauge_t(needle_name, text_name, eu_name, x, y, width)
{
  console.log("needle_name " + needle_name);
  console.log("text_name " + text_name);
  //console.log("length " + this.length);
  this.max=200.0;
  this.needle_obj=document.getElementById(needle_name);
  this.text_obj=document.getElementById(text_name);
  this.eu_obj=document.getElementById(eu_name);
  this.eu_label="not set";
  this.x=x;
  this.y=y;
  this.width=width;
  this.cx=this.x + (this.width / 2.0)
  this.cy=this.y + (this.width / 2.0)
  console.log("needle_obj: " + this.needle_obj);
  console.log("text_obj: " + this.text_obj);
}

function gauge_init_f(val)
{
  this.max=val.scale_hi;
  this.eu_label=val.eu;
  this.decimal_places = val.decimal_places;
  this.eu_obj.textContent = this.eu_label;
  console.log("max: " + this.max);
  console.log("eu_label: " + this.eu_label);
      var sinfo = new ScaleInfo();
      var tdata = new TicData();
      var shape;
      var angle;
      if (val.hi_caution_enable)
      {
        angle = ((val.hi_caution / this.max) * 270) + 45; 
        if ((angle > 45) && (angle < 315))
        {
          gauge_create_band(angle, 315, this.cx, this.cy, 
                  this.width * (65.5/150), this.width * (70/150), "yellow");
        }
      }
      if (val.hi_alarm_enable)
      {
        angle = ((val.hi_alarm / this.max) * 270) + 45; 
        if ((angle > 45) && (angle < 315))
        {
          gauge_create_band(angle, 315, this.cx, this.cy, 
                  this.width * (65.5/150), this.width * (70/150), "red");
        }
      }
      if (val.lo_caution_enable)
      {
        angle = ((val.lo_caution / this.max) * 270) + 45; 
        if ((angle > 45) && (angle < 315))
        {
          gauge_create_band(45, angle, this.cx, this.cy, 
                  this.width * (65.5/150), this.width * (70/150), "yellow");
        }
      }
      if (val.lo_alarm_enable)
      {
        angle = ((val.lo_alarm / this.max) * 270) + 45; 
        if ((angle > 45) && (angle < 315))
        {
          gauge_create_band(45, angle, this.cx, this.cy, 
                  this.width * (65.5/150), this.width * (70/150), "red");
        }
      }
      sinfo.print();
      sinfo.calc(this.max,45,315);
      sinfo.print();
      sinfo.reset_tic();
      while(sinfo.next_tic(tdata))
      {
        var mystr;
        if (tdata.is_major)
        {
          console.log("======== " + tdata.eu_val + ", " + tdata.screen_val);
          shape = document.createElementNS(svgNS, "polygon");
          //shape.setAttribute("points", "75,140 72.5,145 77.5,145");
          shape.setAttribute("points", "" + this.cx + "," + (this.y + (this.width * (140/150))) + " " + 
               (this.cx -(this.width * (2.5/150))) + "," +  (this.y + (this.width * (145.5/150))) + " " + 
               (this.cx + (this.width * (2.5/150))) + "," + (this.y + (this.width * (145.5/150))) );
          shape.setAttribute("style", "fill:black;stroke:none;stroke-width:0");
          mystr= "rotate("+ tdata.screen_val + " " + this.cx + " " + this.cy +")";
          console.log(mystr);
          shape.setAttribute("transform", mystr);
          //document.documentElement.appendChild(shape);
          reactmainobj.appendChild(shape);
          gauge_create_text(tdata.eu_val, tdata.screen_val, this.cx, this.cy, 
                this.width * (52.5/150), this.width * (10/150), this.decimal_places)
        }
        else
        {
          console.log("- " + tdata.eu_val + ", " + tdata.screen_val);
          shape = document.createElementNS(svgNS, "line");
          shape.setAttribute("x1", this.cx);
          shape.setAttribute("y1", this.y + (this.width * (140/150)));
          shape.setAttribute("x2", this.cx);
          shape.setAttribute("y2", this.y + (this.width * (145/150)));
          shape.setAttribute("style", "stroke:black;stroke-width:" + (this.width * (1/150)));
          mystr= "rotate("+ tdata.screen_val + " " + this.cx + " " + this.cy +")";
          console.log(mystr);
          shape.setAttribute("transform", mystr);
          //document.documentElement.appendChild(shape);
          reactmainobj.appendChild(shape);
        }
      }

}
gauge_t.prototype.init=gauge_init_f;

function gauge_update_f(pv)
{
  //console.log("pv: " + pv + ", needle_obj = " + this.needle_obj);
  this.text_obj.textContent = pv.toFixed(this.decimal_places);
  if (pv < 0.0) {pv = 0.0;}
  //console.log("max: " + this.max);
  //console.log("length: " + this.length);
  var angle = 45 + ((pv / this.max) * 270)
  var mystr= "rotate("+ angle + " " + this.cx + " " + this.cy +")";
  this.needle_obj.setAttribute("transform", mystr);
}
gauge_t.prototype.update=gauge_update_f;

