/*Author: Georgia Koutsandria*/
#define S_FUNCTION_NAME  server
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <pthread.h>
#include <modbus/modbus.h>
#include <sys/resource.h>
#include "modbusServerMain.h"
#include "modbusRegisters.h"
#include <math.h>
#include <time.h>
/*constant variables of width of ports*/
#define ANALOG_INPUT 2
#define ANALOG_OUTPUT 2
#define DIGITAL_INPUT 8
#define DIGITAL_OUTPUT 2

#define LENGTH 100

#define DEFAULT_CLIENT_IP "169.254.234.254"
#define DEFAULT_PORT 502
#define period_t 0.00555556666
#define NO_SAMPLE 16
/*-------------------------------------------------------------------------
  STRUCTURES
  -------------------------------------------------------------------------
  structure used in conversion from Big Endian to Little Endian*/
typedef  union 
{
	float f;
	char c[sizeof(float)]; 
} float_char;


/*structure with data to send*/
typedef struct Frame_Input
{
	float_char table_AI[ANALOG_INPUT];
	bool table_DI[DIGITAL_INPUT];
} frame_Input;
frame_Input x;

/*structure with data to receive*/
typedef struct Frame_Output
{
	float_char table_AO[ANALOG_OUTPUT];
	bool table_DO[DIGITAL_OUTPUT];
} frame_Output;  

/*-------------------------------------------------------------------------
  GLOBAL VARIABLES
 */

/*parameters*/
char_T IP_string[LENGTH];
int portNumber;
int optionTCP = 0;
int optionBigEndian = 0;

#define SAMPLERATE 5
pthread_t thread1;
/*global variables that define socket*/
int ListenSocket;
int ClientSocket;
struct sockaddr_in socketListening;
char * modbusClientIP = DEFAULT_CLIENT_IP;
int modbusPort = DEFAULT_PORT;
int socket1;
modbus_t *ctx;
modbus_mapping_t *mb_mapping;

int requestSocket;
pthread_mutex_t modbusRegisterAccessLock = PTHREAD_MUTEX_INITIALIZER;
InputRealPtrsType inputs_A;
pthread_t thread;
bool *outputs_D;
int write1;
struct arg_struct {

	modbus_t *arg1;
	modbus_mapping_t *arg2;
	SimStruct *arg3;     
};
/*-------------------------------------------------------------------------
  mdlInitializeSizes
  -------------------------------------------------------------------------
 */    
void processTCPrequest();
void processTCP_write();

/*modbus_t * ctx,modbus_mapping_t * mb_mapping,SimStruct *S);*/
static void mdlInitializeSizes(SimStruct *S)
{
	
	int optionProtocole;
	int optionEndian;

	/*amount of parameters*/
	ssSetNumSFcnParams(S, 3);
	if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) 
	{
		ssSetErrorStatus(S, "No parameters!\n");
		/*stop of simulation*/
		ssSetStopRequested(S, 1);
	}

	/*take values of the parameters
	  number of port*/
	portNumber = (int)(*mxGetPr(ssGetSFcnParam(S,0)));
	/*protocol*/
	optionProtocole = 1;
	/*option of endianess*/
	optionEndian = (int)(*mxGetPr(ssGetSFcnParam(S,2)));
    if(optionEndian == 1)
	{
		optionBigEndian = 1;
	}
	else
	{
		optionBigEndian = 0;
	}


	if (optionProtocole == 1) 
	{
		optionTCP = 1;
		
	} 
	/*PORTS         

	  amount of inputs*/
	if (!ssSetNumInputPorts(S, 2)) return;
	/*width of port 1*/
	ssSetInputPortWidth(S, 0, ANALOG_INPUT); 
	/*width of port 2*/
	ssSetInputPortWidth(S, 1, DIGITAL_INPUT); 
    
	/*type of input port*/
	ssSetInputPortDataType(S, 1, SS_BOOLEAN);

	/*	ssSetInputPortDirectFeedThrough(S, 0, 1);
		ssSetInputPortDirectFeedThrough(S, 1, 1);
	 */

	/*amount of outputs */
	if (!ssSetNumOutputPorts(S,2)) return;
	/*width of port 1*/
	ssSetOutputPortWidth(S, 0, ANALOG_OUTPUT); 
	/*width of port 2*/
	ssSetOutputPortWidth(S, 1, DIGITAL_OUTPUT); 

    	
	/*type of output port*/
	ssSetOutputPortDataType(S, 1, SS_BOOLEAN);

	ssSetNumSampleTimes(S, 1);

	ssSetOptions(S,
			SS_OPTION_WORKS_WITH_CODE_REUSE |
			SS_OPTION_EXCEPTION_FREE_CODE |
			SS_OPTION_USE_TLC_WITH_ACCELERATOR);
	ssSetInputPortSampleTime(S,0,CONTINUOUS_SAMPLE_TIME);	
	printf("Completed initialization\n");
  
}
/*mdlInitializeSampleTimes*/


static void mdlInitializeSampleTimes(SimStruct *S)
{
	ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
}

