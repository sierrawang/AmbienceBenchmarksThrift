namespace cpp agent

struct bench_result {
        1: i64 call_sum = 0,
        2: i64 call_sq_sum,
        3: i64 return_sum,
        4: i64 return_sq_sum,
}

service agent {
        bench_result start(1:i64 param)
}
