/***************************************************************************
Copyright (c) 2004 Jorge F. Gutierrez Ramirez

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
*************************************************************************/

#include <stdio.h>
/*
#ifndef JL_SEC_COLECTION_H
#define JL_SEC_COLECTION_H
*/
#ifndef MAX_SECTIONS
    #define MAX_SECTIONS (60)
#endif

    struct jl_section_t
    {
      int sec_key;
      int used_by_train;
      bool busy; 
      /*
       *actually it is not needed because the used_by would have a
       * negative number if there are no train but it may come handful
       * to detect unconsistent states
       *  */
      bool is_an_entry;
      bool is_an_used_entry;
      int entry_key;
    };
    
    class jl_sec_colection{
    public:
        jl_sec_colection(int m_trains, int nsections, char* pmessage);
        virtual ~jl_sec_colection();
        int getSize();
        bool setSecKey(int keys[], int n_keys);
        bool isBusy(int station);
        void ocupy(int section, int ntrain);
        void free(int section);
        int getNtrain(int section);
        int getSecKey(int section);
        void insertNewTrain(int section);
        bool getEntry(int section);
        void freeEntry(int section);
        void setEntrys(int entry_sections[], int entry_keys[], int num_entrys);
        void exitTrain(int section);
        bool cancelEntry(int section);
    private:
        jl_section_t section_col[MAX_SECTIONS];
        int size;
        int number_of_trains;
        int idtrains;
        int max_trains;
        char *msg;
    };

//#endif // JL_SEC_COLECTION_H
