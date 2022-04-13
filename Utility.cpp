

#include "Utility.h"

void Graph::BuildReverseLabelIndex(const int label_num) {
    reverse_label_offset_ = new int[label_num + 1];
    reverse_label_vertices_ = new int[vertices_num_];
    labels_num_ = 0;



    memset(reverse_label_offset_, 0, sizeof(int) * (label_num + 1));



    for (int i = 0; i < vertices_num_; ++i) {
        int label = labels_[i];
        if (reverse_label_offset_[label] == 0)
            labels_num_ += 1;
        reverse_label_offset_[label] += 1;
    }


    int temp = reverse_label_offset_[0];
    reverse_label_offset_[0] = 0;
    for (int i = 1; i <= label_num; ++i) {
        int cur = reverse_label_offset_[i];
        reverse_label_offset_[i] = reverse_label_offset_[i - 1] + temp;
        temp = cur;
    }


    for (int i = 0; i < vertices_num_; ++i) {
        int label = labels_[i];
        reverse_label_vertices_[reverse_label_offset_[label]++] = i;
    }


    for (int i = label_num; i >= 1; --i) {
        reverse_label_offset_[i] = reverse_label_offset_[i - 1];
    }

    reverse_label_offset_[0] = 0;


    max_vertices_num_with_same_label_ = -1;
    for (int i = 0; i < label_num; ++i) {
        if (reverse_label_offset_[i + 1] - reverse_label_offset_[i] > max_vertices_num_with_same_label_)
            max_vertices_num_with_same_label_ = reverse_label_offset_[i + 1] - reverse_label_offset_[i];
    }
}

void Graph::ComputeLabelFrequency() {
    map<int, int> label_frequency;

    for (int i = 0; i < vertices_num_; ++i) {
        int label = labels_[i];

        if (label_frequency.find(label) == label_frequency.end())
            label_frequency[label] = 0;
        label_frequency[label] += 1;
    }

    label_frequency_ = new pair<int, int>[label_frequency.size()];
    labels_num_ = label_frequency.size();

    int i = 0;
    for (map<int, int>::iterator iter = label_frequency.begin(); iter != label_frequency.end(); iter++) {
        label_frequency_[i++] = make_pair(iter->first, iter->second);
    }
}


void Graph::SortEdge() {
    for (int i = 0; i < vertices_num_; ++i) {
        sort(edges_ + edges_offset_[i], edges_ + edges_offset_[i + 1]);
    }
}


void Graph::SortEdgeByLabel() {
    edges_sort_by_label_ = new int[edges_num_ * 2];
    copy(edges_, edges_ + edges_num_ * 2, edges_sort_by_label_);

    for (int i = 0; i < vertices_num_; ++i) {
        sort(edges_sort_by_label_ + edges_offset_[i], edges_sort_by_label_ + edges_offset_[i + 1],
            [this](const int& x, const int& y) -> bool {
                if (labels_[x] < labels_[y])
                {
                    return true;
                }
                else if (labels_[x] == labels_[y]) {
                    if (x < y) {
                        return true;
                    }
                }
                return false;
            });
    }
}


void Graph::BuildNLF() {
    nlf_ = new unordered_map<int, int>[vertices_num_];
    for (int i = 0; i < vertices_num_; ++i) {
        int count;
        const int* neighbors = Neighbors(i, count);
        for (int j = 0; j < count; ++j) {
            int neighbor = neighbors[j];
            int label = Label(neighbor);

            if (nlf_[i].find(label) == nlf_[i].end()) {
                nlf_[i][label] = 1;
            }
            else {
                nlf_[i][label] += 1;
            }
        }
    }
}

void Graph::ComputeMaxDegree() {
    max_degree_ = -1;
    for (int i = 0; i < vertices_num_; ++i) {
        if (Degree(i) > max_degree_)
            max_degree_ = Degree(i);
    }
}

