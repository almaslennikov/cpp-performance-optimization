#include "employee_registry.hpp"
#include "filter.hpp"

#include <chrono>
#include <iostream>
#include <memory>

#include "ittnotify.h"

__itt_domain* domain = __itt_domain_create("filtering");
__itt_string_handle* shInitializingTask = __itt_string_handle_create("Loading data from csv");
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

    __itt_task_begin(domain, __itt_null, __itt_null, shInitializingTask);

    auto registry = IEmployeeRegistry::load("C:\\Users\\Alexander-PC\\Documents\\cppconf-piter-2019-optimization\\employees.csv");

    __itt_task_end(domain);

    auto initializationFinish = std::chrono::high_resolution_clock::now();
    std::cout << "Initialization finished. Time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(initializationFinish - start).count() << "ms\n";

    size_t counter = 0;
    
    __itt_task_begin(domain, __itt_null, __itt_null, shFilteringTask);

    IFilter<std::string>::Ptr techPosition = 
        std::make_shared<EqualsAnyFilter<std::string>>(std::initializer_list<std::string>({ INTERN, ENGINEER, MANAGER }));
    IFilter<std::string>::Ptr intern = std::make_shared<EqualsFilter<std::string>>(INTERN);
    IFilter<std::string>::Ptr servicePosition = 
        std::make_shared<EqualsAnyFilter<std::string>>(std::initializer_list<std::string>({ HR, SECURITY, ADMINISTRATOR, JANITOR }));
    IFilter<float>::Ptr lowIncome = std::make_shared<LessFilter<float>>(LOW_INCOME_UPPER_LIMIT);
    IFilter<float>::Ptr highIncome = std::make_shared<GreaterFilter<float>>(HIGH_INCOME_LOWER_LIMIT);
    IFilter<float>::Ptr greaterThanLowIncome = std::make_shared<GreaterFilter<float>>(MEDIUM_INCOME_LOWER_LIMIT);
    IFilter<float>::Ptr lessThanHighIncome = std::make_shared<LessFilter<float>>(MEDIUM_INCOME_UPPER_LIMIT);
    IFilter<int>::Ptr young = std::make_shared<LessFilter<int>>(YOUNG_AGE_UPPER_LIMIT);
    IFilter<int>::Ptr senior = std::make_shared<GreaterFilter<int>>(SENIOR_AGE_LOWER_LIMIT);
    IFilter<int>::Ptr youngerThanSenior = std::make_shared<LessFilter<int>>(MIDDLE_AGE_UPPER_LIMIT);
    IFilter<int>::Ptr olderThanYoung = std::make_shared<GreaterFilter<int>>(MIDDLE_AGE_LOWER_LIMIT);

    {
        FilterLogger logger;
        auto middleAgedInternsWithLowIncome = registry->filter({}, { intern }, { youngerThanSenior, olderThanYoung }, { lowIncome });
    }
    {
        FilterLogger logger;
        auto techEmployeesWithHighIncome = registry->filter({}, { techPosition }, {}, { highIncome });
    }
    {
        FilterLogger logger;
        auto seniorPeopleWithMediumIncome = registry->filter({}, {}, { senior }, { lessThanHighIncome, greaterThanLowIncome });
    }
    {
        FilterLogger logger;
        auto youngServicePeople = registry->filter({}, { servicePosition }, { young }, {});
    }
    {
        FilterLogger logger;
        auto peopleWithMediumIncome = registry->filter({}, {}, {}, { lessThanHighIncome, greaterThanLowIncome });
    }
    {
        FilterLogger logger;
        auto peopleWithLowIncome = registry->filter({}, {}, {}, { lowIncome });
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