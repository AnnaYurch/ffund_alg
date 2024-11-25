#include <iostream>
#include <bitset>

class logical_values_array {
    private:
        unsigned int value;
    
    public:
        logical_values_array(unsigned int val = 0) : value(val) {} 

        unsigned int get_value() const {
            return value;
        }

        void set_value(unsigned int val) {
            value = val;
        }

        logical_values_array inversion() const {
            return logical_values_array(~value);
        }

        logical_values_array conjunction(const logical_values_array &other) const {
            return logical_values_array(value & other.value);
        }

        logical_values_array disjunction(const logical_values_array &other) const {
            return logical_values_array(value | other.value);
        }

        logical_values_array implication(const logical_values_array &other) const {
            return logical_values_array((*this).inversion().disjunction(other));
        }

        logical_values_array coimplication(const logical_values_array &other) const {
            return logical_values_array(((*this).implication(other)).inversion());
        }

        logical_values_array addition_modulo_2(const logical_values_array &other) const { // (a^b)
            return logical_values_array(value ^ other.value);
        }

        logical_values_array equivalence(const logical_values_array &other) const { // !(a^b)
            return logical_values_array(((*this).addition_modulo_2(other)).inversion());
        }

        logical_values_array arrow_pierce(const logical_values_array &other) const { // !(a|b)
            return logical_values_array(((*this).disjunction(other)).inversion());
        }

        logical_values_array sheffer_stroke(const logical_values_array &other) const { // !(a&b)
            return logical_values_array(((*this).conjunction(other)).inversion());
        }

        static bool equals(const logical_values_array &a, const logical_values_array &b) {
            return a.value == b.value;
        }

        int get_bit(int pos) const {
            if (pos < 0 || pos >= (int)(sizeof(unsigned int) * 8)) {
                std::cout << "Uncorrect position ";
                return -1;
            }
            return (value >> pos) & 1;
        }

        void to_binary_string(char *buf) const {
            int len = sizeof(unsigned int) * 8;

            for (int i = 0; i < len; ++i) {
                buf[i] = ((value >> (len - 1 - i)) & 1) ? '1' : '0';
            }

            buf[len] = '\0';
        }
};


int main() {
    logical_values_array a(5); //101
    logical_values_array b(3); //011

    std::cout << "Value a: " << a.get_value() << std::endl;
    std::cout << "Value b: " << b.get_value() << std::endl;

    //4294967295 - max
    logical_values_array res = a.inversion();  
    std::cout << "Value a inversion: " << res.get_value() << std::endl;
    
    res = a.conjunction(b); // 001
    std::cout << "Value a and b conjunction: " << res.get_value() << std::endl;

    res = a.disjunction(b); // 111
    std::cout << "Value a and b disjunction: " << res.get_value() << std::endl;

    logical_values_array c(4294967295); //11111111111111111111111111111111
    res = c.implication(b); // 011
    std::cout << "Value c and b implication: " << res.get_value() << std::endl;

    res = c.coimplication(b); // 11111111111111111111111111111100
    std::cout << "Value c and b coimplication: " << res.get_value() << std::endl;

    res = a.addition_modulo_2(b); // 110
    std::cout << "Value a and b addition_modulo_2: " << res.get_value() << std::endl;

    res = a.equivalence(b); // 11111111111111111111111111111001
    std::cout << "Value a and b equivalence: " << res.get_value() << std::endl;

    res = a.arrow_pierce(b); // 11111111111111111111111111111000
    std::cout << "Value a and b arrow_pierce: " << res.get_value() << std::endl;

    res = a.sheffer_stroke(b); // 11111111111111111111111111111110
    std::cout << "Value a and b sheffer_stroke: " << res.get_value() << std::endl;

    // 0
    std::cout << "Values a and b are equals??: " << logical_values_array::equals(a, b) << std::endl;

    std::cout << "a get_bit: " << a.get_bit(3) << a.get_bit(2) << a.get_bit(1) << a.get_bit(0) << std::endl;
    std::cout << "a get_bit: " << a.get_bit(-3) << std::endl;
    
    char buf[sizeof(unsigned int) * 8 + 1];
    a.to_binary_string(buf);
    std::cout << "a: " << buf << std::endl;
    return 0;
}
