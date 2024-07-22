#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define COLS 80
#define ROWS 25
#define CELL '#'
#define BACKGROUND ' '

void menu(int state, int **matrix);
void input_matrix(int **matrix);
void display_matrix(int **matrix);
int** allocate_matrix();
void update_matrix(int **matrix);
int count_neighbors(int **matrix, int row, int col);
void read_matrix_from_file(const char* filename, int **matrix);

int main() {
    printf("\tИгра жизни\nВыберите один из вариантов определения исходного состояния ячеек:\n");
    printf("1. Введите исходное состояние\n2. Cостояние DRAGON\n3. Cостояние GLIDER\n4. Cостояние PULSAR\n5. Cостояние R-pentomino\n6. Cостояние GOSPER GLIDER GUN\n");
    
    int state;
    if (scanf("%d", &state) != 1) {
        printf("n/a");
        return 0;
    }

    while (getchar() != '\n');

    int **matrix = allocate_matrix();
    
    menu(state, matrix);
    
    while (1) {
        printf("\33[H\33[2J");
        display_matrix(matrix);
        update_matrix(matrix);
        sleep(1);
    }

    for (int i = 0; i < ROWS; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}

void menu(int state, int **matrix) {
    printf("\33[0d\33[2J");
    switch (state) {
    case 1:
        printf("Введите матрицу размером 25х80, где «0» — мертвые клетки, а «1» — живые клетки:\n");
        input_matrix(matrix);
        break;
    case 2:
        read_matrix_from_file("ship.txt", matrix);
        break;
    case 3:
        read_matrix_from_file("agar.txt", matrix);
        break;
    case 4:
        read_matrix_from_file("pulsar.txt", matrix);
        break;
    case 5:
        read_matrix_from_file("hassler.txt", matrix);
        break;
    case 6:
        read_matrix_from_file("glidergun.txt", matrix);
        break;
    default:
        printf("Неверный вариант.\n");
        exit(EXIT_FAILURE);
    }
}

void read_matrix_from_file(const char* filename, int **matrix) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    int i = 0, j = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '0' || c == '1') {
            if (j < COLS) {
                matrix[i][j] = (c == '1') ? 1 : 0;
                j++;
            }
        } else if (c == '\n') {
            if (j != COLS) {
                printf("Ошибка при чтении файла %s на строке %d. Ожидалось 80 символов, но найдено %d.\n", filename, i + 1, j);
                fclose(file);
                exit(EXIT_FAILURE);
            }
            i++;
            j = 0;
        } else {
            printf("Неожиданный символ '%c' в файле %s, строка %d, столбец %d. Допустимы только '0' и '1'.\n", c, filename, i + 1, j + 1);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        if (i >= ROWS) {
            break;
        }
    }

    if (i != ROWS) {
        printf("Ошибка при чтении файла %s. Ожидалось 25 строк, но найдено %d.\n", filename, i);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void input_matrix(int **matrix) {
    int input;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (scanf("%1d", &input) != 1 || (input != 0 && input != 1)) {
                printf("Неверный ввод на строке %d, столбце %d. Убедитесь, что вводите только '0' или '1'.\n", i + 1, j + 1);
                exit(EXIT_FAILURE);
            }
            matrix[i][j] = input;
        }
    }
    while (getchar() != '\n');
}

void display_matrix(int **matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c", matrix[i][j] == 1 ? CELL : BACKGROUND);
        }
        printf("\n");
    }
}

int** allocate_matrix() {
    int** matrix = (int**)calloc(ROWS, sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        matrix[i] = (int*)calloc(COLS, sizeof(int));
    }

    return matrix;
}

void update_matrix(int **matrix) {
    int **new_matrix = allocate_matrix();

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int neighbors = count_neighbors(matrix, i, j);
            if (matrix[i][j] == 1) {
                if (neighbors < 2 || neighbors > 3) {
                    new_matrix[i][j] = 0;
                } else {
                    new_matrix[i][j] = 1;
                }
            } else {
                if (neighbors == 3) {
                    new_matrix[i][j] = 1;
                }
            }
        }
    }

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j] = new_matrix[i][j];
        }
    }

    for (int i = 0; i < ROWS; i++) {
        free(new_matrix[i]);
    }
    free(new_matrix);
}

int count_neighbors(int **matrix, int row, int col) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            int newRow = row + i;
            int newCol = col + j;
            if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
                count += matrix[newRow][newCol];
            }
        }
    }
    return count;
}
