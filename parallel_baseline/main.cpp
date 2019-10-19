#include "employee_registry.hpp"
#include "filter.hpp"
#include "filter_loader.hpp"

#include <chrono>
#include <iostream>
#include <memory>

#include "ittnotify.h"

#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>

__itt_domain* domain = __itt_domain_create("filtering");
__itt_string_handle* shLoadingDataTask = __itt_string_handle_create("Loading data from csv");
__itt_string_handle* shLoadingFiltersTask = __itt_string_handle_create("Loading filters from csv");
__itt_string_handle* shFilteringTask = __itt_string_handle_create("Applying filters");
__itt_string_handle* shFilteringSubTask = __itt_string_handle_create("Applying a filter");

using namespace filtering;

size_t g_counter = 0;

struct FilterLogger final
{
    FilterLogger() : start(std::chrono::high_resolution_clock::now())
    {
        __itt_task_begin(domain, __itt_null, __itt_null, shFilteringSubTask);
    }

    ~FilterLogger()
    {
        __itt_task_end(domain);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Filter " << ++g_counter << " finished. Time: " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }

    std::chrono::time_point<std::chrono::steady_clock> start;
};

int main(int argc, char** argv)
{
    tbb::task_scheduler_init();
    std::string employeesCsv, filtersCsv;
    if (argc == 2 && (argv[1] == "-h" || argv[1] == "--help"))
    {
        std::cout << "Usage: 'parallel_baseline <path_to_employees.csv> <path_to_filters.csv>'\n";
        return 0;
    }
    else if (argc == 3)
    {
        employeesCsv = argv[1];
        filtersCsv = argv[2];
    }
    else
    {
        std::cout << "Incorrect command line parameters. Execute 'parallel_baseline --help' for details.\n";
        return 2;
    }

    auto start = std::chrono::high_resolution_clock::now();

    __itt_task_begin(domain, __itt_null, __itt_null, shLoadingDataTask);

    auto registry = IEmployeeRegistry::load(employeesCsv);

    __itt_task_end(domain);

    auto initializationFinish = std::chrono::high_resolution_clock::now();
    std::cout << "Initialization finished. Time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(initializationFinish - start).count() << "ms\n";

    size_t counter = 0;


    __itt_task_begin(domain, __itt_null, __itt_null, shLoadingFiltersTask);

    FilterLoader loader({
        {"name", Types::String},
        {"position", Types::String},
        {"age", Types::Int},
        {"salary", Types::Float}
    });

    auto filters = loader.load(filtersCsv);

    __itt_task_end(domain);

    __itt_task_begin(domain, __itt_null, __itt_null, shFilteringTask);

    tbb::task_group group;

    for (int i = 0; i < 10; i++)
    {
        for (auto& filter : filters)
        {
            group.run([filter, &registry]
            {
                FilterLogger logger;

                IFilter<std::string>::Ptr nameFilter = nullptr;
                auto nameFilterIt = filter.find("name");
                if (nameFilterIt != filter.end())
                {
                    nameFilter = std::holds_alternative<IFilter<std::string>::Ptr>(nameFilterIt->second)
                        ? std::get<IFilter<std::string>::Ptr>(nameFilterIt->second)
                        : nullptr;
                }

                IFilter<std::string>::Ptr positionFilter = nullptr;
                auto positionFilterIt = filter.find("position");
                if (positionFilterIt != filter.end())
                {
                    positionFilter = std::holds_alternative<IFilter<std::string>::Ptr>(positionFilterIt->second)
                        ? std::get<IFilter<std::string>::Ptr>(positionFilterIt->second)
                        : nullptr;
                }

                IFilter<int>::Ptr ageFilter = nullptr;
                auto ageFilterIt = filter.find("age");
                if (ageFilterIt != filter.end())
                {
                    ageFilter = std::holds_alternative<IFilter<int>::Ptr>(ageFilterIt->second)
                        ? std::get<IFilter<int>::Ptr>(ageFilterIt->second)
                        : nullptr;
                }

                IFilter<float>::Ptr salaryFilter = nullptr;
                auto salaryFilterIt = filter.find("salary");
                if (salaryFilterIt != filter.end())
                {
                    salaryFilter = std::holds_alternative<IFilter<float>::Ptr>(salaryFilterIt->second)
                        ? std::get<IFilter<float>::Ptr>(salaryFilterIt->second)
                        : nullptr;
                }

                auto result = registry->filter(
                    std::move(nameFilter),
                    std::move(positionFilter),
                    std::move(ageFilter),
                    std::move(salaryFilter));

                std::cout << "Filtered: " << result.size() << "\n";
            });
        }
    }

    group.wait();

    __itt_task_end(domain);

    auto filteringFinish = std::chrono::high_resolution_clock::now();

    std::cout << "Filtering finished. Time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(filteringFinish - initializationFinish).count() << "ms\n";

    std::cout << "Queries per second: "
        << (static_cast<float>(g_counter * 1000) / std::chrono::duration_cast<std::chrono::milliseconds>(filteringFinish - initializationFinish).count()) << "\n";
    std::cout << "Total time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(filteringFinish - start).count() << "ms\n";
}