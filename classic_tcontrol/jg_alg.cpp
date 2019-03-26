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
//#define NDEBUG
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "alg.h"
#include "jg_alg.h"
#define STOPKEY keys[0]
#define STARTCRITIC try{
#define ENDCRITIC   }catch( char *s){logThis(s);sendHALT(s);}
/*****************************************************************/

jl_algorithm_t::jl_algorithm_t(void)
{
    log_file=fopen("./jg_log_file.txt","w+");
    time(&hora);
    sprintf(bigstr,"Starting log at %d",int(hora));
    logThis(bigstr);
    notifier = NULL; 
    jlsecs = NULL;
}

/******************************************************************/

void jl_algorithm_t::set_notify_object(alg_notify_object_t *the_object, int the_estop_key)
{
    notifier=the_object;
    STOPKEY=the_estop_key;
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo el notificador:",int(m-hora));
    logThis(bigstr);
}

/******************************************************************/

void jl_algorithm_t::initialize_sizes(int sec_cnt, int max_trains)
{
  sprintf(bigstr3,"Eventos............................................");
  char *pmessage=&bigstr3[10];
  STARTCRITIC
    time(&m);
    sprintf(bigstr,"\n a las %d\t conozco de %d secciones y un máximo de %d trenes:"
    ,int(m-hora),sec_cnt,max_trains);
    logThis(bigstr);
    jlsecs=new jl_sec_colection(max_trains,sec_cnt,pmessage);
  ENDCRITIC 
}

/******************************************************************/

void jl_algorithm_t::set_section_keys(int keys[], int n_keys)
{
    STARTCRITIC
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo %d llaves",int(m-hora),n_keys);
    logThis(bigstr);
    /*
    It has no sense to receive a diferent number keys than the actual number of sections
    */
    assert(jlsecs!=NULL);
    if(jlsecs->getSize()!=n_keys)
    {
        sprintf(bigstr,"[%d:%d]",jlsecs->getSize(),n_keys);
        sendHALT(bigstr);
    }
    if(MAX_SECTIONS<=n_keys)
    {
        sprintf(bigstr,"se supero el limite de %d en %d llaves ",MAX_SECTIONS,n_keys);
        sendHALT(bigstr);
    }
    jlsecs->setSecKey(keys,n_keys);
    ENDCRITIC

}

/******************************************************************/

void jl_algorithm_t::set_entry_sections(int entry_sections[], int entry_keys[], int num_entrys)
{
    STARTCRITIC
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo lista de %d entradas",int(m-hora),num_entrys); 
    logThis(bigstr);
    jlsecs->setEntrys(entry_sections,entry_keys,num_entrys);
    ENDCRITIC
}

/******************************************************************/

void jl_algorithm_t::initial_state(int train_locations[], int n_locations)
{
    STARTCRITIC
    int i;
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo el estado inicial de %d estaciones",int(m-hora),n_locations);
    logThis(bigstr);
    /*
     * TODO Verify the correctness of the train_locations array
     * */
     assert(jlsecs!=NULL);
    for(i=0; i < n_locations; i++)
    {
        sprintf(bigstr,"\n \t sec[%d] tren(%d)",train_locations[i],i);
        logThis(bigstr);
        jlsecs->insertNewTrain(train_locations[i]);

    }
    tcount=n_locations;
    for(i=0; i< jlsecs->getSize(); i++)
    {
        //int back,next,nextnext;
        int next;
        //back=get_prior(i);
        next=get_next(i);
        /*nextnext=*/get_next(next);
        if(jlsecs->isBusy(i)&&!jlsecs->isBusy(next))
        {
            allowSecEntry(next);
        }
    }
    ENDCRITIC
}
                                                                                
/******************************************************************/

void jl_algorithm_t::crossing_event(int section)
{
    STARTCRITIC
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo evento de cruce en la secc: %d",int(m-hora),section);
    logThis(bigstr);
    int next,back;
    back=get_prior(section);
    next=get_next(section);
    
    if(jlsecs->isBusy(section)){
        /*
         * This means that we have received the crossing event at
         * a section that is already used, so it looks like a disaster
         * about to happen
         */
        sprintf(bigstr,
        ":''(There is already a train (%d) in section %d where you are trying to enter with(%d)from %d",
        jlsecs->getNtrain(section),section,jlsecs->getNtrain(back),back);
        sendHALT(bigstr);
        return;
    }
    /*
     * *First step is to deny any further access to the recently used section
     */
    denySecEntry(section);
    jlsecs->ocupy(section,jlsecs->getNtrain(back));
    jlsecs->free(back);
    int pback=get_prior(back);
    if(jlsecs->isBusy(pback)) 
    {
        /*
         *This means that there is a train about to enter
         * to the just released prior section so it is safe to turn the
         * green ligth.
         */
        allowSecEntry(back);    
    }

    /***Try to request the next section if available****/
    if(!jlsecs->isBusy(next)){
        /*
         * This is for the case that the next section is free
         * but since there were no train this section when the next
         * was released the system have put in red, so we need to ask
         * ask for it
         */
        allowSecEntry(next);    
    }
    ENDCRITIC       
}

