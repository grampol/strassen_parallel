#include <iostream>
#include <cstdlib>
#include <omp.h>

#define COUT std::cout
#define ENDL std::endl

template<typename T> struct matrix
{
    T** matrix;
    size_t X;
    size_t Y;
};

template<typename T> matrix<T> alloc_mat(size_t xdim, size_t ydim, T (*gen)(size_t,size_t)) // gen(i,j) ydim, xdim
{
    size_t i, j;
    matrix<T> result;
    T ** mat = (T**)std::malloc(ydim * sizeof(T*));
    for (i = 0; i < ydim; i++)
    {
        mat[i] = (T*)std::malloc(xdim * sizeof(T));
    }

    if (gen == nullptr)
    {
        for (i = 0; i < ydim; i++)
        {
            for (j = 0; j < xdim; j++)
            {
                mat[i][j] = i+j;
            }
        }
    }
    else
    {
        for (i = 0; i < ydim; i++)
        {
            for (j = 0; j < xdim; j++)
            {
                
                mat[i][j] = gen(i,j);
            }
        }
    }
    result.matrix = mat;
    result.X = xdim;
    result.Y = ydim;
    return result;
}

template<typename T> void print_mat(matrix<T> Matrix)
{
    T ** idx;
    idx = Matrix.matrix;
    size_t xdim = Matrix.X, ydim = Matrix.Y;
    COUT << "[" << ENDL;
    for (size_t i = 0; i < xdim; i++)
    {
        COUT << "\t";
        for (size_t j = 0; j < xdim-1; j++)
        {
            COUT << idx[i][j]
                 << ",\t";
        }
        COUT << idx[i][xdim-1] << ENDL;
    }
    COUT << "]" << ENDL;
}

template<typename T> matrix<T> normal_mul(matrix<T> MatrixA, matrix<T> MatrixB)
{
    size_t xA,yA, xB,yB;
    T** matrixA, matrixB, resMatrix;
    matrix<T> result;

    matrixA = MatrixA.matrix;
    matrixB = MatrixB.matrix;
    xA = MatrixA.X;
    yA = MatrixA.Y;
    xB = MatrixB.X;
    yB = MatrixB.Y;

    if (xA != yB)
    {
        result = NULL;
    }
    else if (xA == xB)
    {
        size_t i,j,k;
        T ** ptrA = matrixA;
        T ** ptrB = matrixB;
        resMatrix = (T**) std::malloc (yB * sizeof(T*));
        for (i = 0; i < yB; i++)
            resMatrix[i] = (T*) std::malloc (xA * sizeof(T));

        
        for (i = 0; i < yB; i++)
        {
            for (j = 0; j < xA; j++)
            {
                for (k = 0; k < yA; k++)
                {
                    resMatrix[i][j] += ptrA[i][k] * ptrB[k][j];    // equal dimensions
                }
            }
        }
        return result;
    }
    else
    {
        size_t i,j,k;
        T ** ptrA = matrixA;
        T ** ptrB = matrixB;
        resMatrix = (T**) std::malloc (yB * sizeof(T*));
        for (i = 0; i < yB; i++)
            resMatrix[i] = (T*) std::malloc (xA * sizeof(T));
        for (i = 0; i < yB; i++)
        {
            for (j = 0; j < xA; j++)
            {
                for (k = 0; k < yA; k++)
                {
                    resMatrix[i][j] += ptrA[k][j] * ptrB[i][k];    // non-equal dimensions
                }
            }
        }
    }
    result.X = (result != NULL) ? xA : 0;
    result.Y = (result != NULL) ? yB : 0;
    result.matrix = resMatrix;
    return result;
}

