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

#include "ri_tplib.h"

const char* g_TPRC_Descriptions[] =
  {
    "Operation successful", "FOPEN failed", "FCLOSE failed", "FREAD failed",
    "FFLUSH failed", "NULL FILE pointer", "Wrong file size", "Wrong configuration file",
    "Wrong Time-To-Travel", "Wrong Time-To-Sensor", "Wrong time column", "Not enough sensors",
    "Not enough stations", "Not enough departures", "Out of memory", "Invalid trainpos return code!"
  };

const unsigned g_TimeParserTransitionFunction [256][11] =
  { { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 1,3,3,5,6,5,7,5,9,10,5}, { 1,3,3,5,6,5,7,5,9,10,5 },
    { 2,3,3,5,6,5,7,5,9,10,5 }, { 3,3,3,5,6,5,7,5,9,10,5 },
    { 3,3,5,5,6,5,7,5,9,10,5 }, { 3,3,5,5,6,5,7,5,9,10,5 },
    { 3,3,5,5,5,5,7,5,5,10,5 }, { 3,3,5,5,5,5,7,5,5,10,5 },
    { 3,3,5,5,5,5,7,5,5,10,5 }, { 3,3,5,5,5,5,7,5,5,10,5 },
    { 5,4,4,4,5,5,8,8,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },
    { 5,5,5,5,5,5,5,5,5,5,5 }, { 5,5,5,5,5,5,5,5,5,5,5 },};

////////////////////////////////////////////////////////////////////////////////////////////////////
// parseTimeString function implementation                                                        //
//                                                                                                //
// Description: Parses a string and determines if it's a valid time, in format HH:mm:ss           //
// Parameters: szWord[in].- The string to parse. It must be a valid null-terminated string.       //
// Return value: True if the szWord is a valid time string. False otherwise.                      //
////////////////////////////////////////////////////////////////////////////////////////////////////

