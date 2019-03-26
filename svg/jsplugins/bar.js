
function bar_t(bar_name, text_name, x1, y1, x2, y2, stroke_width)
{
  console.log("bar_name " + bar_name);
  console.log("text_name " + text_name);
  this.length=y2-y1;
  console.log("length " + this.length);
  this.max=200.0;
  this.bar_obj=document.getElementById(bar_name);
  this.text_obj=document.getElementById(text_name);
  this.eu_label="not set";
  this.x1=x1;
  this.y1=y1;
  this.x2=x2;
  this.y2=y2;
  this.stroke_width = stroke_width;
  console.log("bar_obj: " + this.bar_obj);
  console.log("text_obj: " + this.text_obj);
}

function bar_draw_alarm(x, y1, y2, color, width)
{
  var shape = document.createElementNS(svgNS, "line");
  shape.setAttribute("x1", x);
  shape.setAttribute("y1", y1);
  shape.setAttribute("x2", x);
  shape.setAttribute("y2", y2);
  shape.setAttribute("stroke", color);
  shape.setAttribute("stroke-width", width);
  //document.documentElement.appendChild(shape);
  reactmainobj.appendChild(shape);
}

function bar_init_f(val)
{
  this.max=val.scale_hi;
  this.eu_label=val.eu;
  this.decimal_places = val.decimal_places;
  console.log("max: " + this.max);
  console.log("eu_label: " + this.eu_label);
  if (val.hi_caution_enable || val.hi_alarm_enable ||
      val.lo_caution_enable || val.lo_alarm_enable)
  {
      var ay;
      var awidth = (this.x2 - this.x1) * 0.3;
      var ax = this.x1 - (awidth * 0.5) - (this.stroke_width * 0.5);
      bar_draw_alarm(ax, this.y1, this.y2, "lime", awidth);
      if (val.hi_caution_enable)
      {
        ay = this.y2 -((val.hi_caution / this.max) * (this.y2 - this.y1));
        if ((ay > this.y1) && (ay < this.y2))
        {
          bar_draw_alarm(ax, this.y1, ay, "yellow", awidth);
        }
      }
      if (val.hi_alarm_enable)
      {
        ay = this.y2 -((val.hi_alarm / this.max) * (this.y2 - this.y1));
        if ((ay > this.y1) && (ay < this.y2))
        {
          bar_draw_alarm(ax, this.y1, ay, "red", awidth);
        }
      }

      if (val.lo_caution_enable)
      {
        ay = this.y2 -((val.lo_caution / this.max) * (this.y2 - this.y1));
        if ((ay > this.y1) && (ay < this.y2))
        {
          bar_draw_alarm(ax, ay, this.y2, "yellow", awidth);
        }
      }
      if (val.lo_alarm_enable)
      {
        ay = this.y2 -((val.lo_alarm / this.max) * (this.y2 - this.y1));
        if ((ay > this.y1) && (ay < this.y2))
        {
          bar_draw_alarm(ax, ay, this.y2, "red", awidth);
        }
      }
  }

      console.log("Starting tics");
      var sinfo = new ScaleInfo();
      var tdata = new TicData();
      var shape;
      sinfo.print();
      console.log("Starting tics2:");
      sinfo.calc(this.max,this.y2,this.y1);
      sinfo.print();
      sinfo.reset_tic();
      console.log("Starting tics3:");
      while(sinfo.next_tic(tdata))
      {
        var mystr;
        if (tdata.is_major)
        {
          console.log("======== " + tdata.eu_val + ", " + tdata.screen_val);
          shape = document.createElementNS(svgNS, "line");
          shape.setAttribute("x1", this.x2);
          shape.setAttribute("y1", tdata.screen_val);
          shape.setAttribute("x2", this.x2+(this.length*0.02));
          shape.setAttribute("y2", tdata.screen_val);
          shape.setAttribute("style", "stroke:black;stroke-width:" + this.stroke_width);
          reactmainobj.appendChild(shape);
          //document.documentElement.appendChild(shape);

          var text_height = this.length * 0.06; 
          shape = document.createElementNS(svgNS, "text");
          shape.setAttribute("x", this.x2 + (this.length * 0.03));
          shape.setAttribute("y", tdata.screen_val + (text_height * 0.33));
          shape.setAttribute("font-size", text_height);
          //shape.setAttribute("baseline-shift", "-33%");
          shape.setAttribute("text-anchor", "start");
          shape.textContent=tdata.eu_val.toFixed(this.decimal_places);
          //document.documentElement.appendChild(shape);
          reactmainobj.appendChild(shape);

        }
        else
        {
          console.log("- " + tdata.eu_val + ", " + tdata.screen_val);
          shape = document.createElementNS(svgNS, "line");
          shape.setAttribute("x1", this.x2);
          shape.setAttribute("y1", tdata.screen_val);
          shape.setAttribute("x2", this.x2+(this.length * 0.015));
          shape.setAttribute("y2", tdata.screen_val);
          shape.setAttribute("style", "stroke:black;stroke-width:" + (this.stroke_width * 0.5));
          reactmainobj.appendChild(shape);
          //document.documentElement.appendChild(shape);
        }
      }

}
bar_t.prototype.init=bar_init_f;

function bar_update_f(pv)
{
  //console.log("pv: " + pv + ", bar_obj = " + this.bar_obj);
  this.text_obj.textContent = pv.toFixed(this.decimal_places) + " " + this.eu_label;
  if (pv < 0.0) {pv = 1.0;}
  //console.log("max: " + this.max);
  //console.log("length: " + this.length);
  this.bar_obj.setAttribute("height", pv * (this.length / this.max));
}
bar_t.prototype.update=bar_update_f;

