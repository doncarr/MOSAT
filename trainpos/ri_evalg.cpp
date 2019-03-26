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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg_queue.h"
#include "tcontrol.h"
#include "event_alg.h"
#include "section_reader.h"
#include "trainpos.h"
#include "ri_evalg.h"
#include "rtcommon.h"
#include "arg.h"
#include <iostream>

/**************************************************************************************************/
/* Implementation of ri_evalg_t                                                                   */
/**************************************************************************************************/

ri_evalg_t::ri_evalg_t(): m_secNum(0)
{
  m_Logger.open("ri_log.txt");
  m_Logger << "BEGINNING OF LOG.\n";
}

ri_evalg_t::~ri_evalg_t()
{
  m_Logger << "END OF LOG. ";
  m_Logger.close();
}

void ri_evalg_t::init(const char* config_file)
{
  m_Logger << INDENT << "Entering ri_evalg_t::init...\n";
  m_Logger.increaseIndent();
  ri_cfReader cfReader(m_Logger);
  TPRC_ENUM rCode = cfReader.readTPConfig(config_file);
  if (TPRC_OS == rCode)
  {
    rCode = cfReader.readTimeTable(m_timeTable);
    if (TPRC_OS == rCode)
    {
      rCode = cfReader.readSections(m_listOfSections);
      if (TPRC_OS == rCode)
        rCode = cfReader.readDisplay();
    }
  }
  m_Logger.decreaseIndent();
  m_Logger << INDENT << "Exiting ri_evalg_t::init.\n";
}

void ri_evalg_t::update(time_t ts)                                                                    // <-- This method is used to update de position of the trains with estimated position since it's called even when an event has not happend.
{
/*  m_Logger << INDENT << "Entering ri_evalg::update(...)...\n";
  m_Logger.increaseIndent();*/
  for (LinkedList<ri_rwSection>::Iterator ss = m_listOfSections.NewIterator(); !ss.EndReached(); ss++)
    if (ss->isBusy())
      ss->updatePos(ts);
  updateTabInfo();
/*  m_Logger.decreaseIndent();
  m_Logger << INDENT << "Exiting ri_evalg::update(...)...\n";*/
}

void ri_evalg_t::process_event(crossing_event_t ev)
{
/*  m_Logger << INDENT << "Entering ri_evalg_t::process_event...\n";
  m_Logger.increaseIndent();*/
  if (m_listOfSections.Size() == ev.section)
    m_listOfSections.Retrieve(ev.section - 1).procTLS();
  else
  {
    ri_rwSection& curSection = m_listOfSections.Retrieve(ev.section);
    if (ev.departure)
      if (0 == ev.section)
      {
        curSection.procTES(m_secNum, m_timeTable.Retrieve(m_secNum).dtest(), ev.time_stamp);
        m_secNum++;
      }
      else
        curSection.procTCS(0, ev.time_stamp);
    else
      curSection.procTAS(m_listOfSections.Retrieve(ev.section-1), ev.time_stamp);
  }
  updateTabInfo();
  m_Logger.flush();
/*  m_Logger.decreaseIndent();
  m_Logger << INDENT << "Exiting ri_evalg_t::process_event.\n";*/
}

void ri_evalg_t::updateTabInfo()
{
  TPRC_ENUM rCode;
  //m_Logger << INDENT << "Entering ri_evalg_t::updateTabInfo...\n";
  m_Logger.increaseIndent();
  //m_Logger << INDENT << "Opening ri_tabinfo.htm...\n";
  rCode = m_tabInfo.open("ri_tabinfo.htm");
  if (TPRC_OS != rCode)
    m_Logger << INDENT << "Oops!. " << getTPRCDesc(rCode);
  else
  {
    //m_Logger << INDENT << "Updating ri_tabinfo.htm...\n";
    m_tabInfo << HTML_F_FIXED_SECTION;
    LinkedList<ri_rwSection>::Iterator i1 = m_listOfSections.NewIterator();
    LinkedList<ri_rwSection>::Iterator i2 = m_listOfSections.NewIterator();
    for (++i2; !i1.EndReached(); ++i1,++i2)
    {
        if (i2.EndReached())
        i2 = m_listOfSections.NewIterator();
      if (i1->isBusy())
      {
        m_tabInfo << "        <TR VALIGN=TOP>\n";
        for (unsigned ss = 0; ss < 6; ++ss)
        {
          m_tabInfo << "          <TD WIDTH=16%%>\n";
          if (0 == ss)
            m_tabInfo << "            <P ALIGN=CENTER>" << m_timeTable.Retrieve(i1->trainData().m_secNum).m_trainId << "</P>\n";
          else if (1 == ss)
            m_tabInfo << "            <P ALIGN=CENTER>" << i1->trainData().m_secNum << "</P>\n";
          else if (2 == ss)
            m_tabInfo << "            <P ALIGN=CENTER>" << i1->trainData().m_est << "</P>\n";
          else if (3 == ss)
            m_tabInfo << "            <P ALIGN=CENTER>" << i1->name() << " - " << i2->name() << "</P>\n";
          else if (4 == ss)
            m_tabInfo << "            <P ALIGN=CENTER>" << int(i1->trainData().m_cPos) << "%</P>\n";
          else if (5 == ss)
            m_tabInfo << "            <P ALIGN=CENTER>" << i1->trainData().m_delay << "</P>\n";
          m_tabInfo << "          </TD>\n";
        }
        m_tabInfo << INDENT << "        </TR>\n";
      }
    }
    m_tabInfo << HTML_S_FIXED_SECTION;
    //m_Logger << INDENT << "ri_tabinfo.htm updated.\n";
    //m_Logger << INDENT << "Closing ri_tabinfo.htm.\n";
    m_tabInfo.close();
  }
  m_Logger.decreaseIndent();
  //m_Logger << INDENT << "Exiting ri_evalg_t::updateTabInfo.\n";
}
