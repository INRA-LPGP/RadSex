#include "distrib.h"

void distrib(Parameters& parameters) {

    /* The sex_distribution function parses through a file generated by process_reads and checks for each sequence
     * the number of males and females in which the sequence was found. The output is a table with five columns:
     * Number of males | Number of females | Number of sequences | P-value | Significant
     *     <int>       |       <int>       |       <int>         | <float> |   <bool>
     */

    Popmap popmap = load_popmap(parameters);
    std::string group1 = parameters.group1;
    std::string group2 = parameters.group2;

    // Find number of males and females

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

        // Vector of group for each individual (by column index)
        std::vector<std::string> sex_columns = get_column_sex(popmap.groups, line);

        // Define variables used to read the file
        char buffer[65536];
        uint k = 0, field_n = 0;
        sd_table results;
        std::unordered_map<std::string, uint> sex_count;

        do {

            // Read a chunk of size given by the buffer
            input_file.read(buffer, sizeof(buffer));
            k = static_cast<uint>(input_file.gcount());

            for (uint i=0; i<k; ++i) {

                // Read the buffer character by character
                switch(buffer[i]) {

                    case '\t':  // New field
                        if (field_n > 2 and static_cast<uint>(std::stoi(temp)) >= parameters.min_depth) ++sex_count[sex_columns[field_n]];  // Increment the appropriate counter
                        temp = "";
                        ++field_n;
                        break;

                    case '\n':  // New line (also a new field)
                        if (field_n > 2 and static_cast<uint>(std::stoi(temp)) >= parameters.min_depth) ++sex_count[sex_columns[field_n]];  // Increment the appropriate counter

                        ++results[sex_count[group1]][sex_count[group2]].first; // Update the results
                        // Reset variables
                        temp = "";
                        field_n = 0;
                        sex_count[group1] = 0;
                        sex_count[group2] = 0;
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
        for (uint f=0; f <= popmap.counts[group1]; ++f) {
            for (uint m=0; m <= popmap.counts[group2]; ++m) {
                if (f + m != 0) {
                    chi_squared = get_chi_squared(f, m, popmap.counts[group1], popmap.counts[group2]);
                    results[f][m].second = std::min(1.0, get_chi_squared_p(chi_squared)); // p-value corrected with Bonferroni, with max of 1
                }
            }
        }

        // Generate the output file
        if (!parameters.output_matrix) {

            output_distrib(parameters.output_file_path, results, popmap.counts[group1], popmap.counts[group2], group1, group2,
                           parameters.signif_threshold, parameters.disable_correction);

        } else {

            output_distrib_matrix(parameters.output_file_path, results, popmap.counts[group1], popmap.counts[group2]);

        }
    }
}