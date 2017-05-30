import os
from radseq_analyses_pipeline import analyse_directory
# from radseq_analyses_pipeline import visualise_directory

root_dir = '/home/rferon/work/code/radseq_analyses_pipeline/'
files_dir = os.path.join(root_dir, 'data', 'results_m_5_n_1_M_3')
output_dir = os.path.join(root_dir, 'data', 'output')

# Maximum proportion of individuals deviating from the population
error_threshold = 0.1

# Number of threads to use when possible
n_threads = 4

analyse_directory(root_dir, files_dir, output_dir,
                  error_threshold, n_threads, visualize=True)
# visualise_directory(files_dir, output_dir, error_threshold)
