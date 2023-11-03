/* -*- c++ -*- */
/*
 * Copyright 2023 Jan Gromes <gromes.jan@gmail.com>
 *
 * This file is part of gr-satellites
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_SATELLITES_PDU_INSERT_BYTES_H
#define INCLUDED_SATELLITES_PDU_INSERT_BYTES_H

#include <gnuradio/block.h>
#include <satellites/api.h>

namespace gr {
namespace satellites {

/*!
 * \brief <+description of block+>
 * \ingroup satellites
 *
 */
class SATELLITES_API pdu_insert_bytes : virtual public gr::block
{
public:
    typedef std::shared_ptr<pdu_insert_bytes> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of satellites::pdu_insert_bytes.
     *
     * To avoid accidental use of raw pointers, satellites::pdu_insert_bytes's
     * constructor is in a private implementation
     * class. satellites::pdu_insert_bytes::make is the public interface for
     * creating new instances.
     */
    static sptr make(int pos, const std::string& bytes);
};

} // namespace satellites
} // namespace gr

#endif /* INCLUDED_SATELLITES_PDU_INSERT_BYTES_H */
