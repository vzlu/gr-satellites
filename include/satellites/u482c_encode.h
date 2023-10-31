/* -*- c++ -*- */
/*
 * Copyright 2016 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of gr-satellites
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_SATELLITES_U482C_ENCODE_H
#define INCLUDED_SATELLITES_U482C_ENCODE_H

#include <gnuradio/block.h>
#include <satellites/api.h>

namespace gr {
namespace satellites {

/*!
 * \brief <+description of block+>
 * \ingroup satellites
 *
 */
class SATELLITES_API u482c_encode : virtual public gr::block
{
public:
    typedef std::shared_ptr<u482c_encode> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of satellites::u482c_encode.
     *
     * To avoid accidental use of raw pointers, satellites::u482c_encode's
     * constructor is in a private implementation
     * class. satellites::u482c_encode::make is the public interface for
     * creating new instances.
     */
    static sptr make(bool verbose, int viterbi, int scrambler, int rs);
};

} // namespace satellites
} // namespace gr

#endif /* INCLUDED_SATELLITES_U482C_ENCODE_H */
