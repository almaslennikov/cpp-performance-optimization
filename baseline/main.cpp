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

constexpr char INTERN[] = "Intern";
constexpr char ENGINEER[] = "Engineer";
constexpr char HR[] = "HR";
constexpr char MANAGER[] = "Manager";
constexpr char SECURITY[] = "Security";
constexpr char ADMINISTRATOR[] = "Administrator";
constexpr char JANITOR[] = "Janitor";

constexpr int YOUNG_AGE_UPPER_LIMIT = 30;
constexpr int MIDDLE_AGE_LOWER_LIMIT = YOUNG_AGE_UPPER_LIMIT;
constexpr int MIDDLE_AGE_UPPER_LIMIT = 55;
constexpr int SENIOR_AGE_LOWER_LIMIT = MIDDLE_AGE_UPPER_LIMIT;

constexpr float LOW_INCOME_UPPER_LIMIT = 100000.0;
constexpr float MEDIUM_INCOME_LOWER_LIMIT = LOW_INCOME_UPPER_LIMIT;
constexpr float MEDIUM_INCOME_UPPER_LIMIT = 250000.0;
constexpr float HIGH_INCOME_LOWER_LIMIT = MEDIUM_INCOME_UPPER_LIMIT;

struct FilterLogger final
{
    FilterLogger(): start(std::chrono::high_resolution_clock::now())
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

    auto registry = IEmployeeRegistry::load("C:\\Users\\Alexander-PC\\Documents\\cppconf-piter-2019-optimization\\employees.csv"); // TODO change

    __itt_task_end(domain);

    auto initializationFinish = std::chrono::high_resolution_clock::now();
    std::cout << "Initialization finished. Time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(initializationFinish - start).count() << "ms\n";

    size_t counter = 0;
    

    __itt_task_begin(domain, __itt_null, __itt_null, shLoadingFiltersTask);

    auto filters = loadFilters("C:\\Users\\Alexander-PC\\Documents\\cppconf-piter-2019-optimization\\filters.csv"); // TODO change

    __itt_task_end(domain);

    __itt_task_begin(domain, __itt_null, __itt_null, shFilteringTask);

    for (auto& filter : filters)
    {
        FilterLogger logger;

        auto result = registry->filter(
            std::move(filter.nameFilters),
            std::move(filter.positionFilters),
            std::move(filter.ageFilters),
            std::move(filter.salaryFilters));

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