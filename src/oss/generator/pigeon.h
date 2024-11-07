// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "../Scheme.h"

namespace oss::generator {

auto pigeon(int minK, int K, int sigma) -> Scheme;
auto pigeon_trivial(int minK, int K) -> Scheme;
auto pigeon_opt(int minK, int K) -> Scheme;

}
