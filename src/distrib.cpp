#include "distrib.h"

void distrib(Parameters& parameters) {

    /* The sex_distribution function parses through a file generated by process_reads and checks for each sequence
     * the number of males and females in which the sequence was found. The output is a table with five columns:
     * Number of males | Number of females | Number of sequences | P-value | Significant
     *     <int>       |       <int>       |       <int>         | <float> |   <bool>
     */

    std::unordered_map<std::string, bool> popmap = load_popmap(parameters);

    // Find number of males and females
    uint n_males = 0, n_females = 0;
    for (auto i: popmap) if (i.second) ++n_males; else ++n_females;

    std::ifstream input_file;
    input_file.open(parameters.markers_table_path);

    if (input_file) {

        std::vector<std::string> line;
        std::string temp = "";

        // First line is a comment with number of markers in the table
        std::getline(input_file, temp);
        line = split(temp, " : ");
        if (line.size() == 2) uint n_markers = static_cast<uint>(std::stoi(line[1]));

        // Second line is the header. The header is parsed to get the sex of each field in the table.
        std::getline(input_file, temp);
        line = split(temp, "\t");

        // Map with column number --> index of sex_count (0 = male, 1 = female, 2 = no sex)
        std::unordered_map<uint, uint> sex_columns = get_column_sex(popmap, line);

        // Define variables used to read the file
        char buffer[65536];
        uint k = 0, field_n = 0;
        sd_table results;
        uint sex_count[3] = {0, 0, 0}; // Index: 0 = male, 1 = female, 2 = no sex

        do {

            // Read a chunk of size given by the buffer
            input_file.read(buffer, sizeof(buffer));
            k = static_cast<uint>(input_file.gcount());

            for (uint i=0; i<k; ++i) {

                // Read the buffer character by character
                switch(buffer[i]) {

                    case '\t':  // New field
                        if (sex_columns[field_n] != 2 and static_cast<uint>(std::stoi(temp)) >= parameters.min_depth) ++sex_count[sex_columns[field_n]];  // Increment the appropriate counter
                        temp = "";
                        ++field_n;
                        break;

                    case '\n':  // New line (also a new field)
                        if (sex_columns[field_n] != 2 and static_cast<uint>(std::stoi(temp)) >= parameters.min_depth) ++sex_count[sex_columns[field_n]];  // Increment the appropriate counter
                        ++results[sex_count[0]][sex_count[1]].first; // Update the results
                        // Reset variables
                        temp = "";
                        field_n = 0;
                        sex_count[0] = 0;
                        sex_count[1] = 0;
                        sex_count[2] = 0;
                        break;

                    default:
                        temp += buffer[i];
                        break;
                }
            }

        } while (input_file);

        input_file.close();

        // Calculate p-values for association with sex for each combination of males and females
        double chi_squared = 0;

        // Compute p-values
        for (uint f=0; f <= n_females; ++f) {
            for (uint m=0; m <= n_males; ++m) {
                if (f + m != 0) {
                    chi_squared = get_chi_squared(m, f, n_males, n_females);
                    results[m][f].second = std::min(1.0, get_chi_squared_p(chi_squared)); // p-value corrected with Bonferroni, with max of 1
                }
            }
        }

        // Generate the output file
        if (!parameters.output_matrix) {
            output_distrib(parameters.output_file_path, results, n_males, n_females, parameters.signif_threshold, parameters.disable_correction);
        } else {
            output_distrib_matrix(parameters.output_file_path, results, n_males, n_females);
        }
    }
}
