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

namespace gr { namespace VHFPulseDetect {

pulse_detect__ff::sptr pulse_detect__ff::make()
{
    return gnuradio::get_initial_sptr (new pulse_detect__ff_impl());
}


pulse_detect__ff_impl::~pulse_detect__ff_impl()
{

}

#if 0
    # https://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data/22640362#22640362

    We sort of follow this algorithm

    # Let y be a vector of timeseries data of at least length lag+2
    # Let mean() be a function that calculates the mean
    # Let std() be a function that calculates the standard deviaton
    # Let absolute() be the absolute value function

    # Settings (the ones below are examples: choose what is best for your data)
    set lag to 5;          # lag 5 for the smoothing functions
    set threshold to 3.5;  # 3.5 standard deviations for signal
    set influence to 0.5;  # between 0 and 1, where 1 is normal influence, 0.5 is half

    # Initialise variables
    set signals to vector 0,...,0 of length of y;   # Initialise signal results
    set filteredY to y(1),...,y(lag)                # Initialise filtered series
    set avgFilter to null;                          # Initialise average filter
    set stdFilter to null;                          # Initialise std. filter
    set avgFilter(lag) to mean(y(1),...,y(lag));    # Initialise first value
    set stdFilter(lag) to std(y(1),...,y(lag));     # Initialise first value

