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
      : gr::sync_block        ("pulse_detect__ff", gr::io_signature::make(1, 1, sizeof(float)), gr::io_signature::make(1, 1, sizeof(float)))
      , _sampleCount          (0)
      , _sampleRate           (3000000.0 / 256.0)
      , _pulseSampleCount     (0)
      , _backgroundNoise      (1000)
      , _pulseMax             (-1)
      , _risingThreshold      (2)
      , _fallingThreshold     (0.8)
      , _lastPulseSeconds     (0)
      , _trackingPossiblePulse(false)
      , _pulseComplete        (false)
      , _noPulseTime          (3)
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
        out[i] = 0; // no pulse detected

        float pulseValue = in[i];
        if (std::isnan(pulseValue)) {
          continue;
        }

        _sampleCount++;

        bool pulseRising = false; // Tracks the pulse going up and will go false when the pulse begins to fall back down
        double curSampleSeconds = _sampleCount / _sampleRate;

        if (_trackingPossiblePulse) {
          // Look for a drop in signal larger than threshold
          if (pulseValue < _pulseMax * _fallingThreshold) {
            _pulseComplete = true;
            printf("Pulse stop pulseValue(%f) _pulseSampleCount(%d)\n", pulseValue, _pulseSampleCount);
          } else if (pulseValue > _pulseMax) {
            _pulseMax = pulseValue;
          }
          _pulseSampleCount++;
        } else if (pulseValue > _backgroundNoise * _risingThreshold) {
          _trackingPossiblePulse = true;
          _pulseMax = pulseValue;
          _pulseSampleCount = 1;
          _backgroundNoisePulseStart = _backgroundNoise;
          printf("Pulse start pulseValue(%f) backgroundNoise(%f)\n", pulseValue, _backgroundNoise);
        }

        if (_trackingPossiblePulse && _pulseComplete) {
          double pulseLength = _pulseSampleCount / _sampleRate * 1000.0;
          if (true /*pulseLength > 20.0*/) {
            _lastPulseSeconds = curSampleSeconds;
            out[i] = _pulseMax;
            printf("Full Pulse pulseMax(%f) length(%f) backgroundNoise(%f)\n",
                  _pulseMax, pulseLength, _backgroundNoise);
          } else {
            printf("Short pulse pulseMax(%f) length(%f) backgroundNoise(%f)\n",
                  _pulseMax, pulseLength, _backgroundNoise);            
          }
          _trackingPossiblePulse = false;
          _pulseComplete = false;
          _pulseSampleCount = 0;
          _pulseMax = -1;
        } else {
          _backgroundNoise = (_backgroundNoise * 0.99) + (pulseValue * 0.01);
        }

        if (curSampleSeconds > _lastPulseSeconds + _noPulseTime) {
          _lastPulseSeconds = curSampleSeconds;
          _trackingPossiblePulse = false;
          _pulseComplete = false;
          _pulseSampleCount = 0;
          _pulseMax = -1;
          printf("No pulse for %f seconds backgroundNoise(%f) pulseValue(%f)\n", _noPulseTime, _backgroundNoise, pulseValue);
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace VHFPulseDetect */
} /* namespace gr */

