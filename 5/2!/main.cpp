#include <iostream>
#include <vector>
#include <fstream>
#include <string>


class encoder {
private:
    std::vector<std::byte> key;

    void key_init(std::vector<std::byte> &s) const {
        size_t k_len = key.size();
        for (size_t i = 0; i < 256; ++i) {
            s[i] = static_cast<std::byte>(i);
        }

        size_t j = 0;

        for (size_t i = 0; i < 256; i++) {
            j = (j + static_cast<size_t>(s[i]) + static_cast<size_t>(key[i % k_len])) % 256;
            std::byte tmp = s[i]; //j = (j + S[i] + key[i % keyLength]) % 256;
            s[i] = s[j];
            s[j] = tmp;
        }
    }

    void rc4(std::ifstream &in, std::ofstream &out) const {
        std::vector<std::byte> s(256);
        key_init(s);

        size_t i = 0, j = 0;
        char byte;
        while (in.get(byte)) {
            i = (i + 1) % 256;
            j = (j + static_cast<size_t>(s[i])) % 256; // y = (y + S[x]) % 256;
            std::byte tmp = s[i];
            s[i] = s[j];
            s[j] = tmp;
            std::byte k = s[(static_cast<size_t>(s[i]) + static_cast<size_t>(s[j])) % 256]; //S[(S[x] + S[y]) % 256];
            char out_byte = byte ^ static_cast<char>(k);
            out.put(out_byte);
        }
    }

public:
    explicit encoder(const std::vector<std::byte> &k) : key(k) {}

    void edit_key(const std::vector<std::byte> &n_key) {
        key = n_key;
    }

    int encode(const std::string &name_input_file, const std::string &name_output_file, bool flag) const {
        std::ifstream in(name_input_file, std::ios::binary); 
        if (!in.is_open()) {
            std::cerr << "Error with opening input file" << std::endl; 
            return 0;
        }
        
        std::ofstream out(name_output_file, std::ios::binary);  
        if (!out.is_open()) {
            std::cerr << "Error with opening output file" << std::endl;
            return 0;
        }

        rc4(in, out);

        in.close(); 
        out.close();
        return 1;
    }
 
};

int main () {
    try {
        std::byte b1 = std::byte{1};
        std::byte b2 = std::byte{23};
        std::byte b3 = std::byte{3};
        std::byte b4 = std::byte{255};

        std::vector<std::byte> key = { b1, b2, b3, b4 }; 

        encoder enc(key);

        //шифрование
        if (enc.encode("input.txt", "encrypted.bin", true)) {
            std::cout << "Файл зашифрован успешно." << std::endl;
        }

        //дешифрование
        if (enc.encode("encrypted.bin", "decrypted.txt", false)) {
            std::cout << "Файл дешифрован успешно." << std::endl;
        }
    } catch (const std::exception &ex) {
        std::cout << "Ошибка: " << ex.what() << std::endl; //возвращает const char*
    }

    return 0;
}