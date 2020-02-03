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
//#include <assert.h>
#define MAX_CAD 255
#define RANGECHECK(fase)    if(section>=getSize() || section<0 ){\
                            sprintf(msg,"In \"fase\"There were an attempt to access an\
                            invalid section%d",section);\
                            throw msg;\
                            }
#define LIMCHECK(fase)  if(section>getSize() || section<0 ){\
                            sprintf(msg,"In \"fase\"The pretended limit [%d] is bigger\
                             (or lower) than the allowd",section);\
                            throw msg;\
                            }
                            
#include "jg_sec_colection.h"


jl_sec_colection::jl_sec_colection(int m_trains, int nsections, char *pmessage)
{
    msg=pmessage;
    if(nsections<=0||m_trains<=0)
    {
        
        sprintf(msg,
        "Tried to operate a system with zero or negative trains(%d) or sections(%d)"
        ,m_trains,nsections);
         throw msg;
    }
    if(m_trains>=MAX_SECTIONS)
    {
        
        sprintf(msg,
        "The system requieres more sections(%d)than rains(%d)"
        ,nsections,m_trains);
         throw msg;
    }
    if(nsections>MAX_SECTIONS)
    {
        
        sprintf(msg,
        "The system can support at most %d sections, you asked for %d"
        ,MAX_SECTIONS,nsections);
         throw msg;
    }
    max_trains=m_trains;
    size=nsections;
    number_of_trains=0;
    idtrains=0;
    for(int i=0;i<MAX_SECTIONS;i++)
    {
        section_col[i].sec_key=-1;
        section_col[i].used_by_train=-1;
        section_col[i].busy=false;
        section_col[i].is_an_entry=false;
        section_col[i].is_an_used_entry=false;
        section_col[i].entry_key=-1;
    }
}
/*************************************************************************/
jl_sec_colection::~jl_sec_colection()
{
    
}
/*************************************************************************/
int jl_sec_colection::getSize()
{
    return size;
}
/*************************************************************************/
int jl_sec_colection::getSecKey(int section)
{
        RANGECHECK("Security Key")
        return section_col[section].sec_key;
}
/*************************************************************************/
bool jl_sec_colection::setSecKey(int keys[], int n_keys)
{
    int section=n_keys;
    LIMCHECK("Number of asigned Security Keys,")
    for(int i=0; i<n_keys; i++)
    {
        section_col[i].sec_key=keys[i]; 
    }
    return true;
}
/*************************************************************************/
bool jl_sec_colection::isBusy(int section)
{
    RANGECHECK("Busy State Check")
    return section_col[section].busy;
}
/*************************************************************************/
void jl_sec_colection::ocupy(int section, int ntrain)
{
    RANGECHECK("Ocupy a section")
    if(section_col[section].busy || section_col[section].used_by_train!=-1){
        
        sprintf(msg,
        "There were an attempt to ocuppy section %d with %d train while it is used by %d",
        section,ntrain,section_col[section].used_by_train);
         throw msg;
    }
    section_col[section].busy=true;
    section_col[section].used_by_train=ntrain;
}
/*************************************************************************/
void jl_sec_colection::free(int section)
{
    RANGECHECK("Freeing section")
    if(!section_col[section].busy)
    {
        
        sprintf(msg,
        "There were an attempt to release section %d but it was not busy",
        section);
        throw msg;
    }
    else
    {
        section_col[section].busy=false;
        section_col[section].used_by_train=-1;
    }
}
/*************************************************************************/
int jl_sec_colection::getNtrain(int section)
{
    RANGECHECK("Obtaining train information")
    if(!section_col[section].busy)
    {
        
        sprintf(msg,
        "There were an attempt to obtain the id of train in section %d, but it is free",
        section);
        throw msg;
    }
    return section_col[section].used_by_train;
}
/*************************************************************************/

/*************************************************************************/
/*
 * This is just used for initializing the system
 */
