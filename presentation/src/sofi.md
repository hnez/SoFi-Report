SoFi
====

![Phased array image](diagrams/phased_array.svg)

Direction of arrival estimation
using commodity hardware

---

Idea
====

![FFT Phase differences](diagrams/annotated_fft_phase_zoom.svg)

- Take multiple SDRs
- Synchronize them
- Use phase differences to locate signal sources

---

Take multiple SDRs
==================

![Original SDRs](images/sdrs_original.jpg)

- Go to ${ONLINE_MARKET}
- Search for RTL SDR
- Click buy
- Pay ~50€
- Receive four DVB-T sticks that can be used as software defined radio
  devices

---

RTL SDR
=======

![Open SDR](images/sdr_open.jpg)

- DVB-T sticks containing a Realtek 2832U DVB-T Decoder IC
- Bundled Windows software contained a feature to listen to FM-Radio
- Linux developers wanted to add support for FM to their driver
- Noticed, that the chip did not contain a FM Tuner but passed raw
  IQ-Samples to the computer instead and can be tuned to a wide
  range of frequencies
- Started an SDR "revolution"

---

Synchronization
===============

![SDR Clocks Original](diagrams/sdr_clocks.svg)

_Problem:_

- Receivers are built down to a prize
- Clock crystal frequencies deviate due to various factors
- Makes comparing signal phases difficult
     - LO frequencies are different
     - Sampling frequency is different

---

![SDR Clocks Chained](diagrams/sdr_clocks_chained.svg)

_Solution:_

- Feed the receivers from a single clock source
- All the frequencies are derived from this clock using
  phase locked loops <br />
  ⇒ no frequency differences

---

_1st attempt_

![Clock SRC v1](images/clock_src_v1.jpg)

- Used a NAND gate as an inverter to drive the crystal
- Fed two receivers as a proof of concept
- Had some signal integrity issues

---

_2nd attempt_

![Clock SRC v2](images/clock_src_v2.jpg)

- Used a hex inverter to drive the crystal
- Fed four receivers for actual DOA estimation
- Also had signal integrity issues

---

_3rd attempt_

![Clock SRC v3](diagrams/annotated_hw_mods.svg)

- Used the tuner's clock output
- Receivers are daisy-chained with short wires
- No more signal integrity issues

---

Synchronization
===============

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

- Samples at the same position in the buffer do
  were not received at the same time
- Because of high sample-rates & slow startup the
  offset may be thousands of samples

---

Sample aquisition offset
========================

![Samples synchronized time](diagrams/samples_synchronized_time.svg)

- Synchronize by [calculating the cross-correlation][code_cross_correlate]
  and [discarding samples][code_discard_samples]
- Uses FFT and iFFT for fast cross-correlation calculations over
  [`2¹⁸=262144` samples][code_correlation_len]

[code_cross_correlate]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/synchronize.c#L130
[code_discard_samples]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/synchronize.c#L184
[code_correlation_len]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/libsofi.c#L108