template<typename T> matrix<T> add(matrix<T> MatrixA, matrix<T> MatrixB)
{
    size_t xA, xB, yA, yB, i, j;
    T ** result, ** matrixA, ** matrixB;
    matrix<T> Result;

    xA = MatrixA.X;
    xB = MatrixB.X;
    yA = MatrixA.Y;
    yB = MatrixB.Y;

    matrixA = MatrixA.matrix;
    matrixB = MatrixB.matrix;
    
    if (xA == 1 && yA == 1)
    {
        result = (T**) std::malloc (yB * sizeof(T*));
        for (i = 0; i < yB; i++)
            result[i] = (T*) std::malloc (xB * sizeof(T));
        
        for (i = 0; i < yB; i++)
        {
            for (j = 0; j < xB; j++)
            {
                result[i][j] =  MatrixA.matrix[0][0] +  MatrixB.matrix[i][j];
            }
        }
        Result.X = xB;
        Result.Y = yB;
    }
    else if (xB == 1 && yB == 1)
    {
        result = (T**) std::malloc (yA * sizeof(T*));
        for (i = 0; i < yA; i++)
            result[i] = (T*) std::malloc (xA * sizeof(T));
        
        for (i = 0; i < yA; i++)
        {
            for (j = 0; j < xA; j++)
            {
                result[i][j] =  MatrixB.matrix[0][0] +  MatrixA.matrix[i][j];
            }
        }
        Result.X = xA;
        Result.Y = yA;
    }
    else
    {
        if (xA == yA && xB == yB)
        {
            result = (T**) std::malloc (yA * sizeof(T*));
            for (i = 0; i < xB; i++)
                result[i] = (T*) std::malloc (xA * sizeof(T));
            
            for (i = 0; i < yA; i++)
            {
                for (j = 0; j < xA; j++)
                {
                    result[i][j] =  MatrixB.matrix[i][j] +  MatrixA.matrix[i][j];
                }
            }
            Result.X = xA;
            Result.Y = yB;
        }
        else
        {
            result = NULL;
            Result.X = 0;
            Result.Y = 0;
        }
    }
    Result.matrix = Result;
    return Result;
}

template<typename T> void cleanMatrix(matrix<T> * Matrix)
{
    size_t i;
    for (i = 0; i < Matrix->X; i++)
        std::free((void*) Matrix->matrix[i]);
    std::free(Matrix->matrix);
}

void test_mat_a(size_t xdim, size_t ydim)
{
    size_t i,j;
    matrix<int> mat = alloc_mat<int>(xdim, ydim, NULL);
    print_mat<int>(mat);

    cleanMatrix<int>(&mat);
    COUT << "CLEANED " << ENDL;
}

// void test_add(size_t xA, size_t yA, size_t xB, size_t yB, int (*fptrA)(size_t,size_t), int (*fptrB)(size_t,size_t))
// {
//     size_t i;
//     int ** matrixA, ** matrixB, **result;
//     COUT << "matrixA =" << ENDL;
//     matrixA = alloc_mat<int>(xA, yA, fptrA);
//     int ** ptrA = matrixA;
//     print_mat<int>(matrixA, xA, yA);
    
//     COUT << "matrixB =" << ENDL;
//     matrixB = alloc_mat<int>(xB,yB, fptrB);
//     print_mat<int>(matrixB, xB, yB);

//     COUT << "result =" << ENDL;
//     result = add<int>(matrixA, xA,yA, matrixB, xB,yB);
//     if (result != NULL)
//         print_mat<int>(result, xA,yB);
//     else
//         COUT << "[ERROR]" << ENDL;
    
//     for (i = 0; i < xA; i++)
//         delete[] matrixA[i];
//     delete[] matrixA;
    
//     for (i = 0; i < xB; i++)
//         delete[] matrixB[i];
//     delete[] matrixB;
// }

// void test_mult(size_t xA, size_t yA, size_t xB, size_t yB, int (*fptrA)(size_t,size_t), int (*fptrB)(size_t,size_t))
// {
//     size_t i;
//     int ** matrixA, ** matrixB, **result;
//     COUT << "matrixA =" << ENDL;
//     matrixA = alloc_mat<int>(xA, yA, fptrA);
//     int ** ptrA = matrixA;
//     print_mat<int>(matrixA, xA, yA);
    
//     COUT << "matrixB =" << ENDL;
//     matrixB = alloc_mat<int>(xB,yB, fptrB);
//     print_mat<int>(matrixB, xB, yB);

//     COUT << "result =" << ENDL;
//     result = normal_mul<int>(matrixA, xA,yA, matrixB, xB,yB);
//     if (result != NULL)
//         print_mat<int>(result, xA,yB);
//     else
//         COUT << "[ERROR]" << ENDL;
    
//     for (i = 0; i < xA; i++)
//         delete[] matrixA[i];
//     delete[] matrixA;
    
