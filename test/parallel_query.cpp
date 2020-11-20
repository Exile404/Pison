#include "parallel_query.h"


#define MAX_THREAD 70

struct GlobalQry {
    pthread_t threads[MAX_THREAD];
    ThreadMetaData th[MAX_THREAD];
    TaskQueue tq;
};

GlobalQry gq;

query_callback execute_query;

void* query_subrecs_in_thread(void* arg) {
    ThreadMetaData md = (ThreadMetaData)(*(ThreadMetaData*)arg);
    int thread_id = md.thread_id;
    // cout<<thread_id<<"th thread starts querying sub-records."<<endl;
    // bind CPU
    cpu_set_t mask;
    cpu_set_t get;
    CPU_ZERO(&mask);
    CPU_SET(thread_id, &mask);
    if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
        cout<<"CPU binding failed for thread "<<thread_id<<endl;
    struct timeval begin,end;
    double duration;
      /* gettimeofday(&begin,NULL);
        while(1)
        {
            gettimeofday(&end,NULL);
            duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
            if(duration>3000000) break;
        }*/
    gettimeofday(&begin,NULL);
    BitmapIterator* iter = md.iter;
    long start_idx = 0, end_idx = 0;
    while (next_batch(gq.tq, start_idx, end_idx)) {
//       cout<<"start index "<<start_idx<<" end index "<<end_idx<<endl;
       //end_idx = gq.tq.end_idx;
       while (start_idx < end_idx) {
           iter->moveToIndex(start_idx);
           if (iter->down()) {
               execute_query(iter, *gq.th[thread_id].output, gq.th[thread_id].output_size);
               iter->up();
           }
           ++start_idx;
       }
    }
    //gq.th[thread_id].output = md.output;
    //gq.th[thread_id].output_size = md.output_size;
    delete iter;
    //cout<<thread_id<<"th thread finishes querying sub-records"<<endl;
    gettimeofday(&end,NULL);
    duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
    //printf("The duration of query execution is %lf\n",duration/1000000);
    return NULL;
}

void parallel_query(BitmapIterator* iter, query_callback qry, int num_recs, string* output, long& output_size, int thread_num) {
    if (output == NULL) {
        output = new string[thread_num];
    }
    execute_query = qry;
    gq.tq.batch_size = get_batch_size(num_recs);
 //   cout<<"num recs "<<num_recs<<" bacth size "<<gq.tq.batch_size<<endl;
    int start_idx = 0;
    ThreadMetaData md;
    // inputs for each thread
    for (int i = 0; i < thread_num; ++i) {
        gq.th[i].thread_id = i;
        gq.th[i].iter = iter->getCopy();
        gq.th[i].output = &output[i];
        gq.th[i].output_size = 0;
    }
    gq.tq.start_idx = 0;
    gq.tq.end_idx = num_recs;
    pthread_mutex_init(&gq.tq.mutex, NULL);
    for (int i = 0; i < thread_num; ++i) {
        int rc = pthread_create(&gq.threads[i], NULL, query_subrecs_in_thread, &gq.th[i]);
        if (rc)
        {
            cout<<"Thread Error; return code is "<<rc<<endl;
            return;
        }
    }
    for(int i = 0;i < thread_num;i++)
    {
        int rc = pthread_join(gq.threads[i], NULL);
        if (rc)
        {
            cout<<"Thread Error; return code is "<<rc<<endl;
            return;
        }
    }
    //cout<<" all done"<<endl;
    // get total output matches from different threads
    for (int i = 0; i < thread_num; ++i) {
//        cout<<"thread i "<<output[i]<<endl;
        //output[i] = gq.th[i].output;
        output_size += gq.th[i].output_size;
        //delete gq.th[i].iter;
    }
}
