/*
* Copyright (C) 2020 Romain Feron
* This file is part of RADSex.

* RADSex is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* RADSex is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with RADSex.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include <mutex>
#include <queue>
#include "parameters.h"
#include "popmap.h"
#include "utils.h"


struct Marker {

    std::string id = "";  // Marker ID
    std::string sequence = "";  // Marker sequence
    std::vector<uint16_t> individuals;  // Marker depth in each individual
    uint n_individuals = 0;  // Total number of individuals in which the marker is present
    std::unordered_map<std::string, uint> groups;  // Number of individuals in which the marker is present for each group
    float p = 0;

    void reset(bool sex_stats_only) {
        if (not sex_stats_only) this->id = "";
        if (not sex_stats_only) this->sequence = "";
        for (auto& group: this->groups) group.second = 0;
        this->n_individuals = 0;
        for (auto& individual: this->individuals) individual = 0;
    }

    void output_table(std::ofstream& output_file) const {

        output_file << this->id << "\t" << this->sequence;
        for (auto& individual: this->individuals) output_file << "\t" << individual;
        output_file << "\n";
    }

    void output_fasta(std::ofstream& output_file, uint min_depth) const {

        output_file << ">" << this->id;
        for (auto group: this->groups) output_file << "_" << group.first << ":" << group.second;
        output_file << "_p:" << this->p << "_mindepth:" << min_depth << "\n" << this->sequence << "\n";
    }

};


struct MarkersQueue {

    std::queue<Marker> markers;
    uint64_t n_markers = 0;
    uint64_t loaded_markers = 0;

};


typedef std::vector<std::string> Header;

void table_parser(Parameters& parameters, const Popmap& popmap, MarkersQueue& markers_queue, std::mutex& queue_mutex, bool& parsing_ended, bool no_seq = true, bool no_group = false);

std::vector<Marker> get_batch(MarkersQueue& markers_queue, std::mutex& queue_mutex, ulong batch_size=1000);

// Create a sex <-> column index correspondance map
std::vector<std::string> get_column_group(const std::unordered_map<std::string, std::string>& groups, const std::vector<std::string>& header);
