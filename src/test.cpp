#include "external_sort.hpp"

int main()
{
    ExternalSorter<int> aboba(1000);
    aboba("ints_big.txt", "ints_big.sorted");
    //aboba("ints.txt", "ints.sorted");
    
    return 101;
}