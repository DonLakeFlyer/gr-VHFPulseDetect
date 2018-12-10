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
     public:
        pulse_detect__ff_impl(float threshold, int minSamplesForPulse);
        ~pulse_detect__ff_impl();

        virtual float threshold() const { return _threshold; }
        virtual void set_threshold(float threshold) { _threshold = threshold; }

        virtual int minSamplesForPulse() const { return _minSamplesForPulse; }
        virtual void set_minSamplesForPulse(int minSamplesForPulse) { _minSamplesForPulse = minSamplesForPulse; }

        // Where all the action really happens
        int work(int noutput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

     private:
        static const int _cLagWindow =           175 * 20;

        unsigned int _sampleCount;
        int     _minSamplesForPulse;
        double  _noPulseTime;
        double  _sampleRate;
        int     _pulseSampleCount;
        float   _pulseMax;
        double  _lastPulseSeconds;
        bool    _trackingPossiblePulse;
        float   _threshold;
        double  _movingAvg;
        double  _movingVariance;
        double  _movingStdDev;
        double  _rgMovingAvg[_cLagWindow];
        double  _rgMovingAvgPart[_cLagWindow];
        double  _rgMovingVariancePart[_cLagWindow];
        int     _nextLagWindowIndex;
    };

  } // namespace VHFPulseDetect
} // namespace gr

#endif /* INCLUDED_VHFPULSEDETECT_PULSE_DETECT__FF_IMPL_H */

