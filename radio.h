#ifndef _RADIO_H
#define _RADIO_H

#include <SoapySDR/Device.h>
#include <SoapySDR/Formats.h>
#include <liquid/liquid.h>
#include <complex.h>

typedef struct _radio_t
{
  SoapySDRDevice *device;
  SoapySDRStream *tx_stream;
  SoapySDRStream *rx_stream;
} radio_t;

int radio_open(const char *driver, double tx_frequency, radio_t *radio);
int radio_loopback_enable(radio_t *radio);

#endif
