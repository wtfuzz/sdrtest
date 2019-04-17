#include <stdio.h>
#include "radio.h"

int radio_open(const char *driver, double tx_frequency, radio_t *radio)
{
  if(driver == NULL)
  {
    fprintf(stderr, "Driver not specified\n");
    return -1;
  }

  SoapySDRKwargs args = {};
  SoapySDRKwargs_set(&args, "driver", driver);

  radio->device = SoapySDRDevice_make(&args);
  if(radio->device == NULL)
  {
    fprintf(stderr, "Failed to open device\n");
    return -1;
  }

  SoapySDRDevice_setSampleRate(radio->device, SOAPY_SDR_TX, 0, 100000);
  SoapySDRDevice_setBandwidth(radio->device, SOAPY_SDR_TX, 0, 1500000);
  SoapySDRDevice_setFrequency(radio->device, SOAPY_SDR_TX, 0, tx_frequency, NULL);
  radio->tx_stream = SoapySDRDevice_setupStream(radio->device, SOAPY_SDR_TX, SOAPY_SDR_CF32, NULL, 0, NULL);
  SoapySDRDevice_activateStream(radio->device, radio->tx_stream, 0, 0, 0); 

  SoapySDRDevice_setSampleRate(radio->device, SOAPY_SDR_RX, 0, 100000);
  SoapySDRDevice_setBandwidth(radio->device, SOAPY_SDR_RX, 0, 1500000);
  SoapySDRDevice_setFrequency(radio->device, SOAPY_SDR_RX, 0, tx_frequency, NULL);
  radio->rx_stream = SoapySDRDevice_setupStream(radio->device, SOAPY_SDR_RX, SOAPY_SDR_CF32, NULL, 0, NULL);
  SoapySDRDevice_activateStream(radio->device, radio->rx_stream, 0, 0, 0); 

  return 0;
}
