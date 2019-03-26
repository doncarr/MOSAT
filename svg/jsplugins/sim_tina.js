var sim;
// This simulation adapted from C++ code written by Gerónimo Orozco
const TANK_HI_LEVEL_CM = (27.0)
const TANK_LO_LEVEL_CM = (24.0)

const START_TEMPERATURE = (55.0)
const START_LEVEL = (25.0)

const FILTER1_CHANGE_TIME = (22.0)
const FILTER2_CHANGE_TIME = (44.0)

const COLD_WATER_FILL_RATE = (20.0/60)
const PUMP_DRAIN_RATE = (-35.0/60)

const VALVE_ON_TEMPERATURE_RATE = (-10.0/60)
const VALVE_OFF_TEMPERATURE_RATE = (20.0/60)

function sim_object_t()
{
  this.fill_rate = 0;
  this.temperature_rate = 0;
  this.tank_level = START_LEVEL; 
  this.tank_temperature = START_TEMPERATURE;
  this.motor_on = false;
  this.last_time = 0.0;
  this.filter_1_time = 0.0;
  this.filter_1_state = 0;
  this.filter_1_valve = false;
  this.filter_2_time = 0.0;
  this.filter_2_state = 0;
  this.on_pressure = 121.6; 

  this.pv =
  {
    "TEMP_1":55,
    "LEVEL":25,
    "PRESSURE":0,
    "LO_LEVEL":true,
    "HI_LEVEL":false,
    "HYDRAULIC_1A":false,
    "HYDRAULIC_1B":false,
    "HYDRAULIC_2A":false,
    "HYDRAULIC_2B":false,
    "RED_DO_1":false,
    "YELLOW_DO_1":false,
    "GREEN_DO_1":true,
    "RED_DO_2":false,
    "YELLOW_DO_2":false,
    "GREEN_DO_2":true,
    "MOTOR_ON":false,
    "VALVE_1":false,
    "VALVE_2":false,
  }

  this.cfg =
  {
    "TEMP_1":{"tag":"TEMP_1","description":"Tank Temp 1","eu":"degC","driver":0,"card":0,"channel":0,"eu_lo":0.000000,"eu_hi":10.000000,"raw_lo":0.000000,"raw_hi":10.000000,"decimal_places":1,"zero_cutoff":-5000.000000,"lo_alarm":50.000000,"lo_caution":52.000000,"hi_caution":58.000000,"hi_alarm":60.000000,"deadband":0.500000,"lo_alarm_enable":true,"lo_caution_enable":true,"hi_caution_enable":true,"hi_alarm_enable":true,"scale_lo":0.000000, "scale_hi":70.000000},

    "LEVEL":{"tag":"LEVEL","description":"Tank Level","eu":"cm","driver":0,"card":0,"channel":2,"eu_lo":0.000000,"eu_hi":10.000000,"raw_lo":0.000000,"raw_hi":10.000000,"decimal_places":1,"zero_cutoff":-5000.000000,"lo_alarm":21,"lo_caution":23,"hi_caution":28,"hi_alarm":30,"deadband":1,"lo_alarm_enable":true,"lo_caution_enable":true,"hi_caution_enable":true,"hi_alarm_enable":true,"scale_lo":0.000000, "scale_hi":35.000000},
    "PRESSURE":{"tag":"PRESSURE","description":"Pressure","eu":"psi","driver":0,"card":0,"channel":3,"eu_lo":0.000000,"eu_hi":10.000000,"raw_lo":0.000000,"raw_hi":10.000000,"decimal_places":0,"zero_cutoff":-5000.000000,"lo_alarm":21,"lo_caution":23,"hi_caution":28,"hi_alarm":30,"deadband":1,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false,"scale_lo":0.000000, "scale_hi":150.000000},

    "HI_LEVEL":{"tag":"hi_level","description":"Above high Level","driver":0,"card":0,"channel":9, "lo_desc":"ON","hi_desc":"OFF", "alarm_state":"NONE","shutdown_state":"NONE"},

    "LO_LEVEL":{"tag":"lo_level","description":"Above low Level","driver":0,"card":0,"channel":8, "lo_desc":"ON","hi_desc":"OFF", "alarm_state":"NONE","shutdown_state":"NONE"},

    "HYDRAULIC_1A":{"tag":"hydraulic_1a","description":"hydraulic switch 1a","driver":0,"card":0,"channel":0, "lo_desc":"ON","hi_desc":"OFF", "alarm_state":"NONE","shutdown_state":"NONE"},

    "HYDRAULIC_1B":{"tag":"hydraulic_1b","description":"hydraulic switch 1b","driver":0,"card":0,"channel":1, "lo_desc":"ON","hi_desc":"OFF", "alarm_state":"NONE","shutdown_state":"NONE"},

    "RED_DO_1":{"tag":"red_do_1","description":"Red Light 1", "lo_desc":"ON","hi_desc":"OFF"},

    "YELLOW_DO_1":{"tag":"yellow_do_1","description":"Yellow Light 1", "lo_desc":"ON","hi_desc":"OFF"},

    "GREEN_DO_1":{"tag":"green_do_1","description":"Green Light 1", "lo_desc":"ON","hi_desc":"OFF"},

    "HYDRAULIC_2A":{"tag":"hydraulic_2a","description":"hydraulic switch 2a","driver":0,"card":0,"channel":2, "lo_desc":"ON","hi_desc":"OFF", "alarm_state":"NONE","shutdown_state":"NONE"},

    "HYDRAULIC_2B":{"tag":"hydraulic_2b","description":"hydraulic switch 2b","driver":0,"card":0,"channel":3, "lo_desc":"ON","hi_desc":"OFF", "alarm_state":"NONE","shutdown_state":"NONE"},

    "RED_DO_2":{"tag":"red_do_2","description":"Red Light 2", "lo_desc":"ON","hi_desc":"OFF"},

    "YELLOW_DO_2":{"tag":"yellow_do_2","description":"Yellow Light 2", "lo_desc":"ON","hi_desc":"OFF"},

    "GREEN_DO_2":{"tag":"green_do_2","description":"Green Light 2", "lo_desc":"ON","hi_desc":"OFF"},

    "MOTOR_ON":{"tag":"motor_on","description":"Motor On","driver":0,"card":0,"channel":6, "lo_desc":"ON","hi_desc":"OFF", "alarm_state":"NONE","shutdown_state":"NONE"},

    "VALVE_1":{"tag":"valve_1","description":"Valve 1", "lo_desc":"ON","hi_desc":"OFF"},

    "VALVE_2":{"tag":"valve_2","description":"Valve 2", "lo_desc":"ON","hi_desc":"OFF"}

  }
} 