void Graph::BuildPrunedGraph() {
    pruned_edges_offset_ = new int[vertices_num_ + 1];
    pruned_edges_ = new int[edges_num_ * 2];

    pruned_edges_offset_[0] = 0;

    pruned_vertices_num_ = 0;

    int pruned_edges_num = 0;

    map<int, int> label_frequency;


    for (int i = 0; i < vertices_num_; ++i) {

        int degree = Degree(i);

        if (degree > 1) {

            int count;
            const int* neighbors = SortedNeighbors(i, count);


            for (int j = 0; j < count; ++j) {
                int neighbor = neighbors[j];
                int neighbor_degree = Degree(neighbor);
                if (neighbor_degree > 1) {
                    pruned_edges_[pruned_edges_num++] = neighbor;
                }
            }

            pruned_vertices_num_ += 1;

            int label = labels_[i];

            if (label_frequency.find(label) == label_frequency.end())
                label_frequency[label] = 0;
            label_frequency[label] += 1;
        }



        pruned_edges_offset_[i + 1] = pruned_edges_num;
    }



    pruned_label_frequency_ = new pair<int, int>[label_frequency.size()];
    pruned_labels_num_ = label_frequency.size();

    int i = 0;
    for (map<int, int>::iterator iter = label_frequency.begin(); iter != label_frequency.end(); iter++) {
        pruned_label_frequency_[i++] = make_pair(iter->first, iter->second);
    }
}


void Graph::BuildDegreeOneVertices() {
    degree_one_vertices_num_ = 0;
    degree_one_labels_num_ = 0;
    degree_one_vertices_ = new int[vertices_num_];
    degree_one_offset_ = new int[vertices_num_ + 1];


    degree_one_labels_count_ = new int[vertices_num_];
    memset(degree_one_labels_count_, 0, sizeof(int) * vertices_num_);


    degree_one_parent_labels_count_ = new int[vertices_num_];
    memset(degree_one_parent_labels_count_, 0, sizeof(int) * vertices_num_);

    map<int, int> label_frequency;


    for (int i = 0; i < vertices_num_; ++i) {

        if (Degree(i) == 1) {
            degree_one_vertices_[degree_one_vertices_num_++] = i;
            int label = labels_[i];

            if (label_frequency.find(label) == label_frequency.end())
                label_frequency[label] = 0;

            label_frequency[label] += 1;
        }

        else {
            int count;

            const int* neighbors = Neighbors(i, count);

            map<int, int> parent_label_frequency;

            for (int j = 0; j < count; ++j) {
                int neighbor = neighbors[j];

                if (Degree(neighbor) == 1) {
                    int label = labels_[neighbor];
                    if (parent_label_frequency.find(label) == parent_label_frequency.end())
                        parent_label_frequency[label] = 0;
                    parent_label_frequency[label] += 1;
                }
            }

            for (int j = 0; j < count; ++j) {
                int neighbor = neighbors[j];

                if (Degree(neighbor) == 1) {

                    int label = labels_[neighbor];
                    degree_one_parent_labels_count_[neighbor] = parent_label_frequency[label];
                }
            }
        }
    }

    for (int i = 0; i < degree_one_vertices_num_; ++i) {
        int u = degree_one_vertices_[i];
        int label = labels_[u];
        degree_one_labels_count_[u] = label_frequency[label];
    }




    sort(degree_one_vertices_, degree_one_vertices_ + degree_one_vertices_num_, [this](const int& x, const int& y) -> bool {

        if (this->degree_one_labels_count_[x] < this->degree_one_labels_count_[y]) {
            return true;
        }

        else if (this->degree_one_labels_count_[x] == this->degree_one_labels_count_[y]) {
            if (this->labels_[x] < this->labels_[y]) {
                return true;
            }

            else if (this->labels_[x] == this->labels_[y]) {
                if (this->degree_one_parent_labels_count_[x] < this->degree_one_parent_labels_count_[y]) {
                    return true;
                }

                else if (this->degree_one_parent_labels_count_[x] == this->degree_one_parent_labels_count_[y]){
                    int x_parent = edges_[edges_offset_[x]];
                    int y_parent = edges_[edges_offset_[y]];

                    if (x_parent < y_parent) {
                        return true;
                    }
                }
            }
        }

        return false;
    });

    degree_one_labels_num_ = 0;
    int cur_label = -1;
    for (int i = 0; i < degree_one_vertices_num_; ++i) {
        int u = degree_one_vertices_[i];
        int label = labels_[u];

        if (cur_label != label) {

            cur_label = label;

            degree_one_offset_[degree_one_labels_num_++] = i;
        }
    }
    degree_one_offset_[degree_one_labels_num_] = degree_one_vertices_num_;
}







