#include "output.h"

void output_process_reads(std::string& output_file_path, std::vector<std::string>& individuals, std::unordered_map<std::string, std::unordered_map<std::string, uint16_t>>& results, uint min_depth) {

    /* Input:
     * - Path to an output file
     * - A list of individual names
     * - A matrix of results [Individual: [Sequence, Coverage]]
     * Output:
     * - A matrix of depth [Individual: [Sequence, Coverage]]
     */

    FILE* output_file;
    output_file = fopen(output_file_path.c_str(), "w");

    uint id = 0;

    // Prints the header
    std::fprintf(output_file, "id\tsequence");
    for (auto i: individuals) {
        std::fprintf(output_file, "\t%s", i.c_str());
    }
    std::fprintf(output_file, "\n");

    bool print = false;
    // Fill line by line
    for (auto r: results) {
        for (auto i: r.second) {
            if (i.second >= min_depth) {
                print = true;
                break;
            }
        }
        if (print) {
            std::fprintf(output_file, "%i\t%s", id, r.first.c_str());
            for (auto i: individuals) {
                std::fprintf(output_file, "\t%i", r.second[i]);
            }
            std::fprintf(output_file, "\n");

            ++id;
        }
        print = false;
    }

    fclose(output_file);
}



void output_sex_distribution_matrix(std::string& output_file_path, sd_table& results, uint n_males, uint n_females) {

    /* Input:
     * - Path to an output file
     * - A matrix of counts [Males: [Females: Count]]
     * Output:
     * - A matrix of counts (males in columns and females in rows)
     */

    std::ofstream output_file;
    output_file.open(output_file_path);

    results[0][0].first = 0; // Sequences found in none of the individuals (after filtering for minimum coverage) should not appear in the results

    // Generate output
    for (uint f=0; f <= n_females; ++f) {
        for (uint m=0; m <= n_males; ++m) {
            output_file << results[m][f].first;
            (m < n_males) ? output_file << "\t" : output_file << "\n";
        }
    }
}



void output_sex_distribution(std::string& output_file_path, sd_table& results, uint n_males, uint n_females) {

    /* Input:
     * - Path to an output file
     * - A matrix of counts [Males: [Females: Count]]
     * Output: a table with the following structure:
     * Number of males | Number of females | Number of sequences | P-value
     *     <int>       |       <int>       |       <int>         | <float>
     */

    std::ofstream output_file;
    output_file.open(output_file_path);

    // Output file header
    output_file << "Males" << "\t" << "Females" << "\t" << "Markers" << "\t" << "P" << "\t" << "Signif" << "\n";

    uint n_markers = 0;
    double threshold = 0.05;

    // Determine the total number of markers
    for (uint f=0; f <= n_females; ++f) {
        for (uint m=0; m <= n_males; ++m) {
            if (f + m != 0) n_markers += results[m][f].first; // Exclude markers present in 0 individuals
        }
    }

    threshold /= n_markers;

    // Generate output file
    for (uint m=0; m <= n_males; ++m) {
        for (uint f=0; f <= n_females; ++f) {
            if (f + m != 0) {
                output_file << m << "\t" << f << "\t" << results[m][f].first << "\t" << results[m][f].second << "\t" << (results[m][f].second < threshold ? "True" : "False") << "\n";
            }
        }
    }
}



void output_group_loci(std::string& output_file_path, std::unordered_map<std::string, std::vector<Locus>>& results, std::vector<std::string>& header) {

    /* Input:
     * - Path to an output file
     * - A matrix of loci [Sequence ID: [Associated sequences]]
     * Output:
     * - A table with following columns:
     * Locus ID | Marker ID | Sequence | Origin | Cov Ind. 1 | Cov Ind. 2 ...
     */

    std::ofstream output_file;
    output_file.open(output_file_path);

    output_file << "Locus" << "\t" << "Marker" << "\t" << "Sequence" << "\t" << "Origin" << "\t";
    for (uint i=2; i<header.size(); ++i) {
        output_file << header[i];
        if (i < header.size() - 1) output_file << "\t";
    }
    output_file << "\n";

    std::string seq_id;
    uint locus_id = 0;

    for (auto sequence: results) {
        seq_id = sequence.first;

        // First iteration to output the original sequence
        for (auto locus: sequence.second) {
            if (locus.id == seq_id) {
                output_file << locus_id << "\t" << locus.id << "\t" << locus.sequence << "\t";
                if (seq_id == locus.id) output_file << "Original" << "\t"; else output_file << "Recovered" << "\t";
                for (uint i=0; i<locus.coverage.size(); ++i) {
                    output_file << locus.coverage[i];
                    if (i < locus.coverage.size() - 1) output_file << "\t";
                }
                output_file << "\n";
            }
        }

        // Second iteration to output the other sequences (this is not optimized but this step is not intensive anyway)
        for (auto locus: sequence.second) {
            if (locus.id != seq_id) {
                output_file << locus_id << "\t" << locus.id << "\t" << locus.sequence << "\t";
                if (seq_id == locus.id) output_file << "Original" << "\t"; else output_file << "Recovered" << "\t";
                for (uint i=0; i<locus.coverage.size(); ++i) {
                    output_file << locus.coverage[i];
                    if (i < locus.coverage.size() - 1) output_file << "\t";
                }
                output_file << "\n";
            }
        }
        ++locus_id;
    }
}



// Create output file for the mapping analysis
void output_mapping(std::string& output_file_path, std::vector<MappedSequence> sequences) {

    std::ofstream output_file;
    output_file.open(output_file_path);

    output_file << "Sequence" << "\t" << "Contig" << "\t" << "Position" << "\t" << "SexBias" << "\t" << "P" << "\t" << "Signif" << "\n";

    double threshold = 0.05 / sequences.size();

    for (auto s: sequences) {
        output_file << s.id << "\t" << s.contig << "\t" << s.position << "\t" << s.sex_bias << "\t" << s.p << "\t" <<
                       (s.p < threshold ? "True" : "False") << "\n";
    }

    output_file.close();
}
