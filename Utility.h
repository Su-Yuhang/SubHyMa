
#ifndef FSC_UTILITY_H
#define FSC_UTILITY_H

#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
using namespace std;
#define MAX_QUERY_GRAPH_VERTICES_COUNT 128
#define ENABLE_NEIGHBOR_LABEL_FREQUENCY_FILTER

class Graph {
private:

    int vertices_num_;
    int edges_num_;
    int labels_num_;
    int max_degree_;
    int max_vertices_num_with_same_label_;
    int* labels_;
    int* edges_offset_;
    int* edges_;
    unordered_map<int, int>* nlf_;
    int* reverse_label_offset_;
    int* reverse_label_vertices_;
    pair<int, int>* label_frequency_;
    int* edges_sort_by_label_;
    int* pruned_edges_offset_;
    int* pruned_edges_;

    pair<int, int>* pruned_label_frequency_;

    int pruned_labels_num_;
    int pruned_vertices_num_;

    int* degree_one_vertices_;


    int* degree_one_labels_count_;


    int* degree_one_parent_labels_count_;



    int* degree_one_offset_;

    int degree_one_labels_num_;
    int degree_one_vertices_num_;




public:
    void BuildNLF();
    void BuildReverseLabelIndex(const int label_num);
    void ComputeLabelFrequency();


    void SortEdge();
    void SortEdgeByLabel();
    void BuildPrunedGraph();
    void BuildDegreeOneVertices();
    void ComputeMaxDegree();

public:
    Graph() {
        vertices_num_ = 0;
        edges_num_ = 0;
        labels_num_ = 0;
        max_degree_ = 0;

        labels_ = NULL;
        edges_offset_ = NULL;
        edges_ = NULL;
        reverse_label_offset_ = NULL;
        reverse_label_vertices_ = NULL;
        label_frequency_ = NULL;
        nlf_ = NULL;

        edges_sort_by_label_ = NULL;

        pruned_edges_offset_ = NULL;
        pruned_edges_ = NULL;
        pruned_label_frequency_ = NULL;

        degree_one_vertices_ = NULL;
        degree_one_labels_count_ = NULL;
        degree_one_parent_labels_count_ = NULL;
        degree_one_offset_ = NULL;
    }

    ~Graph() {
        delete[] labels_;
        labels_ = NULL;
        delete[] edges_offset_;
        edges_offset_ = NULL;
        delete[] edges_;
        edges_ = NULL;

        delete[] reverse_label_offset_;
        reverse_label_offset_ = NULL;
        delete[] reverse_label_vertices_;
        reverse_label_vertices_ = NULL;
        delete[] label_frequency_;
        label_frequency_ = NULL;
        delete[] nlf_;
        nlf_ = NULL;

        delete[] edges_sort_by_label_;
        edges_sort_by_label_ = NULL;

        delete[] pruned_edges_offset_;
        pruned_edges_offset_ = NULL;
        delete[] pruned_edges_;
        pruned_edges_ = NULL;
        delete[] pruned_label_frequency_;
        pruned_label_frequency_ = NULL;

        delete[] degree_one_vertices_;
        degree_one_vertices_ = NULL;
        delete[] degree_one_labels_count_;
        degree_one_labels_count_ = NULL;
        delete[] degree_one_parent_labels_count_;
        degree_one_parent_labels_count_ = NULL;
        delete[] degree_one_offset_;
        degree_one_offset_ = NULL;
    }


    inline const int Label(const int id) const {
        return labels_[id];
    }




    inline void SetLabel(const int id, const int label) {
        labels_[id] = label;
    }

    inline const int Degree(const int id) const {
        return edges_offset_[id + 1] - edges_offset_[id];
    }




    inline const int* Neighbors(const int id, int& count) const {
        count = edges_offset_[id + 1] - edges_offset_[id];
        return edges_ + edges_offset_[id];
    }






    inline const int* SortedNeighbors(const int id, int& count) const {
        count = edges_offset_[id + 1] - edges_offset_[id];
        return edges_sort_by_label_ + edges_offset_[id];
    }




    inline const int* PrunedNeighbors(const int id, int& count) const {
        count = pruned_edges_offset_[id + 1] - pruned_edges_offset_[id];
        return pruned_edges_ + pruned_edges_offset_[id];
    }

    inline const int PrunedVerticesCount() const  {
        return pruned_vertices_num_;
    }

    inline const int GetOffset(const int id) const {
        return edges_offset_[id];
    }

    inline void SetOffset(const int id, const int offset) {
        edges_offset_[id] = offset;
    }

    inline void SetNeighbor(const int index, const int id) {
        edges_[index] = id;
    }

    inline void SetVerticesCount(const int count) {
        vertices_num_ = count;

        if (labels_ == NULL)
            delete[] labels_;

        if (edges_offset_ == NULL)
            delete[] edges_offset_;

        labels_ = new int[vertices_num_];
        edges_offset_ = new int[vertices_num_ + 1];
        memset(edges_offset_, 0, sizeof(int) * (vertices_num_ + 1));
    }

    inline const int VerticesCount() const {
        return vertices_num_;
    }

    inline void SetEdgesCount(const int count) {
        edges_num_ = count;

        if (edges_ == NULL)
            delete[] edges_;

        edges_ = new int[count * 2];
    }

    inline const int EdgesCount() const {
        return edges_num_;
    }

    inline const int VerticesCount(const int label_id) const {
        return reverse_label_offset_[label_id + 1] - reverse_label_offset_[label_id];
    }

    inline const pair<int, int>* LabelFrequency() const {
        return label_frequency_;
    };

    inline const int LabelsCount() const {
        return labels_num_;
    }

    inline const int MaxDegree() const {
        return max_degree_;
    }

    inline const int MaxVerticesNumWithSameLabel() const {
        return max_vertices_num_with_same_label_;
    }

    inline const unordered_map<int, int>* NLF(const int id) const {
        return nlf_ + id;
    }

    inline const int* VerticesWithLabel(const int label_id, int& count) const {
        count = reverse_label_offset_[label_id + 1] - reverse_label_offset_[label_id];
        return reverse_label_vertices_ + reverse_label_offset_[label_id];
    }
    inline const bool IsEdge(const int u, const int v) const {
        int count = 0;
        const int* neighbors =  Neighbors(v, count);
        int begin = 0;
        int end = count - 1;
        while (begin <= end) {
            int mid = begin + ((end - begin) >> 1);
            if (neighbors[mid] == u) {
                return true;
            }
            else if (neighbors[mid] > u)
                end = mid - 1;
            else
                begin = mid + 1;
        }

        return false;
    }
};



class Utility {
private:
    Utility() {}

public:

    static void GetKCore(const Graph* graph, int* core_table);
    static timeval GetTime();
    static unsigned long TimeDiffInMicroseconds(const timeval start, const timeval end);
    static double TimeDiffInSeconds(const timeval start, const timeval end);
    static unsigned long UpperPowerOfTwo(unsigned long v);
    static void old_cheap(int* col_ptrs, int* col_ids, int* match, int* row_match, int n, int m);
    static void match_bfs(int* col_ptrs, int* col_ids, int* match, int* row_match, int* visited,
                          int* queue, int* previous, int n, int m);

};

#endif //FSM_UTILITY_H

