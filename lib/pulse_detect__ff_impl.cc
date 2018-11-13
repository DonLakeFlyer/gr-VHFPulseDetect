/* -*- c++ -*- */
/* 
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "pulse_detect__ff_impl.h"

namespace gr {
  namespace VHFPulseDetect {

    pulse_detect__ff::sptr
    pulse_detect__ff::make()
    {
      return gnuradio::get_initial_sptr
        (new pulse_detect__ff_impl());
    }

    /*
     * The private constructor
     */
    pulse_detect__ff_impl::pulse_detect__ff_impl()
      : gr::sync_block    ("pulse_detect__ff", gr::io_signature::make(1, 1, sizeof(float)), gr::io_signature::make(1, 1, sizeof(float)))
      , _sampleCount      (0)
      , _sampleRate       (3000000.0 / 256.0)
      , _pulseSampleCount (0)
      , _pulseTriggerValue(0)
      , _backgroundNoise  (1000)
      , _snrThreshold     (5)
      , _pulseMax         (0)
      , _lastPulseSeconds (0)
    {

    }

    /*
     * Our virtual destructor.
     */
    pulse_detect__ff_impl::~pulse_detect__ff_impl()
    {
    
    }

    int
    pulse_detect__ff_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      for (int i=0; i<noutput_items; i++) {
        out[i] = in[i];

        float pulseValue = in[0];
        if (isnan(pulseValue)) {
          continue;
        }

        _sampleCount++;

        double lastSampleSeconds = _sampleCount / _sampleRate;
        bool pulseTriggered = false;

        if (_pulseSampleCount != 0) {
          if (pulseValue >= _pulseTriggerValue) {
            pulseTriggered = true;
          }
        } else if (pulseValue > _backgroundNoise * _snrThreshold) {
          _pulseTriggerValue = pulseValue;
          pulseTriggered = true;
        }

        if (pulseTriggered) {
          _pulseSampleCount++;
          if (pulseValue > _pulseMax) {
            _pulseMax = pulseValue;
          }
        } else if (_pulseSampleCount != 0) {
          _lastPulseSeconds = lastSampleSeconds;
          printf("True pulse detected pulseMax:secs:length:backgroundNoise %f %f %f %f\n",
                _pulseMax, _lastPulseSeconds, _pulseSampleCount / _sampleRate * 1000.0, _backgroundNoise);
          _pulseSampleCount = 0;
          _pulseMax = 0;
        } else {
          _backgroundNoise = (_backgroundNoise * 0.99) + (pulseValue * 0.01);
        }

        if (lastSampleSeconds > _lastPulseSeconds + 2.1) {
          _lastPulseSeconds = lastSampleSeconds;
          printf("No pulse for 2.1 seconds\n");
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace VHFPulseDetect */
} /* namespace gr */

