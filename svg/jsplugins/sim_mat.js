var sim;

function pump_1_timeout()
{
  sim.toggle_pump1();
};

function pump_2_timeout()
{
  sim.toggle_pump2();
};

function pump_3_timeout()
{
  sim.toggle_pump3();
};

function sim_object_t()
{
  this.lo_level=89;
  this.hi_level=270;
  this.hi_hi_level=325;
  this.rate=0;
  this.up=true;
  this.last_time = 0;

  this.pv =
  {
    "RAW_COUNT":0,
    "COUNT":0,
    "FORCE":0.0,
    "POWER_ON":false,
    "COLLECT_ON":false,
  };

  this.cfg =
  {
    "RAW_COUNT":{"tag":"RAW_COUNT","description":"Raw Count","eu":"","decimal_places":0,"lo_alarm":0,"lo_caution":0,"hi_caution":0,"hi_alarm":0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false, "scale_lo":0,"scale_hi":4095},
    "COUNT":{"tag":"COUNT","description":"Count From Start","eu":"","decimal_places":0,"lo_alarm":0,"lo_caution":0,"hi_caution":0,"hi_alarm":0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false, "scale_lo":0,"scale_hi":400},
    "FORCE":{"tag":"FORCE","description":"The Force","eu":"kg/cm^2","decimal_places":1,"zero_cutoff":0,"lo_alarm":0,"lo_caution":0,"hi_caution":0,"hi_alarm":0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false,"scale_lo":0, "scale_hi":200},
    "POWER_ON": {"tag":"POWER_ON","description":"Power ON","lo_desc":"Off","hi_desc":"ON", "invert_pv":false},
    "COLLECT_ON":{"tag":"COLLECT_ON","description":"Collect ON","lo_desc":"stop","hi_desc":"start", "invert_pv":false}
  }; 
}

function sim_object_init_f(val)
{

}
sim_object_t.prototype.init=sim_object_init_f;

function toggle_pump2_f(val)
{
  this.pv.PUMP2_ON = !this.pv.PUMP2_ON
  console.log("Toggle pump 2 to:", this.pv.PUMP2_ON);
}
sim_object_t.prototype.toggle_pump2=toggle_pump2_f;

function toggle_pump3_f(val)
{
  this.pv.PUMP3_ON = !this.pv.PUMP3_ON
  console.log("Toggle pump 3 to:", this.pv.PUMP3_ON);
}
sim_object_t.prototype.toggle_pump3=toggle_pump3_f;

function toggle_pump1_f(val)
{
  this.pv.PUMP1_ON = !this.pv.PUMP1_ON
  console.log("Toggle pump 1 to:", this.pv.PUMP1_ON);
}
sim_object_t.prototype.toggle_pump1=toggle_pump1_f;

function sim_object_get_cfg_f(tag)
{
  console.log("getting config for: " + tag);
  var the_val = this.cfg[tag.toUpperCase()];
  console.log("cfg value: " + the_val);
  return the_val;
}
sim_object_t.prototype.get_cfg=sim_object_get_cfg_f;

function sim_object_get_pv_f(tag)
{
  //console.log("getting PV for: " + tag);
  var the_val = this.pv[tag.toUpperCase()];
  //console.log("PV: " + the_val);
  return the_val;
}
sim_object_t.prototype.get_pv=sim_object_get_pv_f;

function sim_object_set_pv_f(tag, new_val)
{
  console.log("setting PV for: " + tag);
  this.pv[tag.toUpperCase()]=new_val;
  console.log("new PV: " + new_val);
}
sim_object_t.prototype.set_pv=sim_object_set_pv_f;

function next_value(tau, steady_state_value, last_value, delta_t)
{
  if (Math.abs(steady_state_value - last_value) < 0.01)
  {
    return steady_state_value;
  }
  var fraction = 1.0 - Math.exp(-delta_t/tau);
  return last_value + ((steady_state_value - last_value) * fraction);
}

function sim_object_update_f(now)
{
  if (this.pv.POWER_ON)
  {
    this.pv.FORCE = 168+((Math.random()-0.5)*3.1);
    this.pv.RAW_COUNT += 1;
  }
  else
  {
    this.pv.FORCE = 0;
    this.pv.COLLECT_ON = 0;
  }

  if ((this.pv.RAW_COUNT) > 4095) //rolover at 4095
  {
    this.pv.RAW_COUNT -= 4096;
  }

  if (this.pv.POWER_ON && this.pv.COLLECT_ON)
  {
    this.pv.COUNT += 1;
  }
  else
  {
    this.pv.COUNT = 0;
  }
  //setTimeout("pump_3_timeout()", 7000);
  //setTimeout("pump_1_timeout()", 7000);
}
sim_object_t.prototype.update=sim_object_update_f;

sim = new sim_object_t;

