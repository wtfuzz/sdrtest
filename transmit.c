#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include "radio.h"

static int frame_callback(unsigned char *  _header,
                         int              _header_valid,
                         unsigned char *  _payload,
                         unsigned int     _payload_len,
                         int              _payload_valid,
                         framesyncstats_s _stats,
                         void *           _userdata)
{
  printf("-=[ Packet Callback ]=-\n");
  framesyncstats_print(&_stats);
  printf("Header valid: %d\nPayload valid: %d\n", _header_valid, _payload_valid);

  printf("Header:\n");
  for(int i=0;i<8;i++)
    printf("%02X ", _header[i]);
  printf("\n");

  printf("Payload:\n");
  for(int i=0;i<64;i++)
  {
    printf("%02X ", _payload[i]);
    if((i+1) % 16 == 0)
      printf("\n");
  }

  return 0;
}

static void *tx_thread(void *arg)
{
  radio_t *radio = (radio_t *)arg;

  framegen64 framegen;
  framegen = framegen64_create();
  framegen64_print(framegen);

  complex float buffer[LIQUID_FRAME64_LEN];
  uint8_t header[8];
  uint8_t payload[64];

  memset(header, 0xAB, 8);
  memset(payload, 0x1D, 64);
  
  while(1)
  {
    framegen64_execute(framegen, header, payload, buffer);

    void *buffs[] = {buffer};
    int flags = 0;
    int ret;

    ret = SoapySDRDevice_writeStream(
      radio->device,
      radio->tx_stream,
      (const void * const *) buffs,
      LIQUID_FRAME64_LEN,
      &flags, 0, 100000);
    
  }

  return NULL;
}

static void *rx_thread(void *arg)
{
  radio_t *radio = (radio_t *)arg;

  framesync64 framesync;
  framesync = framesync64_create(&frame_callback, (void *)radio);

  complex float buffer[LIQUID_FRAME64_LEN];

  while(1)
  {
    void *buffs[] = {buffer};
    int flags = 0;
    int ret;
    long long time;

    ret = SoapySDRDevice_readStream(radio->device, radio->rx_stream, buffs, LIQUID_FRAME64_LEN, &flags, &time, 100000);

    if(ret < 0)
    {
      fprintf(stderr, "Read failed\n");
    }

    if(ret != LIQUID_FRAME64_LEN)
      fprintf(stderr, "short read: %d\n", ret);

    if(ret > 0)
      framesync64_execute(framesync, buffer, ret);
  }
}

int main(int argc, char **argv)
{
  radio_t radio;
  pthread_t tx_tid, rx_tid;

  if(radio_open(argv[1], 915e6, &radio) < 0)
  {
    return EXIT_FAILURE;
  }

  radio_loopback_enable(&radio);

  pthread_create(&tx_tid, NULL, tx_thread, (void *)&radio);
  pthread_create(&rx_tid, NULL, rx_thread, (void *)&radio);
  
  pthread_join(tx_tid, NULL);
  pthread_join(rx_tid, NULL);
}
