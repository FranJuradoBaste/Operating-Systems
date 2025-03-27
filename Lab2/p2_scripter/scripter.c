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
    char *comandos[max_commands];
    int num_comandos = tokenizar_linea(linea, "|", comandos, max_commands);

    //Check if background is indicated
    if (strchr(comandos[num_comandos - 1], '&')) {
        background = 1;
        char *pos = strchr(comandos[num_comandos - 1], '&'); 
        //remove character 
        *pos = '\0';
    }

    //Finish processing
    for (int i = 0; i < num_comandos; i++) {
        int args_count = tokenizar_linea(comandos[i], " \t\n", argvv, max_args);
        procesar_redirecciones(argvv);

        /********* This piece of code prints the command, args, redirections and background. **********/
        /*********************** REMOVE BEFORE THE SUBMISSION *****************************************/
        /*********************** IMPLEMENT YOUR CODE FOR PROCESSES MANAGEMENT HERE ********************/
        printf("Comando = %s\n", argvv[0]);
        for(int arg = 1; arg < max_args; arg++)
            if(argvv[arg] != NULL)
                printf("Args = %s\n", argvv[arg]); 
                
        printf("Background = %d\n", background);
        if(filev[0] != NULL)
            printf("Redir [IN] = %s\n", filev[0]);
        if(filev[1] != NULL)
            printf("Redir [OUT] = %s\n", filev[1]);
        if(filev[2] != NULL)
            printf("Redir [ERR] = %s\n", filev[2]);
        /**********************************************************************************************/
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
    char c;
    int linea_num = 0;

    //Leemos el fichero
    while (read(fd, &c, 1) == 1) {
        if (c == '\n' || i >= max_line - 1) {
            linea[i] = '\0';
            printf(">> Línea %d: '%s'\n", linea_num, linea);

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

                background = 0;
                for (int j = 0; j < max_args; j++) argvv[j] = NULL;
                for (int j = 0; j < max_redirections; j++) filev[j] = NULL;

                // ejecutamos el comando
                int n_commands = procesar_linea(linea);

                //Ejecutamos los procesos con sus redirecciones
                pid_t pid = fork();
                if (pid == 0) {
                    // Proceso hijo
                    if (filev[0] != NULL) { // Redirección de entrada
                        int fd_in = open(filev[0], O_RDONLY);
                        if (fd_in < 0) {
                        perror("Error al abrir archivo de entrada");
                        exit(1);
                        }
                        dup2(fd_in, STDIN_FILENO);
                        close(fd_in);
                        }           

                    if (filev[1] != NULL) { // Redirección de salida
                        int fd_out = open(filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out < 0) {
                        perror("Error al abrir archivo de salida");
                        exit(1);
                        }
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_out);
                        }

                    if (filev[2] != NULL) { // Redirección de error
                        int fd_err = open(filev[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_err < 0) {
                        perror("Error al abrir archivo de error");
                        exit(1);
                        }
                        dup2(fd_err, STDERR_FILENO);
                        close(fd_err);
                        }

                    // Filtrar argumentos: eliminar redirecciones y sus archivos
                    char *args_filtrados[max_args];
                    int real_argc = 0;
                    for (int k = 0; k < max_args && argvv[k] != NULL; k++) {
                        if (
                            strcmp(argvv[k], "<") == 0 ||
                            strcmp(argvv[k], ">") == 0 ||
                            strcmp(argvv[k], "!>") == 0
                        ) {
                        k++; // saltar el archivo que sigue
                        } else {
                        args_filtrados[real_argc++] = argvv[k];
                        }
                        }
                    args_filtrados[real_argc] = NULL;
                    // Imprimir el comando que se va a ejecutar (debug)
                    fprintf(stderr, "Ejecutando comando: %s", args_filtrados[0]);
                    for (int z = 1; args_filtrados[z] != NULL; z++) {
                        fprintf(stderr, " %s", args_filtrados[z]);
                        }
                    fprintf(stderr, "\n");
                    execvp(args_filtrados[0], args_filtrados);
                    perror("execvp");
                    exit(1);
                    } 
                else if (pid > 0) {
                    // Proceso padre
                    if (!background) {
                        waitpid(pid, NULL, 0);
                    } else {
                        // Solo imprimimos el PID si es background
                        printf("Proceso en background con PID: %d\n", pid);
                    }
                } else {
                    perror("fork");
                    exit(1);
                }
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