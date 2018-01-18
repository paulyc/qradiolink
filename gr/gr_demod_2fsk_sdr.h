// Written by Adrian Musceac YO8RZZ , started March 2016.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef GR_DEMOD_2FSK_SDR_H
#define GR_DEMOD_2FSK_SDR_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/endianness.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/digital/clock_recovery_mm_cc.h>
#include <gnuradio/blocks/unpack_k_bits_bb.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/multiply_const_cc.h>
#include <gnuradio/filter/rational_resampler_base_ccf.h>
#include <gnuradio/digital/binary_slicer_fb.h>
#include <gnuradio/blocks/divide_ff.h>
#include <gnuradio/blocks/threshold_ff.h>
#include <gnuradio/analog/rail_ff.h>
#include <gnuradio/blocks/complex_to_real.h>
#include <gnuradio/filter/fft_filter_ccf.h>
#include <gnuradio/filter/fft_filter_ccc.h>
#include <gnuradio/digital/descrambler_bb.h>
#include <gnuradio/fec/decode_ccsds_27_fb.h>
#include <gnuradio/blocks/packed_to_unpacked_bb.h>
#include <gnuradio/blocks/complex_to_mag.h>
#include <gnuradio/blocks/multiply_const_ff.h>
#include <gnuradio/blocks/add_const_ff.h>
#include <gnuradio/blocks/delay.h>
#include "gr_deframer_bb.h"

class gr_demod_2fsk_sdr;

typedef boost::shared_ptr<gr_demod_2fsk_sdr> gr_demod_2fsk_sdr_sptr;
gr_demod_2fsk_sdr_sptr make_gr_demod_2fsk_sdr(int sps=125, int samp_rate=250000, int carrier_freq=1700,
                                          int filter_width=8000);

class gr_demod_2fsk_sdr : public gr::hier_block2
{
public:
    explicit gr_demod_2fsk_sdr(std::vector<int> signature, int sps=4, int samp_rate=8000, int carrier_freq=1600,
                               int filter_width=1800);
    std::vector<unsigned char> *getFrame1();
    std::vector<unsigned char> *getFrame2();

private:
    gr::blocks::multiply_const_cc::sptr _multiply_symbols;
    gr::blocks::float_to_complex::sptr _float_to_complex;
    gr::filter::fft_filter_ccf::sptr _symbol_filter;
    gr::digital::clock_recovery_mm_cc::sptr _clock_recovery;
    gr::filter::rational_resampler_base_ccf::sptr _resampler;
    gr::filter::fft_filter_ccf::sptr _filter;
    gr::filter::fft_filter_ccc::sptr _lower_filter;
    gr::filter::fft_filter_ccc::sptr _upper_filter;
    gr::blocks::complex_to_mag::sptr _mag_lower;
    gr::blocks::complex_to_mag::sptr _mag_upper;
    gr::blocks::divide_ff::sptr _divide;
    gr::blocks::threshold_ff::sptr _threshhold;
    gr::analog::rail_ff::sptr _rail;
    gr::digital::binary_slicer_fb::sptr _binary_slicer;
    gr::blocks::complex_to_real::sptr _complex_to_real;
    gr::digital::descrambler_bb::sptr _descrambler;
    gr::digital::descrambler_bb::sptr _descrambler2;
    gr::blocks::delay::sptr _delay;
    gr::blocks::multiply_const_ff::sptr _multiply_const_fec;
    gr::blocks::add_const_ff::sptr _add;
    gr::fec::decode_ccsds_27_fb::sptr _cc_decoder;
    gr::fec::decode_ccsds_27_fb::sptr _cc_decoder2;
    gr::blocks::packed_to_unpacked_bb::sptr _packed_to_unpacked;
    gr::blocks::packed_to_unpacked_bb::sptr _packed_to_unpacked2;
    gr_deframer_bb_sptr _deframer1;
    gr_deframer_bb_sptr _deframer2;


    int _samples_per_symbol;
    int _samp_rate;
    int _carrier_freq;
    int _filter_width;
    int _target_samp_rate;

};

#endif // GR_DEMOD_2FSK_SDR_H
