#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <string>

class Matrix {
private:
    unsigned Xsize, Ysize;
    unsigned* result_indexes;
    double** matrix;
    double* extension;
    double* result;
public:
    Matrix(unsigned, unsigned);
    unsigned getXsize() { return Xsize; }
    unsigned getYsize() { return Ysize; }
    double* getExtension() { return extension; }
    double L1Norm();
    double* getResult() { return result; }
    double* checkAccuracy(Matrix&);
    int forwardElimPartial();
    int forwardElimNo();
    int forwardElimComplete();
    void printResult();
    void fill(unsigned = INT32_MAX);
    void backSub(unsigned&);
    void read(std::string);
    void write(std::string);
    void writeResult(std::string);
    void calculateInversedMatrix(Matrix&);
    double* operator[](unsigned);
    Matrix& operator=(Matrix&);
    friend std::ostream& operator<<(std::ostream&, Matrix&);
    friend std::istream& operator>>(std::istream&, Matrix&);
    ~Matrix();
};

#endif