void Utility::GetKCore(const Graph *graph, int *core_table) {
    int vertices_num = graph->VerticesCount();
    int max_degree = graph->MaxDegree();

    int* vertices = new int[vertices_num];
    int* position = new int[vertices_num];
    int* degree_bin = new int[max_degree + 1];
    int* offset = new int[max_degree + 1];

    memset(degree_bin, 0, sizeof(int) * (max_degree + 1));

    for (int i = 0; i < vertices_num; ++i) {
        int degree = graph->Degree(i);
        core_table[i] = degree;
        degree_bin[degree] += 1;
    }

    int start = 0;
    for (int i = 0; i < max_degree + 1; ++i) {
        offset[i] = start;
        start += degree_bin[i];
    }

    for (int i = 0; i < vertices_num; ++i) {
        int degree = graph->Degree(i);
        position[i] = offset[degree];
        vertices[position[i]] = i;
        offset[degree] += 1;
    }

    for (int i = max_degree; i > 0; --i) {
        offset[i] = offset[i - 1];
    }
    offset[0] = 0;

    for (int i = 0; i < vertices_num; ++i) {
        int v = vertices[i];

        int count;
        const int* neighbors = graph->Neighbors(v, count);

        for(int j = 0; j < count; ++j) {
            int u = neighbors[j];

            if (core_table[u] > core_table[v]) {


                int cur_degree_u = core_table[u];
                int position_u = position[u];
                int position_w = offset[cur_degree_u];
                int w = vertices[position_w];

                if (u != w) {

                    position[u] = position_w;
                    position[w] = position_u;
                    vertices[position_u] = w;
                    vertices[position_w] = u;
                }

                offset[cur_degree_u] += 1;
                core_table[u] -= 1;
            }
        }
    }

    delete[] vertices;
    delete[] position;
    delete[] degree_bin;
    delete[] offset;
}

timeval Utility::GetTime()
{
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp;
}

unsigned long Utility::TimeDiffInMicroseconds(const timeval start, const timeval end)
{
    unsigned long micro_second_diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    return micro_second_diff;
}

double Utility::TimeDiffInSeconds(const timeval start, const timeval end)
{
    double second_diff = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / (double)1000000.0;
    return second_diff;
}

unsigned long Utility::UpperPowerOfTwo(unsigned long v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;

}

void Utility::old_cheap(int* col_ptrs, int* col_ids, int* match, int* row_match, int n, int m) {
    int ptr;
    int i = 0;
    for(; i < n; i++) {
        int s_ptr = col_ptrs[i];
        int e_ptr = col_ptrs[i + 1];
        for(ptr = s_ptr; ptr < e_ptr; ptr++) {
            int r_id = col_ids[ptr];
            if(row_match[r_id] == -1) {
                match[i] = r_id;
                row_match[r_id] = i;
                break;
            }
        }
    }
}

void Utility::match_bfs(int* col_ptrs, int* col_ids, int* match, int* row_match, int* visited,
                        int* queue, int* previous, int n, int m) {
    int queue_ptr, queue_col, ptr, next_augment_no, i, j, queue_size,
            row, col, temp, eptr;

    memset(visited, 0, sizeof(int) * m);

    next_augment_no = 1;
    for(i = 0; i < n; i++) {
        if(match[i] == -1 && col_ptrs[i] != col_ptrs[i+1]) {
            queue[0] = i; queue_ptr = 0; queue_size = 1;

            while(queue_size > queue_ptr) {
                queue_col = queue[queue_ptr++];
                eptr = col_ptrs[queue_col + 1];
                for(ptr = col_ptrs[queue_col]; ptr < eptr; ptr++) {
                    row = col_ids[ptr];
                    temp = visited[row];

                    if(temp != next_augment_no && temp != -1) {
                        previous[row] = queue_col;
                        visited[row] = next_augment_no;

                        col = row_match[row];

                        if(col == -1) {

                            while(row != -1) {
                                col = previous[row];
                                temp = match[col];
                                match[col] = row;
                                row_match[row] = col;
                                row = temp;
                            }
                            next_augment_no++;
                            queue_size = 0;
                            break;
                        } else {

                            queue[queue_size++] = col;
                        }
                    }
                }
            }

            if(match[i] == -1) {
                for(j = 1; j < queue_size; j++) {
                    visited[match[queue[j]]] = -1;
                }
            }
        }
    }
}
