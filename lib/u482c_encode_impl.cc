/* -*- c++ -*- */
/*
 * Copyright 2023 Jan Gromes <gromes.jan@gmail.com>
 *
 * This file is part of gr-satellites
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>

#include <cstdio>

#include <new>
#include <utility>

#include "u482c_encode_impl.h"
#include <gnuradio/io_signature.h>

extern "C" {
#include "golay24.h"
#include "libfec/fec.h"
#include "randomizer.h"
#include "viterbi.h"
}

#define AUTO -1
#define OFF 0
#define ON 1

namespace gr {
namespace satellites {

u482c_encode::sptr u482c_encode::make(bool verbose, int viterbi, int scrambler, int rs)
{
    return gnuradio::make_block_sptr<u482c_encode_impl>(verbose, viterbi, scrambler, rs);
}

/*
 * The private constructor
 */
u482c_encode_impl::u482c_encode_impl(bool verbose, int viterbi, int scrambler, int rs)
    : gr::block("u482c_encode",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_verbose(verbose),
      d_viterbi(viterbi),
      d_scrambler(scrambler),
      d_rs(rs)
{
    // init FEC
    if (d_viterbi != OFF) {
        int16_t polys[2] = { V27POLYA, V27POLYB };
        set_viterbi_polynomial_packed(polys);
        d_vp = create_viterbi_packed(d_rs_len * 8);
        if (!d_vp)
            throw std::bad_alloc();
    }

    if (d_scrambler != OFF) {
        ccsds_generate_sequence(d_ccsds_sequence.data(), d_ccsds_sequence.size());
    }

    message_port_register_out(pmt::mp("out"));
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->msg_handler(msg); });
}

/*
 * Our virtual destructor.
 */
u482c_encode_impl::~u482c_encode_impl()
{
    if (d_vp)
        delete_viterbi_packed(d_vp);
}

void u482c_encode_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
}

int u482c_encode_impl::general_work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    return 0;
}

void u482c_encode_impl::msg_handler(pmt::pmt_t pmt_msg)
{
    size_t frame_len(0);
    auto msg = pmt::u8vector_elements(pmt::cdr(pmt_msg), frame_len);

    auto data_len = std::min(frame_len, d_data.size());
    memset(d_data.data(), 0, d_data.size());
    memcpy(&d_data[d_header_len], msg, data_len);

    // RS encoding
    size_t tx_len = frame_len + d_header_len;
    auto packet = &d_data[d_header_len];
    if (d_rs == ON) {
        encode_rs_8(packet, &d_data[d_header_len + frame_len], RS_LEN - (frame_len + 32));
        frame_len += 32;
        tx_len += 32;
    }
    // Scrambling
    if (d_scrambler == ON) {
        ccsds_xor_sequence(packet, d_ccsds_sequence.data(), frame_len);
    }

    // Viterbi encoding
    if (d_viterbi == ON) {
        if (d_verbose) {
            std::printf("TODO implement Viterbi encoder.\n");
        }

    } else {
        tx_len = frame_len;
    }

    if (d_verbose) {
        std::printf("Golay encoding length field. Frame length: %ld, Viterbi: %d, "
                    "Scrambler %d, Reed-Solomon: %d.\n",
                    frame_len,
                    d_viterbi,
                    d_scrambler,
                    d_rs);
    }

    // encode length field
    uint32_t length_field = frame_len & 0xff;
    // TODO these flags seem to be broken, at least on AX.100 in Mode 5
    /*if(d_viterbi == ON) { length_field |= 0x100; }
    if(d_scrambler == ON) { length_field |= 0x200; }
    if(d_rs == ON) { length_field |= 0x400; }*/
    encode_golay24(&length_field);
    d_data[0] = (length_field & 0xff0000) >> 16;
    d_data[1] = (length_field & 0x00ff00) >> 8;
    d_data[2] =  length_field & 0x0000ff;
    std::printf("tx_len=%ld\n", tx_len);

    // Send via GNU Radio message
    message_port_pub(pmt::mp("out"),
                     pmt::cons(pmt::PMT_NIL, pmt::init_u8vector(tx_len, d_data.data())));
}
} /* namespace satellites */
} /* namespace gr */
