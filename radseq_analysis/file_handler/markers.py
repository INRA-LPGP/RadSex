

def get_markers(markers_file_path):

    '''
    Extract information from a markers file (list of markers ID)
    '''

    markers_file = open(markers_file_path)
    markers = [line[:-1] for line in markers_file if line[:-1]]

    return markers
