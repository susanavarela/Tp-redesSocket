#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
/* sockets */
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define SERV_PORT       5000
#define SERV_HOST_ADDR "192.168.0.31"
#define BUF_SIZE        100
#define BACKLOG         5


void guardarDatos(int opcion);
void fechaYhora(char guardar[50]);
 void validar(char calculo[100]);
int calcular(char primero[20], int simbolo, char segundo[20]);
int potencia(int base, int expo);
int factorial(int numero);





int main(int argc, char* argv[])
{
    int sockfd, connfd;
    unsigned int len;
    struct sockaddr_in servaddr, client;

    int  len_rx, len_tx = 0;
    char buff_tx[BUF_SIZE] = "hola cliente, soy el servidor";
    char buff_rx[BUF_SIZE];


    /* crear el socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        fprintf(stderr, "la creacion del socket fallo %d: %s \n", errno, strerror( errno ));
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));

    /* asigna IP, SERV_PORT, IPV4 */
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
    servaddr.sin_port        = htons(SERV_PORT);


    /* Bind socket */
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0){
        fprintf(stderr, "fallo socket bind %d: %s \n", errno, strerror( errno ));
        return -1;
    }
    /* Escucha */
    if ((listen(sockfd, BACKLOG)) != 0){
        fprintf(stderr, "la escucha del socket fallo %d: %s \n", errno, strerror( errno ));
        return -1;
    }else{
        printf("escuchando en el puerto %d \n\n", ntohs(servaddr.sin_port) );
        guardarDatos(1);
    }

    len = sizeof(client);


      while(1)
      {
        printf("Esperando cliente...\n");
        connfd = accept(sockfd, (struct sockaddr *)&client, &len);
        if (connfd < 0){
            fprintf(stderr, "conexion no aceptada. %d: %s \n", errno, strerror( errno ));
            return -1;
        }else{
             printf("Conectado al cliente...\n");
            guardarDatos(2);
            write(connfd, buff_tx, strlen(buff_tx));
            int inactividad = 0;
            while(1){

                int opcion = 0;
                char msjCliente[100];
                opcion = read(connfd, msjCliente, sizeof(msjCliente));

                if(opcion == -1){
                    fprintf(stderr, "connfd no se puede leer. %d: %s \n", errno, strerror( errno ));

                }else if(opcion == 0){
                    printf("socket del cliente cerrado \n\n");
                    close(connfd);

                    break;

                }else{
                   while(opcion != 0 && opcion != -1){
                        opcion = read(connfd, msjCliente, sizeof(msjCliente));

                       if(strstr(msjCliente, "c")){
                            if(strlen(msjCliente) > 21 ||  strlen(msjCliente) == 0) {
                                char error[100] = "La operación debe tener entre 1 y 20 caracteres";
                                write(connfd, error, strlen(error));

                            }else{
                                validar(msjCliente);
                                write(connfd, msjCliente, strlen(msjCliente));
                                memset(msjCliente,0,100);
                            }
                        } else if(strstr(msjCliente, "h")){

                            int fd, leidos;
                           fd  = open("/home/dev/Escritorio/codeblock/pruebaServidor/calculadora.txt", O_RDONLY);
                            char aux[100] = "";
                            while(leidos = read(fd,&aux, 100)>0){
                                    read(aux, 100, fd);
                                    write(connfd, aux, strlen(aux));

                                    memset(aux,0,100);
                             }
                            memset(aux,0,100);
                            write(connfd, "fin log", strlen("fin log"));
                            close(fd);


                    }else if(strstr(msjCliente, "i")){
                           printf("Conexion finalizada por inactividad.\n");
                           guardarDatos(4);
                           inactividad = 1;
                            opcion = 0;
                             break;
                    }


                }

              }
               break;
            }
            if(inactividad != 1){
            printf("Conexion finalizada.\n");
                    guardarDatos(3);
            }
        }
    }

 }




 void guardarDatos(int opcion){
    FILE *fd;
    fd = open("/home/dev/Escritorio/codeblock/pruebaServidor/calculadora.txt", O_WRONLY | O_APPEND);
    char guardar[50]="";
    fechaYhora(guardar);
    switch(opcion){
        case 1://opcion 1
                char socketCreado[100]= "socket creado. Puerto escucha: ";
                strcat(socketCreado, "5000=====\n\0");
                strcat(guardar, socketCreado);
                break;
        case 2:
                char aceptada[100]= "Conexion aceptada=====\n\0";
                strcat(guardar, aceptada);
                break;
        case 3:
                char cerrada[100]= "Conexion cerrada=====\n\0";
                strcat(guardar, cerrada);
                break;
        case 4:
                char inactividad[100]= "Conexion cerrada por inactividad=====\n\0";
                strcat(guardar, inactividad);
                break;
                default: printf("\ error opcion: %d\n", opcion);break;
     }
    //printf("\nCompleto: %s\n", guardar);
    write(fd, guardar, strlen(guardar));
    close(fd);
 }


