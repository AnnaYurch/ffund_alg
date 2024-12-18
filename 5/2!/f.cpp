Алгоритм RC4 строится как и любой потоковый шифр на основе параметризованного ключом генератора 
псевдослучайных битов с равномерным распределением. Основные преимущества шифра — высокая скорость 
работы и переменный размер ключа. 

Ядро алгоритма состоит из функции генерации ключевого потока. Эта функция генерирует последовательность битов, 
которая затем объединяется с открытым текстом посредством суммирования по модулю два. 
Расшифровка состоит из регенерации этого ключевого потока и суммирования его с шифрограммой по модулю два, 
восстанавливая исходный текст. Другая главная часть алгоритма — функция инициализации, которая использует ключ 
переменной длины для создания начального состояния генератора ключевого потока. 

Ключ тоже выступает в качестве входных данных. Для алгоритма RC4 он может быть от 8 до 2048 бит, но обычно 
используется диапазон 40 — 256 бит. 

void rc4(std::ifstream &in, std::ofstream &out) const {
    std::vector<std::byte> s(256);
    key_init(s); // Инициализация массива s с использованием ключа

    size_t i = 0, j = 0;
    std::vector<std::byte> buffer; // Временный буфер для хранения данных

    // Чтение данных из входного файла и шифрование
    while (in) {
        std::byte byte;
        in.read(reinterpret_cast<char*>(&byte), sizeof(std::byte)); // Чтение одного байта

        if (in) { // Проверка, успешно ли прочитан байт
            // Генерация псевдослучайного байта
            i = (i + 1) % 256;
            j = (j + static_cast<size_t>(s[i])) % 256;

            // Обмен значениями в массиве s
            std::swap(s[i], s[j]);

            // Получение псевдослучайного байта для шифрования
            std::byte k = s[(static_cast<size_t>(s[i]) + static_cast<size_t>(s[j])) % 256];

            // Шифрование (или дешифрование) байта
            byte = static_cast<std::byte>(static_cast<size_t>(byte) ^ static_cast<size_t>(k));

            // Запись зашифрованного байта в выходной файл
            out.write(reinterpret_cast<const char*>(&byte), sizeof(std::byte));
        }
    }
}