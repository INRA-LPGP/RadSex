#include "freq.h"


void freq(Parameters& parameters) {

    /* The freq function parses through a file generated by process_reads to compute the distribution of markers
     * between individuals. The output file has the following structure:
     * Number of individuals | Number of markers found in this number of individuals
     */

    std::chrono::steady_clock::time_point t_begin = std::chrono::steady_clock::now();
    log("RADSex freq started");

    Header header = get_header(parameters.markers_table_path);
    Popmap popmap;  // Create dummy popmap
    uint n_individuals = header.size() - 1; // Number of columns - 2 (id and seq columns) +1 (because range is 0 - n_individuals)

    bool parsing_ended = false;
    MarkersQueue markers_queue;
    std::mutex queue_mutex;

    std::vector<uint32_t> frequencies(n_individuals, 0);

    std::thread parsing_thread(table_parser, std::ref(parameters), std::ref(popmap), std::ref(markers_queue), std::ref(queue_mutex), std::ref(header), std::ref(parsing_ended), true, true);
    std::thread processing_thread(processor, std::ref(markers_queue), std::ref(queue_mutex), std::ref(frequencies), std::ref(parsing_ended), 100);

    parsing_thread.join();
    processing_thread.join();

    std::ofstream output_file = open_output(parameters.output_file_path);

    output_file << "Frequency" << "\t" << "Count" << "\n";

    // Cannot iterate over the map normally as it
    for (uint i=1; i < n_individuals; ++i) output_file << i << "\t" << frequencies[i] << "\n";  // Iterate over the map

    output_file.close();

    log("RADSex freq ended (total runtime: " + get_runtime(t_begin) + ")");
}


void processor(MarkersQueue& markers_queue, std::mutex& queue_mutex, std::vector<uint32_t>& frequencies, bool& parsing_ended, ulong batch_size) {

    // Give 100ms headstart to table parser thread (to get number of individuals from header)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::vector<Marker> batch;
    bool keep_going = true;

    uint marker_processed_tick = static_cast<uint>(markers_queue.n_markers / 100);
    uint64_t n_processed_markers = 0;

    while (keep_going) {

        // Get a batch of markers from the queue
        batch = get_batch(markers_queue, queue_mutex, batch_size);

        if (batch.size() > 0) {  // Batch not empty

            for (auto& marker: batch) {

                ++frequencies[marker.n_individuals];
                log_progress(n_processed_markers, marker_processed_tick);
            }

        } else {

            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Batch empty: wait 10ms before asking for another batch

        }

        if (parsing_ended and markers_queue.markers.size() == 0) keep_going = false;
    }
}
