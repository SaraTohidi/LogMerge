#include  <iostream>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>
#include <set>
#include <fstream>
#include <list>

using namespace std;


/* This function separate Filters and put them in a vector
 * Input: input_filters: the raw input of filters that user entered
 * Output: all filteres */
vector<string> separateFilters(string input_filters){
    //spliting the given filters
    vector<string> filters;
    boost::split(filters, input_filters, boost::is_any_of(","));
    return filters;
}

/* This function does input Validation
 *      check if the given input are valid
 * Inputs are:
 * 1. filter: if there is any filter
 * 2. dir: if there is any directory
 * Output: true/false that user entered correct inputs */
bool inputValidation(bool filter, bool dir){
    if(dir == true && filter == true){
        return true;
    }
    else{
        cerr << " Wrong inputs! Try again later. " << endl;
        return false;
    }
}

/* This function sorts Files
 *      get all files that are .log format
 *      sort their name
 * Input: directory: the given path to the directory
 * Output: name of all files in a given directory by order */
set<string> sortingFiles(string directory_path){
    set<string> sorted_files;
    DIR *dr;
    struct dirent *en;
    const char * path = directory_path.c_str();
    dr = opendir(path);
    if (dr) {
        string log_file = ".log";
        while ((en = readdir(dr)) != NULL) {
            string line = en->d_name;
            //get all the files that ends with ".log"
            if (line.find(log_file) != std::string::npos) {
                sorted_files.insert(line);
            }
        }
        closedir(dr);
        return sorted_files;
    }else{
        cerr << "Could not find the directory! Try again later."<<endl;
        return sorted_files;
    }
}

/* This function check if lines are filtered
 * Inputs are:
 * 1. line: the current line we are looking at
 * 2. filters: the filteres that are given if any
 * Output: true/false depend on if the line has the filter or not if any */
bool checkFilterLines(string line,vector<string> filters)
{
    if( filters.empty()) return true;
    //get the words in a line
    istringstream words (line);
    string word = "";
    words >> word;
    //find a match between filters and word
    for (auto filter : filters){
        if(word == filter){
            return true;
        }
    }
    return false;
}

/* This function read Next Line
 *      opening and closing each file in the given order
 *      everytime it opens a file it read lines till we get to the line that we previously checked
 *      if the current line is the chosen one(pre_line) it updates that line to the next is any exist
 * Inputs are:
 * 1. pre_line: the current line that selected to enter the merge file
 * 2. filters: the filteres to be considered if any
 * 3. file_lines: the current line of each line that we are looking at
 * 4. sorted_files: name of the files in the directory in alphabetic order
 * 5. dir_name: name of the directory
 * 6. first_time_initialization: check box whether it is the first time reading the line
 * Output: updated line of each file */
vector<string> readNextLine(string pre_line, vector<string> filters, vector<string> file_lines, set<string> sorted_files, string dir_name, bool first_time_initialization){
    vector<string> updated_file_lines;
    int i = -1;
    for (string file_name: sorted_files){
        i++;
        ifstream file;
        string line ="";
        //opening the file
        file.open(dir_name + file_name);
        //check if file opened
        if(!file) cerr<< "could not open the " + file_name<< endl;
        //read the first line
        getline(file, line);
        //check for the filter(s)
        bool filtered = checkFilterLines(line, filters);
        while (!filtered){
            getline(file, line);
            filtered = checkFilterLines(line, filters);
        }
        //check if it is the first time
        if(!first_time_initialization){
            //check if the current line is the line we previously looked at
            while(line != file_lines[i] && line != ""){
                getline(file, line);
            }
            //check if the current line is the minUTC time we picked
            if(line == pre_line){
                getline(file, line);
                filtered = checkFilterLines(line, filters);
                //check for the filters for new line
                while (!filtered && line != ""){
                    getline(file, line);
                    filtered = checkFilterLines(line, filters);
                }
            }
        }
        updated_file_lines.push_back(line);
        file.close();
    }


    return updated_file_lines;

}
/* This function get Min Time In Lines
 * Input: all_timestamp: the timestamp of each line and the line itself
 * Output: the smallest timestamp which is the earlieast data entered as well */
