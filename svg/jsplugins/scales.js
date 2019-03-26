
function TicData()
{
  this.screen_val=0.0;
  this.eu_val=0.0;
  this.is_major=false;
}

function ScaleInfo()
{
  this.max_val=0.0;
  this.max_major=0.0;
  this.major_inc=0.0;
  this.minor_inc=0.0;
  this.n_major=0;
  this.n_minor=0;
  this.n_dec=0;
  this.screen_min=0.0;
  this.screen_max=0.0;
}

function f_do_print(max)
{
  console.log("max_val = " + this.max_val);
  console.log("max_major = " + this.max_major);
  console.log("major_inc = " + this.major_inc);
  console.log("minor_inc = " + this.minor_inc);
  console.log("n_major = " + this.n_major);
  console.log("n_minor = " + this.n_minor);
  console.log("n_dec = " + this.n_dec);
}
ScaleInfo.prototype.print=f_do_print;


function log10(a_val)
{
  //console.log("In log10 a_val = " + a_val);
  var the_log = Math.log(a_val) / 2.302585;
  //console.log("In log10 log = " + the_log);
  return the_log;
}

function f_reset_tic()
{
  this.major_tic = 0;
  this.minor_tic = 0;
  return true;
}
ScaleInfo.prototype.reset_tic=f_reset_tic;

function f_next_tic(tic_data)
{
  if (this.minor_tic >= this.n_minor)
  {
    this.major_tic++;
    this.minor_tic=0;
  }

  var screen_span=(this.screen_max-this.screen_min) * (this.max_major / this.max_val);
  var tic_span=screen_span / (this.n_major * this.n_minor);
  var eu_span=this.max_major / (this.n_major * this.n_minor);   

  tic_data.screen_val=this.screen_min + (screen_span * (this.major_tic / this.n_major));
  tic_data.eu_val=this.max_major * (this.major_tic / this.n_major);
  //console.log("next: " + screen_span + ", " + tic_span + ", " + eu_span);
  //console.log(" : " + tic_data.screen_val + ", " + tic_data.eu_val);

  //console.log("n_minor: " + this.n_minor + ", n: " + this.minor_tic);
  //console.log("** tics: " + this.major_tic + ", " + this.minor_tic);

  if (this.minor_tic==0)
  {
    this.minor_tic=1;
    tic_data.is_major=true; 
    if (tic_data.eu_val > this.max_val)
    {
      return false;
    } 
    else
    {
      return true;
    }
  }
  else
  {
    //console.log("@@ " + tic_data.screen_val + "," + this.minor_tic + ", " + tic_span);
    //console.log("@@ " + tic_data.eu_val + "," + this.minor_tic + ", " + eu_span);
    tic_data.screen_val += this.minor_tic*tic_span;
    tic_data.eu_val += this.minor_tic*eu_span;
    tic_data.is_major=false;
    //console.log("@@ " + tic_data.screen_val + "," + this.minor_tic + ", " + tic_span);
    //console.log("@@ " + tic_data.eu_val + "," + this.minor_tic + ", " + eu_span);
    if (tic_data.eu_val > this.max_val)
    {
      return false;
    } 
    else
    {
      this.minor_tic++;
      return true;
    }
  }
}
ScaleInfo.prototype.next_tic=f_next_tic;

function f_do_calcs(max, screen_min, screen_max)
{
  console.log("max = " + max);
  var the_log = log10(max); 
  var the_floor = Math.floor(the_log);
  var z99 = max / Math.pow(10.0, (the_floor - 1)); 
  var the_scale=0.0;
  var top_major=0.0;

  console.log("log = " + the_log);
  console.log("floor = " + the_floor);
  console.log("z99 = " + z99);

  this.screen_min = screen_min;
  this.screen_max = screen_max;
  if (z99 >= 100.0) 
  {
    the_scale = 100;
    top_major = 100;
    this.n_major = 5;
    this.n_minor = 4;
  } 
  else if (z99 >= 90.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 90;
    this.n_major = 9;
    this.n_minor = 2;
  }
  else if (z99 >= 80.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 80;
    this.n_major = 4;
    this.n_minor = 4;
  }
  else if (z99 >= 60.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 60;
    this.n_major = 3;
    this.n_minor = 4;
  }
  else if (z99 >= 50.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 50;
    this.n_major = 5;
    this.n_minor = 4;
  }
  else if (z99 >= 40.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 40;
    this.n_major = 4;
    this.n_minor = 4;
  }
  else if (z99 >= 35.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 35;
    this.n_major = 7;
    this.n_minor = 2;
  }
  else if (z99 >= 30.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 30;
    this.n_major = 3;
    this.n_minor = 4;
  }
  else if (z99 >= 25.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 25;
    this.n_major = 5;
    this.n_minor = 5;
  }
  else if (z99 >= 20.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 20;
    this.n_major = 4;
    this.n_minor = 5;
  }
  else if (z99 >= 18.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 18;
    this.n_major = 6;
    this.n_minor = 3;
  }
  else if (z99 >= 15.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 15;
    this.n_major = 5;
    this.n_minor = 3;
  }
  else if (z99 >= 14.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 14;
    this.n_major = 7;
    this.n_minor = 2;
  }
  else if (z99 >= 12.0) 
  {
    the_scale = Math.ceil(z99);
    top_major = 12;
    this.n_major = 6;
    this.n_minor = 2;
  }
  else
  {
    the_scale = Math.ceil(z99);
    top_major = 10;
    this.n_major = 5;
    this.n_minor = 2;
  }
  console.log("top = " + the_scale);
  console.log("top_major = " + top_major);
  console.log("n_major = " + this.n_major);
  console.log("n_minor = " + this.n_minor);
  this.n_dec = 0; 
  if (the_floor < 1)
  {
    this.n_dec = (-the_floor) + 1;
  }

  this.major_inc = ((top_major) /(this.n_major)) *  Math.pow(10.0, (the_floor - 1));
  this.minor_inc = ((top_major) / (this.n_major * this.n_minor)) *  Math.pow(10.0, (the_floor - 1));
  this.max_val = (the_scale) *  Math.pow(10.0, (the_floor - 1));
  this.max_major = (top_major) *  Math.pow(10.0, (the_floor - 1));
}
ScaleInfo.prototype.calc=f_do_calcs;

