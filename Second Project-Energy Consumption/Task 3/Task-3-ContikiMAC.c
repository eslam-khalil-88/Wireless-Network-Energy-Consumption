/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Demonstrating the powertrace application with broadcasts
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include "powertrace.h"

#include "dev/button-sensor.h"

#include "project-conf.h"

#include "dev/leds.h"

#include "sys/energest.h"

#include <stdio.h>

#define I_transmit 0.021
#define I_receiv 0.023
#define I_cpu 0.002
#define I_lpm 0.0012
#define V_volt 3.6

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{

  float t_cpu, t_lpm, t_transmit, t_listen;
  unsigned long E_transmit , E_receiv , E_cpu , E_lpm;
  unsigned long time = 0 , radio = 0;

  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  unicast_open(&uc, 146, &unicast_callbacks);

  

  while(1) {
    static struct etimer et;
    rimeaddr_t addr;
    
    etimer_set(&et, CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom("Hello", 5);
    addr.u8[0] = 1;
    addr.u8[1] = 0;
    if(!rimeaddr_cmp(&addr, &rimeaddr_node_addr)) {
      unicast_send(&uc, &addr);
           }

      
   // Write code here
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

    

    printf("Unicast message sent\n");
    printf("Transmit energy is: %lu\n", E_transmit);
    printf("Receive energy is: %lu\n", E_receiv);
    printf("CPU energy is: %lu\n", E_cpu);
    printf("LPM energy is: %lu\n", E_lpm);
   
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/