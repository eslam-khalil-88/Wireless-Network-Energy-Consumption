
#include "contiki.h"
#include "net/rime.h"
#include "random.h"



#include "dev/button-sensor.h"

#include "dev/leds.h"

#include "project-conf.h"
#include "sys/energest.h"

#define I_transmit 0.021
#define I_receiv 0.023
#define I_cpu 0.002
#define I_lpm 0.0012
#define V_volt 3.6

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "BROADCAST example");
AUTOSTART_PROCESSES(&example_broadcast_process);




/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  float t_cpu, t_lpm, t_transmit, t_listen;
  unsigned long E_transmit , E_receiv , E_cpu , E_lpm;

  unsigned long time = 0 , radio = 0;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();


  /* Start powertracing, once every two seconds. */
  //powertrace_start(CLOCK_SECOND * 2);
  
  broadcast_open(&broadcast, 129, &broadcast_call);

  while(1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom("Hello", 6);
    broadcast_send(&broadcast);

    t_cpu = energest_type_time(ENERGEST_TYPE_CPU);
    t_cpu /= CLOCK_SECOND;
    t_lpm = energest_type_time(ENERGEST_TYPE_LPM);
    t_lpm /= CLOCK_SECOND;
    t_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
    t_transmit /= CLOCK_SECOND;
    t_listen = energest_type_time(ENERGEST_TYPE_LISTEN);
    t_listen /= CLOCK_SECOND;
   
     
     time = t_cpu + t_lpm;
     radio = t_transmit + t_listen;

     E_transmit = t_transmit * I_transmit * V_volt;
     E_receiv = t_listen * I_receiv * V_volt;
     E_cpu = t_cpu * I_cpu * V_volt;     
     E_lpm = t_lpm * I_lpm * V_volt;

    

    printf("broadcast message sent\n");
    printf("Transmit energy is: %lu\n", E_transmit);
    printf("Receive energy is: %lu\n", E_receiv);
    printf("CPU energy is: %lu\n", E_cpu);
    printf("LPM energy is: %lu\n", E_lpm);            
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/