/*mdlStart*/

#define MDL_START
#if defined(MDL_START) 
static void mdlStart(SimStruct *S)
{

	/*PRIORITY OF THREAD*/
	/*process and its thread have class of "real time"*/
	setpriority(PRIO_PROCESS, 0,-10);
	pthread_t thId = pthread_self();
	pthread_attr_t thAttr;
	int policy = 0;
	int max_prio_for_policy = 0;

	pthread_attr_init(&thAttr);
	pthread_attr_getschedpolicy(&thAttr, &policy);
	max_prio_for_policy = sched_get_priority_max(policy);


	pthread_setschedprio(thId, max_prio_for_policy);
	/*OPTION TCP*/
	if( optionTCP == 1)
	{

		ctx = modbus_new_tcp(modbusClientIP, modbusPort);
		if (ctx == NULL) 
		{
			fprintf(stderr, "Unable to allocate libmodbus context\n");
			exit(-1);
		}
		else
		{
			mb_mapping = modbus_mapping_new(DIGITAL_OUTPUT,DIGITAL_INPUT,ANALOG_OUTPUT,ANALOG_INPUT);
			if (mb_mapping == NULL) 
			{
				fprintf(stderr, "Failed to allocate the mapping: %s\n",
				modbus_strerror(errno));
				modbus_free(ctx);
				exit(1);
			}
		}
    
    }
	inputs_A = ssGetInputPortRealSignalPtrs(S,0);	
	outputs_D = (bool *)ssGetOutputPortSignal(S,1);

	/*mdlOutputs*/
}
#endif
int cycle=2;
int part_cycle=1;
double sample[NO_SAMPLE];
double sum=0;
double my_time=1;
int coil1, coil2;

static void mdlOutputs(SimStruct *S, int_T tid)
{
    int i;
    if(my_time>0)
    {
        coil1 = 1;
        coil2 = 1;
        outputs_D[0] = coil1;
        printf("Initial status of circuit breaker 1(CB1): %d \n", coil1, outputs_D[0]);
        outputs_D[1] = coil2;
        printf("Initial status of circuit breaker 2(CB2): %d \n", coil2, outputs_D[1]);
        my_time = 0;
        
    }
    if(*inputs_A[1]>=((part_cycle*period_t/NO_SAMPLE)+(cycle*period_t)))
    {
        sample[part_cycle]=*inputs_A[0];        
        if(part_cycle%NO_SAMPLE==0)
        {
            for(i=0;i<part_cycle;i++)
            {
                sample[i]=2*pow(sample[i],2);     
            }
            for(i=0;i<part_cycle;i++)
            {
                sum+=sample[i];
            }
            sum/=NO_SAMPLE; 
            setInputRegisterValue(mb_mapping,0, (int)round(sqrt(sum)));
            printf("\n--------------Cycle %d--------------\n", cycle);
            printf("Value of current measured: %d\n",(int)round(sqrt(sum)) );
            processTCPrequest();/*accept a read request*/
            processTCPrequest();/*accept a write request*/ 
             
            cycle++;
            part_cycle=0;
            sum=0;
        }
        part_cycle++;
    }	
}

void processTCPrequest()
{
    ctx = modbus_new_tcp(modbusClientIP, modbusPort);
    socket1 = modbus_tcp_listen(ctx, 1);
    requestSocket = modbus_tcp_accept(ctx, &socket1);
     
	int i;
	bool k;
	{
		uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH+1];
		int rc;
		if(ctx!=NULL)
		{
			rc=modbus_receive(ctx, query);
		}
	
        if (rc > 0) 
        {
            int fc = query[7];
            if(fc==4)
            {
                printf("Type of packet query: Read register\n");
                pthread_mutex_lock(&modbusRegisterAccessLock);
                modbus_reply(ctx, query, rc, mb_mapping);
                pthread_mutex_unlock(&modbusRegisterAccessLock);
            }
            else if(fc==15)
            {
                printf("Type of packet query: Write coils\n");
                pthread_mutex_lock(&modbusRegisterAccessLock);
                modbus_reply(ctx, query, rc, mb_mapping);
            	pthread_mutex_unlock(&modbusRegisterAccessLock);
                int coil1 = getRegisterBit(mb_mapping,0);
                int coil2 = getRegisterBit(mb_mapping,1);
                outputs_D[0] = coil1;
                printf("Status of circuit breaker 1(CB1): %d \n", coil1, outputs_D[0]);
                outputs_D[1] = coil2;
                printf("Status of circuit breaker 2(CB2): %d \n", coil2, outputs_D[1]);
            }	
		}
		else if (rc  == -1)
        {
			/*printf("error");*/
          
		}
	}
   	close(socket1);
    close(requestSocket);      
}

static void mdlTerminate(SimStruct *S)
{
	close(socket1);
    close(requestSocket);
	modbus_mapping_free(mb_mapping);
	modbus_free(ctx);

	printf("\n--------------------End of connection---------------------\n");
	close(ClientSocket);
	close(ListenSocket);

}


#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