function sim_object_init_f(val)
{

}
sim_object_t.prototype.init=sim_object_init_f;

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


function sim_sm1_f(now)
{
  var elapsed_time = now - this.filter_1_time;

  if (this.filter_1_state == 0)
  {
    if (elapsed_time > (FILTER1_CHANGE_TIME - 10))
    {
      this.pv.YELLOW_DO_1=true;
      this.pv.GREEN_DO_1=false;
      this.filter_1_state++;
      this.filter_1_time=now;
    }
  }
  else if (this.filter_1_state == 1)
  {
    if (elapsed_time > 10)
    {
      this.pv.RED_DO_1=true;
      this.pv.YELLOW_DO_1=false;
      this.filter_1_time=now;
      this.filter_1_state++;
    }
  }
  else if (this.filter_1_state == 2)
  {
    if (elapsed_time > 10)
    {
      this.pv.RED_DO_1=true;
      this.pv.YELLOW_DO_1=false;
      this.filter_1_time=now;
      this.filter_1_state++;
    }
  }
  else if (this.filter_1_state == 3)
  {
    if ((elapsed_time > 1) && (!this.motor_on))
    {
      this.motor_on=true;
      this.filter_1_time=now;
      this.filter_1_state++;
    }
  }
  else if (this.filter_1_state == 4)
  {
    if (elapsed_time > 1)
    {
      if (this.filter_1_valve)
      {
        this.pv.HYDRAULIC_1A = true;
      }
      else
      {
        this.pv.HYDRAULIC_1B = true;
      }
      this.filter_1_time=now;
      this.filter_1_state++;
    }
  }
  else if (this.filter_1_state == 5)
  {
    if (elapsed_time > 2)
    {
      this.pv.RED_DO_1=true;
      this.pv.YELLOW_DO_1=true;
      this.pv.GREEN_DO_1=true;
      this.filter_1_time=now;
      this.filter_1_state++;
    }
  }
  else if (this.filter_1_state == 6)
  {
    if (elapsed_time > 5)
    {
      if (this.filter_1_valve)
      {
        this.pv.HYDRAULIC_1A = false;
      }
      else
      {
        this.pv.HYDRAULIC_1B = false;
      }
      this.filter_1_valve = !this.filter_1_valve;
      this.filter_1_time=now;
      this.filter_1_state++;
    }
  }
  else if (this.filter_1_state == 7)
  {
    if (elapsed_time > 4)
    {
      this.pv.RED_DO_1=false;
      this.pv.YELLOW_DO_1=false;
      this.motor_on=false;
      this.filter_1_time=now;
      this.filter_1_state=0;
    }
  }
}
sim_object_t.prototype.state_machine_filter_1=sim_sm1_f;

