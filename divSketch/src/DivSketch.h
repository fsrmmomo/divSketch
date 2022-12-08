//
// Created by momo on 2022/5/30.
//

#ifndef USER_READER_DIVSKETCH_H
#define USER_READER_DIVSKETCH_H

#include "algorithms.h"
#include "HeavyPart.h"
#include "LightPart.h"
#include "HeavyPart.cpp"
#include "LightPart.cpp"
#include "param.h"

template<int bucket_num, int tot_memory_in_bytes>
//template<int i>
class DivSketch : public Algorithm {
//    static constexpr int i = 0;
//    static constexpr int tot_mem_list[10] = {200 * 1024, 300 * 1024, 400 * 1024, 500 * 1024, 600 * 1024, 700 * 1024, 800 * 1024,
//                                  900 * 1024, 1000 * 1024, 1100 * 1024};
//    static constexpr int heavy_mem_list[10] = {100 * 1024, 100 * 1024, 100 * 1024, 100 * 1024, 100 * 1024, 150 * 1024, 150 * 1024,
//                                    150 * 1024, 150 * 1024, 200 * 1024};
//    static constexpr int heavy_mem = heavy_mem_list[i];
//    static constexpr int light_mem = tot_mem_list[i]-heavy_mem;
//    static constexpr int bucket_num = heavy_mem_list[i]/COUNTER_PER_BUCKET/8;
    static constexpr int heavy_mem = bucket_num * COUNTER_PER_BUCKET * 8;
    static constexpr int light_mem = tot_memory_in_bytes - heavy_mem;

public:
    HeavyPart<bucket_num> heavy_part;
    LightPart<light_mem> light_part;

    DivSketch() : Algorithm("DivSketch") {}

    ~DivSketch() override;

    int insert(uint8_t *key, struct Val f);

    int query(uint8_t *key);

    void clear();

    void get_flowsize(vector<string> &flowIDs, unordered_map<string, int> &freq);

    void get_heavy_hitters(int threshold, vector<pair<string, int>> &results);

    int get_cardinality();

private:
//    void quick_insert(uint8_t *key, int f = 1);

//    int query(uint8_t *key);
//    int query_compressed_part(uint8_t *key, uint8_t *compress_part, int compress_counter_num);
//
//    int get_compress_width(int ratio);
//    void compress(int ratio, uint8_t *dst);
//
//    int get_bucket_num();
//    double get_bandwidth(int compress_ratio) ;
//


//    double get_entropy();
//    void get_distribution(vector<double> &dist);

public:
    void *operator new(size_t sz);

    void operator delete(void *p);
};


#endif //USER_READER_DIVSKETCH_H
