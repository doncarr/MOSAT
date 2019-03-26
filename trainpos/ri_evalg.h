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

using namespace std;

/**************************************************************************************************/
/* Macros to create the HTML report                                                               */
/**************************************************************************************************/

#define HTML_F_FIXED_SECTION \
  "  <!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n\
  <HTML>\n\
  <HEAD>\n\
    <META HTTP-EQUIV=\"refresh\" CONTENT=\"0.5\">\n\
    <META HTTP-EQUIV=\"pragma\" CONTENT=\"no-cache\">\n\
    <META HTTP-EQUIV=\"CONTENT-TYPE\" CONTENT=\"text/html; charset=utf-8\">\n\
    <TITLE></TITLE>\n\
    <STYLE TYPE=\"text/css\">\n\
    <!--\n\
      @page { size: 8.5in 11in; margin: 0.79in }\n\
      P { margin-bottom: 0.08in }\n\
      TD P { margin-bottom: 0in }\n\
      TH P { margin-bottom: 0in }\n\
    -->\n\
    </STYLE>\n\
  </HEAD>\n\
  <BODY LANG=\"en-US\" DIR=\"LTR\">\n\
    <TABLE WIDTH=100%% BORDER=1 BORDERCOLOR=\"#000000\" CELLPADDING=4 CELLSPACING=0>\n\
      <COL WIDTH=85*>\n\
      <COL WIDTH=85*>\n\
      <COL WIDTH=85*>\n\
      <COL WIDTH=85*>\n\
      <COL WIDTH=85*>\n\
      <TR VALIGN=TOP>\n\
        <TH WIDTH=20%% BGCOLOR=\"#000080\">\n\
          <P><FONT COLOR=\"#ffffff\"><SPAN STYLE=\"background: #000080\">Tren\n\
          </SPAN></FONT></P>\n\
        </TH>\n\
        <TH WIDTH=10%% BGCOLOR=\"#000080\">\n\
          <P><FONT COLOR=\"#ffffff\"><SPAN STYLE=\"background: #000080\">Número\n\
          Secuencial</SPAN></FONT></P>\n\
        </TH>\n\
        <TH WIDTH=20%% BGCOLOR=\"#000080\">\n\
          <P><FONT COLOR=\"#ffffff\"><SPAN STYLE=\"background: #000080\">Hora\n\
          de entrada en servicio</SPAN></FONT></P>\n\
        </TH>\n\
        <TH WIDTH=30%% BGCOLOR=\"#000080\">\n\
          <P><FONT COLOR=\"#ffffff\"><SPAN STYLE=\"background: #000080\">Sección\n\
          actual</SPAN></FONT></P>\n\
        </TH>\n\
        <TH WIDTH=10%% BGCOLOR=\"#000080\">\n\
          <P><FONT COLOR=\"#ffffff\"><SPAN STYLE=\"background: #000080\">Posición\n\
          </SPAN></FONT></P>\n\
        </TH>\n\
        <TH WIDTH=10%% BGCOLOR=\"#000080\">\n\
          <P><FONT COLOR=\"#ffffff\"><SPAN STYLE=\"background: #000080\">Retrazo\n\
          </SPAN></FONT></P>\n\
        </TH>\n\
      </TR>\n"
#define HTML_S_FIXED_SECTION \
  "    </TABLE>\n\
  </BODY>\n\
  </HTML>\n"

/**************************************************************************************************/
/*                                                                                                */
/**************************************************************************************************/

class ri_evalg_t : public event_alg_t
{
  private:
  unsigned m_secNum;                                                                               // m_secNum = "Secuential number", a consecutive value assigned to every train entering service.
  ri_fileWriter m_Logger;
  ri_fileWriter m_tabInfo;
  LinkedList<ri_timeTableRow> m_timeTable;
  LinkedList<ri_rwSection> m_listOfSections;
  void updateTabInfo();                                                                               // Creates or updates the tabular information (ri_tabinfo.htm) of the trains in operation.
  public:
  ri_evalg_t();
  ~ri_evalg_t();
  void init(const char* config_file);
  void update(time_t time);
  void process_event(crossing_event_t ev);
};

