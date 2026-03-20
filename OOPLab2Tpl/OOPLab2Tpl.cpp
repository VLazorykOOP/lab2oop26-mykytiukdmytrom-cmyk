#include <iostream>
using namespace std;

void task_1() {
    int a, b, c, d;
    cout << "Enter a, b, c, d: ";
    cin >> a >> b >> c >> d;

    int part1 = ((a << 7) - a + (c << 5)) >> 12;
    int part2 = (d << 10) + (d << 7) + (d << 5) + (d << 4);
    int part3 = (b << 7) + (b << 1) + b;

    int result = part1 - part2 + part3;

    cout << "Result: " << result << endl;

}

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Парність
int parity(int a, int b) {
    return (a ^ b) & 1;
}

// ШИФРУВАННЯ
void encode() {
    ifstream fin("input.txt");
    ofstream fout("encoded.bin", ios::binary);

    if (!fin) {
        cout << "Помилка відкриття input.txt\n";
        return;
    }

    string line;

    for (int row = 0; row < 8; row++) {
        getline(fin, line);

        while (line.size() < 8)
            line += ' ';

        for (int i = 0; i < 8; i++) {
            unsigned char ch = line[i];
            int ascii = ch;

            int low = ascii & 0b1111;
            int high = (ascii >> 4) & 0b1111;

            unsigned char byte1 = 0;
            byte1 |= (row & 0b111) << 5;
            byte1 |= (low & 0b1111) << 1;
            byte1 |= parity(row, low);

            unsigned char byte2 = 0;
            byte2 |= (high & 0b1111) << 4;
            byte2 |= (i & 0b111) << 1;
            byte2 |= parity(high, i);

            fout.write((char*)&byte1, 1);
            fout.write((char*)&byte2, 1);
        }
    }

    cout << "Шифрування завершено!\n";
}

// ДЕШИФРУВАННЯ
void decode() {
    ifstream fin("encoded.bin", ios::binary);
    ofstream fout("decoded.txt");

    if (!fin) {
        cout << "Помилка відкриття encoded.bin\n";
        return;
    }

    char matrix[8][8] = {};

    while (true) {
        unsigned char byte1, byte2;

        if (!fin.read((char*)&byte1, 1)) break;
        if (!fin.read((char*)&byte2, 1)) break;

        int row = (byte1 >> 5) & 0b111;
        int low = (byte1 >> 1) & 0b1111;

        int high = (byte2 >> 4) & 0b1111;
        int col = (byte2 >> 1) & 0b111;

        unsigned char ch = (high << 4) | low;

        matrix[row][col] = ch;
    }

    cout << "\nРозшифрований текст:\n";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << matrix[i][j];
            fout << matrix[i][j];
        }
        cout << endl;
        fout << endl;
    }
}

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Примушуємо компілятор не додавати "порожні" байти для вирівнювання
#pragma pack(push, 1)

// ----------- БІТОВІ СТРУКТУРИ -----------

struct Byte1Bits {
    unsigned char parity : 1;
    unsigned char low : 4;
    unsigned char row : 3;
};

struct Byte2Bits {
    unsigned char parity : 1;
    unsigned char col : 3;
    unsigned char high : 4;
};

// Union для роботи з байтом
union Byte1 {
    Byte1Bits bits;
    unsigned char byte;
};

union Byte2 {
    Byte2Bits bits;
    unsigned char byte;
};

#pragma pack(pop)

// Функція розрахунку парності (простий XOR)
int get_parity(int a, int b) {
    return (a ^ b) & 1;
}

// ----------- ШИФРУВАННЯ -----------
void encode1() {
    ifstream fin("input.txt");
    ofstream fout("encoded.bin", ios::binary);

    if (!fin.is_open()) {
        cout << "Помилка: Створіть файл input.txt у папці з проектом!\n";
        return;
    }

    string line;
    int rowCount = 0;

    // Читаємо по рядках (макс 8 рядків для матриці 8x8)
    while (getline(fin, line) && rowCount < 8) {
        // Доповнюємо рядок пробілами до 8 символів
        while (line.size() < 8) line += ' ';

        for (int i =0; i < 8; i++) {
            unsigned char ch = (unsigned char)line[i];

            Byte1 b1 = { 0 }; // Ініціалізація нулем
            Byte2 b2 = { 0 };

            int low = ch & 0x0F;
            int high = (ch >> 4) & 0x0F;

            b1.bits.row = (unsigned char)rowCount;
            b1.bits.low = (unsigned char)low;
            b1.bits.parity = (unsigned char)get_parity(rowCount, low);

            b2.bits.col = (unsigned char)i;
            b2.bits.high = (unsigned char)high;
            b2.bits.parity = (unsigned char)get_parity(i, high);

            fout.write((char*)&b1.byte, 1);
            fout.write((char*)&b2.byte, 1);
        }
        rowCount++;
    }

    fin.close();
    fout.close();
    cout << "Шифрування завершено успішно!\n";
}

// ----------- ДЕШИФРУВАННЯ -----------
void decode2() {
    ifstream fin("encoded.bin", ios::binary);
    if (!fin.is_open()) {
        cout << "Помилка: Файл encoded.bin не знайдено.\n";
        return;
    }

    char matrix[8][8];
    // Обнуляємо матрицю (Visual Studio це любить)
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            matrix[i][j] = ' ';

    while (!fin.eof()) {
        Byte1 b1 = { 0 };
        Byte2 b2 = { 0 };

        if (!fin.read((char*)&b1.byte, 1)) break;
        if (!fin.read((char*)&b2.byte, 1)) break;

        // Перевірка на всяк випадок (щоб не вийти за межі матриці)
        if (b1.bits.row < 8 && b2.bits.col < 8) {
            unsigned char ch = (b2.bits.high << 4) | b1.bits.low;
            matrix[b1.bits.row][b2.bits.col] = (char)ch;
        }
    }

    fin.close();

    cout << "\n--- Розшифрований текст ---\n";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << matrix[i][j];
        }
        cout << endl;
    }
}


#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "ukr");
    int choice;

    cout << "1 завдання\n";
    cout << "2 Шифрувати\n";
    cout << "3 Розшифрувати\n";
    cout << "4 Шифрувати(біти)\n";
    cout << "5 Розшифрувати(біти)\n";
    cout << "Ваш вибір:\n";

    cin >> choice;

    if (choice == 1)
        task_1();
    else if (choice == 2)
        encode();
    else if (choice == 3)
        decode();
    else if (choice == 4)
        encode1();
    else if (choice == 5)
        decode2();

    else
        cout << "Невірний вибір!\n";

    return 0;
}
