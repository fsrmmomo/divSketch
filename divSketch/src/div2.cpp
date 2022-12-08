//
// Created by momo on 2022/5/30.
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>       /* open */
#include <unistd.h>       /* exit */
#include <fstream>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include<io.h>

#if _WIN32

#elif __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/ioctl.h> /* ioctl */
#endif
//#include <hiredis/hiredis.h> // 操作数据库
#include <errno.h>
#include <time.h> // 时间戳
#include <thread> // 操作线程
#include <future> // 线程
#include <iomanip>
#include "util.h"
#include "tuple.h"
#include "DivSketch.h"
#include "DivSketch.cpp"
#include "ringbuffer.h"
#include "param.h"

#define FILE_NUM 2
#define WriteARE 0
#define WriteHHD 0
#define WriteHHD_ARE 1
#define WriteHHD_F1 1
#define WriteCE 0
#define WriteWMRE 0
using namespace std;


struct FIVE_TUPLE {
    unsigned short length;
    char key[13];
    char flag;
};
vector<string> files;
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[200];

void ReadInTraces(const char *filename, int i) {
    char datafileName[100];
    sprintf(datafileName, filename);
    FILE *fin = fopen(datafileName, "rb");
    FIVE_TUPLE tmp_five_tuple;
    traces[i].clear();
    while (fread(&tmp_five_tuple, 1, 16, fin) == 16) {
        traces[i].push_back(tmp_five_tuple);
    }
    fclose(fin);

}

