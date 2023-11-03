#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright 2019-2022 Daniel Estevez <daniel@destevez.net>
#
# This file is part of gr-satellites
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

'''
gr-satellites framer components

The framers transform frames into soft symbols, setting packet
boundaries and performing error correction and checking as needed.

The input to these hierarchical blocks are PDUs with the frames
and the output is a stream of soft symbols.
'''

from .ax100_framer import ax100_framer
