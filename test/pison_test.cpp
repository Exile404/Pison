#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"
#include "test_queries.h"
#include "parallel_query.h"
#include <sys/time.h>
#include <sys/file.h>
#include <unordered_set>
using namespace std;

void testPisonSmallRecords(char* file_path, query_callback qry = NULL, int level = MAX_LEVEL, bool support_array = true, bool print = true) {
    struct timeval begin,end;
    double duration;
    RecordLoader loader(file_path);
    Records* recs = loader.loadRecords();;
    if (recs == NULL) {
        cout<<"record loading fails."<<endl;
        return;
    }
    int num_iterations = 10;
    vector<double> exe_time;
    long output_size = 0;
    for (int i = 1; i <= num_iterations; ++i) {
        gettimeofday(&begin,NULL);
        string output;
        output_size = 0;
        // visit each record sequentially
        int start_idx = 0;
        int end_idx = recs->num_records;
        int thread_num = 1;
        while (start_idx < end_idx) {
            Bitmap* bm = BitmapConstructor::construct(recs, start_idx, thread_num, level, support_array);
            if (qry != NULL) {
                BitmapIterator* iter = BitmapConstructor::getIterator(bm);
                qry(iter, output, output_size);
                delete iter;
            }
            delete bm;
            ++start_idx;
        }
        gettimeofday(&end,NULL);
        duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
        double time = duration / (double) 1000000;
        exe_time.push_back(time);
        /*printf("The duration of small records processing is %lf %lf\n",duration/1000000, time);
        if (qry != NULL)
            cout<<"the total number of output matches is "<<output_size<<endl;*/
    }
    if (print == true) {
        double sum_exe_time = 0.0;
        for (int i = 0; i < num_iterations; ++i) {
            sum_exe_time += exe_time[i];
            //cout<<"exe time "<<exe_time[i]<<" "<<sum_exe_time<<endl;
        }
        //cout<<"sum exe time "<<sum_exe_time<<" iterations "<<num_iterations<<endl;
        double avg_exe_time = sum_exe_time / (double)num_iterations;
        cout<<"[Performance Result]: total execution time is "<<avg_exe_time<<" seconds (avg. of 10 runs)"<<endl;
        if (qry != NULL)
            cout<<"[Number of Matches]: "<<output_size<<endl;
    }
    delete recs;
}

// records in National Statistics Postcode Lookup (NSPL) dataset contain both object records and array records, which are different
// from other datasets. Current BitmapConstructor::getIterator function generates leveled colon bitmaps for the first level of the
// record by default. In fact, if the record type is not matched, BitmapConstructor::getIterator function can be skipped, so that
// the performance can be further improved.
void testPisonSmallRecordsSpecial(char* file_path, query_callback qry = NULL, int level = MAX_LEVEL, bool support_array = true) {
    struct timeval begin,end;
    double duration;
    RecordLoader loader(file_path);
    Records* recs = loader.loadRecords();;
    if (recs == NULL) {
        cout<<"record loading fails."<<endl;
        return;
    }
    int num_iterations = 10;
    vector<double> exe_time;
    long output_size = 0;
    for (int i = 1; i <= num_iterations; ++i) {
        gettimeofday(&begin,NULL);
        string output;
        output_size = 0;
        // visit each record sequentially
        int start_idx = 0;
        int end_idx = recs->num_records;
        int thread_num = 1;
        while (start_idx < end_idx) {
            Bitmap* bm = BitmapConstructor::construct(recs, start_idx, thread_num, level, support_array);
            if (qry != NULL) {
                // check whether the first non-empty character is '{'
                char* record = recs->text + recs->rec_start_pos[start_idx];
                int idx = 0;
                while (record[idx] == ' ') ++idx;
                if (record[idx++] != '{') {
                    delete bm;
                    ++start_idx;
                    continue;
                }
                BitmapIterator* iter = BitmapConstructor::getIterator(bm);
                qry(iter, output, output_size);
                delete iter;
            }
            delete bm;
            ++start_idx;
        }
        gettimeofday(&end,NULL);
        duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
        double time = duration / (double) 1000000;
        exe_time.push_back(time);
    }
    double sum_exe_time = 0.0;
    for (int i = 0; i < num_iterations; ++i) {
        sum_exe_time += exe_time[i];
    }
    double avg_exe_time = sum_exe_time / (double)num_iterations;
    cout<<"[Performance Result]: total execution time is "<<avg_exe_time<<" seconds (avg. of 10 runs)"<<endl;
    if (qry != NULL)
        cout<<"[Number of Matches]: "<<output_size<<endl;
    delete recs;
}

