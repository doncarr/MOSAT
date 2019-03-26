#define HUMIDITY_ONE 1  //PF1   Honeywell humidity
#define HUMIDITY_TWO 3  //PF3   GEneral humidity
#define TEMPERATURE 2   //PF2   General temperature


/*Virtuales*/

#define HOUR_ONE 16             //Channel to store the starting hour and minute of the time range, expressed in minutes
#define TIME_SPAN_ONE 17        //Channel to store duration of the time range, expressed in minutes
#define HOUR_TWO 18
#define TIME_SPAN_TWO 19
#define HOUR_THREE 20
#define TIME_SPAN_THREE 21
#define HOUR_FOUR 22
#define TIME_SPAN_FOUR 23

#define HUMIDITY_ONE_LIMIT_H 24 //Channel to store the  high level of humidity
#define HUMIDITY_ONE_LIMIT_L 25 //Channel to store the  low level of humidity
#define HUMIDITY_TWO_LIMIT_H 26
#define HUMIDITY_TWO_LIMIT_L 27
#define HUMIDITY_THREE_LIMIT_H 28
#define HUMIDITY_THREE_LIMIT_L 29
#define TIME_BTW_FANS 30      //Channel to store the Number of seconds between each fan when they turn on
#define MIN_TIME_ON 31          //Channel to store the Minimum number of minutes that the fans must be on when hey are turn on
//#define HUMIDITY_FOUR_LIMIT_H 30
//#define HUMIDITY_FOUR_LIMIT_L 31
