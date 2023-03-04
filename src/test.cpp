#include "external_sort.hpp"

using std::string;

class Record
{
private:
    int m_amount;
    float m_price;
    string m_id;
public:
    Record() {}
    Record(int val1, float val2, string val3) : 
        m_amount(val1), m_price(val2), m_id(val3) {}

    Record(Record&& other)
    {
        this->m_amount = other.m_amount;
        this->m_price = other.m_price;
        this->m_id = std::move(other.m_id);
    }

    Record& operator=(Record&& other)
    {
        if (this != &other)
        {
            this->m_amount = other.m_amount;
            this->m_price = other.m_price;
            this->m_id = std::move(other.m_id);
        }
        return *this;
    }

    friend bool operator>(const Record& left, const Record& right)
    {
        return left.m_price > right.m_price;
    }

    friend bool operator<(const Record& left, const Record& right)
    {
        return left.m_price < right.m_price;
    }

    friend std::istream& operator>>(std::istream& is, Record& rec)
    {
        is >> rec.m_amount >> rec.m_price >> std::ws;
        getline(is, rec.m_id);
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const Record& rec)
    {
        os << rec.m_amount << " " << rec.m_price << '\n' << rec.m_id;
        return os;
    }
};

int main()
{
    ExternalSorter<int> intSorter(1000);
    intSorter("test/ints.txt", "test/ints.sorted");

    ExternalSorter<Record, 2> recordSorter(4);
    recordSorter("test/records.txt", "test/records.sorted");
    
    return 0;
}