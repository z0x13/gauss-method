#include "Matrix.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>

void parse_args(const int argc, char* const* argv, 
        unsigned &size, unsigned &max_element,
        unsigned &type, bool &is_generate,
        bool &is_console_out, bool &is_debug, 
        bool &is_error, bool &is_benchmark) {
    int value;
    while ((value = getopt(argc, argv, "gm:s:hrdet:b")) != -1) {
        switch (value) {
            case 'g':
                is_generate = true;
                break;
            case 'h':
                std::cout << "-h show this menu \
                    \n-s specify size \
                    \n-m specify max element \
                    \n-t specify solution type (0 = no pivoting, 1 = partial pivoting, 2 = complete pivoting) \
                    \n-r show result \
                    \n-g generate new system \
                    \n-b run benchmark \
                    \n-e show error \n\n";
                break;
            case 's':
                size = atoi(optarg);
                std::cout << "Size " << size << " specified\n";
                break;
            case 'm':
                max_element = atoi(optarg);
                std::cout << "Max element " << max_element << " specified\n";
                break;
            case 'r':
                is_console_out = true;
                break;
            case 'd':
                is_debug = true;
                break;
            case 'b':
                is_benchmark = true;
                break;
            case 'e':
                is_error = true;
                break;
            case 't':
                type = atoi(optarg);
                if (type == 2) {
                    std::cout << "Complete pivoting\n";
                }
                else if (type == 1) {
                    std::cout << "Partial pivoting\n";
                }
                else {
                    std::cout << "No pivoting\n";
                }
                break;
            default:
                 std::cout << "-h show help menu\n";
        }
    }
}

// Функция для подсчета относительной погрешности
double checkAccuracy(Matrix &A, Matrix &B) {
    //double* result = new double[A.getXsize()];
    double sum_norm { 0 }, res_norm { 0 };
    double* result = A.checkAccuracy(B);
    
    for (unsigned i = 0; i < A.getXsize(); i++) {
        res_norm += fabs(A.getResult()[i]);
        sum_norm += fabs(result[i]);
    }

    return sum_norm / res_norm; 
}

void benchmark(unsigned size) {
    std::ofstream fout;

    std::cout << "Benchmark: \n";

    for (unsigned type = 0; type < 3; type++) {
        fout.open(std::to_string(type) + ".csv");
        for (unsigned i = 2; i < size; i++) {
            for (unsigned j = 1; j <= 5; j++) {
                std::cout << "size = " << i << ", iteration = " << j << "\n";

                Matrix A(i, i);
                A.fill();
                Matrix B(i, i);
                B = A;

                // Количество процессорных тактов до выполнения алгоритма
                unsigned long long ret { 0 }, ret2 { 0 };
                unsigned eax, edx;
                asm volatile("rdtsc" : "=a" (eax), "=d" (edx));
                ret  = ((unsigned long long)eax) | (((unsigned long long)edx) << 32);
        
                if (type == 2) {
                    A.forwardElimComplete();
                }
                else if (type == 1) {
                    A.forwardElimPartial();
                }
                else {
                    A.forwardElimNo();
                }
                A.backSub(type);

                // Количество процессорных тактов после выполнения алгоритма 
                asm volatile("rdtsc" : "=a" (eax), "=d" (edx));
                ret2  = ((unsigned long long)eax) | (((unsigned long long)edx) << 32);
        
                Matrix C(i, i);
                C = B;
                Matrix D(i, i);
                C.calculateInversedMatrix(D);

                double norm = B.L1Norm() * D.L1Norm();
        
                double error = checkAccuracy(A, B);

                fout << i << "," << norm << "," << error << "," << ret2 - ret << "\n";
            }
        }
    fout.close();
    }
}

int main(const int argc, char* const* argv) {
    srand(time(0));
    
    // is_console = будут ли выведены в консоль результаты вычислений
    // is_debug = будут ли выведены промежуточные шаги, результат и погрешность
    // is_error = будет ли выведена погрешность 
    // is_generate = будет ли сгенерирована новая матрица
    // is_benchmark = будет ли запущен бенчмарк
    bool is_console = false, is_debug = false, is_error = false,
         is_generate = false, is_benchmark = false;

    // Программа не будет работать без передачи пользователем 
    // размера матрицы (количества уравнений в системе). 
    // По умолчанию нет ограничений на размер максимального 
    // элемента матрицы, стандартный тип решения - без поиска
    // опорного элемента
    unsigned size = 0, type = 0, max_element = INT32_MAX;
    parse_args(argc, argv, size, max_element, type, 
            is_generate, is_console, is_debug, is_error, is_benchmark);
    if (!size) {
        std::cout << "You need to specify size using -s\n";
        std::cout << "-h show help\n\n";
        return 0;
    }

    if (is_benchmark) {
        benchmark(size);
        return 0;
    }

    // Аргументы определяют количество строк и столбцов в создаваемой матрице
    Matrix A(size, size);

    // Метод fill класса Matrix заполняет матрицу случайными значениями
    if (is_generate) {
        A.fill(max_element);
        A.write("input.txt");
    }

    // Метод read() класса Matrix позволяет прочитать матрицу из файла
    A.read("input.txt");

    // Создаем копию начальной матрицы (нужна для подсчета погрешностей)
    Matrix B(size, size);
    B = A;

    if (is_debug) {
        std::cout << "\nSource matrix:\n" << A << "\n";
    }
    
    // Время выполнения в тактах процессора
    unsigned long long ret { 0 }, ret2 { 0 };
    double freq = 4.6 * pow(10, 9);
    unsigned eax, edx;
    asm volatile("rdtsc" : "=a" (eax), "=d" (edx));
    ret  = ((unsigned long long)eax) | (((unsigned long long)edx) << 32);

    // Прямой ход метода Гаусса
    int check = 0;
    if (type == 1) {
        check = A.forwardElimPartial();
    }
    else if (type == 2) {
        check = A.forwardElimComplete();
    }
    else {
        check = A.forwardElimNo();
    }

    // Метод forwardElim вернул -1 если матрица невырожденная
    if (check != -1) {
        if (A.getExtension()[check])
            std::cout << "No solutions\n";
        else
            std::cout << "Infinite solutions\n";
        return 0;
    }

    if (is_debug) {
        std::cout << "Forward elimination:\n" << A << "\n";
    }

    // Обратный ход метода Гаусса
    A.backSub(type);

    // Время выполнения в тактах процессора
    asm volatile("rdtsc" : "=a" (eax), "=d" (edx));
    ret2  = ((unsigned long long)eax) | (((unsigned long long)edx) << 32);

    if (is_console || is_debug) { 
        std::cout << "Result:\n";
        A.printResult();
    }

    if (is_error || is_debug) {
        std::cout << "\nError = " << checkAccuracy(A, B) * 100 << "%\n";
        std::cout << "\nExecution time:\nclocks = " << ret2 - ret << 
            "\n" << "time = " << (ret2 - ret) / freq << "\n";
    }
    
    Matrix C(size, size);
    Matrix D(size, size);
    D = B;
    D.calculateInversedMatrix(C);
    std::cout << "\ncond(A) = " 
        << B.L1Norm() * C.L1Norm() << "\n"; 

    // Метод writeResult() класса Matrix записывает в файл итоговый ответ
    A.writeResult("output.txt");

    return 0;
}
