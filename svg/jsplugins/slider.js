
function slider_t(tag, slide_name, rect_name, text_name, x1, y1, length, width, timeout_fn) 
{
  this.slide_obj = document.getElementById(slide_name);
  this.rect_obj = document.getElementById(rect_name);
  this.text_obj = document.getElementById(text_name);

  console.log("slide_name: " + slide_name);
  console.log("rect_name: " + rect_name);
  console.log("text_name: " + text_name);
  console.log("slide_obj: " + this.slide_obj);
  console.log("rect_obj: " + this.rect_obj);
  console.log("text_obj: " + this.text_obj);

  this.tag = tag;
  this.x1 = x1;
  this.y1 = y1;
  this.length = length;
  this.width = width;
  this.timeout_fn = timeout_fn;
  this.mouse_dragging = false;
  this.output_delayed=false;
  this.pv = 0.0;
  this.last_output_time = (new Date()).getTime() - 100000; //no output delay on startup
  console.log("timeout function: " + this.timeout_fn);
}

function slider_mouseup_f(evt)
{
  this.mouse_dragging = false;
  //document.body.deselectAll();
  //document.selection.empty();
  window.getSelection().removeAllRanges();
  console.log("mouse up");
  console.log("X: " + evt.clientX + " Y: " + evt.clientY);
}
slider_t.prototype.mouseup=slider_mouseup_f;

function slider_mousedown_f(evt)
{
  this.mouse_dragging = true;
  this.mousemove(evt);
  console.log("mouse down");
}
slider_t.prototype.mousedown=slider_mousedown_f;


function slider_timeout_f()
{
  this.output_delayed = false;
  console.log("Slider TIMEOUT!");
  if (this.had_outputs)
  {
    this.output();
  }
}
slider_t.prototype.timeout=slider_timeout_f;


function slider_output_f()
{
  if(this.output_delayed)
  {
    this.had_outputs = true;
  }
  else
  {
    this.output_delayed = true; 
    this.had_outputs = false;
    setTimeout(this.timeout_fn, MAX_OUTPUT_RATE);
    this.last_output_time = (new Date()).getTime();
    this.tag = convert_tag(this.tag);
    send_output(this.tag, this.pv);
    console.log("Output sent !!!!!!!!!!!!!!!!!!!!!");
  }
}
slider_t.prototype.output=slider_output_f;

function slider_mousemove_f(evt)
{
  if (this.mouse_dragging)
  {
    if(this.slide_obj) 
    {
      var pnt = transform_mouse_xy(evt, this.slide_obj)
      if (pnt.x < this.x1){ pnt.x = this.x1;}
      if (pnt.x > (this.x1 + this.length)){pnt.x = this.x1 + this.length;}
      this.slide_obj.setAttribute("cx", pnt.x); 
      this.rect_obj.setAttribute("width", pnt.x - this.x1);
      var fraction = (pnt.x - this.x1) / this.length;

      var pv_now =  this.scale_lo + (fraction * (this.scale_hi - this.scale_lo)); 
      this.text_obj.textContent = pv_now.toFixed(this.decimal_places);
      this.text_obj.setAttribute("x", pnt.x); 
      this.pv = pv_now;
      this.output();
    }
  }
}
slider_t.prototype.mousemove=slider_mousemove_f;

/***
function slider_barclick_f(evt)
{
  this.mouse_dragging=true;
  this.mousemove(evt);
  //this.mouse_dragging=false;
}
slider_t.prototype.barclick=slider_barclick_f;
****/

function slider_reinit_f(val)
{
  this.scale_hi = val.scale_hi;
  this.scale_lo = val.scale_lo;
  this.eu = val.eu;
  this.decimal_places = val.decimal_places;
  this.scale_lo_text.textContent=this.scale_lo.toFixed(this.decimal_places);
  this.scale_hi_text.textContent=this.scale_hi.toFixed(this.decimal_places);
}
slider_t.prototype.reinit=slider_reinit_f;

function slider_init_f(val)
{
  console.log("slider init: tag = " + this.tag);
  var text_height = this.width * 1.0; 
  var text_y = this.y1 + (this.width * 2.5); 
  console.log("Text, x: " +  this.x1 + " y: " + text_y + " h: " + text_height + " val: " + val.scale_lo.toFixed(val.decimal_places)); 
  shape = document.createElementNS(svgNS, "text");
  shape.setAttribute("x", this.x1);
  shape.setAttribute("y", text_y);
  shape.setAttribute("font-size", text_height);
  //shape.setAttribute("baseline-shift", "-66%");
  shape.setAttribute("text-anchor", "middle");
  shape.textContent="";
  //document.documentElement.appendChild(shape);
  reactmainobj.appendChild(shape);
  this.scale_lo_text = shape;

  shape = document.createElementNS(svgNS, "text");
  shape.setAttribute("x", this.x1 + this.length);
  shape.setAttribute("y", text_y);
  shape.setAttribute("font-size", text_height);
  shape.setAttribute("text-anchor", "middle");
  shape.textContent="";
  reactmainobj.appendChild(shape);
  this.scale_hi_text = shape;

  this.reinit(val);
}
slider_t.prototype.init=slider_init_f;


function slider_update_f(pv)
{
  var now = (new Date()).getTime();
  // ONLY set the value if we have not sent an output in the last second.
  if ((now - this.last_output_time) > UPDATE_DELAY)
  {
    var fraction =  (pv - this.scale_lo) / (this.scale_hi - this.scale_lo); 
    var x = this.x1 + (fraction * this.length);
    this.rect_obj.setAttribute("width", x - this.x1);
    this.text_obj.textContent = pv.toFixed(this.decimal_places);
    this.slide_obj.setAttribute("cx", x); 
    this.text_obj.setAttribute("x", x); 
    this.pv = pv;
  }
  else
  {
    console.log("Update ignored, slider moved recently .........");
  }
}
slider_t.prototype.update=slider_update_f;

function slider_set_tag_f(the_new_tag)
{
  this.tag = the_new_tag;
  this.reinit(get_config(the_new_tag));
}
slider_t.prototype.set_tag=slider_set_tag_f;

function slider_interval_handler_f()
{
}
slider_t.prototype.interval_handler=slider_interval_handler_f;


