
function analog_pv_t(text_name)
{
  console.log("text_name " + text_name);
  this.text_obj=document.getElementById(text_name);
  this.eu_label="not set";
  this.decimal_places = 0;
  console.log("text_obj: " + this.text_obj);
}

function analog_pv_init_f(val)
{
  this.decimal_places = val.decimal_places;
  this.eu_label=val.eu;
  console.log("max: " + this.max);
  console.log("eu_label: " + this.eu_label);
}
analog_pv_t.prototype.init=analog_pv_init_f;

function analog_pv_update_f(pv)
{
  //console.log("pv: " + pv + ", rect_obj = " + this.rect_obj);
  this.text_obj.textContent = pv.toFixed(this.decimal_places) + " " + this.eu_label;
  //if (pv < 0.0) {pv = 1.0;}
  //console.log("max: " + this.max);
  //console.log("length: " + this.length);
  //this.rect_obj.setAttribute("height", pv * (this.length / this.max));
}
analog_pv_t.prototype.update=analog_pv_update_f;

