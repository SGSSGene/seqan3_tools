// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "backtracking.h"
#include <cassert>

namespace oss::generator {

inline auto kucherov(int N, int minK, int K) -> Scheme {
    assert(N > minK);
    assert(N > K);
    assert(minK <= K);
    if (K == 0) {
        return {
            {{1}, {0}, {0}}
        };
    } else if (N == 2 and minK == 0 and K == 1) {
        return {
            {{1, 2}, {0, 0}, {0, 1}},
            {{2, 1}, {0, 0}, {0, 1}}
        };
    } else if (N == 3 and minK == 0 and K == 2) {
        return {
            {{1, 2, 3}, {0, 0, 0}, {0, 2, 2}},
            {{3, 2, 1}, {0, 0, 0}, {0, 1, 2}},
            {{2, 1, 3}, {0, 0, 1}, {0, 1, 2}}
        };
    } else if (N == 4 and minK == 0 and K == 3) {
        return {
            {{1, 2, 3, 4}, {0, 0, 0, 0}, {0, 1, 3, 3}},
            {{2, 1, 3, 4}, {0, 0, 1, 1}, {0, 1, 3, 3}},
            {{3, 4, 2, 1}, {0, 0, 0, 0}, {0, 1, 3, 3}},
            {{4, 3, 2, 1}, {0, 0, 1, 1}, {0, 1, 3, 3}},
        };
    } else if (N == 5 and minK == 0 and K == 4) {
        return {
            {{1, 2, 3, 4, 5}, {0, 0, 0, 0, 0}, {0, 2, 2, 4, 4}},
            {{5, 4, 3, 2, 1}, {0, 0, 0, 0, 0}, {0, 1, 3, 4, 4}},
            {{2, 1, 3, 4, 5}, {0, 0, 1, 3, 3}, {0, 1, 3, 4, 4}},
            {{1, 2, 3, 4, 5}, {0, 0, 1, 3, 3}, {0, 1, 3, 4, 4}},
            {{4, 3, 5, 2, 1}, {0, 0, 0, 1, 1}, {0, 1, 2, 4, 4}},
            {{3, 2, 1, 4, 5}, {0, 0, 0, 1, 3}, {0, 1, 2, 4, 4}},
            {{2, 1, 3, 4, 5}, {0, 0, 1, 2, 4}, {0, 1, 2, 4, 4}},
            {{1, 2, 3, 4, 5}, {0, 0, 0, 3, 4}, {0, 0, 4, 4, 4}},

        };
    } else if (N == 2 and K == 0) {
        return {
            {{1, 2}, {0, 0}, {0, 0}}
        };
    } else if (N == 3 and minK == 0 and K == 1) {
        return {
            {{1, 2, 3}, {0, 0, 0}, {0, 1, 1}},
            {{2, 3, 1}, {0, 0, 0}, {0, 0, 1}}
        };
    } else if (N == 4 and minK == 0 and K == 2) {
        return {
            {{1, 2, 3, 4}, {0, 0, 0, 0}, {0, 1, 1, 2}},
            {{4, 3, 2, 1}, {0, 0, 0, 0}, {0, 1, 2, 2}},
            {{2, 3, 4, 1}, {0, 0, 0, 1}, {0, 0, 1, 2}},
            {{1, 2, 3, 4}, {0, 0, 0, 2}, {0, 0, 2, 2}}
        };
    } else if (N == 5 and minK == 0 and K == 3) {
        return {
            {{1, 2, 3, 4, 5}, {0, 0, 0, 0, 0}, {0, 1, 2, 3, 3}},
            {{2, 3, 4, 5, 1}, {0, 0, 0, 0, 0}, {0, 1, 2, 2, 3}},
            {{3, 4, 5, 2, 1}, {0, 0, 0, 0, 1}, {0, 1, 1, 3, 3}},
            {{4, 5, 3, 2, 1}, {0, 0, 0, 1, 2}, {0, 0, 3, 3, 3}},
        };
    } else if (N == 6 and minK == 0 and K == 4) {
        return {
            {{1, 2, 3, 4, 5, 6}, {0, 0, 0, 0, 0, 0}, {0, 1, 2, 3, 4, 4}},
            {{2, 3, 4, 5, 6, 1}, {0, 0, 0, 0, 0, 0}, {0, 1, 2, 3, 4, 4}},
            {{6, 5, 4, 3, 2, 1}, {0, 0, 0, 0, 0, 1}, {0, 1, 2, 2, 4, 4}},
            {{4, 5, 6, 3, 2, 1}, {0, 0, 0, 0, 1, 2}, {0, 1, 1, 3, 4, 4}},
            {{3, 4, 5, 6, 2, 1}, {0, 0, 0, 0, 2, 3}, {0, 1, 1, 2, 4, 4}},
            {{5, 6, 4, 3, 2, 1}, {0, 0, 0, 1, 3, 3}, {0, 0, 3, 3, 4, 4}},
            {{1, 2, 3, 4, 5, 6}, {0, 0, 0, 3, 3, 3}, {0, 0, 3, 3, 4, 4}},
            {{1, 2, 3, 4, 5, 6}, {0, 0, 0, 0, 4, 4}, {0, 0, 2, 4, 4, 4}},
            {{3, 4, 2, 1, 5, 6}, {0, 0, 0, 1, 2, 4}, {0, 0, 2, 2, 4, 4}},
            {{5, 6, 4, 3, 2, 1}, {0, 0, 0, 0, 4, 4}, {0, 0, 1, 4, 4, 4}},
        };
    }


    return {};
}

}
