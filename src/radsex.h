#pragma once
#include <map>
#include <iostream>
#include "analysis.h"
#include "sex_distribution.h"
#include "process_reads.h"
#include "subset.h"
#include "frequencies.h"
#include "demultiplexing.h"
#include "group_loci.h"
#include "significant_sequences.h"

class RadSex {

    public:
        // Command-line arguments are stored as variables to be used by analysis-specific parsers
        int argc;
        char** argv;

        // Store the type of analysis to run detected from the command-line arguments in the RadSex constructor
        Analysis analysis;

        // Store all parameter values and properties inside a list.
        Parameters parameters;

        // Define all analyses
        std::map<std::string, Analysis> analyses { {"sex_distribution", Analysis("sex_distribution", "Compute a matrix of sequences distribution between sexes",
                                                                                 std::vector<std::string> {"input_file_path", "output_file_path", "popmap_file_path", "min_cov"},
                                                                                 sex_distribution)},
                                                   {"process_reads", Analysis("process_reads", "Compute a matrix of coverage from a set of demultiplexed reads files",
                                                                              std::vector<std::string> {"input_dir_path", "output_file_path", "n_threads", "min_cov"},
                                                                              process_reads)},
                                                   {"subset", Analysis("subset", "Extract a subset of the coverage matrix",
                                                                       std::vector<std::string> {"input_file_path", "output_file_path", "popmap_file_path", "min_cov",
                                                                                                 "min_males", "min_females", "max_males", "max_females", "min_individuals", "max_individuals"},
                                                                       subset)},
                                                   {"frequencies", Analysis("frequencies", "Calculate the frequency of markers in the population",
                                                                            std::vector<std::string> {"input_file_path", "output_file_path", "min_cov"},
                                                                            frequencies)},
//                                                   {"demultiplexing", Analysis("demultiplexing", "Demultiplexes a set of reads files",
//                                                                            std::vector<std::string> {"input_file_path", "output_dir_path", "barcodes_file_path", "min_cov"},
//                                                                            demultiplexing)},
                                                   {"group_loci", Analysis("group_loci", "Recreate polymorphic loci from a subset of coverage matrix",
                                                                              std::vector<std::string> {"input_file_path", "coverage_matrix_path", "output_file_path",
                                                                                                        "max_distance", "n_threads", "min_cov"},
                                                                              group_loci)},
                                                   {"significant_sequences", Analysis("significant_sequences", "Extract sequences significantly associated with sex from the coverage matrix.",
                                                                              std::vector<std::string> {"input_file_path", "output_file_path", "popmap_file_path", "min_cov"},
                                                                              significant_sequences)},
                                                 };

        // In the constructor, the type of analysis is detected and all analysis objects are initialized
        RadSex(int& argc, char** argv);

        // Parse command-line arguments
        void parse();

        // Runs the selected analysis
        void run();

        // Print the general radsex usage
        void usage();

        // Print usage for the specified analysis
        void usage(Analysis analysis);

};
