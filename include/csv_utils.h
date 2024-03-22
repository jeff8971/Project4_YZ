/*
  Bruce A. Maxwell, modified by Yuan Zhao

  utility functions for reading and writing csv files with a specific format

  each line of the csv file is a object label in the first column, followed by numeric data for the remaining columns
  each line of the csv file has to have the same number of columns
 */

#ifndef CVS_UTIL_H
#define CVS_UTIL_H

#include <string>
#include <vector>

#define CORNER_PER_ROW 9
#define CORNER_PER_COL 6

// append a line of data to a csv format file
// the object label is written to the first column
// the object features are all written to the following columns as doubles
// 
// filename: the name of the file to append to
// object_label: the label for the object
// object_feature: the features for the object
// reset_file: if true, then the file is opened in 'write' mode and the existing contents are cleared.
// if false, then the file is opened in 'append' mode and the new data is appended to the end of the file.
// returns a non-zero value if something goes wrong
int append_object_data_csv(std::string filename, std::string object_label, std::vector<double> &object_feature, int reset_file = 0);

// read data from a csv format file
// whose first column is the object label and the remaining columns are the object features
// and return the object labels and features in two vectors
// 
// filename: the name of the file to read from
// object_labels: the labels for the objects as a std::vector of std::string
// object_features: the features for the objects as a 2D std::vector<double>
// echo_file: if true, then the contents of the file are printed to the console
// returns a non-zero value if something goes wrong
int read_object_data_csv(std::string filename, std::vector<std::string> &object_labels, std::vector<std::vector<double>> &object_features, int echo_file = 0);

#endif