void getFiles(string path, vector<string> &files) {
    //文件句柄
    long hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            //如果是目录,迭代之
            //如果不是,加入列表
            if ((fileinfo.attrib & _A_SUBDIR)) {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
            } else {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

void get_all_files(string fileDir, vector<string> &files) {
////获取该路径下的所有文件
    getFiles(fileDir, files);
    int size = files.size();
    cout << size << endl;
    for (int i = 0; i < size; i++) {
        cout << files[i].c_str() << endl;
    }
}

#define HEAVY_HITTER_THRESHOLD(total_packet) (total_packet * 2 / 10000)
//#endif
//#if WriteHHD
//    DivSketch<81920 / COUNTER_PER_BUCKET / 8, 204800> *div1 = new DivSketch<(81920 / COUNTER_PER_BUCKET /8), 204800>();
//    DivSketch<92160 / COUNTER_PER_BUCKET / 8, 307200> *div2 = new DivSketch<(92160 / COUNTER_PER_BUCKET /8), 307200>();
//    DivSketch<122880 / COUNTER_PER_BUCKET / 8, 409600> *div3 = new DivSketch<(122880 / COUNTER_PER_BUCKET /8), 409600>();
//    DivSketch<153600 / COUNTER_PER_BUCKET / 8, 512000> *div4 = new DivSketch<(153600 / COUNTER_PER_BUCKET /8), 512000>();
//    DivSketch<184320 / COUNTER_PER_BUCKET / 8, 614400> *div5 = new DivSketch<(184320 / COUNTER_PER_BUCKET /8), 614400>();
//    DivSketch<204800 / COUNTER_PER_BUCKET / 8, 716800> *div6 = new DivSketch<(204800 / COUNTER_PER_BUCKET /8), 716800>();
//    DivSketch<204800 / COUNTER_PER_BUCKET / 8, 819200> *div7 = new DivSketch<(204800 / COUNTER_PER_BUCKET /8), 819200>();
//    DivSketch<204800 / COUNTER_PER_BUCKET / 8, 921600> *div8 = new DivSketch<(204800 / COUNTER_PER_BUCKET /8), 921600>();
//    DivSketch<204800 / COUNTER_PER_BUCKET / 8, 1024000> *div9 = new DivSketch<(204800 / COUNTER_PER_BUCKET /8), 1024000>();
//    DivSketch<204800 / COUNTER_PER_BUCKET / 8, 1126400> *div10 = new DivSketch<(204800 / COUNTER_PER_BUCKET /8), 1126400>();
//    char *ARE_FILE = ;
//#endif
void measure() {
    /* region define divSketch */
    DivSketch<1024 * 80 / COUNTER_PER_BUCKET / 8, 204800> *div1 = new DivSketch<(1024 * 80 / COUNTER_PER_BUCKET /  8), 204800>();
    DivSketch<102400 / COUNTER_PER_BUCKET / 8, 307200> *div2 = new DivSketch<(102400 / COUNTER_PER_BUCKET /   8), 307200>();
    DivSketch<102400 / COUNTER_PER_BUCKET / 8, 409600> *div3 = new DivSketch<(102400 / COUNTER_PER_BUCKET /   8), 409600>();
    DivSketch<102400 / COUNTER_PER_BUCKET / 8, 512000> *div4 = new DivSketch<(102400 / COUNTER_PER_BUCKET /   8), 512000>();
    DivSketch<102400 / COUNTER_PER_BUCKET / 8, 614400> *div5 = new DivSketch<(102400 / COUNTER_PER_BUCKET /   8), 614400>();
    DivSketch<153600 / COUNTER_PER_BUCKET / 8, 716800> *div6 = new DivSketch<(153600 / COUNTER_PER_BUCKET /   8), 716800>();
    DivSketch<153600 / COUNTER_PER_BUCKET / 8, 819200> *div7 = new DivSketch<(153600 / COUNTER_PER_BUCKET /   8), 819200>();
    DivSketch<153600 / COUNTER_PER_BUCKET / 8, 921600> *div8 = new DivSketch<(153600 / COUNTER_PER_BUCKET /   8), 921600>();
    DivSketch<153600 / COUNTER_PER_BUCKET / 8, 1024000> *div9 = new DivSketch<(153600 / COUNTER_PER_BUCKET /    8), 1024000>();
    DivSketch<204800 / COUNTER_PER_BUCKET / 8, 1126400> *div10 = new DivSketch<(204800 / COUNTER_PER_BUCKET /     8), 1126400>();
    /* endregion */
//#if WriteARE
#if WriteARE
    ofstream ofs;
    ofs.open("./data/result/ARE", ios::out);
#endif
#if WriteHHD_ARE|WriteHHD
    ofstream ofs1;
    ofs1.open("./data/result/HHD_ARE1", ios::out);
#endif
#if WriteHHD_F1|WriteHHD
    ofstream ofs3;
    ofs3.open("./data/result/HHD_F11", ios::out);
#endif
#if WriteCE
    ofstream ofs2;
    ofs2.open("./data/result/CE",ios::out);
#endif
#if WriteWMRE
    ofstream ofs4;
    ofs4.open("./data/result/WMRE1",ios::out);
#endif
    for (int i = 0; i < FILE_NUM; ++i) {
        /* region 初始化*/
        cout << i << endl;
        div1->clear();
        div2->clear();
        div3->clear();
        div4->clear();
        div5->clear();
        div6->clear();
        div7->clear();
        div8->clear();
        div9->clear();
        div10->clear();
        unordered_map<string, int> Real_Freq;
        int packet_cnt = (int) traces[i].size();
        for (int j = 0; j < packet_cnt; ++j) {
            string str((const char *) (traces[i][j].key), 4);
            struct Val f = {1, static_cast<uint8_t>(traces[i][j].flag)};
//            div1->insert(reinterpret_cast<uint8_t *>(traces[i][j].key), f);
            div1->insert((uint8_t *) (traces[i][j].key), f);
            div2->insert((uint8_t *) (traces[i][j].key), f);
            div3->insert((uint8_t *) (traces[i][j].key), f);
            div4->insert((uint8_t *) (traces[i][j].key), f);
            div5->insert((uint8_t *) (traces[i][j].key), f);
            div6->insert((uint8_t *) (traces[i][j].key), f);
            div7->insert((uint8_t *) (traces[i][j].key), f);
            div8->insert((uint8_t *) (traces[i][j].key), f);
            div9->insert((uint8_t *) (traces[i][j].key), f);
            div10->insert((uint8_t *) (traces[i][j].key), f);
            Real_Freq[str]++;
        }

        double ARE[11] = {0.0};

        unordered_map<int, int> real_fm;
        unordered_map<int, int> est_fm1;
        unordered_map<int, int> est_fm2;
        unordered_map<int, int> est_fm3;
        unordered_map<int, int> est_fm4;
        unordered_map<int, int> est_fm5;
        unordered_map<int, int> est_fm6;
        unordered_map<int, int> est_fm7;
        unordered_map<int, int> est_fm8;
        unordered_map<int, int> est_fm9;
        unordered_map<int, int> est_fm10;
        /* endregion */
        for (unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it) {
            uint8_t key[4];
            memcpy(key, (it->first).c_str(), 4);
            int est_val1 = div1->query(key);
            int est_val2 = div2->query(key);
            int est_val3 = div3->query(key);
            int est_val4 = div4->query(key);
            int est_val5 = div5->query(key);
            int est_val6 = div6->query(key);
            int est_val7 = div7->query(key);
            int est_val8 = div8->query(key);
            int est_val9 = div9->query(key);
            int est_val10 = div10->query(key);

            int dist1 = std::abs(it->second - est_val1);
            int dist2 = std::abs(it->second - est_val2);
            int dist3 = std::abs(it->second - est_val3);
            int dist4 = std::abs(it->second - est_val4);
            int dist5 = std::abs(it->second - est_val5);
            int dist6 = std::abs(it->second - est_val6);
            int dist7 = std::abs(it->second - est_val7);
            int dist8 = std::abs(it->second - est_val8);
            int dist9 = std::abs(it->second - est_val9);
            int dist10 = std::abs(it->second - est_val10);

//            int est_val = div9->query(key);
//            int dist = std::abs(it->second - est_val);

            ARE[1] += dist1 * 1.0 / (it->second);
            ARE[2] += dist2 * 1.0 / (it->second);
            ARE[3] += dist3 * 1.0 / (it->second);
            ARE[4] += dist4 * 1.0 / (it->second);
            ARE[5] += dist5 * 1.0 / (it->second);
            ARE[6] += dist6 * 1.0 / (it->second);
            ARE[7] += dist7 * 1.0 / (it->second);
            ARE[8] += dist8 * 1.0 / (it->second);
            ARE[9] += dist9 * 1.0 / (it->second);
            ARE[10] += dist10 * 1.0 / (it->second);

//            ARE += dist * 1.0 / (it->second);

//            maxnum = max(maxnum,it->second);
            est_fm1[est_val1]++;
            est_fm2[est_val2]++;
            est_fm3[est_val3]++;
            est_fm4[est_val4]++;
            est_fm5[est_val5]++;
            est_fm6[est_val6]++;
            est_fm7[est_val7]++;
            est_fm8[est_val8]++;
            est_fm9[est_val9]++;
            est_fm10[est_val10]++;
            real_fm[it->second]++;
        }


        double WMRE_up[11] = {0.0};
        double WMRE_down[11] = {0.0};
        double WMRE[11] = {0.0};
#if WriteWMRE
        for (int j = 1; j < 500000; j++) {
            WMRE_up[1] += abs(est_fm1[j] - real_fm[j]);
            WMRE_down[1] += abs(est_fm1[j] + real_fm[j]);
            WMRE_up[2] += abs(est_fm2[j] - real_fm[j]);
            WMRE_down[2] += abs(est_fm2[j] + real_fm[j]);
            WMRE_up[3] += abs(est_fm3[j] - real_fm[j]);
            WMRE_down[3] += abs(est_fm3[j] + real_fm[j]);
            WMRE_up[4] += abs(est_fm4[j] - real_fm[j]);
            WMRE_down[4] += abs(est_fm4[j] + real_fm[j]);
            WMRE_up[5] += abs(est_fm5[j] - real_fm[j]);
            WMRE_down[5] += abs(est_fm5[j] + real_fm[j]);
            WMRE_up[6] += abs(est_fm6[j] - real_fm[j]);
            WMRE_down[6] += abs(est_fm6[j] + real_fm[j]);
            WMRE_up[7] += abs(est_fm7[j] - real_fm[j]);
            WMRE_down[7] += abs(est_fm7[j] + real_fm[j]);
            WMRE_up[8] += abs(est_fm8[j] - real_fm[j]);
            WMRE_down[8] += abs(est_fm8[j] + real_fm[j]);
            WMRE_up[9] += abs(est_fm9[j] - real_fm[j]);
            WMRE_down[9] += abs(est_fm9[j] + real_fm[j]);
            WMRE_up[10] += abs(est_fm10[j] - real_fm[j]);
            WMRE_down[10] += abs(est_fm10[j] + real_fm[j]);
        }


        for (int j = 1; j <= 10; ++j) {
            ARE[j] /= (int) Real_Freq.size();
            WMRE[j] = WMRE_up[j]/(WMRE_down[j]/2);
        }
#endif
#if WriteARE
        ofs << files[i] << endl;
        cout << files[i] << endl;
        for (int j = 1; j <= 10; ++j) {
            ofs << ARE[j] << endl;
            cout << ARE[j] << endl;
        }
#endif
        int threshold = HEAVY_HITTER_THRESHOLD(packet_cnt);
        cout << threshold << endl;
        unordered_map<string, int> Real_heavy_hitters;
#if WriteHHD_ARE|WriteHHD



        for (unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it) {
            if (it->second > threshold) {
                Real_heavy_hitters[it->first] = it->second;
            }
        }


        double HHD_ARE[11] = {0.0};
        for (unordered_map<string, int>::iterator it = Real_heavy_hitters.begin();
             it != Real_heavy_hitters.end(); ++it) {
            uint8_t key[4];
            memcpy(key, (it->first).c_str(), 4);
            int est_val1 = div1->query(key);
            int est_val2 = div2->query(key);
            int est_val3 = div3->query(key);
            int est_val4 = div4->query(key);
            int est_val5 = div5->query(key);
            int est_val6 = div6->query(key);
            int est_val7 = div7->query(key);
            int est_val8 = div8->query(key);
            int est_val9 = div9->query(key);
            int est_val10 = div10->query(key);

            int dist1 = std::abs(it->second - est_val1);
            int dist2 = std::abs(it->second - est_val2);
            int dist3 = std::abs(it->second - est_val3);
            int dist4 = std::abs(it->second - est_val4);
            int dist5 = std::abs(it->second - est_val5);
            int dist6 = std::abs(it->second - est_val6);
            int dist7 = std::abs(it->second - est_val7);
            int dist8 = std::abs(it->second - est_val8);
            int dist9 = std::abs(it->second - est_val9);
            int dist10 = std::abs(it->second - est_val10);

//            int est_val = div9->query(key);
//            int dist = std::abs(it->second - est_val);

            HHD_ARE[1] += dist1 * 1.0 / (it->second);
            HHD_ARE[2] += dist2 * 1.0 / (it->second);
            HHD_ARE[3] += dist3 * 1.0 / (it->second);
            HHD_ARE[4] += dist4 * 1.0 / (it->second);
            HHD_ARE[5] += dist5 * 1.0 / (it->second);
            HHD_ARE[6] += dist6 * 1.0 / (it->second);
            HHD_ARE[7] += dist7 * 1.0 / (it->second);
            HHD_ARE[8] += dist8 * 1.0 / (it->second);
            HHD_ARE[9] += dist9 * 1.0 / (it->second);
            HHD_ARE[10] += dist10 * 1.0 / (it->second);
        }
        for (int j = 1; j <= 10; ++j) {
            HHD_ARE[j] /= (int) Real_heavy_hitters.size();
        }
#endif
#if WriteHHD_F1|WriteHHD
        vector<pair<string, int> > heavy_hitters1;
        div1->get_heavy_hitters(threshold, heavy_hitters1);
        vector<pair<string, int> > heavy_hitters2;
        div2->get_heavy_hitters(threshold, heavy_hitters2);
        vector<pair<string, int> > heavy_hitters3;
        div3->get_heavy_hitters(threshold, heavy_hitters3);
        vector<pair<string, int> > heavy_hitters4;
        div4->get_heavy_hitters(threshold, heavy_hitters4);
        vector<pair<string, int> > heavy_hitters5;
        div5->get_heavy_hitters(threshold, heavy_hitters5);
        vector<pair<string, int> > heavy_hitters6;
        div6->get_heavy_hitters(threshold, heavy_hitters6);
        vector<pair<string, int> > heavy_hitters7;
        div7->get_heavy_hitters(threshold, heavy_hitters7);
        vector<pair<string, int> > heavy_hitters8;
        div8->get_heavy_hitters(threshold, heavy_hitters8);
        vector<pair<string, int> > heavy_hitters9;
        div9->get_heavy_hitters(threshold, heavy_hitters9);
        vector<pair<string, int> > heavy_hitters10;
        div10->get_heavy_hitters(threshold, heavy_hitters10);

        double TP[11] = {0.0};


        for (int k = 0; k < (int) heavy_hitters1.size(); ++k) {
            string key = heavy_hitters1[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[1] += 1;
            }
        }
        for (int k = 0; k < (int) heavy_hitters2.size(); ++k) {
            string key = heavy_hitters2[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[2] += 1;
            }
        }

        for (int k = 0; k < (int) heavy_hitters3.size(); ++k) {
            string key = heavy_hitters3[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[3] += 1;
            }
        }
        for (int k = 0; k < (int) heavy_hitters4.size(); ++k) {
            string key = heavy_hitters4[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[4] += 1;
            }
        }
        for (int k = 0; k < (int) heavy_hitters5.size(); ++k) {
            string key = heavy_hitters5[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[5] += 1;
            }
        }
        for (int k = 0; k < (int) heavy_hitters6.size(); ++k) {
            string key = heavy_hitters6[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[6] += 1;
            }
        }
        for (int k = 0; k < (int) heavy_hitters7.size(); ++k) {
            string key = heavy_hitters7[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[7] += 1;
            }
        }
        for (int k = 0; k < (int) heavy_hitters8.size(); ++k) {
            string key = heavy_hitters8[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[8] += 1;
            }
        }
        for (int k = 0; k < (int) heavy_hitters9.size(); ++k) {
            string key = heavy_hitters9[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[9] += 1;
            }
        }
        for (int k = 0; k < (int) heavy_hitters10.size(); ++k) {
            string key = heavy_hitters10[k].first;
            if (Real_heavy_hitters.find(key) != Real_heavy_hitters.end()) {
                TP[10] += 1;
            }
        }

        double precesion[11] = {0.0};
        double recall[11] = {0.0};
        double F1_Score[11] = {0.0};

        precesion[1] = TP[1] / (double) heavy_hitters1.size();
        precesion[2] = TP[2] / (double) heavy_hitters2.size();
        precesion[3] = TP[3] / (double) heavy_hitters3.size();
        precesion[4] = TP[4] / (double) heavy_hitters4.size();
        precesion[5] = TP[5] / (double) heavy_hitters5.size();
        precesion[6] = TP[6] / (double) heavy_hitters6.size();
        precesion[7] = TP[7] / (double) heavy_hitters7.size();
        precesion[8] = TP[8] / (double) heavy_hitters8.size();
        precesion[9] = TP[9] / (double) heavy_hitters9.size();
        precesion[10] = TP[10] / (double) heavy_hitters10.size();

        for (int j = 1; j <= 10; ++j) {
            recall[j] = TP[j] / (double) Real_heavy_hitters.size();
            F1_Score[j] = (2 * precesion[j] * recall[j]) / (precesion[j] + recall[j]);
        }
#endif

#if WriteCE
        double est_ce[11] = {0.0};
        est_ce[1] = div1->get_cardinality();
        est_ce[2] = div2->get_cardinality();
        est_ce[3] = div3->get_cardinality();
        est_ce[4] = div4->get_cardinality();
        est_ce[5] = div5->get_cardinality();
        est_ce[6] = div6->get_cardinality();
        est_ce[7] = div7->get_cardinality();
        est_ce[8] = div8->get_cardinality();
        est_ce[9] = div9->get_cardinality();
        est_ce[10] = div10->get_cardinality();
        double real_ce = Real_Freq.size();
        double CE_RE[11] = {0.0};
        for (int j = 1; j <= 10; ++j){
            CE_RE[j] = abs(est_ce[j]-real_ce)*1.0/real_ce;
        }



#endif
// WMRE
        cout << files[i] << endl;
#if  WriteHHD_ARE|WriteHHD
        ofs1 << files[i] << endl;
//        cout << files[i] << endl;
        cout << (int) Real_heavy_hitters.size() << endl;
        for (int j = 1; j <= 10; ++j) {
            ofs1 << HHD_ARE[j] << endl;
            cout << HHD_ARE[j] << endl;
        }
#endif
#if  WriteHHD_F1|WriteHHD
        ofs3 << files[i] << endl;
        for (int j = 1; j <= 10; ++j) {
//            ofs1 << HHD_ARE[j] << endl;
            cout << F1_Score[j] << endl;
            ofs3 << F1_Score[j] << endl;
        }
#endif
#if WriteCE
        ofs2 << files[i] << endl;
        for (int j = 1; j <= 10; ++j) {
//            ofs1 << HHD_ARE[j] << endl;
            cout <<fixed << setprecision(8)<< CE_RE[j] << endl;
            ofs2 <<fixed << setprecision(8)<< CE_RE[j] << endl;
        }
#endif
#if WriteWMRE
        ofs4 << files[i] << endl;
        for (int j = 1; j <= 10; ++j) {
//            ofs1 << HHD_ARE[j] << endl;
            cout << WMRE[j] << endl;
            ofs4 << WMRE[j] << endl;
        }
#endif



    }
#if WriteARE
    ofs.close();
#endif
#if  WriteHHD_ARE|WriteHHD
    ofs1.close();
#endif
#if  WriteHHD_F1|WriteHHD
    ofs3.close();
#endif

#if WriteCE
    ofs2.close();
#endif
#if WriteWMRE
    ofs4.close();
#endif
}

int main(int argc, char *argv[]) {

//      string dir = "../data/slice/";
    string dir = "./data/slice/";

    get_all_files(dir, files);
    for (int i = 0; i < FILE_NUM; ++i) {
        ReadInTraces(files[i].c_str(), i);
    }
//    DivSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> *div1 = new DivSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();
    const int tot_mem = 200 * 1024;
    const int heavy_mem = 100 * 1024;
    const int tot_mem_list[10] = {200 * 1024, 300 * 1024, 400 * 1024, 500 * 1024, 600 * 1024, 700 * 1024, 800 * 1024,
                                  900 * 1024, 1000 * 1024, 1100 * 1024};
    const int heavy_mem_list[10] = {100 * 1024, 100 * 1024, 100 * 1024, 100 * 1024, 100 * 1024, 150 * 1024, 150 * 1024,
                                    150 * 1024, 150 * 1024, 200 * 1024};
    int bucket_num_list[10];
    for (int i = 0; i < 10; ++i) {
//        tot_mem_list[i] = (100*i+200)*1024;
//        heavy_mem_list[i] = heavy_mem_list[i]*1024;
        bucket_num_list[i] = heavy_mem_list[i] / COUNTER_PER_BUCKET / 8;
    }

    measure();

//    struct Val f = {1, static_cast<uint8_t>(t.tos&0x01)};
//    // printf("%d %d %ld %lf\n", f.pkts, f.sizes, f.delay, f.loss);
//    div1->insert((uint8_t *) &t,f);
//    uint8_t k[4] = {0};
//    printf("query result%d",div1->query(k));
    return 0;
}