void jl_sec_colection::insertNewTrain(int section)
{
    RANGECHECK("Inserting a new train")
    if(number_of_trains>=max_trains)
    {
        
        sprintf(msg,
        "There were an attempt to inserting a new train but there are no more \
        allowed max(%d)",section);
        throw msg;
    }
    number_of_trains++;
    ocupy(section,idtrains++);
}
/*************************************************************************/
bool jl_sec_colection::getEntry(int section)
{
    RANGECHECK("Managing entry request")
    /*
     * TODO
     * CHECKS
     */
     
    if(number_of_trains>=max_trains)
    {
        
        sprintf(msg,
        "There were an attempt to inserting a new train but there are no more \
        allowed max(%d)",section);
        throw msg;
        return false;
    }
      if(!section_col[section].is_an_entry)
     {
        
        sprintf(msg,"The section %d is not an entry point",section);
        throw msg;
        return false;
     }
     if(section_col[section].is_an_used_entry)
     {
        /*
         *  section_col[section].is_an_used_entry=true;
         * This is because the simulator if any algorith
         * denyied de request it will send a cancel event to
         * all the algortihm (event the one that denied
         * so in order of keeping consistency we need to
         * marck it as used entry.
         */
        return false;
     }
     section_col[section].is_an_used_entry=true;
     return true;
}

/*************************************************************************/
void jl_sec_colection::freeEntry(int section)
{
    /*
     * TODO
     * CHECKS
     * Doubts The entry_system and exit_system event are mutex???
     * in other words while a train is entering 
     * then another train can not enter to the system???
     */
     RANGECHECK("Entering a train to te system")
     if(!section_col[section].is_an_entry)
     {
        
        sprintf(msg,"The section %d is not an entry point",section);
        throw msg;
     }
     if(!section_col[section].is_an_used_entry)
     {
        sprintf(msg,"The %d section did not have a request but a entry event has just happened",section);
        throw msg;
     }
     section_col[section].is_an_used_entry=false;
     number_of_trains++;
     ocupy(section,idtrains++);
}

/*************************************************************************/
bool jl_sec_colection::cancelEntry(int section)
{
    RANGECHECK("Canceling an entry event")
    /*
     * TODO
     * CHECKS
     */
     if(!section_col[section].is_an_entry)
     {
        
        sprintf(msg,"The section %d is not an entry point",section);
        throw msg;
        return false;
     }
     
     if(!section_col[section].is_an_used_entry)
     {
        
        sprintf(msg,"The %d section did not have a request but it was requested a cancelation",section);
        throw msg;
        return false;
     }
     section_col[section].is_an_used_entry=false;
     return true;
}

/*************************************************************************/
void jl_sec_colection::exitTrain(int section)
{
    RANGECHECK("Managing an exit event:")
    /*
     * TODO
     * CHECKS
     */
     if(!section_col[section].is_an_entry)
     {
        
        sprintf(msg,"I have received and exit event from %d section\
        but my records do not recognize it as an entry point",section);
        throw msg;
     }

     if(!section_col[section].busy)
     {

        
        sprintf(msg,"I have received and exit event from %d section\
        but my records do not show a train in that place",section);
        throw msg;
     }
     section_col[section].used_by_train=-1;
     section_col[section].busy=false;
     number_of_trains--;
}

/*************************************************************************/
void jl_sec_colection::setEntrys(int entry_sections[], int entry_keys[], int num_entrys)
{
    {int section=num_entrys;LIMCHECK("Defining entry points and their keys")}
    /*
     * TODO
     * CHECKS
     */
     if(num_entrys>getSize())
     {
        
            sprintf(msg,"The number of entry points is bigger than the number of sections %d %d",
                 num_entrys,getSize());
            throw msg;
     }
     for(int i=0;i<num_entrys;i++)
     {
        int pos=entry_sections[i];
        if(pos<0||pos>=getSize())
        {
            
            sprintf(msg,"The %d section is not part of the system and can not be\
            a entry point",pos);
            throw msg;
        }
        section_col[pos].is_an_entry=true;
        section_col[pos].is_an_used_entry=false;
        section_col[pos].entry_key=entry_keys[pos];
     }
}
