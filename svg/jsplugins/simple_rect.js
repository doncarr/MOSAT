
function simple_rect_t(rect_name, text_name, length)
{
  console.log("rect_name " + rect_name);
  console.log("text_name " + text_name);
  console.log("length " + length);
  this.max=200.0;
  this.length=length;
  this.rect_obj=document.getElementById(rect_name);
  this.text_obj=document.getElementById(text_name);
  this.eu_label="not set";
  console.log("rect_obj: " + this.rect_obj);
  console.log("text_obj: " + this.text_obj);
}

function simple_rect_init_f(val)
{
  this.max=val.scale_hi;
  this.eu_label=val.eu;
  this.decimal_places = val.decimal_places;
  console.log("max: " + this.max);
  console.log("eu_label: " + this.eu_label);
}
simple_rect_t.prototype.init=simple_rect_init_f;

function simple_rect_update_f(pv)
{
  //console.log("pv: " + pv + ", rect_obj = " + this.rect_obj);
  this.text_obj.textContent = pv.toFixed(this.decimal_places) + " " + this.eu_label;
  if (pv < 0.0) {pv = 1.0;}
  //console.log("max: " + this.max);
  //console.log("length: " + this.length);
  this.rect_obj.setAttribute("height", pv * (this.length / this.max));
}
simple_rect_t.prototype.update=simple_rect_update_f;

