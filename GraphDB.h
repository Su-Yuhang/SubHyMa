

#ifndef FSC_GRAPHDB_H
#define FSC_GRAPHDB_H

#include <unordered_map>
#include <string>
#include "Utility.h"
using namespace std;

enum GraphFormat {
    FA,
    FB,
    FC
};

class GraphDB {
private:
    unordered_map<string, int>* label_map_;
    unordered_map<string, int>* graph_map_;

    unordered_map<int, int>* data_graph_label_histogram_;

    unordered_map<int, int>* query_graph_label_histogram_;

    vector<Graph*>* graphs_;
public:
    GraphDB();
    ~GraphDB();

    void BuildDB(const string& file, GraphFormat format);


    vector<Graph*>* GetDataGraphs() {
        return graphs_;
    }

    unordered_map<string, int>* GetLabelMap() {
        return label_map_;
    }
    unordered_map<string, int>* GetGraphMap() {
        return graph_map_;
    }

    vector<Graph*>* LoadQueryGraphs(const string& file, GraphFormat format);

    const int LabelsBinSize(const int label) const {
        return (*data_graph_label_histogram_)[label];
    }
};


#endif //SRC_GRAPHDB_H
