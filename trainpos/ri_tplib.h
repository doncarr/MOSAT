/************************************************************************
This is software to monitor and/or control trains.
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#ifndef RI_TPLIB_H
#define RI_TPLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <new>
#include "ri_adtlib.h"
#include "rtcommon.h"

#define MIN_TTT 50
#define MIN_NUM_OF_SENSORS 2
#define MIN_NUM_OF_STATIONS 2
#define MIN_NUM_OF_DEPARTURES 1
#define ROOM_FOR_NULL_CHAR  1
#define MAX_INDENT_LENGTH 0xFF
#define MIN_INDENT_LENGTH 0

using namespace std;
using namespace igrl;

enum TPRC_ENUM { TPRC_OS, TPRC_FOF, TPRC_FCF, TPRC_FRF, TPRC_FFF, TPRC_NFP, TPRC_WFS, TPRC_WCF,       // <-- VERY IMPORTANT!: Never change nor add new return codes to this enum unless you do the
                 TPRC_WTTT,TPRC_WTTS,TPRC_WTC, TPRC_NESE, TPRC_NEST, TPRC_NEDE, TPRC_OOM,             //                      appropriate changes to g_TPRC_Descriptions, at ri_tplib.cpp. Go to the
                 TPRC_WTPRC };                                                                        //                      implementation of getTPRCDesc to see what I mean.

enum SENSOR_STATE_ENUM { CROSSED, NOT_CROSSED };

enum FILE_INDENT_ENUM { INDENT };

////////////////////////////////////////////////////////////////////////////////////////////////////
// parseTimeString function declaration                                                           //
//                                                                                                //
// Description: Parses a string and determines if it's a valid time, in format HH:mm:ss           //
// Parameters: szWord[in].- The null-terminated string to parse.                                  //
// Return value: True if the szWord is a valid time and false if it isn't.                        //
////////////////////////////////////////////////////////////////////////////////////////////////////

bool parseTimeString(const char* szWord);

////////////////////////////////////////////////////////////////////////////////////////////////////
// isNumericData function declaration                                                             //
//                                                                                                //
// Description: Determines if a string contains only digits.                                      //
// Parameters: szData[in].- The string to analyze.                                                //
// Return value: True if the string contains only digits; false otherwise.                        //
////////////////////////////////////////////////////////////////////////////////////////////////////

bool isNumericData(const char* szData);

////////////////////////////////////////////////////////////////////////////////////////////////////
// getTPRCDesc function declaration                                                               //
//                                                                                                //
// Description: Takes a trainpos return code and returns its description.                         //
// Parameters: rCode[in].- The trainpos return code to translate.                                 //
// Return value: If rCode is a valid TPRC_ENUM value, its corresponding description is returned. A//
//               string telling the provided TPRC_ENUM value is invalid is returned otherwise.    //
////////////////////////////////////////////////////////////////////////////////////////////////////

const char* getTPRCDesc(TPRC_ENUM rCode);

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_str class declaration                                                                       //
//                                                                                                //
// Description: A class for manipulating char strings.                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////

class ri_str
{
  char m_nullChar;
  char* m_szData;
  unsigned m_dLength;
  public:
  ri_str();
  ri_str(const char* szData);
  ri_str(const ri_str& sObj);
  ~ri_str();
  ri_str& operator=(const char* szROp);
  ri_str& operator=(const ri_str& rOp);
  ri_str& operator+=(const char* szROp);
  ri_str& operator+=(const ri_str& rOp) { return operator+=(rOp.m_szData); };
  unsigned length() { return m_dLength; };
  operator const char*() const { return m_szData; };
  bool operator==(const char* szROp) const { return (!szROp || strcmp(m_szData, szROp)) ? false : true; };
  bool operator==(const ri_str& rOp) const { return strcmp(m_szData, rOp.m_szData) ? false : true; };
  const ri_str& operator+(const char* szROp) const;
  const ri_str& operator+(const ri_str& rOp) const;
  friend const ri_str& operator+(const char* szlOp, const ri_str& rOp);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_fileWriter class declaration                                                                //
//                                                                                                //
// Description: A very simple class for creating files and putting data on them.                  //
////////////////////////////////////////////////////////////////////////////////////////////////////

class ri_fileWriter
{
  unsigned m_indentLength;
  FILE* m_pFile;
  public:
  ri_fileWriter();
  ~ri_fileWriter();
  TPRC_ENUM open(const char* szPath);
  TPRC_ENUM close();
  TPRC_ENUM flush() const;
  void increaseIndent(unsigned length = 1);
  void decreaseIndent(unsigned length = 1);
  const ri_fileWriter& operator<<(FILE_INDENT_ENUM rOp) const;
  const ri_fileWriter& operator<<(int rOp) const;
  const ri_fileWriter& operator<<(unsigned rOp) const;
  const ri_fileWriter& operator<<(float rOp) const;
  const ri_fileWriter& operator<<(char rOp) const;
  const ri_fileWriter& operator<<(const char* szROp) const;
  const ri_fileWriter& operator<<(const ri_str& rOp) const;
  const ri_fileWriter& operator<<(const time_t rOp) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_timeTableItem class declaration                                                             //
//                                                                                                //
// Description: A class for representing the items of the time table.                             //
////////////////////////////////////////////////////////////////////////////////////////////////////

class ri_timeTableRow
{
  time_t m_dtest;
  public:
  const ri_str m_trainId;
  ri_timeTableRow(const char* szTrainId, const char* szHour, const char* szMin, const char* szSec);
  time_t dtest() const { return m_dtest; };
};

struct ri_trainData
{
  unsigned m_secNum;
  float m_cPos;
  float m_mPos;
  int m_delay;
  time_t m_dtest;
  time_t m_est;
  time_t m_dt;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_sensor class declation                                                                    //
//                                                                                                //
// Description: A very simple class for representing the sensors of a railway section. It provides//
//              information such as the expected time required to get to this sensor and the state//
//              of if, while there is a train is the section (CROSSED or NOT_CROSSED).            //
////////////////////////////////////////////////////////////////////////////////////////////////////

class ri_sensor
{
  SENSOR_STATE_ENUM m_state;
  public:
  const unsigned m_relTTS;
  const unsigned m_absTTS;
  ri_sensor(unsigned relTTS, unsigned absTTS);
  void changeState(SENSOR_STATE_ENUM state) { m_state = state; };
  SENSOR_STATE_ENUM state() const { return m_state; };
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_rwSection class declaration                                                                 //
//                                                                                                //
// Description: It represents a railway section between two train stations. It stores and provides//
//              information such as time to travel this section, the number of sensors in this    //
//              section, the time it takes to get to every sensor. It also informs if there is a  //
//              train going through it and an approximate position of it.                         //
////////////////////////////////////////////////////////////////////////////////////////////////////

class ri_rwSection
{
  LinkedList<ri_sensor> m_sensors;                                                                  // <-- A list with the times it takes to get to every sensor, starting with sensor 0, 1 and so on.
  ri_trainData m_trainData;
  const unsigned m_tis;
  const unsigned m_ttt;
  ri_str m_name;
  bool m_isBusy;
  float m_dps;
  public:
  ri_rwSection(const ri_str& name, const LinkedList<ri_sensor>& sensors, unsigned ttt, unsigned tis);
  const ri_trainData& trainData() const { return m_trainData; };
  const ri_str& name() const { return m_name; };
  bool isBusy() const { return m_isBusy; };
  LinkedList<ri_sensor>& sensors() { return m_sensors; };
  void updatePos(time_t ct);
  void procTCS(unsigned sensor, time_t ct);
  void procTAS(ri_rwSection& prevSection, time_t at);
  void procTES(unsigned secNum, time_t dtest, time_t ct);
  void procTLS();
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_cf_reader class declaration                                                                 //
//                                                                                                //
// Description: A class for reading the trainpos configuration files.                             //
////////////////////////////////////////////////////////////////////////////////////////////////////

class ri_cfReader
{
  ri_fileWriter& m_Logger;
  char* m_szFileContent;
  ri_str m_displayPath;
  ri_str m_sectionsPath;
  ri_str m_timeTablePath;
  TPRC_ENUM loadConfigFile(const char* szPath);
  public:
  ri_cfReader(ri_fileWriter& logger);
  ~ri_cfReader();
  TPRC_ENUM readTPConfig(const char* szPath);
  TPRC_ENUM readSections(LinkedList<ri_rwSection>& listOfSections);
  TPRC_ENUM readTimeTable(LinkedList<ri_timeTableRow>& timeTable);
  TPRC_ENUM readDisplay();
};

#endif
