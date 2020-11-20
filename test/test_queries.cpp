#include "test_queries.h"

// $.id
void query_tt0_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject() && iter->moveToKey("id")) {
        // value of "id*/"
        char* value = iter->getValue();
        output.append(value);
        output.append("|");
        ++output_size;
        if (value) free(value);
    }
}

// $[*].id
void query_tt0_large(BitmapIterator* iter, string& output, long& output_size) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return; /* value of "root" */
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue; /* inside the current array element */
            if (iter->isObject() && iter->moveToKey("id")) {
                // value of "id*/"
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (output_size == 1) cout<<"output "<<output<<endl;
                if (value) free(value);
            }
            iter->up();
        }
        //iter->up();
    //}
}

// $[*].id
void query_tt0_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //struct timeval begin,end;
    //double duration;
    //if (iter->isObject() && iter->moveToKey("root")) {
        // dominating array is on the top level
        // iter->parallelDomArrayFlag(true);
        //if (iter->down() == false) return;
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // cout<<"dominating array "<<num_recs<<endl;
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt0_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt0_small, num_recs, output, output_size, 1);
            }
        }
        //iter->up();
    //}
}

// $.user.id
void query_tt1_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject() && iter->moveToKey("user")) {
        if (iter->down() == false) return; /* value of "user" */
        if (iter->isObject() && iter->moveToKey("id")) {
            // value of "id"
            char* value = iter->getValue();
            output.append(value);
            output.append("|");
            ++output_size;
            if (value) free(value);
        }
        iter->up();
    }
}

// $[*].user.id
void query_tt1_large(BitmapIterator* iter, string& output, long& output_size) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return; /* value of "root" */
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("user")) {
                if (iter->down() == false) return; /* value of "user" */
                if (iter->isObject() && iter->moveToKey("id")) {
                // value of "id"
                    char* value = iter->getValue();
                    output.append(value);
                    output.append("|");
                    ++output_size;
                    if (value) free(value);
               }
               iter->up();
           }
           iter->up();
        }
        //iter->up();
    //}
}

// $[*].user.id
void query_tt1_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //iter->parallelDomArrayFlag(true);
        //if (iter->down() == false) return;
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt1_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt1_small, num_recs, output, output_size, 1);
            }
        }
        //iter->up();
    //}
}

// {$.user.id, $.retweet_count}
void query_tt2_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("user");
        set.insert("retweet_count");
        char* key = NULL;
        while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "retweet_count") == 0) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            } else {  /* value of "user" */
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("id")) {
                    char* value = iter->getValue();
                    output.append(value);
                    output.append("|");
                    ++output_size;
                    if (value) free(value);
                }
                iter->up();
            }
        }
    }
}

// {$[*].user.id, $[*].retweet_count}
void query_tt2_large(BitmapIterator* iter, string& output, long& output_size) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return; /* value of "root" */
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject()) {
                unordered_set<char*> set;
                set.insert("user");
                set.insert("retweet_count");
                char* key = NULL;
                while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                    if (strcmp(key, "retweet_count") == 0) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    } else {  /* value of "user" */
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("id")) {
                            char* value = iter->getValue();
                            output.append(value);
                            output.append("|");
                            ++output_size;
                            if (value) free(value);
                        }
                        iter->up();
                    }
                }
            }
            iter->up();
        }
        //iter->up();
    //}
}

// {$[*].user.id, $[*].retweet_count}
void query_tt2_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //iter->parallelDomArrayFlag(true);
        // if (iter->down() == false) return;
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt2_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt2_small, num_recs, output, output_size, 1);
            }
        }
        //iter->up();
    //}
}

// {$.user.id, $.user.lang}
void query_tt3_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject() && iter->moveToKey("user")) {
        if (iter->down() == false) return; /* value of "user" */
        if (iter->isObject()) {
            unordered_set<char*> set;
            set.insert("id");
            set.insert("lang");
            char* key = NULL;
            while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            }
        }
        iter->up();
    }
}

// {$[*].user.id, $[*].user.lang}
void query_tt3_large(BitmapIterator* iter, string& output, long& output_size) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return; /* value of "root" */
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("user")) {
                if (iter->down() == false) return; /* value of "user" */
                if (iter->isObject()) {
                    unordered_set<char*> set;
                    set.insert("id");
                    set.insert("lang");
                    char* key = NULL;
                    while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    }
                }
                iter->up();
            }
            iter->up();
        }
    //}
}