int getMinTimeInLines(list<pair<int, string>> all_timestamp){
    vector<int> times;
    for(auto item: all_timestamp){
        times.push_back(item.first);
    }
    int min_time = *min_element(times.begin(), times.end());

    return min_time;
}

/* This function find The Line that belongs to a
 * Inputs are:
 * 1. time: the timestamp
 * 2. lines_time: the timestamp of each line and the line itself
 * Output: the line that matches the given timestamp */
string findTheLine(int time, list<pair<int,string>> lines_time){
    for(auto item: lines_time){
        if(item.first == time){
            return item.second;
        }
    }
    return "";
}

/* This function get UTC TimeStamp of a given line
 * Input: line: the current line of a file
 * Output: an int of timestamp */
int getUTCTimeStamp(string line){
    istringstream words (line);
    vector<string> line_words;
    for (string line; words >> line;) line_words.push_back(line);
    return stoi(line_words[1]);
}

/* This function get UTC TimeStamp of Each Line
 * Input: file_lines: the current line of all present files in a directory
 * Output: a pair of the stracted timestamp of each line with the line itself */
list<pair<int, string>> getUTCTimeStampEachLine(vector<string> file_lines){
    list<pair<int, string>> all_timestamps_lines;
    for(auto line: file_lines){
        if (line != "") {
            int time = getUTCTimeStamp(line);
            all_timestamps_lines.push_back(make_pair(time, line));
        }
    }
    return all_timestamps_lines;
}

/* This function write Sorted Data In Merge File
 * Inputs are:
 * 1. directory: the directory that log files located
 * 2. filters: filters if there is any */
void writeSortedDataInMFile(string directory, vector<string> filters){
    vector<string> file_lines;
    //get all sorted files from the directory
    set<string> sorted_files = sortingFiles(directory);

    //open the merge file
    std::ofstream merged(directory+ "mergedfile.txt", std::ios_base::app | std::ios_base::out);


    //update the line of each file
    vector<string> updated_file_lines = readNextLine("",filters,file_lines,sorted_files,directory,true);

    //get the time stamps and lines for the first time
    list<pair<int, string>> timestamps_lines = getUTCTimeStampEachLine(updated_file_lines);

    while(!timestamps_lines.empty()){
        //get the min of the given current times and lines
        int min_time = getMinTimeInLines(timestamps_lines);
        //find a line that belongs to the min time that we found
        string pre_line = findTheLine(min_time, timestamps_lines);
        //adding the line with order in merge file
        merged << pre_line << endl;
        //update the line of each file
        updated_file_lines = readNextLine(pre_line,filters,updated_file_lines, sorted_files, directory, false);
        //get the time stamps and lines
        timestamps_lines = getUTCTimeStampEachLine(updated_file_lines);
    }
    merged.close();

}

int main(int argc, char *argv[])
{
    bool find_filter = false;
    bool find_dir = false;
    vector<string> filters;
    string directory;

    //make sure we get all the needed variables
    for (int item = 1; item < argc; ++item) {
        string arg = argv[item];
        if(arg == "-filter"){
            //validate the input
            if(argc != 5){
                cerr << " Wrong inputs! Try again later. " << endl;
                return 0;
            }
            string arg = argv[1];
            string next_arg = argv[2];
            if(arg == "-filter" && next_arg != ""){
                find_filter = true;
                filters = separateFilters(next_arg);
            }
            arg = argv[3];
            next_arg = argv[4];
            if (arg == "-dir" && next_arg != ""){
                find_dir = true;
                directory = next_arg;
            }
            //making sure we get the right inputs
            if(inputValidation(find_filter,find_dir)){
                writeSortedDataInMFile(directory,filters);
            }
            else{
                return 0;
            }
        }
    }
    if(!find_filter){
        string arg = argv[1];
        string next_arg = argv[2];
        if (arg == "-dir" && next_arg != ""){
            find_dir = true;
            directory = next_arg;
        }
        //making sure we got dir
        if(find_dir == true){
            writeSortedDataInMFile(directory,filters);
        }
        else{
            cerr << " Wrong input! Try again later. " << endl;
            return 0;
        }
    }


    return 0;
}
