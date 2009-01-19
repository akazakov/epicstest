/*testcaconnect.c*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "epicsTime.h"
#include "cadef.h"

epicsTimeStamp start_time;
int      accessCallback=0;

typedef struct{
    chid   mychid;
    evid   myevid;
    double connect_time;
    char   value[20];
    int    got_callback;
} MYNODE;

MYNODE *pmynode = NULL;

void accessRightsCallback(struct access_rights_handler_args arha)
{
    accessCallback++;
}

void connection_callback(struct connection_handler_args cha)
{
    chid     chid=cha.chid;
    MYNODE   *pnode = (MYNODE *)ca_puser(chid);
    epicsTimeStamp time_now;

    if(epicsTimeGetCurrent(&time_now)!=0)
        printf("epicsTimeGetCurrent error in connection_callback\n");
    pnode->connect_time = epicsTimeDiffInSeconds(&time_now,&start_time);
}

void event_callback(struct event_handler_args eha)
{
    MYNODE *pnode = (MYNODE *)eha.usr;

    if(!ca_read_access(pnode->mychid)) return;
    strcpy(pnode->value,(char *)eha.dbr);
}

int main(int argc,char **argv)
{
    int    npv;
    int    i;
    float  pend_time;
    int    not_connected;
    double max_connect_time;

    if(argc!=3) {
        printf("usage: testcaconnect <nrecords> <pend_time>\n");
        exit(1);
    }
    sscanf(argv[1],"%d",&npv);
    sscanf(argv[2],"%f",&pend_time);
    pmynode = (MYNODE *)calloc(npv,sizeof(MYNODE));
    for(i=0; i<npv; i++) {
        memset((char *)&pmynode[i],0,sizeof(MYNODE));
    }
    while(1) {
        accessCallback = 0;
        printf("Calling ca_task_initialize\n");
        SEVCHK(ca_task_initialize(),"ca_task_initialize");
        if(epicsTimeGetCurrent(&start_time)!=0)
            printf("epicsTimeGetCurrent error in main\n");
        for(i=0; i<npv; i++) {
            char name[40];
    
            sprintf(name,"mrk%04d",i);
            SEVCHK(ca_search_and_connect(name,&pmynode[i].mychid,
                    connection_callback,&pmynode[i]),
                    "ca_search_and_connect");
            SEVCHK(ca_replace_access_rights_event(pmynode[i].mychid,
                    accessRightsCallback),
                    "ca_replace_access_rights_event");
            SEVCHK(ca_add_event(DBR_STRING,pmynode[i].mychid,event_callback,
                    &pmynode[i],&pmynode[i].myevid),
                    "ca_add_event");
        }
        ca_pend_event(pend_time);
        max_connect_time=0.0;
        not_connected = 0;
        for(i=0; i<npv; i++) {
            if(ca_state(pmynode[i].mychid)!=2) not_connected++;
            if((ca_state(pmynode[i].mychid)==2)
            && (max_connect_time < pmynode[i].connect_time))
                    max_connect_time = pmynode[i].connect_time;
        }
        printf("not connected=%d max_connect_time=%f\n",
            not_connected,max_connect_time);
printf("calling ca_task_exit\n");
        SEVCHK(ca_task_exit(),"ca_task_exit");
printf("returned from ca_task_exit\n");
    }
    return(0);
}
