// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "Scheme.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <optional>
#include <type_traits>

namespace oss {

auto expandCount(int oldLen, int newLen) -> std::vector<int>;

bool isExpandable(SearchTree s, int newLen);

auto expandPI(std::vector<int> const& pi, int _newLen) -> std::vector<int>;

auto expandLowerBound(std::vector<int> const& pi, std::vector<int> bound, int _newLen) -> std::vector<int>;

auto expandUpperBound(std::vector<int> const& pi, std::vector<int> bound, int _newLen) -> std::vector<int>;

auto expand(SearchTree s, int newLen) -> std::optional<SearchTree>;
auto expand(Scheme ss, int newLen) -> Scheme;

}
