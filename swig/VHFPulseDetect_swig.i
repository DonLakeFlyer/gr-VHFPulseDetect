/* -*- c++ -*- */

#define VHFPULSEDETECT_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "VHFPulseDetect_swig_doc.i"

%{
#include "VHFPulseDetect/pulse_detect__ff.h"
%}


%include "VHFPulseDetect/pulse_detect__ff.h"
GR_SWIG_BLOCK_MAGIC2(VHFPulseDetect, pulse_detect__ff);
