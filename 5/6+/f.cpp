vector(vector::Iterator beg, vector::Iterator end) {
    if (end - beg < 0) {
        throw std::invalid_argument("Invalid iterator range");
    }
    size =  end - beg;
    capacity = size;
    data = new double[size];

    for (size_t i = 0; i < size; ++i) {
        data[i] = *(beg + i);
    }
}