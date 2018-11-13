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

#ifndef INCLUDED_VHFPULSEDETECT_PULSE_DETECT__FF_IMPL_H
#define INCLUDED_VHFPULSEDETECT_PULSE_DETECT__FF_IMPL_H

#include <VHFPulseDetect/pulse_detect__ff.h>

namespace gr {
  namespace VHFPulseDetect {

    class pulse_detect__ff_impl : public pulse_detect__ff
    {
     private:
      int     _sampleCount;
      double  _sampleRate;
      int     _pulseSampleCount;
      float   _pulseTriggerValue;
      float   _backgroundNoise;
      float   _snrThreshold;
      float   _pulseMax;
      double  _lastPulseSeconds;

     public:
      pulse_detect__ff_impl();
      ~pulse_detect__ff_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace VHFPulseDetect
} // namespace gr

#endif /* INCLUDED_VHFPULSEDETECT_PULSE_DETECT__FF_IMPL_H */
