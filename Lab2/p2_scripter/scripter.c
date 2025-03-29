#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

/* CONST VARS */
const int max_line = 1024;
const int max_commands = 10;
#define max_redirections 3 //stdin, stdout, stderr
#define max_args 15

/* VARS TO BE USED FOR THE STUDENTS */
char * argvv[max_args];
char * filev[max_redirections];
int background = 0; 

/**
 * This function splits a char* line into different tokens based on a given character
 * @return Number of tokens 
 */
int tokenizar_linea(char *linea, char *delim, char *tokens[], int max_tokens) {
    int i = 0;
    char *token = strtok(linea, delim);
    while (token != NULL && i < max_tokens - 1) {
        tokens[i++] = token;
        token = strtok(NULL, delim);
    }
    tokens[i] = NULL;
    return i;
}

/**
 * This function processes the command line to evaluate if there are redirections. 
 * If any redirection is detected, the destination file is indicated in filev[i] array.
 * filev[0] for STDIN
 * filev[1] for STDOUT
 * filev[2] for STDERR
 */
void procesar_redirecciones(char *args[]) {
    //initialization for every command
    filev[0] = NULL;
    filev[1] = NULL;
    filev[2] = NULL;
    //Store the pointer to the filename if needed.
    //args[i] set to NULL once redirection is processed
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            filev[0] = args[i+1];
            args[i] = NULL;
            args[i + 1] = NULL;
        } else if (strcmp(args[i], ">") == 0) {
            filev[1] = args[i+1];
            args[i] = NULL;
            args[i + 1] = NULL;
        } else if (strcmp(args[i], "!>") == 0) {
            filev[2] = args[i+1];
            args[i] = NULL; 
            args[i + 1] = NULL;
        }
    }
}

/**
 * This function processes the input command line and returns in global variables: 
 * argvv -- command an args as argv 
 * filev -- files for redirections. NULL value means no redirection. 
 * background -- 0 means foreground; 1 background.
 */
int procesar_linea(char *linea) {
    //  Separa la línea en subcomandos por '|'
    char *comandos[max_commands];
    int num_comandos = tokenizar_linea(linea, "|", comandos, max_commands);

    //Detectar background en el último subcomando
    if (strchr(comandos[num_comandos - 1], '&')) {
        background = 1;
        char *pos = strchr(comandos[num_comandos - 1], '&'); 
        //remove character 
        *pos = '\0';
    }

    //Crear tuberías (si hay más de un subcomando)
    int pipes[max_commands - 1][2];
    for (int i = 0; i < num_comandos - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

     // 4) Para cada subcomando, fork + exec
    for (int i = 0; i < num_comandos; i++) {
        // Limpieza de argvv y filev para este subcomando
        for (int j = 0; j < max_args; j++) argvv[j] = NULL;
        for (int j = 0; j < max_redirections; j++) filev[j] = NULL;

        // Tokenizar subcomando por espacios
        int args_count = tokenizar_linea(comandos[i], " \t\n", argvv, max_args);
        // Detectar redirecciones
        procesar_redirecciones(argvv);

        // Hacer fork
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            // Proceso hijo

            // (a) Redirecciones específicas (<, >, !>)
            //  - Solo primer subcomando usa filev[0] (entrada)
            //  - Solo último subcomando usa filev[1] (salida)
            //  - Cualquiera puede usar filev[2] (error)
            if (i == 0 && filev[0]) {
                int fd_in = open(filev[0], O_RDONLY);
                if (fd_in < 0) {
                    perror("Error al abrir archivo de entrada");
                    exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (i == num_comandos - 1 && filev[1]) {
                int fd_out = open(filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    perror("Error al abrir archivo de salida");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            if (filev[2]) {
                int fd_err = open(filev[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_err < 0) {
                    perror("Error al abrir archivo de error");
                    exit(1);
                }
                dup2(fd_err, STDERR_FILENO);
                close(fd_err);
            }

            // (b) Conexión de tuberías
            if (i > 0) {
                // No eres el primer subcomando; lees de la tubería anterior
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            if (i < num_comandos - 1) {
                // No eres el último; escribes en la tubería actual
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // (c) Cerrar tuberías en hijo
            for (int k = 0; k < num_comandos - 1; k++) {
                close(pipes[k][0]);
                close(pipes[k][1]);
            }

            // (d) Ejecutar el comando
            execvp(argvv[0], argvv);
            perror("execvp");
            exit(1);
        }
        // Proceso padre no hace nada especial aquí; sigue para crear/fork el siguiente subcomando
    }

    // (e) Cerrar tuberías en padre
    for (int i = 0; i < num_comandos - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // (f) Si foreground => esperar hijos, si background => no esperar
    if (!background) {
        for (int i = 0; i < num_comandos; i++) {
            wait(NULL);
        }
    } else {
        // Podrías imprimir el PID del padre o un mensaje
        write(STDOUT_FILENO, "Comando en background\n", 23);
    }

    return num_comandos;
}



    
int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(STDERR_FILENO, "Uso: ./scripter <fichero_de_comandos>\n", 39);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("No se pudo abrir el archivo");
        exit(1);
    }

    char linea[max_line];
    int i = 0;
    int linea_num = 0;
    char c;

    while (read(fd, &c, 1) == 1) {
        if (c == '\n' || i >= max_line - 1) {
            linea[i] = '\0';
            if (i == 0) {
                write(STDERR_FILENO, "Error: línea vacía encontrada.\n", 31);
                close(fd);
                exit(1);
            }

            if (linea_num == 0) {
                if (strcmp(linea, "## Script de SSOO") != 0) {
                    write(STDERR_FILENO, "Error: cabecera inválida.\n", 27);
                    close(fd);
                    exit(1);
                }
            } else {
                // Aquí delegamos TODO a procesar_linea
                procesar_linea(linea);
            }
            i = 0;
            linea_num++;
        } else {
            linea[i++] = c;
        }
    }
    close(fd);
    return 0;
}