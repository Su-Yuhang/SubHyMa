

#ifndef FSC_FSC_H
#define FSC_FSC_H


#include "GraphDB.h"
#include "Utility.h"
#include <stddef.h>
#include <utility>
using namespace std;


class SequenceElement {
public:
    int id_;
    int pivot_;
    int bn_count_;
    int* bn_;
public:
    SequenceElement() {
        bn_ = new int[MAX_QUERY_GRAPH_VERTICES_COUNT];
    }

    ~SequenceElement() {
        delete[] bn_;
        bn_ = NULL;
    }
};

class TreeNode {
public:
    int id_;
    int parent_;
    int label_;
    int degree_;
    int level_;
    int under_level_count_;
    int children_count_;
    int bn_count_;
    int fn_count_;
    int nlf_label_count_;
    int* under_level_;
    int* children_;
    int* bn_;
    int* fn_;
    pair<int, int>* nlf_;

public:
    TreeNode() {
        under_level_ = new int[MAX_QUERY_GRAPH_VERTICES_COUNT];
        bn_ = new int[MAX_QUERY_GRAPH_VERTICES_COUNT];
        fn_ = new int[MAX_QUERY_GRAPH_VERTICES_COUNT];
        nlf_ = new pair<int, int>[MAX_QUERY_GRAPH_VERTICES_COUNT];
        children_ = new int[MAX_QUERY_GRAPH_VERTICES_COUNT];
    }

    ~TreeNode() {
        delete[] under_level_;
        delete[] bn_;
        delete[] fn_;
        delete[] nlf_;
        delete[] children_;
    }
};

class FSC {
private:

    GraphDB* graphDB_;

    const vector<Graph*>* data_graphs_;



    SequenceElement* sequence_;

    int total_label_num_;
    int candidate_graph_num_;
    int valid_graph_num_;
    int max_data_vertices_num_;
    int max_data_vertices_with_same_label_;
    int max_data_edges_num_;
    int max_degree_;
    int* data_vertices_flag_;
    int* updated_data_vertices_flag_;
    int updated_data_vertices_count_;
    int core_length_;

    int simulation_order_count_;
    bool* query_vertices_flags_;
    int** candidates_;
    int* candidates_count_;
    int* query_vertices_cores_;
    int* backward_value_;

    int* label_frequency_;

    int* idx_;
    int* matching_;
    int* embedding_;

    bool* data_vertex_visited_;


    int* data_vertex_distinct_neighbor_count_;
    int* data_vertex_visited_update_;


    int** bigraph_offset_;
    int** bigraph_edges_;


    int verification_start_vertex_;
    double* weight_;
    int* pivot_;
    double* directed_weighted_graph_;

    int* target_;
    int target_size_;
    int* result_;
    int result_size_;


    int* candidates_index_count_;
    int const ** candidates_index_;
    int* candidates_index_matching_;


    TreeNode* tree_node_;
    int level_count_;
    int root_vertex_;
    int* bfs_order_;
    int* bfs_order_index_;
    int* level_offset_;

    // Generate matching order.
    size_t** path_cost_;

    vector<vector<pair<int, size_t>>> core_paths_;
    map<int, vector<vector<pair<int, size_t>>>> tree_paths_;
private:

    void InitializeDataGraphResource(const vector<Graph *> *data_graphs);

    void InitializeQueryGraphResource(const Graph* query_graph);
    void ClearDataGraphResource();
    void ClearQueryGraphResource();
    void Reset();
    void ResetQueryGraphResource();
    bool GraphMetadataFilter(const Graph* query_graph, const Graph* data_graph);

    void GenerateBigraphIndex(const Graph *data_graph);
    bool Verification(const Graph* query_graph, const Graph* data_graph);
    void SelectNextVertex(const Graph *query_graph, const int idx, int &next_query_vertex, int &pivot);
    void UpdateBackwardValue(const Graph *query_graph, const int u);
    void SelectStartVertex(const Graph *query_graph);
    void ComputeDirectedWeightedGraph(const Graph *query_graph, const Graph* data_graph);



    void GenerateBFSTree(const Graph* query_graph, const Graph* data_graph);
    void SelectRootVertex(const Graph* query_graph, const Graph* data_graph);
    bool GenerateRootCandidate(const Graph* data_graph);
    void GenerateVerificationOrder(const Graph *query_graph);
    void CorePath(const int depth, const int cur_vertex, vector<pair<int, size_t>>& path);
    void TreePath(const int depth, const int cur_vertex, vector<pair<int, size_t>>& path);

    void UpdateSequenceElement(const Graph* query_graph, const int pivot, const int u, SequenceElement& element);

    void GenerateNLF(const Graph* query_graph);
    bool IsValid(const int level, const int v, const Graph* data_graph);


    bool Simulation(const Graph* query_graph, const Graph* data_graph);

    inline const int* CandidateIndex(const int id, const int index, int& count) {
        count = bigraph_offset_[id][index + 1] - bigraph_offset_[id][index];
        return bigraph_edges_[id] + bigraph_offset_[id][index];
    }

    inline int Candidate(const int id, const int index) {
        return candidates_[id][index];
    }

    void GetMemoryCost();
public:
    double total_time_;
    double total_filtering_time_;
    double total_verification_time_;
    double total_per_verification_time_;
    double total_ordering_time_;
    double total_enumeration_time_;
    double total_save_time_;

    double processing_time_;
    double filtering_time_;
    double verification_time_;
    double ordering_time_;
    double temp_ordering_time_;
    double enumeration_time_;
    double save_time_;

    double peak_memory_cost_;
    double total_filtering_precision;

    int total_candidate_num_;
    int total_answer_num_;

public:
    FSC(GraphDB* graphDB);
    ~FSC();
    void Query(const Graph *query_graph, int *query_results, int &results_count);
};


#endif //FSC_FSC_H
