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
    "LEVEL":0.50,
    "LEVEL1":19253.0,
    "LEVEL2":0.0,
    "PVALVE":0.10,
    "PUMP1_AMP":0,
    "PUMP2_AMP":0,
    "PUMP3_AMP":0,
    "PUMP1_ON":true,
    "PUMP2_ON":true,
    "PUMP3_ON":true,
    "HI_HI_LEVEL":false,
    "HI_LEVEL":false,
    "LO_LEVEL":true,
    "KW":0,
    "KWH":134528.291,
    "KWHM":104.823,
    "SPRAY_PUMP_ON":false,
    "PVALVE_ON":false,
    "XFER_ON":false,
    "PRESSURE":0.0,
    "SLIDER":10.0
  }

  this.cfg =
  {
    "LEVEL":{"tag":"LEVEL","description":"NIVEL","eu":"cm","decimal_places":0,"lo_alarm":15.0,"lo_caution":44.0,"hi_caution":300.0,"hi_alarm":380.0,"lo_alarm_enable":true,"lo_caution_enable":true,"hi_caution_enable":true,"hi_alarm_enable":true, "scale_lo":0,"scale_hi":450},
    "LEVEL1":{"tag":"LEVEL1","description":"Level 1","eu":"liters","decimal_places":0,"zero_cutoff":-125,"lo_alarm":15.0,"lo_caution":44.0,"hi_caution":300.0,"hi_alarm":380.0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false, "scale_lo":0,"scale_hi":20000},
    "LEVEL2":{"tag":"LEVEL2","description":"Level 1","eu":"liters","decimal_places":0,"zero_cutoff":-125,"lo_alarm":15.0,"lo_caution":44.0,"hi_caution":300.0,"hi_alarm":380.0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false, "scale_lo":0,"scale_hi":20000},
    "PRESSURE":{"tag":"PRESSURE","description":"Pressure","eu":"psi","decimal_places":0,"zero_cutoff":-5000,"lo_alarm":21,"lo_caution":23,"hi_caution":28,"hi_alarm":30,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false,"scale_lo":0, "scale_hi":175},
    "PVALVE":{"tag":"PVALVE","description":"Proportion Valve","eu":"","decimal_places":2,"zero_cutoff":-5000,"output_limit_hi":1,"output_limit_lo":0,"scale_lo":0, "scale_hi":1},
    "KW":{"tag":"KW","description":"KW","eu":"kW","decimal_places":1,"zero_cutoff":-125,"lo_alarm":15.0,"lo_caution":44.0,"hi_caution":80.0,"hi_alarm":90.0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":true,"hi_alarm_enable":false, "scale_lo":0,"scale_hi":100},
    "KWH":{"tag":"KWH","description":"KWH","eu":"kWh","decimal_places":5,"zero_cutoff":-125,"lo_alarm":15.0,"lo_caution":44.0,"hi_caution":80.0,"hi_alarm":90.0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false, "scale_lo":0,"scale_hi":10000000},
    "KWHM":{"tag":"KWHM","description":"KWHM","eu":"kWh","decimal_places":5,"zero_cutoff":-125,"lo_alarm":15.0,"lo_caution":44.0,"hi_caution":80.0,"hi_alarm":90.0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false, "scale_lo":0,"scale_hi":10000000},
    "PUMP1_AMP":{"tag":"PUMP1_AMP","description":"BOMBA 1","eu":"amps","decimal_places":1,"zero_cutoff":-12.5,"lo_alarm":0,"lo_caution":0,"hi_caution":30,"hi_alarm":35,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":true,"hi_alarm_enable":true,"scale_lo":0,"scale_hi":40},
    "PUMP2_AMP":{"tag":"PUMP2_AMP","description":"BOMBA 2","eu":"amps","decimal_places":1,"zero_cutoff":-12.5,"lo_alarm":0,"lo_caution":0,"hi_caution":30,"hi_alarm":35,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":true,"hi_alarm_enable":true,"scale_lo":0,"scale_hi":40},
    "SLIDER":{"tag":"SLIDER","description":"Slider Value","eu":"%","decimal_places":1,"zero_cutoff":-100.0,"lo_alarm":0.0,"lo_caution":0.0,"hi_caution":0.0,"hi_alarm":0.0,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":false,"hi_alarm_enable":false,"scale_lo":0,"scale_hi":100},
    "PUMP3_AMP":{"tag":"PUMP3_AMP","description":"BOMBA 3","eu":"amps","decimal_places":1,"zero_cutoff":-12.5,"lo_alarm":0,"lo_caution":0,"hi_caution":30,"hi_alarm":35,"lo_alarm_enable":false,"lo_caution_enable":false,"hi_caution_enable":true,"hi_alarm_enable":true,"scale_lo":0,"scale_hi":40},
    "PUMP1_ON": {"tag":"PUMP1_ON","description":"Bomba 1 ON","lo_desc":"On","hi_desc":"Off", "invert_pv":false},
    "PUMP2_ON":{"tag":"PUMP2_ON","description":"Bomba 1 ON","lo_desc":"On","hi_desc":"Off", "invert_pv":false},
    "PUMP3_ON":{"tag":"PUMP3_ON","description":"Bomba 1 ON","lo_desc":"On","hi_desc":"Off", "invert_pv":false},
    "COV_810_A":{"tag":"COV_810_A","description":"COV-810-A","lo_desc":"to T811","hi_desc":"to T810", "invert_pv":false},
    "SPRAY_PUMP_ON":{"tag":"SPRAY_PUMP_ON","description":"Spray Pump ON","lo_desc":"Off","hi_desc":"On","invert_pv":false},
    "PVALVE_ON":{"tag":"PVALVE_ON","description":"Proportional Valve ON","lo_desc":"25","hi_desc":"75","invert_pv":false},
    "XFER_ON":{"tag":"XFER_ON","description":"Transfer ON","lo_desc":"Stop","hi_desc":"Start","invert_pv":false},
    "HI_HI_LEVEL":{"tag":"HI_HI_LEVEL","description":"Nivel Muy Alto","lo_desc":"BELOW","hi_desc":"ABOVE","invert_pv":false},
    "HI_LEVEL":{"tag":"HI_LEVEL","description":"Nivel Alto","lo_desc":"BELOW","hi_desc":"ABOVE","invert_pv":false},
    "LO_LEVEL":{"tag":"LO_LEVEL","description":"Nivel Bajo","lo_desc":"BELOW","hi_desc":"ABOVE","invert_pv":false},
  }
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
      if (this.up)
      {
        if (this.pv.LEVEL > (this.lo_level+2)) this.pv.LO_LEVEL=true;
        if (this.pv.LEVEL > (this.hi_level+1))
        { 
          this.up=false;
          this.pv.HI_LEVEL=true;
          setTimeout("pump_1_timeout()", 1000);
          setTimeout("pump_2_timeout()", 4000);
          setTimeout("pump_3_timeout()", 7000);
        }
        if (this.pv.LEVEL > (this.hi_hi_level+2)) this.pv.HI_HI_LEVEL=true;
      }
      else
      {
        if (this.pv.LEVEL < (this.lo_level-1)) 
        {
          this.up=true;
          this.pv.LO_LEVEL=false;
          setTimeout("pump_3_timeout()", 1000);
          setTimeout("pump_2_timeout()", 4000);
          setTimeout("pump_1_timeout()", 7000);
        }
        if (this.pv.LEVEL < (this.hi_level-3)) this.pv.HI_LEVEL=false;
        if (this.pv.LEVEL < (this.hi_hi_level-3)) this.pv.HI_HI_LEVEL=false;
      }
      this.rate=10.0;
      if (!this.pv.PUMP1_ON) 
      {
        this.rate -= 6.0;
        this.pv.PUMP1_AMP= (27 +((Math.random()-0.5)*0.6));
      }
      else
      {
        this.pv.PUMP1_AMP= (0 +((Math.random()-0.5)*0.6));
      }

      if (!this.pv.PUMP2_ON) 
      {
        this.rate -= 6.0;
        this.pv.PUMP2_AMP= (27 +((Math.random()-0.5)*0.6));
      }
      else
      {
        this.pv.PUMP2_AMP= (0 +((Math.random()-0.5)*0.6));
      }

      if (!this.pv.PUMP3_ON)
      {
        this.rate -= 6.0;
        this.pv.PUMP3_AMP = (27 +((Math.random()-0.5)*0.6));
      }
      else
      {
        this.pv.PUMP3_AMP = (0 +((Math.random()-0.5)*0.6));
      }
      this.rate = this.rate * 1.5;
      this.pv.LEVEL +=this.rate*0.1;
      //this.pv.LEVEL = ((this.pv.LEVEL+((Math.random()-0.5)*0.8)));
      this.pv.KW = this.pv.LEVEL/3.2;
      this.pv.KWH += this.pv.KW / (3600 * 10); 
      this.pv.KWHM += this.pv.KW / (3600 * 10); 
      if (this.pv.SPRAY_PUMP_ON) 
      {
        this.pv.PRESSURE = next_value(1, 162.5 * this.pv.PVALVE, this.pv.PRESSURE, now - this.last_time)
      }
      else
      {
        this.pv.PRESSURE = next_value(0.5, 0, this.pv.PRESSURE, now - this.last_time)
      }
      //console.log("PRESSURE: " + this.pv.PRESSURE)
      /**
      if (this.pv.PVALVE_ON)
      {
        this.pv.PVALVE = 0.75; 
      }
      else
      {
        this.pv.PVALVE = 0.25; 
      }
      **/
      if (this.pv.XFER_ON) 
      {
        var liters = (now - this.last_time) * 22;
        if (this.pv.LEVEL1 < 100)
        {
          this.pv.XFER_ON = false; // Stop the transfer
        }
        else
        {
          this.pv.LEVEL1 -= liters;
          this.pv.LEVEL2 += liters;
        }
      }
      this.last_time = now; 
    }
    sim_object_t.prototype.update=sim_object_update_f;

sim = new sim_object_t;

