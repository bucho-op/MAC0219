#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double c_x_min, c_x_max, c_y_min, c_y_max;
double pixel_width, pixel_height;

int iteration_max = 200;
int image_size;
unsigned char **image_buffer;
int i_x_max, i_y_max, image_buffer_size;

int gradient_size = 16;
int colors[17][3] = { /* Cores predefinidas, conforme seu código */ };

void allocate_image_buffer(){
    int rgb_size = 3;
    image_buffer = (unsigned char **) malloc(sizeof(unsigned char *) * image_buffer_size);
    for(int i = 0; i < image_buffer_size; i++){
        image_buffer[i] = (unsigned char *) malloc(sizeof(unsigned char) * rgb_size);
    }
}

void update_rgb_buffer(int iteration, int x, int y){
    int color;
    if(iteration == iteration_max){
        image_buffer[(i_y_max * y) + x][0] = colors[gradient_size][0];
        image_buffer[(i_y_max * y) + x][1] = colors[gradient_size][1];
        image_buffer[(i_y_max * y) + x][2] = colors[gradient_size][2];
    } else {
        color = iteration % gradient_size;
        image_buffer[(i_y_max * y) + x][0] = colors[color][0];
        image_buffer[(i_y_max * y) + x][1] = colors[color][1];
        image_buffer[(i_y_max * y) + x][2] = colors[color][2];
    }
}

void write_to_file(){
    FILE *file;
    char *filename = "output.ppm";
    char *comment = "# Mandelbrot Image";

    int max_color_component_value = 255;
    file = fopen(filename, "wb");
    fprintf(file, "P6\n%s\n%d\n%d\n%d\n", comment, i_x_max, i_y_max, max_color_component_value);

    for(int i = 0; i < image_buffer_size; i++){
        fwrite(image_buffer[i], 1, 3, file);
    }
    fclose(file);
}

void *compute_mandelbrot_thread(void *arg) {
    int start_y = *(int *)arg;
    int end_y = start_y + i_y_max / 4;  // Suponha 4 threads dividindo as linhas

    double z_x, z_y, z_x_squared, z_y_squared;
    double escape_radius_squared = 4;
    int iteration, i_x, i_y;
    double c_x, c_y;

    for(i_y = start_y; i_y < end_y; i_y++){
        c_y = c_y_min + i_y * pixel_height;
        if(fabs(c_y) < pixel_height / 2) c_y = 0.0;
        for(i_x = 0; i_x < i_x_max; i_x++){
            c_x = c_x_min + i_x * pixel_width;
            z_x = z_y = z_x_squared = z_y_squared = 0.0;

            for(iteration = 0; iteration < iteration_max && (z_x_squared + z_y_squared < escape_radius_squared); iteration++) {
                z_y = 2 * z_x * z_y + c_y;
                z_x = z_x_squared - z_y_squared + c_x;
                z_x_squared = z_x * z_x;
                z_y_squared = z_y * z_y;
            }
            update_rgb_buffer(iteration, i_x, i_y);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    if(argc < 6){
        printf("usage: ./mandelbrot_pth c_x_min c_x_max c_y_min c_y_max image_size\n");
        exit(0);
    }

    sscanf(argv[1], "%lf", &c_x_min);
    sscanf(argv[2], "%lf", &c_x_max);
    sscanf(argv[3], "%lf", &c_y_min);
    sscanf(argv[4], "%lf", &c_y_max);
    sscanf(argv[5], "%d", &image_size);

    i_x_max = i_y_max = image_size;
    image_buffer_size = image_size * image_size;
    pixel_width = (c_x_max - c_x_min) / i_x_max;
    pixel_height = (c_y_max - c_y_min) / i_y_max;

    allocate_image_buffer();

    // Número de threads e identificadores
    int num_threads = 4; // testes serao feitos com varios numeros diferentes d threads
    pthread_t threads[num_threads];
    int thread_args[num_threads];

    // Criar e lançar threads
    for(int i = 0; i < num_threads; i++){
        thread_args[i] = i * (i_y_max / num_threads); // Cada thread cuida de um quarto da imagem
        pthread_create(&threads[i], NULL, compute_mandelbrot_thread, (void *)&thread_args[i]);
    }

    // Aguardar todas as threads terminarem
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
    }

    write_to_file();
    return 0;
}