bool parseTimeString(const char* szWord)
{
  unsigned currentState = 0;
  for (unsigned ss = 0; szWord[ss]; ss++)
    currentState = g_TimeParserTransitionFunction[(unsigned)szWord[ss]][currentState];
  return 9 == currentState || 10 == currentState ? true : false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// isNumericData function implementation                                                          //
//                                                                                                //
// Description: Determines if a string contains only digits.                                      //
// Parameters: szData[in].- The string to analyze. It MUST be a valid null-terminated string      //
// Return value: True if the string contains only digits. False otherwise.                        //
////////////////////////////////////////////////////////////////////////////////////////////////////

bool isNumericData(const char* szData)
{
  for (unsigned ss = 0; szData[ss]; ss++)
    if (szData[ss] < 48 || szData[ss] > 57)
      return false;
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// getTPRCDesc function implementation                                                            //
//                                                                                                //
// Description: Takes a trainpos return code and returns its description.                         //
// Parameters: rCode[in].- The trainpos return code to translate.                                 //
// Return value: If rCode is a valid TPRC_ENUM value, its corresponding description is returned. A//
//               string telling the provided TPRC_ENUM value is invalid is returned otherwise.    //
////////////////////////////////////////////////////////////////////////////////////////////////////

const char* getTPRCDesc(TPRC_ENUM rCode)
{
  return g_TPRC_Descriptions[rCode < TPRC_OS || rCode >= TPRC_WTPRC ? TPRC_WTPRC : rCode];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_str class implementaion                                                                     //
//                                                                                                //
// Description: A very simple class for manipulation of char strings.                             //
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_str()                                                                               //
// Description: Default constructor.                                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_str::ri_str(): m_nullChar('\0'), m_szData(&m_nullChar), m_dLength(0)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_str(const char* szData)                                                             //
// Description: Constructor.                                                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_str::ri_str(const char* szData): m_nullChar('\0')
{
  m_dLength = szData ? strlen(szData) : 0;
  if (0 == m_dLength)
    m_szData = &m_nullChar;
  else
  {
    try
    {
      m_szData = new char[m_dLength + ROOM_FOR_NULL_CHAR];
    }
    catch (bad_alloc&)                                                                                  // <-- Here we partially handle the bad_alloc exception in order to reset m_dLength;
    {
      m_dLength = 0;
      m_szData = &m_nullChar;
      throw;
    }
    safe_strcpy(m_szData, szData, m_dLength + ROOM_FOR_NULL_CHAR);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_str(const ri_str& sObj)                                                             //
// Description: Copy constructor.                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_str::ri_str(const ri_str& sObj): m_nullChar('\0')
{
  m_dLength = sObj.m_dLength;
  if (0 == m_dLength)
    m_szData = &m_nullChar;
  else
  {
    try
    {
      m_szData = new char[m_dLength + ROOM_FOR_NULL_CHAR];
    }
    catch (bad_alloc&)
    {
      m_dLength = 0;
      m_szData = &m_nullChar;
      throw;
    }
    safe_strcpy(m_szData, sObj.m_szData, m_dLength + ROOM_FOR_NULL_CHAR);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ~ri_str()                                                                              //
// Description: Destructor.                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_str::~ri_str()
{
  if (m_szData != &m_nullChar)
    delete[] m_szData;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: operator=(const char* szROp)                                                           //
// Description: Asignment operator. Asigns the content of a standar c string to ri_str object.    //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_str& ri_str::operator=(const char* szROp)
{
  if (m_szData != &m_nullChar)
    delete[] m_szData;
  m_dLength = szROp ? strlen(szROp) : 0;
  if (0 == m_dLength)
    m_szData = &m_nullChar;
  else
  {
    try
    {
      m_szData = new char[m_dLength + ROOM_FOR_NULL_CHAR];
    }
    catch(bad_alloc&)
    {
      m_dLength = 0;
      m_szData = &m_nullChar;
      throw;
    }
    safe_strcpy(m_szData, szROp, m_dLength + ROOM_FOR_NULL_CHAR);
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: operator=(const ri_str& rOp)                                                           //
// Description: Asignment operator. Asigns the content of a ri_str object to another.             //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_str& ri_str::operator=(const ri_str& rOp)
{
  if (this == &rOp)                                                                                   // <-- A validation against self asignment.
    return *this;
  return operator=(rOp.m_szData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: operator+=(const char* szROp)                                                          //
// Description: Addition-Asignment operator. Appends szROp to the end of this object's content.   //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_str& ri_str::operator+=(const char* szROp)
{
  unsigned rOpLength;
  if (NULL == szROp || 0 == (rOpLength = strlen(szROp)))                                            // <-- If this condition is true...
    return *this;                                                                                   //     there is nothing to do but returning *this.
  ri_str thisObj(*this);                                                                       // <-- At this point we are sure the length of the string m_szData points to is going to change. We
  if (m_szData != &m_nullChar)                                                                      //     have to start creating a copy of *this so we can procede with the update of m_szData.
    delete[] m_szData;
  m_dLength = thisObj.m_dLength + rOpLength;
  try
  {
    m_szData = new char[m_dLength + ROOM_FOR_NULL_CHAR];
  }
  catch(bad_alloc&)
  {
    m_dLength = 0;
    m_szData = &m_nullChar;
    throw;
  }
  safe_strcpy(m_szData, thisObj.m_szData, thisObj.m_dLength + ROOM_FOR_NULL_CHAR);
  safe_strcat(m_szData, szROp, m_dLength + ROOM_FOR_NULL_CHAR);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_fileWriter class implementation                                                             //
//                                                                                                //
// Description: A very simple class for creating and putting data on them.                        //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_fileWriter::ri_fileWriter(): m_indentLength(0), m_pFile(NULL)
{
}

ri_fileWriter::~ri_fileWriter()
{
  close();
}

TPRC_ENUM ri_fileWriter::open(const char* szPath)
{
  m_pFile = fopen(szPath, "w");
  return NULL != m_pFile ? TPRC_OS : TPRC_FOF;
}

TPRC_ENUM ri_fileWriter::close()
{
  if (NULL == m_pFile)
    return TPRC_NFP;
  if (0 != fclose(m_pFile))
    return TPRC_FCF; 
  m_pFile = NULL;
  return TPRC_OS;
}

TPRC_ENUM ri_fileWriter::flush() const
{
  return NULL == m_pFile ? TPRC_NFP : 0 != fflush(m_pFile) ? TPRC_FFF : TPRC_OS;
}

void ri_fileWriter::increaseIndent(unsigned length)
{
  m_indentLength = m_indentLength + length < MAX_INDENT_LENGTH ? m_indentLength + length : MAX_INDENT_LENGTH;
}

void ri_fileWriter::decreaseIndent(unsigned length)
{
  m_indentLength = length <= m_indentLength ? m_indentLength - length : MIN_INDENT_LENGTH;
}

const ri_fileWriter& ri_fileWriter::operator<<(FILE_INDENT_ENUM rOp) const
{
  unsigned ss = 0;
  char szIndent[MAX_INDENT_LENGTH + ROOM_FOR_NULL_CHAR];
  for (; ss < m_indentLength; ss++)
    szIndent[ss] = '\t';
  szIndent[ss] = '\0';
  fprintf(m_pFile, "%s", szIndent);
  return *this; 
}

const ri_fileWriter& ri_fileWriter::operator<<(int rOp) const
{
  fprintf(m_pFile, "%d", rOp);
  return *this;
}

const ri_fileWriter& ri_fileWriter::operator<<(unsigned rOp) const
{
  fprintf(m_pFile, "%u", rOp);
  return *this;
}

const ri_fileWriter& ri_fileWriter::operator<<(float rOp) const
{
  fprintf(m_pFile, "%f", rOp);
  return *this;
}

const ri_fileWriter& ri_fileWriter::operator<<(char rOp) const
{
  fprintf(m_pFile, "%c", rOp);
  return *this;
}

const ri_fileWriter& ri_fileWriter::operator<<(const char* szROp) const
{
  fprintf(m_pFile, "%s", szROp);
  return *this;
}

const ri_fileWriter& ri_fileWriter::operator<<(const ri_str& rOp) const
{
  fprintf(m_pFile, "%s", (const char*)rOp);
  return *this;
}

const ri_fileWriter& ri_fileWriter::operator<<(const time_t rOp) const
{
  tm ts;
  char szTime[9];
  localtime_r(&rOp, &ts);
  strftime(szTime, sizeof(szTime), "%T", &ts);
  fprintf(m_pFile, "%s", szTime);
  return *this;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_timeTableRow class implementation                                                           //
//                                                                                                //
// Description: A class for representing the rows of the time table.                              //
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_timeTableRow::ri_timeTableRow                                                       //
// Purpose: Default constructor.                                                                  //
// Parameters: szTrainId[in].- Train id                                                           //
//             szHour[in].- Hour component of the due-to-enter-service time. It MUST be a valid   //
//                          value (0 to 23).                                                      //
//             szMin[in].- Minute component of the due-to-enter-service time. It MUST be a valid  //
//                         value (0 to 59).                                                       //
//             szSec[in].- Second component of the due-to-enter-service time. It MUST be a valid  //
//                         value (0 to 59).                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_timeTableRow::ri_timeTableRow(const char* szTrainId, const char* szHour, const char* szMin,
  const char* szSec): m_trainId(szTrainId)
{
  tm bdTime;
  time(&m_dtest);
  localtime_r(&m_dtest, &bdTime);
  bdTime.tm_hour = atoi(szHour);
  bdTime.tm_min = atoi(szMin);
  bdTime.tm_sec = atoi(szSec);
  m_dtest = mktime(&bdTime);
 }

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_sensor class implementation                                                                 //
//                                                                                                //
// Description: A very simple class for representing the sensors of a railway section. It provides//
// information such as the expected time required to get to this sensor and the state of if, while//
// there is a train is the section (CROSSED or NOT_CROSSED).                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_sensor:ri_sensor                                                                    //
// Purpose: Constructor of this class. It simply initializes m_timeToTravel, which is a const data//
//          member because this value is not supposed to change throughout the existense of this  //
//          objects.                                                                              //
// Parameters: relTTS[in].- The time it takes to get to this sensor from the train station  //
//                                at the beginning of the section.                                //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_sensor::ri_sensor(unsigned relTTS, unsigned absTTS): m_relTTS(relTTS), m_absTTS(absTTS)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_sensor:changeState(...)                                                             //
// Purpose: It simply changes the state of the sensor to tell if the train that is currently in   //
//          the section to which this sensor belongs, has crossed this sensor.                    //
// Parameters: state[in].- The state of the sensor. The state of all the sensor in a section must //
//                         be set to NOT_CROSSED as soon as a train arrives to the station at the //
//                         beginning of the section, and must be changed, one by one to CROSSED   //
//                         the corresponding crossing event is received. This help us to detect   //
//                         false crossing events or missing crossing events (is case a sensor is  //
//                         damaged.                                                               //
////////////////////////////////////////////////////////////////////////////////////////////////////

// inline void ri_sensor::changeState(SENSOR_STATE_ENUM state)
// {
//   m_state = state;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_sensor:state(...)                                                                   //
// Purpose: This method simply informs the state of the sensor. It important to understand that   //
//          this state is ONLY relevant when there is a train in the section to which this sensor //
//          belongs. It's assumend that ONLY one train can be in a section at a time.             //                                                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////

// inline SENSOR_STATE_ENUM ri_sensor::state() const
// {
//   return m_state;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_cfReader class implementation                                                               //
//                                                                                                //
// Description: A class for reading the trainpos configuration files.                             //
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_cfReader::ri_cfReader()                                                             //
// Purpose: Default constructor.                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_cfReader::ri_cfReader(ri_fileWriter& logger): m_Logger(logger), m_szFileContent(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_cfReader::~ri_cfReader()                                                            //
// Purpose: Destructor.                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_cfReader::~ri_cfReader()
{
  delete[] m_szFileContent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_cfReader::loadConfigFile(const char*)                                               //
// Purpose: This method opens a configuration file and loads its content into the m_szFileContent //
//          buffer, so that other methods, like readTPConfig(...) and readSections(...), can parse//
//          that content.                                                                         //
////////////////////////////////////////////////////////////////////////////////////////////////////

TPRC_ENUM ri_cfReader::loadConfigFile(const char* szPath)
{
  m_Logger << INDENT << "Entering ri_cfReader::loadConfigFile...\n";
  m_Logger.increaseIndent();
  unsigned long fSize;
  size_t nOfChars;
  TPRC_ENUM rCode;
  FILE* pConfigFile;
  delete[] m_szFileContent;                                                                           // <-- First we make sure any allocation made by a previous call to this method is deleted and...
  m_szFileContent = NULL;                                                                             //     m_szFileContent is nullified.
  m_Logger << INDENT << "Opening " << szPath << "...\n";
  pConfigFile = fopen(szPath, "rb");                                                                  // <-- Then we try to open the file as binary file with read-only access.
  rCode = NULL == pConfigFile ? TPRC_FOF : TPRC_OS;
  if (TPRC_OS == rCode)
  {
    fseek(pConfigFile, 0, SEEK_END);                                                                  // <-- Now we movo the position indicator to the end of the file in order to calculate the size
    fSize = ftell(pConfigFile);                                                                       //     of it.
    if (1 < fSize)
    {
      m_Logger << INDENT << "Loading file content into memory...\n";
      try
      {
        m_szFileContent = new char[fSize + ROOM_FOR_NULL_CHAR];                                       // <-- Now we allocate enough room to keep the entire file content in memory.
      }
      catch (bad_alloc&)                                                                              // <-- If the allocation fails...
      {                                                                                               //     we handle the exception, in order to log an appropiate error message and be able to let the
        m_Logger << INDENT << "ERROR!. System running out of memory. It's impossible to continue.\n";          //     caller know the system is running out of memory.
        rCode = TPRC_OOM;
      }
      if (NULL != m_szFileContent)
      {
        fseek(pConfigFile, 0, SEEK_SET);                                                              // <-- To read the data, first we have move back the position indicator to the beginning of the
        nOfChars = fread(m_szFileContent, 1, fSize, pConfigFile);                                     //     file.
        if (nOfChars == fSize)                                                                        // <-- The number of chars read from the file must be equal to m_fSize.
        {
          m_Logger << INDENT << "File loaded successfully.\n";
          m_szFileContent[fSize] = '\0';                                                              // <-- We need to null-terminate the buffer to use it as standard C string.
        }
        else
        {
          m_Logger << INDENT << "ERROR!. fread failed: " << strerror(ferror(pConfigFile)) << ".\n";
          rCode = TPRC_FRF;
        }
      }
    }
    else
    {
      m_Logger << INDENT << "ERROR!. Wrong file size detected.\n";
      rCode = TPRC_WFS;
    }
    m_Logger << INDENT << "Closing file...\n";
    fclose(pConfigFile);                                                                              // <-- Now we have to close the confugration file.
  }
  else
    m_Logger << INDENT << "ERROR!. fopen failed.\n";
  m_Logger.decreaseIndent();
  m_Logger << INDENT << "Exiting ri_cfReader::loadConfigFile.\n";
  return rCode;                                                                                       //     return TPRC_OS.
}

TPRC_ENUM ri_cfReader::readTPConfig(const char* szPath)
{
  m_Logger << INDENT << "Entering ri_cfReader::readTPConfig...\n";
  m_Logger.increaseIndent();
  bool sFound = false, tFound = false, dFound = false;
  char* szKey, * szValue, * szCurLine, * szNextLine;
  TPRC_ENUM rCode = loadConfigFile(szPath);                                                         // <-- Firlst we load the configuration file into memory.
  if (TPRC_OS == rCode)
  {
    rCode = TPRC_WCF;
    szCurLine = strtok_r(m_szFileContent, "\n", &szNextLine);                                         // <-- Now we preparte to tokenize the file content. First, we split the file into lines, using
    while (NULL != szCurLine)                                                                         //     the '\n' char as delimiter...
    {
      szKey = strtok_r(szCurLine, ",", &szValue);                                                     // <-- Now we split the current line using ',' as delimiter. Every line is splited into two parts:
      if (NULL != szValue)                                                                              //     the key and value.
      {
        if (strcmp(szKey, "SECTIONS_FILE") == 0 && !sFound)
        {
          m_Logger << INDENT << "SECTIONS_FILE: " << szValue << "\n";
          m_sectionsPath = szValue;
          sFound = true;
        }
        else if (strcmp(szKey, "TIMETABLE_FILE") == 0 && !tFound)
        {
          m_Logger << INDENT << "TIMETABLE_FILE: " << szValue << "\n";
          m_timeTablePath = szValue;
          tFound = true;
        }
        else if (strcmp(szKey, "DISPLAY_FILE") == 0 && !dFound)
        {
          m_Logger << INDENT << "DISPLAY_FILE: " << szValue << "\n";
          m_displayPath = szValue;
          dFound = true;
        }
      }
      if (sFound && tFound && dFound)
      {
        m_Logger << INDENT << "Configuration file successfully read.\n";
        rCode = TPRC_OS;
        break;
      }
      szCurLine = strtok_r(NULL, "\n", &szNextLine);
    }
    if (TPRC_OS != rCode)
      m_Logger << INDENT << "ERROR!. The configuration file is not valid.\n";
  }
  m_Logger.decreaseIndent();
  m_Logger << INDENT << "Exiting ri_cfReader::readTPConfig.\n";
  return rCode;
}

TPRC_ENUM ri_cfReader::readSections(LinkedList<ri_rwSection>& listOfSections)
{
  m_Logger << INDENT << "Entering ri_cfReader::readSections...\n";
  m_Logger.increaseIndent();
  char* szStationName, * szCurLine, * szNextLine, * szrelTTS, * szTimeToTravel, * szNextSensorCol;
  unsigned lineNum = 1, timeToTravel, relTTS, timeToPrevSensor = 0;
  LinkedList<ri_sensor> sensors;
  TPRC_ENUM rCode = loadConfigFile((const char*)m_sectionsPath);                                      // <-- We start by trying to load the sections file into memory.
  if (TPRC_OS == rCode)
  {
    m_Logger << INDENT << "Reading sections...\n";
    szCurLine =  strtok_r(m_szFileContent, "\n", &szNextLine);                                          // <-- Now we preparte to tokenize the file content. First, we split the file into lines, using
    while (NULL != szCurLine)                                                                           //     the '\n' char as delimeter.
    {
      szStationName = strtok_r(szCurLine, "\t", &szNextSensorCol);
      szTimeToTravel = strtok_r(NULL, "\t", &szNextSensorCol);
      if (!isNumericData(szTimeToTravel))
      {
        m_Logger << INDENT << "ERROR!. Time to travel expected at line " << lineNum << " \n";
        rCode = TPRC_WTTT;
        break;
      }
      timeToTravel = atoi(szTimeToTravel);
      if (MIN_TTT > timeToTravel)
      {
        m_Logger << INDENT << "ERROR!. Wrong time to travel detected at line " << lineNum << ": " << timeToTravel << " \n";
        rCode = TPRC_WTTT;
        break;
      }
      szrelTTS = strtok_r(NULL, "\t", &szNextSensorCol);
      while (NULL != szrelTTS)
      {
        if (!isNumericData(szrelTTS))
        {
          m_Logger << INDENT << "ERROR!. Time to sensor expected at line " << lineNum << ": " << szrelTTS << "\n";
          rCode = TPRC_WTTS;
          break;
        }
        relTTS = atoi(szrelTTS);
        if (relTTS > timeToTravel || relTTS <= timeToPrevSensor)
        {
          m_Logger << INDENT << "ERROR!. Wrong time to sensor detected at line " << lineNum << ": " << relTTS <<  "Previous sensor: " << timeToPrevSensor << "-" << szrelTTS <<"-\n";
          rCode = TPRC_WTTS;
          break;
        }
        unsigned absTTS;
        if (0 != sensors.Size())
          absTTS = relTTS - sensors.Retrieve(sensors.Size()-1).m_relTTS + sensors.Retrieve(sensors.Size()-1).m_absTTS;
        else if (0 == listOfSections.Size())
            absTTS = relTTS/*+ timeInStation*/;
        else
            absTTS = listOfSections.Retrieve(listOfSections.Size()-1).sensors().Retrieve(listOfSections.Retrieve(listOfSections.Size()-1).sensors().Size()-1).m_absTTS + relTTS/*+timeInStation*/;
        sensors.Insert(ri_sensor(relTTS, absTTS));
        timeToPrevSensor = relTTS;
        szrelTTS = strtok_r(NULL, "\t", &szNextSensorCol);
      }
      timeToPrevSensor = 0;
      if (TPRC_OS != rCode)
        break;
      if (MIN_NUM_OF_SENSORS > sensors.Size())
      {
        rCode = TPRC_NESE;
        break;
      }
      listOfSections.Insert(ri_rwSection(szStationName, sensors, timeToTravel, 0));
      m_Logger << INDENT << szStationName << "\n";
      szCurLine =  strtok_r(NULL, "\n", &szNextLine);
      sensors.Clear();
      lineNum++;
    }
  }
  if (TPRC_OS == rCode)
    m_Logger << INDENT << "Sections file successfully read.\n";
  m_Logger.decreaseIndent();
  m_Logger << INDENT << "Exiting ri_cfReader::readSections...\n";
  return rCode;
}

TPRC_ENUM ri_cfReader::readTimeTable(LinkedList<ri_timeTableRow>& timeTable)
{
  m_Logger << INDENT << "Entering ri_cfReader::readTimeTable...\n";
  m_Logger.increaseIndent();
  char* szTrainId, * szTime, * szCurLine, * szNextLine, * szHour, * szMin, * szSec;
  TPRC_ENUM rCode = loadConfigFile((const char*)m_timeTablePath);
  if (TPRC_OS == rCode)
  {
    unsigned nOfDepartures = 0;
    szCurLine = strtok_r(m_szFileContent, "\n", &szNextLine);
    m_Logger << INDENT << "Parsing time columns...\n";
    while (NULL != szCurLine)
    {
      szTrainId = strtok_r(szCurLine, "\t", &szTime);
      if (NULL != szTime && parseTimeString(szTime))
      {
        szHour = strtok_r(szTime, ":", &szMin);
        strtok_r(szMin, ":", &szSec);
        timeTable.Insert(ri_timeTableRow(szTrainId, szHour, szMin, szSec));
        szCurLine = strtok_r(NULL, "\n", &szNextLine);
        nOfDepartures++;
      }
      else
      {
        m_Logger << INDENT << "ERROR!. Invalid time column detected at line " << nOfDepartures + 1 << "\n";
        rCode = TPRC_WTC;
        break;
      }
    }
    if (MIN_NUM_OF_DEPARTURES > nOfDepartures && TPRC_OS == rCode)
      rCode = TPRC_NEDE;
    else
      m_Logger << INDENT << "Time table successfully read.\n";
  }
  m_Logger.decreaseIndent();
  m_Logger << INDENT << "Exiting ri_cfReader::readTimeTable.\n";
  return rCode;
}

TPRC_ENUM ri_cfReader::readDisplay()
{
  return TPRC_OS;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// ri_rwSection class implementation                                                              //
// Purpose: It represents a railway section between two train stations. It stores and provides    //
//          information such as time to travel this section, the number of sensors, the time to   //
//          get to every sensor and the train stations at each end of the section.                //
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method: ri_rwSection::ri_rwSection(...)                                                        //
// Purpose: Constructor. An absolutelly trivial constructor.                                      //
//                                                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////

ri_rwSection::ri_rwSection(const ri_str& name, const LinkedList<ri_sensor>& sensors, unsigned ttt,
  unsigned tis): m_sensors(sensors), m_tis(tis), m_ttt(ttt), m_name(name), m_isBusy(false),
  m_dps(100.0/m_ttt)
{
}

void ri_rwSection::procTAS(ri_rwSection& prevSection, time_t at)
{
  m_isBusy = true;
  prevSection.procTCS(prevSection.sensors().Size()-1, at);
  m_trainData.m_secNum = prevSection.trainData().m_secNum;
  m_trainData.m_dtest = prevSection.trainData().m_dtest;
  m_trainData.m_est = prevSection.trainData().m_est;
  m_trainData.m_cPos = 0;
  m_trainData.m_mPos = m_sensors.Retrieve(0).m_relTTS * m_dps;
  m_trainData.m_delay = prevSection.trainData().m_delay;
}

void ri_rwSection::procTES(unsigned secNum, time_t dtest, time_t ct)
{
  m_isBusy = true;
  m_trainData.m_secNum = secNum;
  m_trainData.m_dtest = dtest;
  m_trainData.m_est = ct;
  m_trainData.m_dt = ct;
  m_sensors.Retrieve(0).changeState(CROSSED);
  m_trainData.m_cPos = m_sensors.Retrieve(0).m_relTTS * m_dps;
  m_trainData.m_mPos = m_sensors.Retrieve(1).m_relTTS * m_dps;
  m_trainData.m_delay = difftime(ct, dtest) - m_sensors.Retrieve(0).m_absTTS;
}

void ri_rwSection::procTLS()
{
  for (LinkedList<ri_sensor>::Iterator ss = m_sensors.NewIterator(); !ss.EndReached(); ss++)
    ss->changeState(NOT_CROSSED);
  m_isBusy = false;
}

void ri_rwSection::procTCS(unsigned sensor, time_t ct)
{
  if (sensor < m_sensors.Size()-1)
  {
    m_sensors.Retrieve(sensor).changeState(CROSSED);
    m_trainData.m_cPos = m_sensors.Retrieve(sensor).m_relTTS * m_dps;
    m_trainData.m_mPos = m_sensors.Retrieve(sensor + 1).m_relTTS * m_dps;
    if (0 == sensor)
      m_trainData.m_dt = ct;
  }
  else if (sensor == m_sensors.Size()-1)
  {
    m_isBusy = false;
    for (LinkedList<ri_sensor>::Iterator ss = m_sensors.NewIterator(); !ss.EndReached(); ss++)
      ss->changeState(NOT_CROSSED);
    m_trainData.m_cPos = 100;
  }
}

void ri_rwSection::updatePos(time_t ct)
{
  float cPos = (difftime(ct, m_trainData.m_dt) + m_sensors.Retrieve(0).m_relTTS) * m_dps;
  if (m_isBusy)
  {
    if (cPos < m_trainData.m_mPos)
      m_trainData.m_cPos = cPos;
    else
      m_trainData.m_cPos = m_trainData.m_mPos;
  }
}
