
#include <iostream>
#include <string>
#include <vector>
#include <omp.h>
#include <algorithm>
#include <fstream>

#include "../common/aes-op.hpp"
#include "../common/csv_read.hpp"
#include "cpa.hpp"
#include "power-models.hpp"
#include "stats.hpp"
#include <iomanip>
#include <cmath>

using namespace std;


bool bool_TOTAL_55nm = true;
bool bool_TOTAL_28nm = false;

/*Location of power cell library*/
string total_pow_55nm_file_loc = cpa::DATA_DIR + "table/total_pow_55nm.csv";
string total_pow_28nm_file_loc = cpa::DATA_DIR + "table/total_pow_28nm.csv";



/*CPA analysis for one master KEY with fixed number of traces using HD and power Model*/
std::vector<std::vector<unsigned char>> cpa::cpa_random_data(std::vector<std::vector<float>> data, std::vector<std::vector<unsigned char>> ciphertext, int TOTAL_TRACE, string KEY_NUM, std::string CUR_ITERATION)
{
	
	
	std::string ofile_pcc_values_path = cpa::DATA_DIR + "output/pcc/pcc_values_" + KEY_NUM + "_" + std::to_string(TOTAL_TRACE) + "_" + CUR_ITERATION + ".txt";
	//std::string ofile_pcc_values_path = cpa::DATA_DIR + "output\\pcc_values_" + KEY_NUM + ".txt";
	std::ofstream out_file_pcc_values;

	bool bool_pcc_file = false;

	const int num_bytes = 16;
	const int num_keys = 256;

//	const int TOTAL_MODELS = 5; // HD, 55nm total, 55nm leakage, 28nm total, 28nm leakage, asic

	int max_index;
	int pre_row;
	int pre_col;
	int post_row;
	int post_col;
	int byte_id;

	size_t num_traces;
	size_t num_pts;
	size_t trace_start;

	float max_pt;
	float data_pt;

	unsigned char pre_byte;
	unsigned char post_byte;
	unsigned char key_byte;
	unsigned char max_byte;


	// Prepare vectors
	std::vector<std::vector<unsigned char>> round_key_all_models; //PARAMETER
	std::vector<unsigned char> round_key_HD(16);
	std::vector<unsigned char> round_key_55nm_total(16);
	std::vector<unsigned char> round_key_28nm_total(16);
	std::vector< std::vector<unsigned char> > cipher(4, std::vector<unsigned char>(4));


	// Record the number of traces and the number of points per trace
	if (TOTAL_TRACE < data.size())
		num_traces = TOTAL_TRACE;
	else
		num_traces = data.size();

	//num_traces = 400;
	num_pts = data.at(0).size();

	trace_start = 0; //num_pts - round_size;

	// Prepare main vectors
	std::vector< std::vector<float> > r_pts_HD(16, std::vector<float>(256, 0.0f));
	std::vector< std::vector<float> > r_pts_55nm_total(16, std::vector<float>(256, 0.0f));
	std::vector< std::vector<float> > r_pts_28nm_total(16, std::vector<float>(256, 0.0f));

	std::vector<float> power_pts(num_traces, 0.0f);
	std::vector< std::vector< std::vector<float> > > hamming_pts_HD(16, std::vector< std::vector<float> >(256, std::vector<float>(num_traces, 0.0f)));
	std::vector< std::vector< std::vector<float> > > hamming_pts_55nm_total(16, std::vector< std::vector<float> >(256, std::vector<float>(num_traces, 0.0f)));
	std::vector< std::vector< std::vector<float> > > hamming_pts_28nm_total(16, std::vector< std::vector<float> >(256, std::vector<float>(num_traces, 0.0f)));

	//Prepare variables to accomodate library_power values
	vector<vector<float>> pow_model_55nm_total(256, vector<float>(256, 0.0f));
	vector<vector<float>> pow_model_28nm_total(256, vector<float>(256, 0.0f));


	// Loading library Cell Power data into pow_model
	if(bool_TOTAL_55nm) csv::read_Library_cell_power(total_pow_55nm_file_loc, pow_model_55nm_total, 6);
	if(bool_TOTAL_28nm) csv::read_Library_cell_power(total_pow_28nm_file_loc, pow_model_28nm_total, 3);

	// Find the power leakage points
	for (unsigned int i = 0; i < num_traces; i++)
	{
		max_pt = 1.0f * data.at(i).at(trace_start) * pow(10, 6);
		for (unsigned int j = trace_start; j < num_pts; j++)
		{
			data_pt = 1.0f * data.at(i).at(j) * pow(10, 6);
			if (max_pt < data_pt)
				max_pt = data_pt;
		}
		power_pts.at(i) = max_pt;
	}

	// Calcuate HD hamming distances
	for (unsigned int i = 0; i < num_traces; i++)
	{
		// Get cipher for this particular trace
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				cipher.at(k).at(j) = ciphertext.at(i).at(j * 4 + k);

		// Find ciphertext bytes at different stages for the hamming distance calculation
		for (int j = 0; j < num_bytes; j++)
		{
			// Select byte
			post_row = j / 4;
			post_col = j % 4;
			post_byte = cipher.at(post_row).at(post_col);

			// Create all possible bytes that could have resulted selected byte
			for (int k = 0; k < num_keys; k++)
			{
				// Undo AES-128 operations
				key_byte = static_cast<unsigned char> (k);
				aes::shift_rows(post_row, post_col, pre_row, pre_col);
				pre_byte = cipher.at(pre_row).at(pre_col);
				pre_byte = aes::add_round_key(key_byte, pre_byte);
				pre_byte = aes::inv_sub_bytes(pre_byte);
				byte_id = pre_col * 4 + pre_row;

				// Find the hamming distance between the bytes
				hamming_pts_HD.at(byte_id).at(k).at(i) = pm::hamming_dist(pre_byte, post_byte, 8);
				if(bool_TOTAL_55nm) hamming_pts_55nm_total.at(byte_id).at(k).at(i) = pow_model_55nm_total.at(pre_byte).at(post_byte);
				if(bool_TOTAL_28nm) hamming_pts_28nm_total.at(byte_id).at(k).at(i) = pow_model_28nm_total.at(pre_byte).at(post_byte);

			}
		}
	}

	if(bool_pcc_file) out_file_pcc_values.open(ofile_pcc_values_path);

	// Perform Pearson r correlation to find the hamming distance set with the highest correlation to the actual data
	for (int i = 0; i < num_bytes; i++)
	{
		for (int j = 0; j < num_keys; j++)
		{
			r_pts_HD.at(i).at(j) = stats::pearsonr(power_pts, hamming_pts_HD.at(i).at(j));
			if(bool_TOTAL_55nm) r_pts_55nm_total.at(i).at(j) = stats::pearsonr(power_pts, hamming_pts_55nm_total.at(i).at(j));
			if(bool_TOTAL_28nm)	r_pts_28nm_total.at(i).at(j) = stats::pearsonr(power_pts, hamming_pts_28nm_total.at(i).at(j));
		}
	}

	
	// Find the hamming distance set with the highest correlation
	
	string header_text_HD = "[" + KEY_NUM + ":" + std::to_string(TOTAL_TRACE) + ":" + CUR_ITERATION + "]HD: Byte ";
	for (int i = 0; i < num_bytes; i++)
	{
		max_index = 0;
		if (bool_pcc_file) out_file_pcc_values << header_text_HD << dec << i << ":";
		for (unsigned int j = 0; j < r_pts_HD.at(i).size(); j++)
		{
			if (bool_pcc_file) out_file_pcc_values << dec << r_pts_HD.at(i).at(j) << ":";

			if (r_pts_HD.at(i).at(max_index) < r_pts_HD.at(i).at(j))
				max_index = j;
		}
		if (bool_pcc_file) out_file_pcc_values << "\n";

		max_byte = static_cast<unsigned char> (max_index);
		round_key_HD.at(i) = max_byte;
	}

	if (bool_TOTAL_55nm) {
		string header_text_55nm = "[" + KEY_NUM + ":" + std::to_string(TOTAL_TRACE) + ":" + CUR_ITERATION + "]55nm: Byte ";
		for (int i = 0; i < num_bytes; i++)
		{
			max_index = 0;
			if (bool_pcc_file) out_file_pcc_values << header_text_55nm << dec << i << ":";
			for (unsigned int j = 0; j < r_pts_55nm_total.at(i).size(); j++)
			{
				if (bool_pcc_file) out_file_pcc_values << dec << r_pts_HD.at(i).at(j) << ":";

				if (r_pts_55nm_total.at(i).at(max_index) < r_pts_55nm_total.at(i).at(j))
					max_index = j;
			}
			if (bool_pcc_file) out_file_pcc_values << "\n";

			max_byte = static_cast<unsigned char> (max_index);
			round_key_55nm_total.at(i) = max_byte;
		}
	}

	if (bool_TOTAL_28nm) {
		string header_text_28nm = "[" + KEY_NUM + ":" + std::to_string(TOTAL_TRACE) + ":" + CUR_ITERATION + "]28nm: Byte ";
		for (int i = 0; i < num_bytes; i++)
		{
			max_index = 0;
			if (bool_pcc_file) out_file_pcc_values << header_text_28nm << dec << i << ":";
			for (unsigned int j = 0; j < r_pts_28nm_total.at(i).size(); j++)
			{
				if (bool_pcc_file) out_file_pcc_values << dec << r_pts_HD.at(i).at(j) << ":";

				if (r_pts_28nm_total.at(i).at(max_index) < r_pts_28nm_total.at(i).at(j))
					max_index = j;
			}
			if (bool_pcc_file) out_file_pcc_values << "\n";

			max_byte = static_cast<unsigned char> (max_index);
			round_key_28nm_total.at(i) = max_byte;
		}
	}

	

	if (bool_pcc_file) out_file_pcc_values.close();
	
	round_key_all_models.push_back(round_key_HD);
	
	if(bool_TOTAL_55nm)
		round_key_all_models.push_back(round_key_55nm_total);

	if(bool_TOTAL_28nm)
		round_key_all_models.push_back(round_key_28nm_total);

	return round_key_all_models;
}