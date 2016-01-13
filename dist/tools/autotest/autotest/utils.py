#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 Martine Lenders <mail@martine-lenders.eu>
#
# Distributed under terms of the MIT license.

class Borg(object):
    """Borg base class

    Multiple bodies, one mind (aka behavior ;-)).

    @see http://code.activestate.com/recipes/66531/
    """
    __shared_state = {}

    def __init__(self):
        self.__dict__ = self.__shared_state
