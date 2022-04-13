#include <iostream>
#include <fstream>
#include "GraphDB.h"
#include "FSC.h"
using namespace std;
#include "Utility.h"
#include <map>
#include <vector>
#include <string>
#include <algorithm>


class CommandParser {
private:
    vector<string> tokens_;

public:
    CommandParser(const int &argc, char **argv) {
        for (int i = 1; i < argc; ++i)
            tokens_.push_back(string(argv[i]));
    }

    const string GetCommandOption(const string &option) const {

        vector<string>::const_iterator itr;
        itr = find(tokens_.begin(), tokens_.end(), option);
        if (itr != tokens_.end() && ++itr != tokens_.end()) {
            return *itr;
        }
        return "";
    }

    bool CommandOptionExists(const string &option) const {
        return find(tokens_.begin(), tokens_.end(), option) != tokens_.end();
    }
};


enum OptionKeyword {
    QueryGraphFile = 1,     // -q, The query graph file path, compulsive parameter
    DataGraphFile = 2,      // -d, The data graph file path, compulsive parameter
    QueryGraphFormat = 3,   // -qf, The query graph file format, compulsive parameter
    DataGraphFormat = 4     // -df, The data graph file format, compulsive parameter
};

class FSCCommand : public CommandParser {
private:
    map<OptionKeyword, string> options_key;
    map<OptionKeyword, string> options_value;
    map<string, GraphFormat> format_key;
    map<OptionKeyword, GraphFormat > format_value;
private:
    void ProcessOptions() {

        for (map<OptionKeyword, string>::iterator iter = options_key.begin(); iter != options_key.end(); iter++) {
            if (!CommandOptionExists(iter->second)) {
                printf("Please specify %s option.\n", iter->second.c_str());
                exit(-1);
            }
        }

        // Query graph file path
        string value = GetCommandOption(options_key[OptionKeyword::QueryGraphFile]);
        if (value == "") {
            printf("The query graph file path can not be empty.\n");
            exit(-1);
        }
        options_value[OptionKeyword::QueryGraphFile] = value;

        // Data graph file path
        value = GetCommandOption(options_key[OptionKeyword::DataGraphFile]);
        if (value == "") {
            printf("The data graph file path can not be empty.\n");
            exit(-1);
        }
        options_value[OptionKeyword::DataGraphFile] = value;

        // Query graph format
        value = GetCommandOption(options_key[OptionKeyword::QueryGraphFormat]);
        if (format_key.find(value) == format_key.end()) {
            printf("The query graph format cannot be supported.\n");
            exit(-1);
        }
        format_value[OptionKeyword::QueryGraphFormat] = format_key[value];

        // Data graph format
        value = GetCommandOption(options_key[OptionKeyword::DataGraphFormat]);
        if (format_key.find(value) == format_key.end()) {
            printf("The data graph format cannot be supported.\n");
            exit(-1);
        }
        format_value[OptionKeyword::DataGraphFormat] = format_key[value];
    }

public:
    FSCCommand(const int &argc, char **argv) : CommandParser(argc, argv) {
        // Initialize options value
        options_key[OptionKeyword::QueryGraphFile] = "-q";
        options_key[OptionKeyword::DataGraphFile] = "-d";
        options_key[OptionKeyword::QueryGraphFormat] = "-qf";
        options_key[OptionKeyword::DataGraphFormat] = "-df";
        format_key["FA"] = GraphFormat::FA;
        format_key["FB"] = GraphFormat::FB;
        format_key["FC"] = GraphFormat::FC;
        ProcessOptions();
    }

    const string& GetDataGraphFilePath() {
        return options_value[OptionKeyword::DataGraphFile];
    }

    const string& GetQueryGraphFilePath() {
        return options_value[OptionKeyword::QueryGraphFile];
    }

    const GraphFormat GetQueryGraphFormat() {
        return format_value[OptionKeyword::QueryGraphFormat];
    }

    const GraphFormat GetDataGraphFormat() {
        return format_value[OptionKeyword::DataGraphFormat];
    }
};


int main(int argc, char** argv) {





    FSCCommand command(argc, argv);
    const string data_graph_path = command.GetDataGraphFilePath();
    const string query_graph_path = command.GetQueryGraphFilePath();

    const GraphFormat data_graph_format = command.GetDataGraphFormat();
    const GraphFormat query_graph_format = command.GetQueryGraphFormat();

    GraphDB graphDB;
    graphDB.BuildDB(data_graph_path, data_graph_format);
    vector<Graph*>* data_graphs = graphDB.GetDataGraphs();


    vector<Graph*>* query_graphs = graphDB.LoadQueryGraphs(query_graph_path, query_graph_format);
    std::cout << "The number of data graphs: " << data_graphs->size() << " ." << endl;
    std::cout << "The number of query graphs: " << query_graphs->size() << " ." << endl;

    FSC fsc(&graphDB);

    int* results = new int[data_graphs->size()];
    int count = 0;
    for (size_t i = 0; i < query_graphs->size(); ++i) {
        cout << "-------------------------------------------------------------------" << endl;
        cout << "Process Query Graph: " << i << " ." << endl;


        fsc.Query((*query_graphs)[i], results, count);

    }

    cout << "-------------------------------------------------------------------" << endl;
    cout << "The average number of candidates : "  << (double)fsc.total_candidate_num_ / query_graphs->size() << " ." << endl;
    cout << "The average number of results : " << (double)fsc.total_answer_num_ / query_graphs->size() << " ." << endl;
    cout << "The average processing time: " << fsc.total_time_ / query_graphs->size() << " us." << endl;
    cout << "The average filtering time: " << fsc.total_filtering_time_ / query_graphs->size() << " us." << endl;
    cout << "The average ordering time: " << fsc.total_ordering_time_ / query_graphs->size() << " us." << endl;
    cout << "The average enumeration time: " << fsc.total_enumeration_time_ / query_graphs->size() << " us." << endl;
    cout << "The average verification time: " << fsc.total_verification_time_ / query_graphs->size() << " us." << endl;
    cout << "The peak memory cost: " << fsc.peak_memory_cost_ << " MB." << endl;

    delete[] results;
    return 0;
}
