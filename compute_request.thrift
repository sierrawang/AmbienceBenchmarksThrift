namespace cpp compute_request

struct Compute {
    1: i32 id,
    2: i32 level,
    3: double price,
    4: i32 time,
    5: bool valid,
}

service compute_request_ui {
    bool create_client(1: string username, 2: string password, 3: i32 level),
    Compute get_compute(1: string username, 2: string password, 3: i32 request_time),

    bool create_provider(1: string username, 2: string password, 3: i32 level),
    bool post_compute(1: string username, 2: string password, 3: i32 request_time),
}

service client {
    bool create_client(1: string username, 2: string password, 3: i32 level),
    Compute get_compute(1: string username, 2: string password, 3: i32 request_time),
}

service provider {
    bool create_provider(1: string username, 2: string password, 3: i32 level),
    bool post_compute(1: string username, 2: string password, 3: i32 request_time),
}

service resource_db {
    Compute get_compute(1: i32 request_time, 2: i32 level),
    bool post_compute(1: i32 request_time, 2: i32 level),
}

service user_verification {
    bool check_levels(1: i32 client_level, 2: i32 provider_level),
}

service time_verification {
    bool check_time(1: i32 client_time, 2: i32 provider_time),
}

service price_calculator {
    double get_price(1: Compute resource),
}