
#include <windows.h>
#include <pdh.h>

double GetRamUsage() {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    GlobalMemoryStatusEx(&memStatus);

    return (static_cast<double>(memStatus.ullTotalPhys - memStatus.ullAvailPhys) / memStatus.ullTotalPhys)*4.9;
}

double GetProcessorUsage()
{
    PDH_HQUERY query;
    PDH_HCOUNTER counter;
    PDH_FMT_COUNTERVALUE value;

    // Open a query object
    PdhOpenQueryW(NULL, 0, &query);

    // Add a counter to the query for total processor usage
    PdhAddCounterW(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter);

    // Collect data
    PdhCollectQueryData(query);
    Sleep(1000);  // Sleep for a short interval to get meaningful data
    PdhCollectQueryData(query);

    // Get the value of the counter
    PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value);

    // Close the query object
    PdhCloseQuery(query);

    return (value.doubleValue/100.0)*4.9;
}