function sim_sm2_f(now)
{
  var elapsed_time = now - this.filter_2_time;

  if (this.filter_2_state == 0)
  {
    if (elapsed_time > (FILTER2_CHANGE_TIME - 10))
    {
      this.pv.YELLOW_DO_2=true;
      this.pv.GREEN_DO_2=false;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 1)
  {
    if (elapsed_time > 10)
    {
      this.pv.RED_DO_2=true;
      this.pv.YELLOW_DO_2=false;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 2)
  {
    if (elapsed_time > 10)
    {
      this.pv.RED_DO_2=true;
      this.pv.YELLOW_DO_2=false;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 3)
  {
    if ((elapsed_time > 1) && (!this.motor_on))
    {
      this.motor_on=true;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 4)
  {
    if (elapsed_time > 1)
    {
      this.pv.HYDRAULIC_2A = true;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 5)
  {
    if (elapsed_time > 2)
    {
      this.pv.RED_DO_2=true;
      this.pv.YELLOW_DO_2=true;
      this.pv.GREEN_DO_2=true;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 6)
  {
    if (elapsed_time > 2)
    {
      this.pv.HYDRAULIC_2A = false;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 7)
  {
    if (elapsed_time > 1)
    {
      this.pv.RED_DO_2=false;
      this.pv.YELLOW_DO_2=false;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 8)
  {
    if (elapsed_time > 5)
    {
      this.pv.HYDRAULIC_2B = true;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 9)
  {
    if (elapsed_time > 5)
    {
      this.pv.HYDRAULIC_2B = false;
      this.filter_2_state++;
      this.filter_2_time=now;
    }
  }
  else if (this.filter_2_state == 10)
  {
    if (elapsed_time > 4)
    {
      this.motor_on=false;
      this.filter_2_time=now;
      this.filter_2_state=0;
    }
  }
}
sim_object_t.prototype.state_machine_filter_2=sim_sm2_f;

function next_value(tau, stead_state_value, last_value, delta_t)
{
  var fraction = 1.0 - Math.exp(-delta_t/tau);
  return last_value + ((steady_state_value - last_value) * fraction);
}

function sim_object_update_f(now)
{
  var elapsed_time = now - this.last_time;
  this.last_time = now;

  if (elapsed_time < 0.05) return;

  this.fill_rate = 0 ;
  this.temperature_rate = 0 ;

  if (this.pv.VALVE_1)
  {
    this.fill_rate += COLD_WATER_FILL_RATE;
    this.temperature_rate = VALVE_ON_TEMPERATURE_RATE;
  }
  else
  {
    this.temperature_rate = VALVE_OFF_TEMPERATURE_RATE;
  }

  if (this.pv.VALVE_2)
  {
    this.fill_rate += PUMP_DRAIN_RATE;
  }

  this.tank_level += this.fill_rate * elapsed_time;
  this.tank_temperature += this.temperature_rate * elapsed_time;


  this.pv.HI_LEVEL = this.tank_level > TANK_HI_LEVEL_CM;
  this.pv.LO_LEVEL = this.tank_level > TANK_LO_LEVEL_CM;


  this.pv.TEMP_1 = this.tank_temperature;
  this.pv.LEVEL = this.tank_level;

  if (this.pv.VALVE_1)
  {
    //console.log("VALVE_1 ON");
    if (this.tank_temperature <= 52.0)
    {
      console.log("Toggle VALVE_1 OFF:");
      this.pv.VALVE_1=false;
    }
  }
  else
  {
    //console.log("VALVE_1 OFF:");
    if (this.tank_temperature >= 58.0)
    {
      console.log("Toggle VALVE_1 ON:");
      this.pv.VALVE_1=true;
    }
  }

  if (this.pv.VALVE_2)
  {
    if (this.tank_level < (TANK_LO_LEVEL_CM-0.5))
    {
      console.log("Toggle VALVE_2 OFF:");
      this.pv.VALVE_2=false;
    }
  }
  else
  {
    if (this.tank_level > (TANK_HI_LEVEL_CM + 0.5))
    {
      console.log("Toggle VALVE_2 ON:");
      this.pv.VALVE_2=true;
    }
  }


  this.state_machine_filter_1(now);
  this.state_machine_filter_2(now);

  this.pv.MOTOR_ON = this.motor_on;
}
sim_object_t.prototype.update=sim_object_update_f;

sim = new sim_object_t;

