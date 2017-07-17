import gzip
from collections import defaultdict
from radseq_analysis.shared import *


def get_individual_sequences(individual_file_path, loci_to_extract=None):

    # Open file, read 2nd line, extract individual number and reset to 2nd line
    individual_file = gzip.open(individual_file_path, 'rt')
    individual_file.readline()
    individual_number = individual_file.readline().split('\t')[1]
    individual_file.close()
    individual_file = gzip.open(individual_file_path, 'rt')
    individual_file.readline()

    individual_data = defaultdict(lambda: defaultdict())

    for line in individual_file:

        tabs = line.split('\t')
        individual_locus_id = tabs[2]

        if (not loci_to_extract or individual_locus_id in
                loci_to_extract[individual_number].keys()):

            catalog_id = loci_to_extract[individual][individual_locus_id]
            sequence = tabs[9]

            sequence_name = tabs[6]
            if tabs[7] != '':
                sequence_name += '_' + tabs[7]

            if sequence_name == 'model':
                pass
            else:
                if sequence_name in individual_data[catalog_id].keys():
                    individual_data[catalog_id][sequence_name][COVERAGE] += 1
                else:
                    individual_data[catalog_id][sequence_name] = {SEQUENCE: sequence,
                                                                  COVERAGE: 1}

    return individual_data
