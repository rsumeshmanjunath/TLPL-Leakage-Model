/* Copyright (c) 2013 Tescase
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// This file contains the statistics functions required
// for the CPA algorithms
//

#ifndef SCA_CPA_STATS_H
#define SCA_CPA_STATS_H

#include <vector>

namespace stats
{

// Calculates the max element in a vector within a given range
float max_in_range(std::vector<float>& vec, unsigned int start, unsigned int range);

// Calculates the mean of a given vector
float mean(std::vector<float>& vec);

// Calculates the sum of squares of a given vector
float sum_of_squares(std::vector<float>& vec);

// Calculates the pearson product-moment correlation coefficient
// of two equally sized vectors
float pearsonr(std::vector<float>& a, std::vector<float>& b);

// Calculates the variance of a given vector
float var(std::vector<float>& vec);

} //end of namespace

#endif