// {$[*].user.id, $[*].user.lang}
void query_tt3_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //iter->parallelDomArrayFlag(true);
        //if (iter->down() == false) return;
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt3_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt3_small, num_recs, output, output_size, 1);
            }
        }
        //iter->up();
    //}
}

// {$.user.name, $.in_reply_to_status_id}
void query_tt4_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("user");
        set.insert("in_reply_to_status_id");
        char* key = NULL;
        while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "in_reply_to_status_id") == 0) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            } else {  /* value of "user" */
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("name")) {
                    char* value = iter->getValue();
                    output.append(value);
                    output.append("|");
                    ++output_size;
                    if (value) free(value);
                }
                iter->up();
            }
        }
    }
}

// {$[*].user.name, $[*].in_reply_to_status_id}
void query_tt4_large(BitmapIterator* iter, string& output, long& output_size) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return; /* value of "root" */
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject()) {
                unordered_set<char*> set;
                set.insert("user");
                set.insert("in_reply_to_status_id");
                char* key = NULL;
                while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                    if (strcmp(key, "in_reply_to_status_id") == 0) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    } else {  /* value of "user" */
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("name")) {
                            char* value = iter->getValue();
                            output.append(value);
                            output.append("|");
                            ++output_size;
                            if (value) free(value);
                        }
                        iter->up();
                    }
                }
            }
            iter->up();
        }
        //iter->up();
    //}
}

// {$[*].user.name, $[*].in_reply_to_status_id}
void query_tt4_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //iter->parallelDomArrayFlag(true);
        //if (iter->down() == false) return;
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt4_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt4_small, num_recs, output, output_size, 1);
            }
        }
        //iter->up();
    //}
}

// {$.user.lang, $.lang}
void query_tt5_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("user");
        set.insert("lang");
        char* key = NULL;
        while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "lang") == 0) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            } else {  /* value of "user" */
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("lang")) {
                    char* value = iter->getValue();
                    output.append(value);
                    output.append("|");
                    ++output_size;
                    if (value) free(value);
                }
                iter->up();
            }
        }
    }
}

// {$[*].user.lang, $[*].lang}
void query_tt5_large(BitmapIterator* iter, string& output, long& output_size) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // if (iter->down() == false) return; /* value of "root" */
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject()) {
                unordered_set<char*> set;
                set.insert("user");
                set.insert("lang");
                char* key = NULL;
                while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                    if (strcmp(key, "lang") == 0) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    } else {  /* value of "user" */
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("lang")) {
                            char* value = iter->getValue();
                            output.append(value);
                            output.append("|");
                            ++output_size;
                            if (value) free(value);
                        }
                        iter->up();
                    }
                }
            }
            iter->up();
        }
        // iter->up();
    // }
}

// {$[*].user.lang, $[*].lang}
void query_tt5_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //iter->parallelDomArrayFlag(true);
        // if (iter->down() == false) return;
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt5_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt5_small, num_recs, output, output_size, 1);
            }
        }
        // iter->up();
    // }
}

// {$.id, $.retweeted_status.id}
void query_tt6_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("id");
        set.insert("retweeted_status");
        char* key = NULL;
        while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "id") == 0) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            } else {  /* value of "user" */
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("id")) {
                    char* value = iter->getValue();
                    output.append(value);
                    output.append("|");
                    ++output_size;
                    if (value) free(value);
                }
                iter->up();
            }
        }
    }
}

// {$[*].id, $[*].retweeted_status.id}
void query_tt6_large(BitmapIterator* iter, string& output, long& output_size) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // if (iter->down() == false) return; /* value of "root" */
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject()) {
                unordered_set<char*> set;
                set.insert("id");
                set.insert("retweeted_status");
                char* key = NULL;
                while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                    if (strcmp(key, "id") == 0) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    } else {  /* value of "user" */
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("id")) {
                            char* value = iter->getValue();
                            output.append(value);
                            output.append("|");
                            ++output_size;
                            if (value) free(value);
                        }
                        iter->up();
                    }
                }
            }
            iter->up();
        }
        // iter->up();
    // }
}

// {$[*].id, $[*].retweeted_status.id}
void query_tt6_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        //iter->parallelDomArrayFlag(true);
        // if (iter->down() == false) return;
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt6_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt6_small, num_recs, output, output_size, 1);
            }
        }
        // iter->up();
    // }
}

