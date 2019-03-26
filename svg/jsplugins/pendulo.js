
function pendulo_t(pendulo_name, pendulo_pv_name,  x, y)
{
  console.log("pendulo_name " + pendulo_name);
  this.pendulo_obj=document.getElementById(pendulo_name);
  this.pv_obj=document.getElementById(pendulo_pv_name);
  this.x = x;
  this.y = y;
  console.log("x: " + this.x);
  console.log("y: " + this.y);
  console.log("pendulo_obj: " + this.pendulo_obj);
  this.max=100;
  this.eu_label="undefined";
  this.decimal_places = 1;
}

function pendulo_init_f(val)
{
  this.max=val.scale_hi;
  this.eu_label=val.eu;
  this.decimal_places = val.decimal_places;
  console.log("max: " + this.max);
  console.log("eu_label: " + this.eu_label);
  console.log("decimal_places: " + this.decimal_places);
}
pendulo_t.prototype.init=pendulo_init_f;

function pendulo_update_f(pv)
{
  //console.log("pv: " + pv + ", pendulo_obj = " + this.pendulo_obj);
  //console.log("max: " + this.max);
  if (pv < 0.0) {pv = 0.0;}
  var angle = 45 - (90.0 * (pv/this.max));
  //console.log("angle: " + angle);
  var mystr= "rotate("+ angle + " " + this.x + " " + this.y + ")";
  //console.log("mystr: " + mystr);
  this.pendulo_obj.setAttribute("transform", mystr);  
  this.pv_obj.textContent = 
     pv.toFixed(this.decimal_places) + " " + this.eu_label + " " + angle.toFixed(1);
}
pendulo_t.prototype.update=pendulo_update_f;

