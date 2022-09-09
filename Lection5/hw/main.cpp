#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <vector>
#include <alloca.h>



std::chrono::duration <double> do_and_measure (std::function <void()> task)
{
    if (task) {
        const auto begin = std::chrono::high_resolution_clock::now ();
        task ();
        const auto end = std::chrono::high_resolution_clock::now ();
        return {end - begin};
    }

    return {};
};



void do_test (std::function <void * (size_t)> allocator
            , std::function <void (void *)> deallocator)
{
    constexpr int limit_size_power {std::numeric_limits <size_t>::digits};
    constexpr int max_try {100};

    bool is_failed {false};
    for (int i = 0; i < limit_size_power && !is_failed; ++i) {
        std::chrono::duration <double> duration_allocation_min {1e9};
        std::chrono::duration <double> duration_free_min {1e9};

        const size_t size = (size_t (1) << i);
        std::vector <char *> buffers {};

        for (int j = 0; j < max_try; ++j) {
            char * buffer {nullptr};
            duration_allocation_min = std::min (duration_allocation_min
                            , do_and_measure ([&](){buffer = reinterpret_cast <char *> (allocator (size) );}) );

            if (!buffer) {
                is_failed = true;
                break;
            }

            buffer [size - 1] = '\0';
            buffers.push_back (buffer);
        }

        for (auto & buffer : buffers) {
            duration_free_min = std::min (duration_free_min, do_and_measure ([&](){deallocator (buffer);}) );
        }

        if (is_failed) {
            std::cout << "Allocation failed for size: 2^" << i << "=" << size << "." << std::endl;
        } else {
            std::cout << "Best allocation time for size: 2^" << i << "=" << size << " is " << duration_allocation_min.count () << std::endl;
            std::cout << "Best free       time for size: 2^" << i << "=" << size << " is " << duration_free_min.count () << std::endl;
        }
    }
}



void do_test_alloca ()
{
    constexpr int limit_size_power {18};
    constexpr int max_try {10};

    for (int i = 0; i < limit_size_power; ++i) {
        std::chrono::duration <double> duration_allocation_min {1e9};

        const size_t size = (size_t (1) << i);

        for (int j = 0; j < max_try; ++j) {
            const auto begin = std::chrono::high_resolution_clock::now ();
            char * buffer = reinterpret_cast <char *> (alloca (size) );
            const auto end = std::chrono::high_resolution_clock::now ();
            duration_allocation_min = std::min (duration_allocation_min, std::chrono::duration <double> {end - begin});

            buffer [size - 1] = '\0';
        }

        std::cout << "Best allocation time for size: 2^" << i << "=" << size << " is " << duration_allocation_min.count () << std::endl;
    }
}



int main (int, char **)
{
    std::cout << "Test procedure for malloc and free." << std::endl;
    do_test (malloc, free);
    std::cout << std::endl;

    std::cout << "Test procedure for calloc (_, 1) and free." << std::endl;
    do_test ([](size_t s){return calloc (s, 1);}, free);
    std::cout << std::endl;

    std::cout << "Test procedure for calloc (1, _) and free." << std::endl;
    do_test ([](size_t s){return calloc (1, s);}, free);
    std::cout << std::endl;

    std::cout << "Test procedure for new and delete." << std::endl;
    do_test ([](size_t s){return operator new (s, std::nothrow);}
           , [](void * p){operator delete (p);});
    std::cout << std::endl;

    std::cout << "Test procedure for alloca." << std::endl;
    do_test_alloca ();
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
