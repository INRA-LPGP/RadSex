#include "distrib.h"

void distrib(Parameters& parameters) {

    /* The sex_distribution function parses through a file generated by process_reads and checks for each sequence
     * the number of males and females in which the sequence was found. The output is a table with five columns:
     * Number of males | Number of females | Number of sequences | P-value | Significant
     *     <int>       |       <int>       |       <int>         | <float> |   <bool>
     */

    Popmap popmap = load_popmap(parameters);
    Header header;

    sd_table results;

    bool parsing_ended = false;
    MarkersQueue markers_queue;
    std::mutex queue_mutex;

    std::thread parsing_thread(table_parser, std::ref(parameters), std::ref(popmap), std::ref(markers_queue), std::ref(queue_mutex), std::ref(header), std::ref(parsing_ended), true, false);
    std::thread processing_thread(processor, std::ref(markers_queue), std::ref(parameters), std::ref(queue_mutex), std::ref(results), std::ref(parsing_ended), 100);

    parsing_thread.join();
    processing_thread.join();

    // Calculate p-values for association with sex for each combination of males and females
    double chi_squared = 0;

    // Compute p-values
    for (uint f=0; f <= popmap.counts[parameters.group1]; ++f) {
        for (uint m=0; m <= popmap.counts[parameters.group2]; ++m) {
            if (f + m != 0) {
                chi_squared = get_chi_squared(f, m, popmap.counts[parameters.group1], popmap.counts[parameters.group2]);
                results[f][m].second = std::min(1.0, get_chi_squared_p(chi_squared)); // p-value corrected with Bonferroni, with max of 1
            }
        }
    }

    // Generate the output file
    if (!parameters.output_matrix) {

        output_distrib(parameters.output_file_path, results, popmap.counts[parameters.group1], popmap.counts[parameters.group2], parameters.group1, parameters.group2,
                       parameters.signif_threshold, parameters.disable_correction);

    } else {

        output_distrib_matrix(parameters.output_file_path, results, popmap.counts[parameters.group1], popmap.counts[parameters.group2]);

    }
}


void processor(MarkersQueue& markers_queue, Parameters& parameters, std::mutex& queue_mutex, sd_table& results, bool& parsing_ended, ulong batch_size) {

    std::vector<Marker> batch;
    bool keep_going = true;

    uint marker_processed_tick = static_cast<uint>(markers_queue.n_markers / 100);
    uint64_t n_processed_markers = 0;

    while (keep_going) {

        // Get a batch of markers from the queue
        batch = get_batch(markers_queue, queue_mutex, batch_size);

        if (batch.size() > 0) {  // Batch not empty

            for (auto marker: batch) {

                ++results[marker.groups[parameters.group1]][marker.groups[parameters.group2]].first;
                if (++n_processed_markers % (10 * marker_processed_tick) == 0) std::cerr << "Processed " << n_processed_markers << " markers (" << n_processed_markers / (marker_processed_tick) << " %)" << std::endl;

            }

        } else {

            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Batch empty: wait 10ms before asking for another batch

        }

        if (parsing_ended and markers_queue.markers.size() == 0) keep_going = false;
    }
}
