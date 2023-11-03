#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright 2019 Daniel Estevez <daniel@destevez.net>
#
# This file is part of gr-satellites
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, pdu, blocks

from ... import u482c_encode
from ... import pdu_insert_bytes
from ...utils.options_block import options_block
from gnuradio.pdu import pdu_to_tagged_stream
from gnuradio.blocks import repack_bits_bb, uchar_to_float


_syncword = '930b51de'
_preamble = 'aa'
_preamble_len = 50


class ax100_framer(gr.hier_block2, options_block):
    """
    Hierarchical block to frame the GOMspace AX100 protocols.

    The input are PDUs with frames (assumed to be CSP frames).
    The output is a float stream of soft symbols.

    Args:
        mode: mode to use ('RS' or 'ASM') (string)
        scrambler: scrambler to use, either 'CCSDS' or 'none'
                   (only for ASM mode) (str)
        syncword: syncword to use (str)
        options: Options from argparse
    """
    def __init__(self, mode, scrambler='CCSDS', syncword=_syncword, 
        options=None):
        gr.hier_block2.__init__(
            self,
            'ax100_framer',
            gr.io_signature(0, 0, 0),
            gr.io_signature(1, 1, gr.sizeof_float))
        options_block.__init__(self, options)

        if scrambler not in ['CCSDS', 'none']:
            raise ValueError(f'invalid scrambler {scrambler}')

        self.message_port_register_hier_in("in")

        if mode not in ['RS', 'ASM']:
            raise Exception("Unsupported AX100 mode. Use 'RS' or 'ASM'")

        if mode == 'RS':
            raise Exception("RS mode not yet supported")

        self.fec = u482c_encode(self.options.verbose_fec, 0,
            1 if scrambler == 'CCSDS' else 0, 1)
        self.insert_syncword = pdu_insert_bytes(0, syncword)
        self.insert_preamble = pdu_insert_bytes(0, _preamble_len*_preamble)
        self.framer = pdu_to_tagged_stream(gr.types.byte_t, 'packet_len')
        self.repack = repack_bits_bb(8, 1, '', False, gr.GR_LSB_FIRST)
        self.convert = uchar_to_float()

        self.msg_connect(
            (self, 'in'),
            (self.fec, 'in'))

        self.msg_connect(
            (self.fec, 'out'),
            (self.insert_syncword, 'in'))

        self.msg_connect(
            (self.insert_syncword, 'out'),
            (self.insert_preamble, 'in'))

        self.msg_connect(
            (self.insert_preamble, 'out'),
            (self.framer, 'pdus'))

        self.connect(
            (self.framer, 0),
            (self.repack, 0))

        self.connect(
            (self.repack, 0),
            (self.convert, 0))

        self.connect(
            (self.convert, 0),
            (self, 0))

    @classmethod
    def add_options(cls, parser):
        """
        Adds AX100 framer specific options to the argparse parser
        """
        parser.add_argument(
            '--verbose_fec', action='store_true', help='Verbose FEC decoder')