void fechaYhora(char guardar[50]){
    time_t ahora = time(NULL);
    struct tm *actual = localtime(&ahora);

     char fecha[50];
     sprintf(fecha,"%2d-%02d-%d %02d:%02d:=====", actual->tm_mday, actual->tm_mon +1, actual->tm_year+1900, actual->tm_hour, actual->tm_min);
     strcat(guardar, fecha);
 }






 void validar(char calculo[20]){

    char simbolos[10] = "/*-+!^";

    char primero[20] = "";
    char segundo[20] = "";
    char simbolo;
    char numeros[10] = "0123456789";
    int pos = 0;


    pos = strcspn(calculo,simbolos);
    simbolo = calculo[pos];
    int errorPos = 0;
    char c[20] = "";
    char error[20] = "";
    int flag = 1;
    int cantSimbolos = 0;



     for(int i = 0; i < strlen(calculo)-1; i++){
            for( int j = 0; j < 11; j++){
                if(calculo[i] == numeros[j]){
                   c[i] = numeros[j];
                   flag = 0;
                }else if(calculo[i] == simbolos[j]){
                    c[i] = simbolos[j];
                    flag = 0;
                }
            }

            if(calculo[i] == '/' || calculo[i] == '*' || calculo[i] == '-' || calculo[i] == '+' || calculo[i] == '^' || calculo[i] == '!'){
                       cantSimbolos++ ;
                    }
            if(flag != 0){
               error[errorPos] = calculo[i];
                errorPos++;
            }
            flag = 1;
        }
    if(strlen(calculo)-1!= strlen(c)){

         memset(calculo,0,100);
        strcat(calculo, "No se pudo realizar la operacion, se encontro un caracter no contemplado: ");
        strcat(calculo,error);

    }else if(cantSimbolos == 0){
        char res[20];

        memset(calculo,0,100);
         strcat(calculo, c);
        strcat(calculo, " = ");
        strcat(calculo, c);
    }else if(simbolo == '!'){

                for(int i = 0; i < pos; i++){
                    for( int j = 0; j < strlen(numeros); j++){
                    if(calculo[i] == numeros[j]){
                        primero[i] = calculo[i];
                      }
                    }
                }

                if(strlen(c) > strlen(primero) + 1){
                    char error2[20] = "";
                    if(c[0] == '!'){
                        error2[0]  = c[pos];
                        error2[1]  = c[pos+1];
                    }else{
                         for( int j = pos-1; j < strlen(c); j++){
                              error2[0]  = c[pos-1];
                              error2[1]  = c[pos];
                              error2[2]  = c[pos+1];
                        }
                     }


                    memset(calculo,0,100);
                    strcat(calculo, "No se pudo realizar la operacion, la operacion esta mal formada:  ");
                    strcat(calculo,error2);


                }else{
                    int resultado = calcular(primero, strcspn(simbolos, c), segundo);
                    char res[20];
                    sprintf(res, " = %d",resultado);
                    memset(calculo,0,100);
                    strcat(c, res);
                    strcat(calculo, c);
                }

        }else{

            if(pos != 0 && (strlen(c)!= pos)){

                 for(int i = 0; i < pos; i++){
                     for( int j = 0; j < strlen(numeros); j++){
                        if(c[i] == numeros[j]){
                            primero[i] = c[i];
                        }
                      }
                    }

                    errorPos = 0;
                    for( int i = (pos+1); i < strlen(c); i++){
                        for( int j = 0; j < strlen(numeros); j++){
                            if(c[i] == numeros[j]){
                                segundo[i -pos-1] = c[i];
                            }
                        }
                    }
                    for( int i = (pos+1); i < strlen(c); i++){
                        for( int j = 0; j < strlen(simbolos); j++){
                            if(c[i] == simbolos[j]){
                                errorPos = i;
                            }
                        }
                    }
                    if(errorPos != 0){
                        for( int j = errorPos-1; j < strlen(c); j++){
                          error[j-errorPos+1] = c[j];
                        }

                        memset(calculo,0,100);
                        strcat(calculo, "No se pudo realizar la operacion, se encontro un caracter no contemplado: ");
                        strcat(calculo,error);


                    }else{

                       if(simbolo == '/' && strlen(segundo) == 1 && segundo[0] == '0'){

                                memset(calculo,0,100);
                                strcat(calculo, "No se puede dividir por cero ");


                        }else{
                            if((strlen(primero) + strlen(segundo)) +1 == strlen(c)){

                                int resultado = calcular(primero, strcspn(simbolos, c), segundo);
                                char res[20];
                                sprintf(res, " = %d",resultado);
                                memset(calculo,0,100);
                                strcat(c, res);
                                strcat(calculo, c);
                            }else{

                                memset(calculo,0,100);
                                strcat(calculo, "No se pudo realizar la operacion, la operacion esta mal formada:  ");
                                strcat(calculo,error);

                            }
                        }
                    }
                }else if(strlen(calculo)-1 == strlen(c) && cantSimbolos == 0){

                    char res[20];
                    sprintf(res, " = %s",c);
                    memset(calculo,0,100);
                    strcat(calculo, res);
                }else if(pos == 0){

                    memset(calculo,0,100);
                    strcat(calculo, "No se pudo realizar la operacion, la operacion esta mal formada:  ");
                    strcat(calculo,c);


                }else{

                    memset(calculo,0,100);
                    strcat(calculo, "Calculo mal formado ");

            }

    }
}

int calcular(char primero[20], int simbolo, char segundo[20]){
int respuesta;
int a = atoi(primero);
int b = atoi(segundo);

switch(simbolo){
    case 0: respuesta = a/b;break;
    case 1: respuesta = a*b;break;
    case 2: respuesta = a-b;break;
    case 3: respuesta = a+b;break;
    case 4: respuesta = factorial(a);break;
    case 5: respuesta = potencia( a, b);break;
    default: break;
}

return respuesta;
}

int potencia(int base, int expo){
int respuesta =1;
if(expo==0){
    return respuesta;
}else{
    for(int i=1; i<=expo; i++){
        if(i==2){
            respuesta= base*base;
        }
        else{
            respuesta=(base*respuesta);
        }
    }
    return base*potencia(base,expo-1);
 }
}

int factorial(int numero) {
    if (numero <= 1)
        return 1;
    return numero * factorial(numero - 1);
}