    for i=lag+1,...,t do
      if absolute(y(i) - avgFilter(i-1)) > threshold*stdFilter(i-1) then
        if y(i) > avgFilter(i-1) then
          set signals(i) to +1;                     # Positive signal
        else
          set signals(i) to -1;                     # Negative signal
        end
        # Make influence lower
        set filteredY(i) to influence*y(i) + (1-influence)*filteredY(i-1);
      else
        set signals(i) to 0;                        # No signal
        set filteredY(i) to y(i);
      end
      # Adjust the filters
      set avgFilter(i) to mean(filteredY(i-lag),...,filteredY(i));
      set stdFilter(i) to std(filteredY(i-lag),...,filteredY(i));
    end
#endif

pulse_detect__ff_impl::pulse_detect__ff_impl()
    : gr::sync_block        ("pulse_detect__ff", gr::io_signature::make(1, 1, sizeof(float)), gr::io_signature::make(6, 6, sizeof(float)))
    , _sampleCount          (0)
    , _noPulseTime          (3)
    , _sampleRate           (3000000.0 / 256.0)
    , _pulseSampleCount     (0)
    , _pulseMax             (0)
    , _lastPulseSeconds     (0)
    , _trackingPossiblePulse(false)
    , _threshold            (4.0)
    , _movingAvg            (0)
    , _movingVariance       (0)
    , _movingStdDev         (0)
    , _nextLagWindowIndex   (0)
{
    memset(_rgMovingAvg,          0, sizeof(_rgMovingAvg));
    memset(_rgMovingAvgPart,      0, sizeof(_rgMovingAvgPart));
    memset(_rgMovingVariancePart, 0, sizeof(_rgMovingVariancePart));
}


int pulse_detect__ff_impl::work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
{
    const float *in = (const float *) input_items[0];

    float *rgOutPulseDetect =   (float *) output_items[0];
    float *rgOutPulseValue =    (float *) output_items[1];
    float *rgOutMovingAvg =     (float *) output_items[2];
    float *rgOutMovingVar =     (float *) output_items[3];
    float *rgOutMovingStdDev =  (float *) output_items[4];
    float *rgOutThreshold =     (float *) output_items[5];

    for (int i=0; i<noutput_items; i++) {
        rgOutPulseDetect[i] =   0; // no pulse detected
        rgOutPulseValue[i] =    0;
        rgOutMovingAvg[i] =     0;
        rgOutMovingVar[i] =     0;
        rgOutMovingStdDev[i] =  0;
        rgOutThreshold[i] =     0;

        _sampleCount++;

        double pulseValue = in[i];
        if (std::isnan(pulseValue)) {
            continue;
        }

        double curSampleSeconds = _sampleCount / _sampleRate; 

        double usedAvg, usedStdDev;
        bool lagWindowFull = _nextLagWindowIndex == _cLagWindow;

        if (lagWindowFull) {
            if (_trackingPossiblePulse) {
                if (pulseValue - _movingAvg < _threshold * _movingStdDev) {
                    if (_pulseSampleCount > _cMinPulseSampleCount) {
                        printf("Trailing edge pulseValue(%f) pulseMax(%f) pulseSampleCount(%d)\n", pulseValue, _pulseMax, _pulseSampleCount);
                        rgOutPulseDetect[i] = _pulseMax;
                        _lastPulseSeconds = curSampleSeconds;
                    } else {
                        printf("Short pulseValue(%f) pulseMax(%f) pulseSampleCount(%d)\n", pulseValue, _pulseMax, _pulseSampleCount);
                    }
                    _trackingPossiblePulse = false;
                    _pulseMax = 0;
                    _pulseSampleCount = 0;
                } else {
                    _pulseMax = pulseValue;
                    _pulseSampleCount++;
                }
            } else {
                if (pulseValue - _movingAvg > _threshold * _movingStdDev) {
                    printf("Leading edge pulseValue(%f)\n", pulseValue);
                    _trackingPossiblePulse =  true;
                    _pulseMax = pulseValue;
                    _pulseSampleCount = 1;
                }
            }
        }

        // Update moving average
        int lastMovingIndex = 0;
        if (lagWindowFull) {
            _movingAvg -= _rgMovingAvgPart[0];
            memmove(&_rgMovingAvgPart[0], &_rgMovingAvgPart[1], (_cLagWindow - 1) * sizeof(_rgMovingAvgPart[0]));
            lastMovingIndex = _cLagWindow - 1;
        } else {
            lastMovingIndex = _nextLagWindowIndex;
        }
        double movingPart = pulseValue / static_cast<double>(_cLagWindow);
        _movingAvg += movingPart;
        _rgMovingAvgPart[lastMovingIndex] = movingPart;
        _rgMovingAvg[lastMovingIndex] = _movingAvg;

        // Update moving variance
        if (lagWindowFull) {
            _movingVariance -= _rgMovingVariancePart[0];
            memmove(&_rgMovingVariancePart[0], &_rgMovingVariancePart[1], (_cLagWindow - 1) * sizeof(_rgMovingVariancePart[0]));
            lastMovingIndex = _cLagWindow - 1;
        } else {
            lastMovingIndex = _nextLagWindowIndex;
        }
        movingPart = pow(pulseValue - _rgMovingAvg[lastMovingIndex], 2);
        _movingVariance += movingPart;
        _rgMovingVariancePart[lastMovingIndex] = movingPart;
        _movingStdDev = sqrt(_movingVariance / static_cast<double>(_cLagWindow));

        if (!lagWindowFull) {
            _nextLagWindowIndex++;
        }

        rgOutPulseValue[i] =    pulseValue;
        rgOutMovingAvg[i] =     _movingAvg;
        rgOutMovingVar[i] =     _movingVariance / static_cast<double>(_cLagWindow);
        rgOutMovingStdDev[i] =  _movingStdDev;
        rgOutThreshold[i] =     _movingAvg + (_threshold * _movingStdDev);

        if (curSampleSeconds - _lastPulseSeconds > _noPulseTime) {
            _lastPulseSeconds = curSampleSeconds; 
            _trackingPossiblePulse = false; 
            _pulseSampleCount = 0; 
            _pulseMax = 0; 
            printf("No pulse for %f seconds\n", _noPulseTime); 
        } 

        //printf("%f %f %f %f\n", pulseValue, _movingAvg, _movingVariance / static_cast<double>(_cLagWindow), _movingStdDev);
    }

    return noutput_items;
}

  } /* namespace VHFPulseDetect */
} /* namespace gr */

