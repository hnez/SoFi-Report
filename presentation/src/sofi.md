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

- USB-Dongles, contain a Realtek 2832U DVB-T Decoder IC
- OEM software can play FM-Radio
- FM-radio-feature is implemented using software-defined radio
- Started a "cheap SDR revolution", prices start at just 10€/dongle

---

_step 2:_

_synchronize them_

---

Synchronize them
================

_Problem:_

![SDR Clocks Original](diagrams/sdr_clocks_highlighted.svg)

- Receivers are built down to a prize
- Clock crystal frequencies may deviate
- Makes comparing signal phases difficult
     - LO frequencies are different
     - Sampling frequency is different

---

Synchronize them
================

_Solution:_

![SDR Clocks Chained](diagrams/sdr_clocks_chained_half_highlighted.svg)

- Feed the receivers from a single clock source
- Other frequencies are derived using PLLs<br />
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

![SDR Clocks Chained](diagrams/sdr_clocks_chained_highlighted.svg)

- Used the tuner's clock output
- Receivers are daisy-chained with short wires
- No more signal integrity issues

---

Synchronize them
================

_3rd attempt_

![Clock SRC v3](diagrams/annotated_hw_mods.svg)

- Used the tuner's clock output
- Receivers are daisy-chained with short wires
- No more signal integrity issues

---

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
- High sample-rates & slow startup <br/>
  ⇒ offset may be thousands of samples

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

- Whole samples are discarded <br/>
  ⇒ Synchronization to an accuracy of one sample <br/>
  ⇒ Remaining offsets of up to ±0.5 Samples
- Compensating these offsets in the time-domain <br/>
  ⇒ Fractional resampler (slow)
- Shift in time domain ⇔ phase rotation in frequency domain <br />
  ⇒ Offset can easily be compensated in frequency domain

---

Frequency domain
================

![Preprocessing chain](diagrams/preprocessing_chain.svg)

- Further processing performed in frequency domain
- Downsampled to reduce processing load
- Algorithm uses phase-differences <br/>
  ⇒ Phase differences are calculated prior to downsampling

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

Remaining constant offset caused by LO-phase differences

⇒ compensated by subtracting a constant

![Completely synchronized](diagrams/annotated_fft_phase_zoom.svg)

---

_step 3:_

_direction estimation_

---

Direction estimation
====================

![Phased array](diagrams/phased_array.svg)

Use phase differences between antennas to
estimate the source direction

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

To estimate the direction of arrival the program:

- Generates a vector of expected phase differences for
  every input direction to test
- Calculate the scalar dot product of these vectors
  and a vector of the measured phase vectors
- The test vector that is parralel to the measured phase
  vector corresponds to the direction of arrival
- The scalar dot product has a maximum if both vectors
  are parallel

---

Implementation
==============

    !Python
    def gen_position_matrix(self, wavelength):
        …
        for (idx, test_angle) in enumerate(test_angles):
            rel_angles= edge_angles + test_angle
            pos_mat[idx, :]= rel_wl * np.sin(rel_angles)

[…][code_gen_dir_matrix]

    !Python
    def get_direction_info(self, phases,
                           idx_start, idx_end):
        …
        pmat= self.get_position_matrix(wl_start, wl_end)
        return(pmat @ phase_vector)

[…][code_get_dir_info]

---

_conclusion_

---

Conclusion
==========

![SoFi UI](diagrams/sofi_ui.svg)

- Directional resolution is not great
- Sometimes fails to lock
- Behaves strangely when indoors (reflections?)

---

Outlook
=======

Use FFT to split signal into bins

Use MUSIC to analyze these bins

![FFT and MUSIC based setup](diagrams/fft_music_setup.svg)

---

The software sources are released under the GNU GPLv3 license:

[github.com/hnez/SoFi](https://github.com/hnez/SoFi)

<div style="border-top: 1px solid rgba(255, 255, 255, 0.17); margin-top: 1em; padding-top: 1em;"></div>

The thesis and this presentation are released under the GNU FDLv1.3:

[github.com/hnez/SoFi-Report](https://github.com/hnez/SoFi-Report)


[code_cross_correlate]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/synchronize.c#L130
[code_discard_samples]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/synchronize.c#L184
[code_correlation_len]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/libsofi.c#L108

[code_phase_difference]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/combiner.c#L128
[code_phase_average]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/libsofi/combiner.c#L133

[code_gen_dir_matrix]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/direction.py#L84
[code_get_dir_info]: https://github.com/hnez/SoFi/blob/009fda7257f8ffa8356bcef2e6556562720c2131/direction.py#L127