// {$.id, $.entities.urls[*].url}
void query_tt7_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("id");
        set.insert("entities");
        char* key = NULL;
        while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "id") == 0) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            } else {  /* value of "entities" */
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("urls")) {
                    if (iter->down() == false) continue;
                    while (iter->isArray() && iter->moveNext() == true) {
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("url")) {
                            char* value = iter->getValue();
                            output.append(value);
                            output.append("|");
                            ++output_size;
                            if (value) free(value);
                        }
                        iter->up();
                    }
                    iter->up();
                }
                iter->up();
            }
        }

    }
}

// {$[*].id, $[*].entities.urls[*].url}
void query_tt7_large(BitmapIterator* iter, string& output, long& output_size) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // iter->down();
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject()) {
                unordered_set<char*> set;
                set.insert("id");
                set.insert("entities");
                char* key = NULL;
                while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                    if (strcmp(key, "id") == 0) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    } else {  /* value of "entities" */
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("urls")) {
                            if (iter->down() == false) continue;
                            while (iter->isArray() && iter->moveNext() == true) {
                                if (iter->down() == false) continue;
                                if (iter->isObject() && iter->moveToKey("url")) {
                                    char* value = iter->getValue();
                                    output.append(value);
                                    output.append("|");
                                    ++output_size;
                                    if (value) free(value);
                                }
                                iter->up();
                            }
                            iter->up();
                        }
                        iter->up();
                    }
                }
            }
            iter->up();
        }
    // }
}

// {$[*].id, $[*].entities.urls[*].url}
void query_tt7_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // iter->parallelDomArrayFlag(true);
        // iter->down();
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt7_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt7_small, num_recs, output, output_size, 1);
            }
        }
    //}
}

// {$.id, $.entities.urls[*].indices[*]}
void query_tt8_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("id");
        set.insert("entities");
        char* key = NULL;
        while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "id") == 0) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            } else {  /* value of "entities" */
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("urls")) {
                    if (iter->down() == false) continue;
                    while (iter->isArray() && iter->moveNext() == true) {
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("indices")) {
                            if (iter->down() == false) continue;
                            while (iter->isArray() && iter->moveNext() == true) {
                                char* value = iter->getValue();
                                output.append(value);
                                output.append("|");
                                ++output_size;
                                //cout<<value<<endl;
                                if (value) free(value);
                            }
                            iter->up();
                        }
                        iter->up();
                    }
                    iter->up();
                }
                iter->up();
            }
        }
    }
}

// {$[*].id, $[*].entities.urls[*].indices[*]}
void query_tt8_large(BitmapIterator* iter, string& output, long& output_size) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //iter->down();
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject()) {
                unordered_set<char*> set;
                set.insert("id");
                set.insert("entities");
                char* key = NULL;
                while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                    if (strcmp(key, "id") == 0) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    } else {  /* value of "entities" */
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("urls")) {
                            if (iter->down() == false) continue;
                            while (iter->isArray() && iter->moveNext() == true) {
                                if (iter->down() == false) continue;
                                if (iter->isObject() && iter->moveToKey("indices")) {
                                    if (iter->down() == false) continue;
                                    while (iter->isArray() && iter->moveNext() == true) {
                                        char* value = iter->getValue();
                                        output.append(value);
                                        output.append("|");
                                        ++output_size;
                                        if (value) free(value);
                                    }
                                    iter->up();
                                }
                                iter->up();
                            }
                            iter->up();
                        }
                        iter->up();
                    }
                }
            }
            iter->up();
        }
    //}
}

// {$[*].id, $[*].entities.urls[*].indices[*]}
void query_tt8_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // iter->parallelDomArrayFlag(true);
        // iter->down();
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt8_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt8_small, num_recs, output, output_size, 1);
            }
        }
    // }
}

// {$.id, $.entities.urls[*].url, $.entities.urls[*].expanded_url, $.entities.urls[*].display_url}
void query_tt9_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("id");
        set.insert("entities");
        char* key = NULL;
        while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "id") == 0) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            } else {  /* value of "entities" */
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("urls")) {
                    if (iter->down() == false) continue;
                    while (iter->isArray() && iter->moveNext() == true) {
                        if (iter->down() == false) continue;
                        if (iter->isObject()) {
                            unordered_set<char*> set1;
                            set1.insert("url");
                            set1.insert("expanded_url");
                            set1.insert("display_url");
                            while (set1.empty() == false && (key = iter->moveToKey(set1)) != NULL) {
                                char* value = iter->getValue();
                                output.append(value);
                                output.append("|");
                                ++output_size;
                                if (value) free(value);
                            }
                        }
                        iter->up();
                    }
                    iter->up();
                }
                iter->up();
            }
        }
    }
}

