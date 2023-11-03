/* -*- c++ -*- */
/*
 * Copyright 2023 Jan Gromes <gromes.jan@gmail.com>
 *
 * This file is part of gr-satellites
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_SATELLITES_PDU_INSERT_BYTES_IMPL_H
#define INCLUDED_SATELLITES_PDU_INSERT_BYTES_IMPL_H

#include <satellites/pdu_insert_bytes.h>

#define PDU_INSERT_TYPE_BITS 0
#define PDU_INSERT_TYPE_BYTES 1

namespace gr {
namespace satellites {

class pdu_insert_bytes_impl : public pdu_insert_bytes
{
private:
    int d_pos;
    std::vector<uint8_t> d_bytes;

public:
    pdu_insert_bytes_impl(int pos, const std::string& bytes);
    ~pdu_insert_bytes_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    void msg_handler(pmt::pmt_t pmt_msg);
};

} // namespace satellites
} // namespace gr

#endif /* INCLUDED_SATELLITES_PDU_INSERT_BYTES_IMPL_H */