/******************************************************************/

void jl_algorithm_t::entry_event(int section)
{
    STARTCRITIC
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo evento de entrada en la secc: %d",int(m-hora),section);
    logThis(bigstr);
    int next=get_next(section);
        /*
         * This is for the case that the next section is free
         * but since we have just entered to the system that means that
         * there were no train in this section so the next light is red
         * so it is necesary to check for the freeness of the next section
         * and if it is available turn on the green light
         */
    jlsecs->freeEntry(section);
    if(!jlsecs->isBusy(next)){
        allowSecEntry(next);    
    }
    ENDCRITIC
}

/******************************************************************/

void jl_algorithm_t::exit_event(int section)
{
    STARTCRITIC
    /*
     * DOUBT:
     *  Once the exit event is received it means that the section
     * is free now????
     */
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo evento de salida en la secc: %d",int(m-hora),section);
    logThis(bigstr);
    jlsecs->exitTrain(section);
    if(jlsecs->isBusy(get_prior(section)))
    {
        allowSecEntry(section);
    }
    denySecEntry(get_next(section));
    ENDCRITIC
}

/******************************************************************/

bool jl_algorithm_t::request_entry(int section)
{
    STARTCRITIC
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo petición de entrada en la secc: %d",int(m-hora),section);
    logThis(bigstr);
    if(!jlsecs->isBusy(section)){
        /*
         * Since we have just allowed the request for practical issues
         * we assume that the train is now in the system
         * so if the request is later cancelled we must take it back
         */
        bool permission=jlsecs->getEntry(section);
        if(permission)
        {
            /*
             * First we prohibit the acces to this section
             * Then we allow the enter of the new train
             */
            denySecEntry(section);
            allowSysEntry(section);
            return true;
        }
        else
        {
            /*
             * Since there is no chance of entering 
             * a new train in this section, because it is
             * busy we, send the deny
             */
            denySysEntry(section);
        }
    }
    return false;
    ENDCRITIC
    return false;
}

/******************************************************************/

void jl_algorithm_t::cancel_entry(int section)
{
    STARTCRITIC
    time(&m);
    sprintf(bigstr,"\n a las %d\t Recibo cancelación de entrada en la secc: %d \n",int(m-hora),section);
    logThis(bigstr);
    jlsecs->cancelEntry(section);
    ENDCRITIC
    /*TODO
     *  figure out what are the implications of the cancelling
     */
}

/******************************************************************/

int jl_algorithm_t::get_prior(int section)
{
    STARTCRITIC
    if(section<0||section>=jlsecs->getSize()){
        sprintf(bigstr,"Section out of bounds [%d]",section);
        sendHALT(bigstr);
        return -1;
    }
    else if(section==0){
        return jlsecs->getSize()-1;
    }
    else
    {
        return section-1;
    }
    ENDCRITIC   
    return -1;
}
/******************************************************************/

int jl_algorithm_t::get_next(int section)
{
    STARTCRITIC
    if(section<0||section>=jlsecs->getSize()){
        sprintf(bigstr,SECTOUTOFBONDS " [%d]",section);
        sendHALT(bigstr);
        return -1;
    }
        else if(section==jlsecs->getSize()-1){
        return 0;
    }
    else
    {
        return section+1;
    }
    ENDCRITIC   
    return -1;
}
/******************************************************************/


void jl_algorithm_t::sendHALT(char *reason)
{
    char cadenota[255];
    time(&m);
    sprintf(cadenota,"\nABORT: a las %d\t Detengo el sitema por\n\t: %s",int(m-hora),reason);
    logThis(cadenota);
    notifier->estop(STOPKEY,bigstr);
}
/******************************************************************/

void jl_algorithm_t::allowSecEntry(int section)
{
    time(&m);
    sprintf(bigstr,"\n a las %d\t Se activa verde en sección: %d",int(m-hora),section);
    logThis(bigstr);
    notifier->permit_section_entry(section,jlsecs->getSecKey(section)); 
}
/******************************************************************/
void jl_algorithm_t::denySecEntry(int section)
{
    time(&m);
    sprintf(bigstr,"\n a las %d\t Se pone rojo en seccion: %d",int(m-hora),section);
    logThis(bigstr);
    notifier->prohibit_section_entry(section,jlsecs->getSecKey(section));   
}
/******************************************************************/

void jl_algorithm_t::allowSysEntry(int section)
{
    time(&m);
    sprintf(bigstr,"\n a las %d\t Se activa verde en la entrada de sección: %d",int(m-hora),section);
    logThis(bigstr);
    notifier->permit_system_entry(section,jlsecs->getSecKey(section));  
}
/******************************************************************/
void jl_algorithm_t::denySysEntry(int section)
{
    time(&m);
    sprintf(bigstr,"\n a las %d\t Se pone rojo en la entrada de seccion: %d",int(m-hora),section);
    logThis(bigstr);
    notifier->prohibit_system_entry(section,jlsecs->getSecKey(section));    
}
/******************************************************************/

void jl_algorithm_t::logThis(char *msglog)
{
    fprintf(log_file, "%s", msglog);
    fflush(log_file);
}

    