//     for (i = 0; i < xB; i++)
//         delete[] matrixB[i];
//     delete[] matrixB;
// }
int CCWcount(size_t x, size_t y)
{
    return (x*10)+y;
}
int Unit(size_t x, size_t y)
{
    return 1;
}
int zero(size_t x, size_t y)
{
    return 0;
}
template<int num> int k(size_t x, size_t y)
{
    return num;
}

int main()
{
    //test_mat_a(5,4);
    //test_mult(2,2,2,2,CCWcount,NULL);
    //test_mult(3,3,3,3,CCWcount,Unit);
    //test_add(1,1,2,2, k<3>, CCWcount);
    test_mat_a(3,3);
    return 0;
}

// template<typename T> T** alloc_mat(size_t xdim, size_t ydim, T (*gen)(size_t,size_t)) // gen(i,j) ydim, xdim
// {
//     size_t i, j;
//     T ** mat = (T**)std::malloc(ydim * sizeof(T*));
//     for (i = 0; i < ydim; i++)
//     {
//         mat[i] = (T*)std::malloc(xdim * sizeof(T));
//     }

//     if (gen == nullptr)
//     {
//         for (i = 0; i < ydim; i++)
//         {
//             for (j = 0; j < xdim; j++)
//             {
//                 mat[i][j] = i+j;
//             }
//         }
//     }
//     else
//     {
//         for (i = 0; i < ydim; i++)
//         {
//             for (j = 0; j < xdim; j++)
//             {
                
//                 mat[i][j] = gen(i,j);
//             }
//         }
//     }
//     return mat;
// }

// template<typename T> void print_mat(T **mat, size_t xdim, size_t ydim)
// {
//     T ** idx;
//     idx = mat;
//     COUT << "[" << ENDL;
//     for (size_t i = 0; i < ydim; i++)
//     {
//         COUT << "\t";
//         for (size_t j = 0; j < xdim-1; j++)
//         {
//             COUT << idx[i][j]
//                  << ",\t";
//         }
//         COUT << idx[i][xdim-1] << ENDL;
//     }
//     COUT << "]" << ENDL;
// }

// template<typename T> T** normal_mul(T** matrixA, size_t xA, size_t yA, T** matrixB, size_t xB, size_t yB)
// {
//     if (xA != yB)
//         return NULL;
//     if (xA == xB)
//     {
//         size_t i,j,k;
//         T ** ptrA = matrixA;
//         T ** ptrB = matrixB;
//         T** result = (T**) std::malloc (yB * sizeof(T*));
//         for (i = 0; i < yB; i++)
//             result[i] = (T*) std::malloc (xA * sizeof(T));

        
//         for (i = 0; i < yB; i++)
//         {
//             for (j = 0; j < xA; j++)
//             {
//                 for (k = 0; k < yA; k++)
//                 {
//                     result[i][j] += ptrA[i][k] * ptrB[k][j];    // equal dimensions
//                 }
//             }
//         }
//         return result;
//     }
//     else
//     {
//         size_t i,j,k;
//         T ** ptrA = matrixA;
//         T ** ptrB = matrixB;
//         T** result = (T**) std::malloc (yB * sizeof(T*));
//         for (i = 0; i < yB; i++)
//             result[i] = (T*) std::malloc (xA * sizeof(T));

        
//         for (i = 0; i < yB; i++)
//         {
//             for (j = 0; j < xA; j++)
//             {
//                 for (k = 0; k < yA; k++)
//                 {
//                     result[i][j] += ptrA[k][j] * ptrB[i][k];    // non-equal dimensions
//                 }
//             }
//         }
//         return result;
//     }
// }

// template<typename T> T** add(T** matrixA, size_t xA, size_t yA, T** matrixB, size_t xB, size_t yB)
// {
//     size_t i, j;
//     T ** result;
//     if (xA == 1 && yA == 1)
//     {
//         result = (T**) std::malloc (xB * sizeof(T*));
//         for (i = 0; i < xB; i++)
//             result[i] = (T*) std::malloc (yB * sizeof(T));
        
//         for (i = 0; i < yB; i++)
//         {
//             for (j = 0; j < xB; j++)
//             {
//                 result[i][j] = matrixA[0][0] + matrixB[i][j];
//             }
//         }
//     }
//     else if (xB == 1 && yB == 1)
//     {
//         result = (T**) std::malloc (xA * sizeof(T*));
//         for (i = 0; i < xB; i++)
//             result[i] = (T*) std::malloc (yA * sizeof(T));
        
