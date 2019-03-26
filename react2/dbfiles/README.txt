Mandatory files:
driver.dat - All the drivers that will be used

Line Structure:

React2 driver so file 
Function name that loads the driver
Auxiliary so file that may require the driver
Parameter(s) to that driver 

Content Example:
../../drivers/sim_plastic/libsimplasticdrv.so|new_simplasticdrv|||

config.dat - Configuration values
LogHome - Root directory for logfiles
HtmlHome - Root directory for the output html files only if web.dat exists or has any lines in it.
SampleRate - Samples per second to aquire 

Content Example:
LogHome|./log/
HtmlHome|./html/
TemplateHome|./template/
SampleRate|1|




Optional:
ac.dat - Air conditioning 
ai.dat - Analog input
ao.dat - Analog output
background.dat - Background scripts to run at startup
calc.dat - Analog calculations
Dallas1Wire.dat - Dallas 1 Wire configuration 
data.dat - Data logging configuration
dcalc.dat - To convert discrete points of integers 
di.dat - Discrete inputs
discrete_calc.dat - discrete calculations (true/false)
discrete_logger.dat - discrete logger (logs all changes from true to flase and viceversa)
do.dat - discrete output
file_logger.dat - To log discrete analog values to a file
fuzzy.dat - To start fuzzy logic - draft state 
int.dat - Integer input, counts etc
level.dat - Keep track of the total cubic meters of water pump, etc
pci9111.dat - The configuration for pluggin data accqusition card 
pci.dat - Pulse Count Input 
pendulum.dat - Draft implementation of a pendulum control 
pid.dat - Proportional Integral Derivative Control Algorithm 
pump.dat - keep track of electricity used for pumps
scan.dat - Data logging to a file one scan at a time via script command
web.dat - Generate html user interface
timer.dat - Count up / Count down timers
analog_value.dat - Stores an analog value 
discrete_value.dat - Stores a discrete value
remote_pid.dat - To control the setpoints for remote PID and monitor 
web_logger.dat - Prototipe to send log values to a remote web server


All of these files also have .config which describes the sctructure field names, database field names, human readable descriptions for the prompt, data types of each attribute.

These will be all use for generate the database read/write functions, read/write from a flat file, user interface, etc.


CONFIG LINE TYPES:
table  
separator
bool
double
dropdown
int
string