void testPison(char* file_path, query_callback qry = NULL, int level = MAX_LEVEL, bool support_array = true, bool print = true) {
    struct timeval begin,end;
    double duration;
    RecordLoader loader(file_path);
    Records* records = loader.loadSingleRecord();
    if (records == NULL) {
        cout<<"record loading fails."<<endl;
        return;
    }
    int num_iterations = 10;
    vector<double> idx_construction_time;
    vector<double> query_exe_time;
    long output_size = 0;
    for (int i = 1; i <= num_iterations; ++i) {
        gettimeofday(&begin,NULL);
        int rec_id = 0;
        int thread_num = 1;
        Bitmap* bm = BitmapConstructor::construct(records, rec_id, thread_num, level, support_array);
        gettimeofday(&end,NULL);
        duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
        double time = duration / (double) 1000000;
        // cout<<"index construction time "<<time<<endl;
        idx_construction_time.push_back(time); 
        if (qry != NULL) {
            gettimeofday(&begin,NULL);
            BitmapIterator* iter = BitmapConstructor::getIterator(bm);
            // query execution
            string output;
            output_size = 0;
            qry(iter, output, output_size);
            //cout<<"the total number of output matches is "<<output_size<<endl;
            delete iter;
            gettimeofday(&end,NULL);
            duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
            time = duration / (double) 1000000;
            query_exe_time.push_back(time);
        }
        delete bm;
    }
    if (print == true) {
        double sum_idx_time = 0.0;
        double sum_query_time = 0.0;
        for (int i = 0; i < num_iterations; ++i) {
            sum_idx_time += idx_construction_time[i];
            if (qry != NULL) sum_query_time += query_exe_time[i];
        }
        double avg_idx_time = sum_idx_time / (double)num_iterations;
        cout<<"[Performance Result]: index construction time is "<<avg_idx_time<<" seconds (avg. of 10 runs)"<<endl;
        double avg_query_time = 0.0;
        if (qry != NULL) {
            avg_query_time = sum_query_time / (double)num_iterations;
            cout<<"[Performance Result]: query evaluation time is "<<avg_query_time<<" seconds (avg. of 10 runs)"<<endl;
            cout<<"[Number of Matches]: "<<output_size<<endl;
        }
        double avg_tot_time = avg_idx_time + avg_query_time;
        cout<<"[Performance Result]: total execution time is "<<avg_tot_time<<" seconds (avg. of 10 runs)"<<endl;
    }
    delete records;
}