//         for (i = 0; i < yA; i++)
//         {
//             for (j = 0; j < xA; j++)
//             {
//                 result[i][j] = matrixB[0][0] + matrixA[i][j];
//             }
//         }
//     }
//     else
//     {
//         if (xA == yA && xB == yB)
//         {
//             result = (T**) std::malloc (xA * sizeof(T*));
//         for (i = 0; i < xB; i++)
//             result[i] = (T*) std::malloc (yA * sizeof(T));
        
//         for (i = 0; i < yA; i++)
//         {
//             for (j = 0; j < xA; j++)
//             {
//                 result[i][j] = matrixB[i][j] + matrixA[i][j];
//             }
//         }
//         }
//         else
//         {
//             result = NULL;
//         }
//     }
//     return result;
// }

// void test_mat_a(size_t xdim, size_t ydim)
// {
//     size_t i,j;
//     int ** mat = alloc_mat<int>(xdim, ydim, NULL);
//     print_mat(mat, xdim, ydim);

//     for (i = 0; i < ydim; i++)
//         delete[] mat[i];

//     delete[] mat;
//     mat = NULL;
// }

// void test_add(size_t xA, size_t yA, size_t xB, size_t yB, int (*fptrA)(size_t,size_t), int (*fptrB)(size_t,size_t))
// {
//     size_t i;
//     int ** matrixA, ** matrixB, **result;
//     COUT << "matrixA =" << ENDL;
//     matrixA = alloc_mat<int>(xA, yA, fptrA);
//     int ** ptrA = matrixA;
//     print_mat<int>(matrixA, xA, yA);
    
//     COUT << "matrixB =" << ENDL;
//     matrixB = alloc_mat<int>(xB,yB, fptrB);
//     print_mat<int>(matrixB, xB, yB);

//     COUT << "result =" << ENDL;
//     result = add<int>(matrixA, xA,yA, matrixB, xB,yB);
//     if (result != NULL)
//         print_mat<int>(result, xA,yB);
//     else
//         COUT << "[ERROR]" << ENDL;
    
//     for (i = 0; i < xA; i++)
//         delete[] matrixA[i];
//     delete[] matrixA;
    
//     for (i = 0; i < xB; i++)
//         delete[] matrixB[i];
//     delete[] matrixB;
// }

// void test_mult(size_t xA, size_t yA, size_t xB, size_t yB, int (*fptrA)(size_t,size_t), int (*fptrB)(size_t,size_t))
// {
//     size_t i;
//     int ** matrixA, ** matrixB, **result;
//     COUT << "matrixA =" << ENDL;
//     matrixA = alloc_mat<int>(xA, yA, fptrA);
//     int ** ptrA = matrixA;
//     print_mat<int>(matrixA, xA, yA);
    
//     COUT << "matrixB =" << ENDL;
//     matrixB = alloc_mat<int>(xB,yB, fptrB);
//     print_mat<int>(matrixB, xB, yB);

//     COUT << "result =" << ENDL;
//     result = normal_mul<int>(matrixA, xA,yA, matrixB, xB,yB);
//     if (result != NULL)
//         print_mat<int>(result, xA,yB);
//     else
//         COUT << "[ERROR]" << ENDL;
    
//     for (i = 0; i < xA; i++)
//         delete[] matrixA[i];
//     delete[] matrixA;
    
//     for (i = 0; i < xB; i++)
//         delete[] matrixB[i];
//     delete[] matrixB;
// }

// int CCWcount(size_t x, size_t y)
// {
//     return (x*10)+y;
// }
// int Unit(size_t x, size_t y)
// {
//     return 1;
// }
// int zero(size_t x, size_t y)
// {
//     return 0;
// }
// template<int num> int k(size_t x, size_t y)
// {
//     return num;
// }

// int main()
// {
//     //test_mat_a(5,4);
//     //test_mult(2,2,2,2,CCWcount,NULL);
//     //test_mult(3,3,3,3,CCWcount,Unit);
//     test_add(1,1,2,2, k<3>, CCWcount);
//     return 0;
// }