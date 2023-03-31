
#include "Matrix.hpp"

// Конструктор. Параметры определяют количество строк и столбцов в матрице
Matrix::Matrix(unsigned x, unsigned y) : 
        Xsize(x), Ysize(y) {
    matrix = new double*[Xsize];
    for (unsigned i = 0; i < Xsize; i++) {
        matrix[i] = new double[Ysize];
    }
    extension = new double[Xsize];
    result = new double[Xsize]; 
    result_indexes = new unsigned[Xsize];
}
// Функция, заполняющая матрицу случайными числами 
void Matrix::fill(unsigned max) {
    for (unsigned i = 0; i < Xsize; i++) {
        for (unsigned j = 0; j < Ysize; j++) {
            matrix[i][j] = rand() % max;
        }
    }
    for (unsigned i = 0; i < Xsize; i++) {
        extension[i] = rand() % max;
    }
}
// Метод для вычисления обратной матрицы
void Matrix::calculateInversedMatrix(Matrix& inversedMatrix) {
    if (inversedMatrix.Xsize != Xsize || inversedMatrix.Ysize != Ysize) return;
    for (unsigned i = 0; i < Xsize; i++) {
        for (unsigned j = 0; j < Ysize; j++) {
            (i == j) ? inversedMatrix[i][j] = 1 : inversedMatrix[i][j] = 0;
        }
    }
    for (unsigned i = 0; i < Ysize; i++) {
        for (unsigned j = 0; j < Xsize; j++) {
            if (j != i) {
                double ratio = matrix[j][i] / matrix[i][i];
                for (unsigned k = 0; k < Xsize; k++) {
                    inversedMatrix[j][k] -= (inversedMatrix[i][k] * ratio);
                    matrix[j][k] -= (matrix[i][k] * ratio);
                }
            }
        }    
    }
    for (unsigned i = 0; i < Xsize; i++) {
        for (unsigned j = 0; j < Ysize; j++) {
            inversedMatrix[i][j] /= matrix[i][i];
        }
    }
}
// Метод для вычисления L1-нормы матрицы
double Matrix::L1Norm() {
    double norm = 0, max = 0;
    for (unsigned j = 0; j < Ysize; j++) {
        norm = 0;
        for (unsigned i = 0; i < Xsize; i++) {
            norm += fabs(matrix[i][j]);
        }
        if (norm > max) {
            max = norm;
        }
    }
    return max;
}
// Прямой ход метода Гаусса (без поиска опорного элемента)
int Matrix::forwardElimNo() {
    for (unsigned i = 0; i < Ysize; i++) {
        double value = matrix[i][i];
        for (unsigned j = i + 1; j < Xsize; j++) {
            double tmp = matrix[j][i];
            for (unsigned k = i; k < Ysize; k++) {
                matrix[j][k] -= (matrix[i][k] / value * tmp);
            }
            extension[j] -= (extension[i] / value * tmp); 
        }
    }
    return -1;
}
// Прямой ход метода Гаусса (с поиском опорного элемента по столбцам)
int Matrix::forwardElimPartial() {
    // Проходимся по столбцам от 0 до последнего
    for (unsigned i = 0; i < Ysize; i++) {
        // Находим опорный элемент
        double max = matrix[i][i];
        unsigned max_index = i;
        for (unsigned j = i + 1; j < Xsize; j++) {
            if (fabs(matrix[j][i]) > fabs(max)) {
                max = matrix[j][i];
                max_index = j;
            }
        }
        // Если опорный элемент находится не в i-той строке - 
        // меняем строки местами
        if (max_index != i) {
            std::swap(matrix[max_index], matrix[i]);
            std::swap(extension[max_index], extension[i]);
        } 
        // Если элемент на главной диагонали не равен 0
        if (max) {
            // Проходимся по строкам от (i+1)-й до последней 
            for (unsigned j = i + 1; j < Xsize; j++) {
                double tmp = matrix[j][i]; 
                // Проходимся по столбцам от i-го до последнего
                for (unsigned k = i; k < Ysize; k++) {
                    matrix[j][k] -= (matrix[i][k] / max * tmp);
                }
                extension[j] -= (extension[i] / max * tmp);
            }
        }
        // Иначе - возвращаем номер строки
        else {
            return i;
        }
    }
    return -1;
}
// Прямой ход метода Гаусса (с поиском опорного элемента и 
// по строкам, и по столбцам)
int Matrix::forwardElimComplete() {    
    // Для того, чтобы после перестановки столбцов вернуть
    // переменные на исходные места, все перестановки будут
    // сохраняться в векторе result_indexes
    for (unsigned i = 0; i < Ysize; i++) {
        result_indexes[i] = i;
    }
    // Проходимся по столбцам от 0 по последнего
    for (unsigned i = 0; i < Ysize; i++) {
        double max = matrix[i][i];
        unsigned max_line_index = i, max_column_index = i;
        // Находим максимальный элемент, сохраняем его индексы
        for (unsigned j = i; j < Xsize; j++) {
            for (unsigned k = i; k < Ysize; k++) {
                if (fabs(matrix[j][k]) > fabs(max)) {
                    max_line_index = j;
                    max_column_index = k;
                    max = matrix[j][k];
                }
            }
        }
        // Если максимальный элемент не в i-той строке -
        // меняем строки местами
        if (max_line_index != i) {
            std::swap(matrix[max_line_index], matrix[i]);
            std::swap(extension[max_line_index], extension[i]);
        }
        // Если максимальный элемент не в i-том столбце - 
        // меняем столбцы местами
        if (max_column_index != i) {
            for (unsigned j = 0; j < Xsize; j++) {
                std::swap(matrix[j][max_column_index], matrix[j][i]);
            }
            // Сохраняем перерестановку в вектор result_indexes
            std::swap(result_indexes[i], result_indexes[max_column_index]); 
        }
        if (max) {
            // Проходимся по строкам от (i+1)-й до последней
            for (unsigned j = i + 1; j < Xsize; j++) {
                double tmp = matrix[j][i]; 
                // Проходимся по столбцам от i-го до последнего
                for (unsigned k = i; k < Ysize; k++) {
                    matrix[j][k] -= (matrix[i][k] / max * tmp);
                }
                extension[j] -= (extension[i] / max * tmp);
            }
        }
        // Иначе - возвращаем номер строки
        else {
            return i;
        }
    }
    return -1;
}
// Обратный ход метода Гаусса
void Matrix::backSub(unsigned& type) {
    // Проходимся по строкам начиная с последнего
    for (int i = Xsize - 1; i >= 0; i--) {
        // Выполняем обратное преобразование
        result[i] = extension[i];
        for (unsigned j = i + 1; j < Xsize; j++) {
            result[i] -= matrix[i][j] * result[j];
        }
        result[i] /= matrix[i][i];
    }
    // Только для метода Гаусса с выбором опорного 
    // элемента и по строкам, и по столбцам
    if (type == 2) {
        double* old_result = new double[Xsize];
        for (unsigned i = 0; i < Xsize; i++) {
            // Так как в процессе старые значения 
            // вектора result будут перезаписаны, 
            // создаем его копию 
            old_result[i] = result[i];
        }
        for (unsigned i = 0; i < Ysize; i++) {
            // Производим обратные перестановки по вектору
            // result_indexes 
            result[result_indexes[i]] = old_result[i];
        }
        if (old_result) {
            delete[] old_result;
        }
    }
}
// Метод для чтения матрицы из файла
void Matrix::read(std::string filename) {
    std::ifstream fin;
    fin.open(filename);
    if (!fin.is_open()) {
        std::cout << "File opening error\n";
        return;
    }
    fin >> *this;
    fin.close();
}
// Метод для записи матрицы в файл
void Matrix::write(std::string filename) {
    std::ofstream fout;
    fout.open(filename);
    fout << *this;
    fout.close();
}
// Метод для вывода результата в консоль
void Matrix::printResult() {
    for (unsigned i = 0; i < Xsize; i++) {
        std::cout << "x_" << i + 1 << " = " << result[i] << "\n";
    }
}
// Метод для записи результата в файл
void Matrix::writeResult(std::string filename) {
    std::ofstream fout;
    fout.open(filename);
    for (unsigned i = 0; i < Xsize; i++) {
        fout << "x_" << i + 1 << " = " << result[i] << "\n";
    }
    fout.close();
}
// Метод для нахождения вектора невязок
double* Matrix::checkAccuracy(Matrix& B) {
    for (unsigned i = 0; i < Xsize; i++) {
        for (unsigned j = 0; j < Ysize; j++) {
            B.extension[i] -= (B[i][j] * result[j]);
        }
    }
    return B.getExtension();
}
double* Matrix::operator[](unsigned int index) {
    if (index < Xsize) {
        return matrix[index];
    }
    return 0;
}
Matrix& Matrix::operator=(Matrix& right) {
    // проверка на самоприсваивание
    if (this == &right) {
        return *this;
    }
    if (Xsize == right.Xsize && 
            Ysize == right.Ysize) {
        for (unsigned i = 0; i < Xsize; i++) {
            for (unsigned j = 0; j < Ysize; j++) {
                matrix[i][j] = right[i][j];
            }
            extension[i] = right.extension[i];
        }
    }
    return *this;
}
std::ostream& operator<<(std::ostream& os, Matrix& matrix) {
    for (unsigned i = 0; i < matrix.Xsize; i++) {
        for (unsigned j = 0; j < matrix.Ysize; j++) {
            os << matrix[i][j] << " ";
        }
        os << "\t" << matrix.extension[i] << "\n";
    }
    return os;
}
std::istream& operator>>(std::istream& is, Matrix& matrix) {
    for (unsigned i = 0; i < matrix.Xsize; i++) {
        for (unsigned j = 0; j < matrix.Ysize; j++) {
            is >> matrix[i][j];
        }
        is >> matrix.extension[i];
    }
    return is;
}
Matrix::~Matrix() {
    if (matrix) {
        for (unsigned i = 0; i < Xsize; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
    }
    if (extension) {
        delete[] extension;
    }
    if (result) {
        delete[] result;
    }
    if (result_indexes) {
        delete[] result_indexes;
    }
}
