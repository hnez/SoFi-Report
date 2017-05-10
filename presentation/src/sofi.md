SoFi
====

![Phased array image](diagrams/phased_array.svg)

Direction of arrival estimation
using commodity hardware

---

_concept_

---

Concept
=======

![FFT Phase differences](diagrams/annotated_fft_phase_zoom.svg)

- Get a few SDRs
- Synchronize them
- Use phase differences to locate signal sources

---

_step 1:_

_get a few SDRs_

---

Get a few SDRs
==============

![Original SDRs](images/sdrs_original.jpg)

- Go to ${ONLINE_MARKET}
- Search for RTL SDR
- Click buy
- Pay ~50€
- Receive four DVB-T sticks that can be used as software defined radio
  devices

---

_RTL SDR:_

![Open SDR](images/sdr_open.jpg)

- DVB-T sticks containing a Realtek 2832U DVB-T Decoder IC
- Bundled Windows software contains a feature to listen to FM-Radio
- Linux developers wanted to add FM-Radio support to their driver
- Noticed, that the chip did not contain a FM Tuner but passed raw
  IQ-Samples to the computer instead and can be tuned to a wide
  range of frequencies
- Started an "SDR revolution" as prices start at just 10€/dongle

---

_step 2:_

_synchronize them_

---

Synchronize them
================

_Problem:_

![SDR Clocks Original](diagrams/sdr_clocks.svg)

- Receivers are built down to a prize
- Clock crystal frequencies deviate due to various factors
- Makes comparing signal phases difficult
     - LO frequencies are different
     - Sampling frequency is different

---

Synchronize them
================

_Solution:_

![SDR Clocks Chained](diagrams/sdr_clocks_chained.svg)

- Feed the receivers from a single clock source
- All the frequencies are derived from this clock using
  phase locked loops <br />
  ⇒ no frequency differences

---

Synchronize them
================

_1st attempt_

![Clock SRC v1](images/clock_src_v1.jpg)

- Used a NAND gate as an inverter to drive the crystal
- Fed two receivers as a proof of concept
- Had some signal integrity issues

---

Synchronize them
================

_2nd attempt_

![Clock SRC v2](images/clock_src_v2.jpg)

- Used a hex inverter to drive the crystal
- Fed four receivers for actual DOA estimation
- Also had signal integrity issues

---

Synchronize them
================

_3rd attempt_

![Clock SRC v3](diagrams/annotated_hw_mods.svg)

- Used the tuner's clock output
- Receivers are daisy-chained with short wires
- No more signal integrity issues

---

Synchronize them
================

_Drifting_ between the receivers is fixed

But there are still _offsets_ to compensate:

- Sample aquisition time
- LO-Phase

---

Sample aquisition offset
========================

![Samples actual time](diagrams/samples_actual_time.svg)

- Devices are not started at the exact same time <br />
  ⇒ Start writing into their buffers at different times

---

Sample aquisition offset
========================

![Samples observed time](diagrams/samples_observed_time.svg)

- Samples at the same position in the buffer
  were not received at the same time
- Because of high sample-rates & slow startup the
  offset may be thousands of samples

---

Sample aquisition offset
========================

![Samples synchronized time](diagrams/samples_synchronized_time.svg)

- Synchronize by [calculating the cross-correlation][code_cross_correlate]
  and [discarding samples][code_discard_samples]
- Uses FFT for fast cross-correlation calculations over
  [`2¹⁸=262144` samples][code_correlation_len]

---

Sample aquisition offset
========================

- By discarding samples the program can only synchronize
  to an accuracy of one sample
- Samples at the same position in the buffers
  may still be offset by upto ±0.5 Samples
- To compensate these offsets in the time-domain
  a slow fractional resampler would be needed
- A shift in the time domain corresponds to a rotation
  of the phase in the frequency domain <br />
  ⇒ Offset can be easily compensated in the frequency domain

---

Frequency domain
================

![Preprocessing chain](diagrams/preprocessing_chain.svg)

- The following processing steps will be performed in
  the frequency domain
- To reduce the processing load the signals are downsampled
- Instead of downsampling the absolute phases, calulate
  the phase differences for every antenna pair

---

Frequency domain
================

![Complex conjugate multiplication](diagrams/complex_conjugate_mul.svg)

- Calculate the phase difference using the
  [complex conjugate multiplication][code_phase_difference]
- Downsample by [averaging][code_phase_average]
  the phase differences


---

Frequency domain
================

_Phase diagram before compensation:_

<img alt="Unsynchronized" style="height: 20%;" src="diagrams/annotated_fft_phase_orig.svg">

_Phase diagram after compensation:_

<img alt="Synchronized" style="height: 20%;" src="diagrams/annotated_fft_phase_timesync.svg">

---

LO-Phase offset
===============

There is still a constant offset caused by LO-phase differences
that can be compensated by subtracting a constant

![Completely synchronized](diagrams/annotated_fft_phase_zoom.svg)

---

_step 3:_

_direction estimation_

---

Direction estimation
====================

Use phase differences between antennas to
estimate the source direction

![Phased array](diagrams/phased_array.svg)

---

Direction estimation
====================

Without noise the direction can be calculated from
the phase difference, wavelength and antenna distance
using simple trigonometry

![Two receiver array](diagrams/angle_two_receivers.svg)

---

Direction estimation
====================

In the presence of noise the phase differences are scaled
down by a common constant

this makes calculating the direction a bit more difficult

---

_implementation_

---

Implementation
==============

Split into two parts:

- Fast multithreaded C-backend
    - Talks to the SDRs using the V4L Linux kernel API
    - Performs sample-accurate synchronization
    - Calculates phase differences from FFT
    - Downsamples
- Slow Python-frontend
    - Performs offset compensations
    - Calculates direction info
    - Displays a GUI

---

Implementation
==============

TODO

---

_conclusion_

---

Conclusion
==========

TODO

[code_cross_correlate]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/synchronize.c#L130
[code_discard_samples]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/synchronize.c#L184
[code_correlation_len]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/libsofi.c#L108

[code_phase_difference]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/combiner.c#L128
[code_phase_average]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/combiner.c#L133