// {$[*].id, $[*].entities.urls[*].url, $[*].entities.urls[*].expanded_url, $[*].entities.urls[*].display_url}
void query_tt9_large(BitmapIterator* iter, string& output, long& output_size) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // iter->down();
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject()) {
                unordered_set<char*> set;
                set.insert("id");
                set.insert("entities");
                char* key = NULL;
                while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                    if (strcmp(key, "id") == 0) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    } else {  /* value of "entities" */
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("urls")) {
                            if (iter->down() == false) continue;
                            while (iter->isArray() && iter->moveNext() == true) {
                                if (iter->down() == false) continue;
                                if (iter->isObject()) {
                                    unordered_set<char*> set1;
                                    set1.insert("url");
                                    set1.insert("expanded_url");
                                    set1.insert("display_url");
                                    while (set1.empty() == false && (key = iter->moveToKey(set1)) != NULL) {
                                        char* value = iter->getValue();
                                        output.append(value);
                                        output.append("|");
                                        ++output_size;
                                        if (value) free(value);
                                    }
                                }
                                iter->up();
                            }
                            iter->up();
                        }
                        iter->up();
                    }
                }
            }
            iter->up();
        }
        // iter->up();
    // }
}

// {$[*].id, $[*].entities.urls[*].url, $[*].entities.urls[*].expanded_url, $[*].entities.urls[*].display_url}
void query_tt9_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        // iter->parallelDomArrayFlag(true);
        // iter->down();
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_tt9_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_tt9_small, num_recs, output, output_size, 1);
            }
        }
    // }
}

// {$.categoryPath[1:3].id}
void query_bb1_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject() && iter->moveToKey("categoryPath")) {
        if (iter->down() == false) return; /* value of "categoryPath" */
        if (iter->isArray()) {
            for (int idx = 1; idx <= 2; ++idx) {
                if (iter->moveToIndex(idx)) {
                    if (iter->down() == false) continue;
                    if (iter->isObject() && iter->moveToKey("id")) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    }
                    iter->up();
                }
            }
        }
        iter->up();
    }
}

// {$.products[*].categoryPath[1:3].id}
void query_bb1_large(BitmapIterator* iter, string& output, long& output_size) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // if (iter->down() == false) return;
        if (iter->isObject() && iter->moveToKey("products")) {
            if (iter->down() == false) return;
            while (iter->isArray() && iter->moveNext() == true) {
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("categoryPath")) {
                    if (iter->down() == false) continue; /* value of "categoryPath" */
                    if (iter->isArray()) {
                        for (int idx = 1; idx <= 2; ++idx) {
                            if (iter->moveToIndex(idx)) {
                                if (iter->down() == false) continue;
                                if (iter->isObject() && iter->moveToKey("id")) {
                                    char* value = iter->getValue();
                                    output.append(value);
                                    output.append("|");
                                    ++output_size;
                                    if (value) free(value);
                                }
                                iter->up();
                            }
                        }
                    }
                    iter->up();
                }
                iter->up();
            }
            iter->up();
        }
        //iter->up();
    //}
}

// {$.products[*].categoryPath[1:3].id}
void query_bb1_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // if (iter->down() == false) return;
        if (iter->isObject() && iter->moveToKey("products")) {
            //iter->parallelDomArrayFlag(true);
            if (iter->down() == false) return;
            if (iter->isArray()) {
                int num_recs = iter->numArrayElements();
                // get dominating array
                if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                    parallel_query(iter, query_bb1_small, num_recs, output, output_size, thread_num);
                } else {
                    // fall back to sequential querying
                    parallel_query(iter, query_bb1_small, num_recs, output, output_size, 1);
                }
            }
            iter->up();
        }
        // iter->up();
    // }
}

