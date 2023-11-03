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

#include "pdu_insert_bytes_impl.h"
#include <gnuradio/io_signature.h>

#include <algorithm>
#include <vector>

namespace gr {
namespace satellites {

pdu_insert_bytes::sptr pdu_insert_bytes::make(int pos, const std::string& bytes)
{
    return gnuradio::make_block_sptr<pdu_insert_bytes_impl>(pos, bytes);
}

/*
 * The private constructor
 */
pdu_insert_bytes_impl::pdu_insert_bytes_impl(int pos, const std::string& bytes)
    : gr::block("pdu_insert_bytes",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_pos(pos)
{
    if (bytes.length() % 2 != 0) {
        throw "Hex string length not even!";
    }

    for (size_t i = 0; i < bytes.length(); i += 2) {
        std::string str = bytes.substr(i, 2);
        d_bytes.push_back((uint8_t)strtol(str.c_str(), NULL, 16));
    }

    message_port_register_out(pmt::mp("out"));
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->msg_handler(msg); });
}

/*
 * Our virtual destructor.
 */
pdu_insert_bytes_impl::~pdu_insert_bytes_impl() {}

void pdu_insert_bytes_impl::forecast(int noutput_items,
                                     gr_vector_int& ninput_items_required)
{
}

int pdu_insert_bytes_impl::general_work(int noutput_items,
                                        gr_vector_int& ninput_items,
                                        gr_vector_const_void_star& input_items,
                                        gr_vector_void_star& output_items)
{
    return 0;
}

void pdu_insert_bytes_impl::msg_handler(pmt::pmt_t pmt_msg)
{
    std::vector<uint8_t> msg = pmt::u8vector_elements(pmt::cdr(pmt_msg));
    std::vector<uint8_t> extended_msg = std::vector<uint8_t>(msg.begin(), msg.end());
    extended_msg.insert(
        std::next(extended_msg.begin(), d_pos), d_bytes.begin(), d_bytes.end());

    message_port_pub(pmt::mp("out"),
                     pmt::cons(pmt::car(pmt_msg),
                               pmt::init_u8vector(extended_msg.size(), extended_msg)));
}

} /* namespace satellites */
} /* namespace gr */
