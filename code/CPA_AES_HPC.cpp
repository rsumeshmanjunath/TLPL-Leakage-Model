#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include<vector>
#include <fstream>


#include "./src/common/aes-op.hpp"
#include "./src/cpa/cpa.hpp"
#include "./src/common/csv_read.hpp"

#include <iomanip>
#include <unordered_set>


bool write_key_to_file(std::ofstream& file_stream, std::vector<unsigned char> key, bool end_of_line) {
	if (file_stream.is_open()) {
		for (unsigned int i = 0; i < key.size(); i++) {
			file_stream << std::hex << std::setfill('0') << std::setw(2) << static_cast<int> (key.at(i));
		}
		if(end_of_line)
			file_stream << std::endl;
		file_stream.flush();
		return true;
	}
	else
		return false;
}



void run_cpa(int KEY, int NUM_TRACE) {
	
	std::cout << " Starting the experiment on KEY " << KEY << " with " << NUM_TRACE << " traces...\n";
	
	clock_t t = 0;
	t = clock();
	//Uncomment to run 55nm
	std::string data_path				= cpa::DATA_DIR + "input/55nm_2000_power/55nm_2000_power_" + std::to_string(KEY) + ".txt";
	std::string ct_path					= cpa::DATA_DIR + "input/55nm_2000_ct/55nm_2000_" + std::to_string(KEY) + ".txt";
	
	//Uncomment to run 28nm
	//std::string data_path				= cpa::DATA_DIR + "input/28nm/pow_trace/28nm_10000_power_" + std::to_string(KEY) + ".csv";
	//std::string ct_path					= cpa::DATA_DIR + "input/28nm/ct/28nm_10000_" + std::to_string(KEY) + ".txt";
	

	std::string unique_string = std::to_string(KEY) + "_" + std::to_string(NUM_TRACE);

	std::string random_input_file_path	= cpa::DATA_DIR + "output/random_input/random_input_" + unique_string + ".txt";
	std::string cpa_keys_file_path		= cpa::DATA_DIR + "output/cpa_keys/cpa_keys_" + unique_string + ".txt";

	std::vector< std::vector<float> > all_data;
	std::vector< std::vector<unsigned char> > all_ciphertext;
	std::vector< std::vector<float> > randomized_data;
	std::vector< std::vector<unsigned char> > randomized_ciphertext;


	std::ofstream out_file_random_input;
	out_file_random_input.open(random_input_file_path);

	std::ofstream out_file_cpa_keys;
	out_file_cpa_keys.open(cpa_keys_file_path);
    
    std::cout << "\nInput File Locations\n";
    std::cout << "Data file location : " << data_path << std::endl;
    std::cout << "Ciphertext file location : " << ct_path << std::endl;
    
    std::cout <<"\nOutput File Locations\n";
    std::cout << "Random input file location : " << random_input_file_path << std::endl;
    std::cout << "CPA keys file location : " << cpa_keys_file_path << std::endl;
    
    
	int random_number = 0;
	int TOTAL_ITERATION = 1000;
	
	std::cout <<"\nTotal Iteration : " << TOTAL_ITERATION << std::endl;
	
	csv::read_data_nabeel(data_path, all_data);
	csv::read_cipher_nabeel(ct_path, all_ciphertext);


   
	
	for (int cur_iteration = 1; cur_iteration <= TOTAL_ITERATION; cur_iteration++) {
	    
	     /*sub code to select random traces between 1 and 2000*/
	    
	    std::unordered_set<int> random_num_set; 
    	srand(time(NULL) + 1 + KEY + NUM_TRACE + cur_iteration);
    	while (random_num_set.size() != NUM_TRACE) {
    		random_number = rand() % 2000; //TODO need to parameterize based on the total traces
    		random_num_set.insert(random_number);
    	}
    	
    	/*load the randomly selected traces into data and its corresponding ciphertext*/
		randomized_data.clear();
		randomized_ciphertext.clear();
	
    	std::string string_random_num;
    	for (auto it = random_num_set.begin(); it != random_num_set.end(); it++) {
    		int cur_num = (int)*it;
    		string_random_num.append(std::to_string(cur_num) + ";");
    
    		randomized_data.push_back(all_data.at(cur_num));
    		randomized_ciphertext.push_back(all_ciphertext.at(cur_num));
    	}
    	std::vector<std::vector<unsigned char>> extracted_key_all = cpa::cpa_random_data(randomized_data, randomized_ciphertext, NUM_TRACE, std::to_string(KEY),  std::to_string(cur_iteration));
    
    	std::string op_file_head_string = "[" + std::to_string(KEY) + ":" + std::to_string(NUM_TRACE) + ":" + std::to_string(cur_iteration) + "]";
    	out_file_cpa_keys << op_file_head_string;
    	
    	
    	//write_key_to_file(out_file_cpa_keys, extracted_key_all.at(0), false);	out_file_cpa_keys << ":";	write_key_to_file(out_file_cpa_keys, extracted_key_all.at(1), false);
    		for (int i = 0; i < extracted_key_all.size(); i++) {
			write_key_to_file(out_file_cpa_keys, extracted_key_all.at(i), false);
			if (i != (extracted_key_all.size() - 1))
				out_file_cpa_keys << ":";
		}
    	out_file_cpa_keys << std::endl;
    		
    	out_file_random_input << op_file_head_string << string_random_num.substr(0, string_random_num.length() - 1) << std::endl;
    
    	out_file_cpa_keys.flush();
    	out_file_random_input.flush();
    	
	}

	out_file_cpa_keys.close();
	out_file_random_input.close();

	// Stop clock and report times
	t = clock() - t;
	std::cout << "\nThe program executed in " << ((float)t) / CLOCKS_PER_SEC << " seconds\n\n";
}


int main(int argc, char* argv[])
{
	// key-num, trace-count comes from arg
	
	int key = std::stoi(argv[1]);
	int trace_count = std::stoi(argv[2]);
	
	run_cpa(key, trace_count);

	return 0;
}