// {$.routes[*].legs[*].steps[*].distance.text}
void query_gmd1_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject() && iter->moveToKey("routes")) {
        if (iter->down() == false) return;
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("legs")) {
                if (iter->down() == false) continue;
                while (iter->isArray() && iter->moveNext() == true) {
                    if (iter->down() == false) continue;
                    if (iter->isObject() && iter->moveToKey("steps")) {
                        if (iter->down() == false) continue;
                        while (iter->isArray() && iter->moveNext() == true) {
                            if (iter->down() == false) continue;
                            if (iter->isObject() && iter->moveToKey("distance")) {
                                if (iter->down() == false) continue;
                                if (iter->isObject() && iter->moveToKey("text")) {
                                    char* value = iter->getValue();
                                    output.append(value);
                                    output.append("|");
                                    ++output_size;
                                    if (value) free(value);
                                }
                                iter->up();
                            }
                            iter->up();
                        }
                        iter->up();
                    }
                    iter->up();
                }
                iter->up();
           }
           iter->up();
        }
        iter->up();
    }
}

// {$[*].routes[*].legs[*].steps[*].distance.text}
void query_gmd1_large(BitmapIterator* iter, string& output, long& output_size) {
     //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return;
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("routes")) {
                if (iter->down() == false) continue;
                while (iter->isArray() && iter->moveNext() == true) {
                    if (iter->down() == false) continue;
                    if (iter->isObject() && iter->moveToKey("legs")) {
                        if (iter->down() == false) continue;
                        while (iter->isArray() && iter->moveNext() == true) {
                            if (iter->down() == false) continue;
                            if (iter->isObject() && iter->moveToKey("steps")) {
                                if (iter->down() == false) continue;
                                while (iter->isArray() && iter->moveNext() == true) {
                                    if (iter->down() == false) continue;
                                    if (iter->isObject() && iter->moveToKey("distance")) {
                                        if (iter->down() == false) continue;
                                        if (iter->isObject() && iter->moveToKey("text")) {
                                            char* value = iter->getValue();
                                            output.append(value);
                                            output.append("|");
                                            ++output_size;
                                            if (value) free(value);
                                        }
                                        iter->up();
                                    }
                                    iter->up();
                                }
                                iter->up();
                            }
                            iter->up();
                        }
                        iter->up();
                    }
                    iter->up();
                }
                iter->up();
            }
            iter->up();
        }
        //iter->up();
    //}
}

// {$[*].routes[*].legs[*].steps[*].distance.text}
void query_gmd1_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // iter->parallelDomArrayFlag(true);
        // iter->down();
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_gmd1_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_gmd1_small, num_recs, output, output_size, 1);
            }
        }
    // }
}

// {$.meta.view.columns[*].name}
void query_nspl1_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject() && iter->moveToKey("meta")) {
        if (iter->down() == false) return;
        if (iter->isObject() && iter->moveToKey("view")) {
            if (iter->down() == false) return;
            if (iter->isObject() && iter->moveToKey("columns")) {
                if (iter->down() == false) return;
                while (iter->isArray() && iter->moveNext() == true) {
                    if (iter->down() == false) continue;
                    if (iter->isObject() && iter->moveToKey("name")) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    }
                    iter->up();
                }
                iter->up();
            }
            iter->up();
        }
        iter->up();
    }
}

// {$.meta.view.columns[*].name}
void query_nspl1_large(BitmapIterator* iter, string& output, long& output_size) {
    // if (iter->isObject() && iter->moveToKey("root")) {
        // if (iter->down() == false) return;
        if (iter->isObject() && iter->moveToKey("meta")) {
            if (iter->down() == false) return;
            if (iter->isObject() && iter->moveToKey("view")) {
                if (iter->down() == false) return;
                if (iter->isObject() && iter->moveToKey("columns")) {
                    if (iter->down() == false) return;
                    while (iter->isArray() && iter->moveNext() == true) {
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("name")) {
                            char* value = iter->getValue();
                            output.append(value);
                            output.append("|");
                            ++output_size;
                            if (value) free(value);
                        }
                        iter->up();
                    }
                    iter->up();
                }
                iter->up();
            }
            iter->up();
        }
        // iter->up();
    // }
}

// {$.meta.view.columns[*].name}
void query_nspl1_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    // only the first sub-record inside dominating array is visited during query execution
    query_nspl1_large(iter, output[0], output_size);
}

// {$.bestMarketplacePrice.price, $.name}
void query_wm1_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("bestMarketplacePrice");
        set.insert("name");
        char* key = NULL;
        while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "name") == 0) {
                char* value = iter->getValue();
                output.append(value);
                output.append("|");
                ++output_size;
                if (value) free(value);
            } else {  /* value of "bestMarketplacePrice" */
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("price")) {
                    char* value = iter->getValue();
                    output.append(value);
                    output.append("|");
                    ++output_size;
                    if (value) free(value);
                }
                iter->up();
            }
        }
    }
}

