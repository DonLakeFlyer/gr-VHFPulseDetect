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


#ifndef INCLUDED_VHFPULSEDETECT_PULSE_DETECT__FF_H
#define INCLUDED_VHFPULSEDETECT_PULSE_DETECT__FF_H

#include <VHFPulseDetect/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace VHFPulseDetect {

    /*!
     * \brief <+description of block+>
     * \ingroup VHFPulseDetect
     *
     */
    class VHFPULSEDETECT_API pulse_detect__ff : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<pulse_detect__ff> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of VHFPulseDetect::pulse_detect__ff.
       *
       * To avoid accidental use of raw pointers, VHFPulseDetect::pulse_detect__ff's
       * constructor is in a private implementation
       * class. VHFPulseDetect::pulse_detect__ff::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace VHFPulseDetect
} // namespace gr

#endif /* INCLUDED_VHFPULSEDETECT_PULSE_DETECT__FF_H */

