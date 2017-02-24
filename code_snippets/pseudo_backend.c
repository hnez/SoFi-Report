#define NUM_SDRS (4)
#define FFT_LEN (1024)
#define SYNC_LEN (1<<17)
#define CENTER_FREQ (975*100*1000)

#include "sdr.h"
#include "fft_thread.h"
#include "synchronize.h"
#include "window.h"
#include "combiner.h"

int main(int argc, char **argv)
{
  struct sdr devs[NUM_SDRS]= {0};
  struct fft_thread ffts[NUM_SDRS]= {0};
  float *window= window_hamming(FFT_LEN);

  /* Setup the radios and FFT threads */
  for (int i=0; i<NUM_SDRS; i++) {
    char path[128];

    sprintf(path, "/dev/swradio%d", i);

    sdr_open(&devs[i], path);
    sdr_connect_buffers(&devs[i], 32);
    sdr_set_center_freq(&devs[i], CENTER_FREQ);
    ft_setup(&ffts[i], &devs[i], window,
             FFT_LEN, 32, 1, true);
  }

  /* Tell the radios to start
   * aquiring samples */
  for (int i=0; i<NUM_SDRS; i++) {
    sdr_start(&devs[i]);
  }

  /* By default the SDRs work at a very slow
   * sample rate. Ramping it up is deferred until
   * now to minimize the number of samples recorded
   * in the long startup phase */
  for (int i=0; i<NUM_SDRS; i++) {
    sdr_set_sample_rate(&devs[i], 2000000);
  }

  /* Discard samples to synchronize the receivers */
  sync_sdrs(devs, NUM_SDRS, SYNC_LEN);

  /* Start the FFT threads */
  for (int i=0; i<NUM_SDRS; i++) {
    ft_start(&ffts[i]);
  }

  /* Calculate and output phase differences */
  cb_run(STDOUT_FILENO, ffts, NUM_SDRS);

  return(0);
}