// {$.items[*].bestMarketplacePrice.price, $.items[*].name}
void query_wm1_large(BitmapIterator* iter, string& output, long& output_size) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return; /* value of "root" */
        if (iter->isObject() && iter->moveToKey("items")) {
            if (iter->down() == false) return;
            while (iter->isArray() && iter->moveNext() == true) {
                if (iter->down() == false) continue;
                if (iter->isObject()) {
                    unordered_set<char*> set;
                    set.insert("bestMarketplacePrice");
                    set.insert("name");
                    char* key = NULL;
                    while (set.empty() == false && (key = iter->moveToKey(set)) != NULL) {
                        if (strcmp(key, "name") == 0) {
                            char* value = iter->getValue();
                            output.append(value);
                            output.append("|");
                            ++output_size;
                            if (value) free(value);
                        } else {  /* value of "bestMarketplacePrice" */
                            if (iter->down() == false) continue;
                            if (iter->isObject() && iter->moveToKey("price")) {
                                char* value = iter->getValue();
                                output.append(value);
                                output.append("|");
                                ++output_size;
                                if (value) free(value);
                            }
                            iter->up();
                        }
                    }
                }
                iter->up();
            }
            iter->up();
        }
        //iter->up();
    //}
}

// {$.items[*].bestMarketplacePrice.price, $.items[*].name}
void query_wm1_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return;
        if (iter->isObject() && iter->moveToKey("items")) {
            //iter->parallelDomArrayFlag(true);
            if (iter->down() == false) return;
            if (iter->isArray()) {
                int num_recs = iter->numArrayElements();
                // get dominating array
                if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                    parallel_query(iter, query_wm1_small, num_recs, output, output_size, thread_num);
                } else {
                    // fall back to sequential querying
                    parallel_query(iter, query_wm1_small, num_recs, output, output_size, 1);
                }
            }
            iter->up();
        }
        //iter->up();
    //}
}

//{$.claims.P150[*].mainsnak.property}
void query_wp1_small(BitmapIterator* iter, string& output, long& output_size) {
    if (iter->isObject() && iter->moveToKey("claims")) {
        if (iter->down() == false) return;
        if (iter->isObject() && iter->moveToKey("P150")) {
            if (iter->down() == false) return;
            while (iter->isArray() && iter->moveNext() == true) {
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("mainsnak")) {
                    if (iter->down() == false) continue;
                    if (iter->isObject() && iter->moveToKey("property")) {
                        char* value = iter->getValue();
                        output.append(value);
                        output.append("|");
                        ++output_size;
                        if (value) free(value);
                    }
                    iter->up();
                }
                iter->up();
            }
            iter->up();
        }
        iter->up();
    }
}

//{$[*].claims.P150[*].mainsnak.property}
void query_wp1_large(BitmapIterator* iter, string& output, long& output_size) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //if (iter->down() == false) return;
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("claims")) {
                if (iter->down() == false) return;
                if (iter->isObject() && iter->moveToKey("P150")) {
                    if (iter->down() == false) return;
                    while (iter->isArray() && iter->moveNext() == true) {
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("mainsnak")) {
                            if (iter->down() == false) continue;
                            if (iter->isObject() && iter->moveToKey("property")) {
                                char* value = iter->getValue();
                                output.append(value);
                                output.append("|");
                                ++output_size;
                                if (value) free(value);
                            }
                            iter->up();
                        }
                        iter->up();
                    }
                    iter->up();
                }
                iter->up();
            }
            iter->up();
        }
        //iter->up();
    //}
}

//{$[*].claims.P150[*].mainsnak.property}
void query_wp1_large_parallel(BitmapIterator* iter, string* output, long& output_size, int thread_num) {
    //if (iter->isObject() && iter->moveToKey("root")) {
        //iter->parallelDomArrayFlag(true);
        //iter->down();
        //iter->parallelDomArrayFlag(false);
        if (iter->isArray()) {
            int num_recs = iter->numArrayElements();
            // get dominating array
            if (num_recs > SINGLE_THREAD_MAXRECORDS) {
                parallel_query(iter, query_wp1_small, num_recs, output, output_size, thread_num);
            } else {
                // fall back to sequential querying
                parallel_query(iter, query_wp1_small, num_recs, output, output_size, 1);
            }
        }
    //}
}
