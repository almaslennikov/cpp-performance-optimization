#include "employee_registry.hpp"
#include "filter.hpp"
#include "filter_loader.hpp"

#include <chrono>
#include <iostream>
#include <memory>

#include "ittnotify.h"

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
    auto start = std::chrono::high_resolution_clock::now();

    __itt_task_begin(domain, __itt_null, __itt_null, shLoadingDataTask);

    auto registry = IEmployeeRegistry::load("C:\\Users\\Alexander-PC\\Documents\\cppconf-piter-2019-optimization\\employees1.csv"); // TODO change

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

    auto filters = loader.load("C:\\Users\\Alexander-PC\\Documents\\cppconf-piter-2019-optimization\\filters.csv"); // TODO change

    __itt_task_end(domain);

    __itt_task_begin(domain, __itt_null, __itt_null, shFilteringTask);

    for (auto& filter : filters)
    {
        FilterLogger logger;

        auto nameFilterType = filter["name"];
        IFilter<std::string>::Ptr nameFilter = 
            std::holds_alternative<IFilter<std::string>::Ptr>(nameFilterType) ? std::get<IFilter<std::string>::Ptr>(nameFilterType) : nullptr;
        auto positionFilterType = filter["position"];
        IFilter<std::string>::Ptr positionFilter = 
            std::holds_alternative<IFilter<std::string>::Ptr>(positionFilterType) ? std::get<IFilter<std::string>::Ptr>(positionFilterType) : nullptr;
        auto ageFilterType = filter["age"];
        IFilter<int>::Ptr ageFilter =
            std::holds_alternative<IFilter<int>::Ptr>(ageFilterType) ? std::get<IFilter<int>::Ptr>(ageFilterType) : nullptr;
        auto salaryFilterType = filter["salary"];
        IFilter<float>::Ptr salaryFilter =
            std::holds_alternative<IFilter<float>::Ptr>(salaryFilterType) ? std::get<IFilter<float>::Ptr>(salaryFilterType) : nullptr;

        auto result = registry->filter(
            std::move(nameFilter),
            std::move(positionFilter),
            std::move(ageFilter),
            std::move(salaryFilter));

        std::cout << "Filtered: " << result.size() << "\n";
    }

    __itt_task_end(domain);

    auto filteringFinish = std::chrono::high_resolution_clock::now();

    std::cout << "Filtering finished. Time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(filteringFinish - initializationFinish).count() << "ms\n";

    std::cout << "Queries per second: "
        << (static_cast<float>(g_counter * 1000) / std::chrono::duration_cast<std::chrono::milliseconds>(filteringFinish - initializationFinish).count()) << "\n";
    std::cout << "Total time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(filteringFinish - start).count() << "ms\n";
}