void testPisonParallel(char* file_path, int thread_num, parallel_query_callback qry = NULL, int level = MAX_LEVEL, bool support_array = true) {
    struct timeval begin,end;
    double duration;
    RecordLoader loader(file_path);
    Records* records = loader.loadSingleRecord();
    if (records == NULL) {
        cout<<"record loading fails."<<endl;
        return;
    }
    int num_iterations = 20;
    vector<double> idx_construction_time;
    vector<double> query_exe_time;
    long output_size = 0;
    for (int i = 1; i <= num_iterations; ++i) {
        gettimeofday(&begin,NULL);
        int rec_id = 0;
        Bitmap* bm = BitmapConstructor::construct(records, rec_id, thread_num, level, support_array);
        gettimeofday(&end,NULL);
        duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
        // cout<<"finish index construction "<<endl;
        double time = duration / (double) 1000000;
        idx_construction_time.push_back(time);
        if (qry != NULL) {
            gettimeofday(&begin,NULL);
            BitmapIterator* iter = BitmapConstructor::getIterator(bm);
            // query execution
            string output[MAX_THREAD];
            output_size = 0;
            qry(iter, output, output_size, thread_num);
            // cout<<"the total number of output matches is "<<output_size<<endl;
            delete iter;
            gettimeofday(&end,NULL);
            duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
            time = duration / (double) 1000000;
            if (i > 10) query_exe_time.push_back(time);
        }
        delete bm;
    }
    double sum_idx_time = 0.0;
    double sum_query_time = 0.0;
    for (int i = 0; i < num_iterations; ++i) {
        sum_idx_time += idx_construction_time[i];
        if (qry != NULL) sum_query_time += query_exe_time[i];
    }
    double avg_idx_time = sum_idx_time / (double)num_iterations;
    cout<<"[Performance Result]: index construction time is "<<avg_idx_time<<" seconds (avg. of 10 runs)"<<endl;
    double avg_query_time = 0.0;
    if (qry != NULL) {
        avg_query_time = sum_query_time / (double)num_iterations;
        cout<<"[Performance Result]: query evaluation time is "<<avg_query_time<<" seconds (avg. of 10 runs)"<<endl;
        cout<<"[Number of Matches]: "<<output_size<<endl;
    }
    double avg_tot_time = avg_idx_time + avg_query_time;
    cout<<"[Performance Result]: total execution time is "<<avg_tot_time<<" seconds (avg. of 10 runs)"<<endl;
    delete records;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("the number of input arguments should be at least three: file_path, input_type(small (for small records), large (for large record)), query_id used in Pison paper\n");
        return -1;
    }
    char* file_path = argv[1];
    char* input_type = argv[2];
    string query_id(argv[3]);
    int thread_num = 1;
    cout<<endl;
    cout<<"------------------------------------------------------------------------------------------"<<endl;
    cout<<"[Start Evaluation]: "<<query_id<<" on file "<<file_path<<endl;
    if (strcmp(input_type, "large") == 0) {
        if (argc < 5) {
            printf("please input the number of threads for large record processing\n");
            return -1;
        }
        thread_num = atoi(argv[4]);
        // cout<<"the number of threads used for large record processing is "<<thread_num<<endl;
    }
    if (thread_num == 1) {
        //cout<<"warming up CPU"<<endl;
        if (strcmp(input_type, "small") == 0) {
            testPisonSmallRecords(file_path, NULL, MAX_LEVEL, true, false);
        } else {
            if (thread_num == 1)
                testPison(file_path, NULL, MAX_LEVEL, true, false);
        }
        //cout<<"finish warming up CPU"<<endl;
    }
    if (strcmp(input_type, "small") == 0) {
        if (query_id == "TT0") {
            testPisonSmallRecords(file_path, query_tt0_small, 0, false);
        } else if (query_id == "TT1") {
            testPisonSmallRecords(file_path, query_tt1_small, 1, false);
        } else if (query_id == "TT2") {
            testPisonSmallRecords(file_path, query_tt2_small, 1, false);
        } else if (query_id == "TT3") {
            testPisonSmallRecords(file_path, query_tt3_small, 1, false);
        } else if (query_id == "TT4") {
            testPisonSmallRecords(file_path, query_tt4_small, 1, false);
        } else if (query_id == "TT5") {
            testPisonSmallRecords(file_path, query_tt5_small, 1, false);
        } else if (query_id == "TT6") {
            testPisonSmallRecords(file_path, query_tt6_small, 1, false);
        } else if (query_id == "TT7") {
            testPisonSmallRecords(file_path, query_tt7_small, 3);
        } else if (query_id == "TT8") {
            testPisonSmallRecords(file_path, query_tt8_small, 4);
        } else if (query_id == "TT9") {
            testPisonSmallRecords(file_path, query_tt9_small, 3);
        } else if (query_id == "BB") {
            testPisonSmallRecords(file_path, query_bb1_small, 2);
        } else if (query_id == "GMD") {
            testPisonSmallRecords(file_path, query_gmd1_small, 7);
        } else if (query_id == "NSPL") {
            testPisonSmallRecordsSpecial(file_path, query_nspl1_small, 4);
        } else if (query_id == "WM") {
            testPisonSmallRecords(file_path, query_wm1_small, 1, false);
        } else if (query_id == "WP") {
            testPisonSmallRecords(file_path, query_wp1_small, 4);
        } else {
            testPisonSmallRecords(file_path);
        }
    } else if (strcmp(input_type, "large") == 0) {
        if (query_id == "TT0") {
            if (thread_num == 1)
                testPison(file_path, query_tt0_large, 1);
            else testPisonParallel(file_path, thread_num, query_tt0_large_parallel, 1);
        } else if (query_id == "TT1") {
            if (thread_num == 1)
                testPison(file_path, query_tt1_large, 2);
            else testPisonParallel(file_path, thread_num, query_tt1_large_parallel, 2);
        } else if (query_id == "TT2") {
            if (thread_num == 1)
                testPison(file_path, query_tt2_large, 2);
            else testPisonParallel(file_path, thread_num, query_tt2_large_parallel, 2);
        } else if (query_id == "TT3") {
            if (thread_num == 1)
                testPison(file_path, query_tt3_large, 2);
            else testPisonParallel(file_path, thread_num, query_tt3_large_parallel, 2);
        } else if (query_id == "TT4") {
            if (thread_num == 1)
                testPison(file_path, query_tt4_large, 2);
            else testPisonParallel(file_path, thread_num, query_tt4_large_parallel, 2);
        } else if (query_id == "TT5") {
            if (thread_num == 1)
                testPison(file_path, query_tt5_large, 2);
            else testPisonParallel(file_path, thread_num, query_tt5_large_parallel, 2);
        } else if (query_id == "TT6") {
            if (thread_num == 1)
                testPison(file_path, query_tt6_large, 2);
            else testPisonParallel(file_path, thread_num, query_tt6_large_parallel, 2);
        } else if (query_id == "TT7") {
            if (thread_num == 1)
                testPison(file_path, query_tt7_large, 4);
            else testPisonParallel(file_path, thread_num, query_tt7_large_parallel, 4);
        } else if (query_id == "TT8") {
            if (thread_num == 1)
                testPison(file_path, query_tt8_large, 5);
            else testPisonParallel(file_path, thread_num, query_tt8_large_parallel, 5);
        } else if (query_id == "TT9") {
            if (thread_num == 1)
                testPison(file_path, query_tt9_large, 4);
            else testPisonParallel(file_path, thread_num, query_tt9_large_parallel, 4);
        } else if (query_id == "BB") {
            if (thread_num == 1)
                testPison(file_path, query_bb1_large, 4);
            else testPisonParallel(file_path, thread_num, query_bb1_large_parallel, 4);
        } else if (query_id == "GMD") {
            if (thread_num == 1)
                testPison(file_path, query_gmd1_large, 8);
            else testPisonParallel(file_path, thread_num, query_gmd1_large_parallel, 8);
        } else if (query_id == "NSPL") {
            if (thread_num == 1)
                testPison(file_path, query_nspl1_large, 4);
            else testPisonParallel(file_path, thread_num, query_nspl1_large_parallel, 4);
        } else if (query_id == "WM") {
            if (thread_num == 1)
                testPison(file_path, query_wm1_large, 3);
            else testPisonParallel(file_path, thread_num, query_wm1_large_parallel, 3);
        } else if (query_id == "WP") {
            if (thread_num == 1)
                testPison(file_path, query_wp1_large, 5);
            else testPisonParallel(file_path, thread_num, query_wp1_large_parallel, 5);
        } else {
            if (thread_num == 1)
                testPison(file_path);
            else testPisonParallel(file_path, thread_num);
        }
    }
    cout<<"[End Evaluation]: "<<query_id<<" on file "<<file_path<<endl;
    cout<<"------------------------------------------------------------------------------------------"<<endl;
    return 0;
}
