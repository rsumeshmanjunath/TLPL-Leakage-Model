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
// This file contains the serial and parallel
// CPA algorithms
//

#ifndef SCA_CPA_CPA_H
#define SCA_CPA_CPA_H

#include <string>
#include<vector>

namespace cpa
{
	int const HAMMING_DISTANCE = 0;
	int const  TOTAL_POWER_LIBRARY_55nm = 1;
	int const  TOTAL_POWER_LIBRARY_28nm = 2;
	int const AES_CIRCUIT_POWER = 3;
	int const LEAKAGE_POWER_LIBRARY_55nm = 4;
	
	std::string static DATA_DIR		= "/scratch/srm638/55nm_pow_analysis/data/";

// The serial CPU based CPA function

	/*take random sample of data and ciphertext return extracted key for each power models */
	std::vector<std::vector<unsigned char>> cpa_random_data(std::vector<std::vector<float>> data, std::vector<std::vector<unsigned char>> ciphertext, int TOTAL_TRACE, std::string KEY_NUM,  std::string CUR_ITERATION);



// The parallel GPU based CPA function
//void pcpa(std::string data_path, std::string ct_path);

} //end namespace

#endif
