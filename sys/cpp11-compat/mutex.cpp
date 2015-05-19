/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpp11-compat
 * @{
 *
 * @file    mutex.cpp
 * @brief   C++11 mutex drop in replacement
 *
 * @author  Raphael Hiesgen <raphael.hiesgen (at) haw-hamburg.de>
 *
 * @}
 */

#include "riot/mutex.hpp"

namespace riot {

mutex::~mutex() {
  // nop
}

void mutex::lock() { mutex_lock(&m_mtx); }

bool mutex::try_lock() noexcept { return (1 == mutex_trylock(&m_mtx)); }

void mutex::unlock() noexcept { mutex_unlock(&m_mtx); }

} // namespace riot
