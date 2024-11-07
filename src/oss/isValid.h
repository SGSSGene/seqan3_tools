// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "Scheme.h"

#include <algorithm>
#include <cassert>

namespace oss {

/* checks if Scheme is valid
 *
 */
auto isValid(SearchTree const& s) -> bool;

// check if all schemes are valid and have same size
auto isValid(Scheme const& ss) -> bool;

}
