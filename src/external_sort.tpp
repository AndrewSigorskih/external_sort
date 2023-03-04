void create_random_dir(char* buf, int len, uint n_tries)
{
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static const size_t max_index = (sizeof(charset) - 2); // skipping \0 at the end
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, max_index);
    auto randchar = [&]() -> char { return charset[ dist(rng) ]; };

    while (true)
    {
        std::generate_n( buf, len, randchar );
        if (FS::create_directory(buf)) 
            break;
        n_tries--;
        if (!n_tries)
        {
            std::cerr << "Could not create directory with randomly-generated name ";
            std::cerr << "in " << n_tries << "tries. Exiting.";
            exit(1);
        } 
    }
}

template <class T, int LinesPerObj>
void ExternalSorter<T, LinesPerObj>::operator()
    (const char* infilename, const char* outfilename)
{
    create_random_dir(m_tempdir, DIRNAME_LEN);
    this->sort(infilename);
    this->merge(outfilename);
    // delete tempdir
    FS::remove_all(m_tempdir);
}

template <class T, int LinesPerObj>
void ExternalSorter<T, LinesPerObj>::sort(const char* infilename)
{
    std::ifstream input{infilename};
    if (!input.good())
    {
        std::cerr << "Cannot open input file, exiting.\n";
        exit(1);
    }
    uint64_t linesCount = 0;
    input.unsetf(std::ios_base::skipws);
    linesCount = std::count(
                    std::istream_iterator<char>(input),
                    std::istream_iterator<char>(), 
                    '\n');
    input.setf(std::ios_base::skipws);
    if ((linesCount % LinesPerObj) != 0)
    {
        std::cerr << "Error: wrong file format!\n";
        std::cerr << "Cannot infer non-fractional number of objects from file, exiting.\n";
        exit(1);
    }
    input.clear();
    input.seekg(0, std::ios::beg);
    uint64_t currCount, linesPerBatch, remainder;
    linesPerBatch = m_batchsize * LinesPerObj;
    remainder = (linesCount % linesPerBatch) / LinesPerObj;
    m_filesNum = (linesCount / linesPerBatch);
    if (remainder) m_filesNum += 1;

    T* arr = new T[m_batchsize];
    std::ofstream output;

    for (uint64_t i = 0; i < m_filesNum; ++i)
    {
        // read input file in batches
        if (!remainder)
            currCount = m_batchsize;
        else
            currCount = i < (m_filesNum-1) ? m_batchsize : remainder;
        //std:: cout << i << currCount << std::endl;
        for (uint64_t j = 0; j < currCount; ++j)
            input >> *(arr+j);
        // sort batch
        std::sort(arr, arr+currCount);
        // write to temporal files in test dir
        output.open(fmt::format("{0}/{1}.tmp", m_tempdir, i));
        for (uint64_t j = 0; j < currCount; ++j)
            output << *(arr+j) << '\n';
        output.close();
    }   
    delete[] arr;
}

template <class T, int LinesPerObj>
void ExternalSorter<T, LinesPerObj>::mergeHelper(uint64_t start,
                                                 uint64_t end,
                                                 uint64_t location)
{
    uint64_t filesCount = end - start ;
    std::ifstream input[filesCount];
    for (uint64_t i = 0; i < filesCount; ++i)
    {
        input[i].open(fmt::format("{0}/{1}.tmp", m_tempdir, start+i));
        if (!input[i].good())
        {
            std::cerr << "Error:cannot open input file:" << start+1 << std::endl;
            exit(1);
        }
    }

    for (uint64_t i = 0; i < filesCount; ++i) 
    {
        if (!input[i].eof()) 
        {
            T item;
            input[i] >> item;
            m_queue.emplace(i, std::move(item));
        }
    }

    std::ofstream output;
    output.open(fmt::format("{0}/{1}.tmp", m_tempdir, location));
    if (!output.good())
    {
        std::cerr << "Error:cannot open output file:" << location << std::endl;
        exit(1);
    }

    while (!m_queue.empty())
    {
        uint64_t index = m_queue.top().m_index;
        output << m_queue.top().data() << '\n';
        m_queue.pop();
        // in case of 1-line objects like int
        // eat trailing \n so we dont go in infinite loop
        input[index] >> std::ws;
        if (!input[index].eof())
        {
            T item;
            input[index] >> item;
            m_queue.emplace(index, std::move(item));
        }
    }
    for (uint64_t i = 0; i < filesCount; ++i)
    {
        input[i].close();
    }
    output.close();
}

template <class T, int LinesPerObj>
void ExternalSorter<T, LinesPerObj>::merge(const char* outfilename)
{
    // iteratively collapse each file batch into new file
    uint64_t start = 0, end = m_filesNum, step = 100uL;
    if (step > end-start)
        step = (end - start + 1) / 2 + 1;
    while (true)
    {
        uint64_t location = end, dist = std::min(step, end-start+1);
        uint64_t mid = start + dist;
        if (mid > end) 
            break;
        mergeHelper(start, mid, location);
        ++end;
        start = mid;
    }
    if (start < end-1)
    { // one last run
        mergeHelper(start, end, end+1);
        start = end + 1;
    }
    std::rename(
        fmt::format("{0}/{1}.tmp", m_tempdir, start).c_str(), 
        outfilename
    );
}
