#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#define BUF_SIZE  100

int fin, inicio;
void *cierreInactividad(void * sockfd);
void menu(int sockfd);
void calculo(int sockfd);
void log(int sockfd);

/* texto de envio*/
char buf_tx[BUF_SIZE] = "Hola servidor. Soy un cliente";
char buf_rx[BUF_SIZE];

int main(){
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        printf("CLIENT: la creacion del socket ha fallado \n");
        return -1;
    }else{
        printf("CLIENT: el socket ha sido creado \n");
    }

         char salir;
         while(2){

            printf("\n1-Conectar \n");
            printf("\n2-Salir \n");
            scanf("%c", &salir);
            if(salir == '1'){
                memset(&servaddr, 0, sizeof(servaddr));
                char ipServer[15]=" ";
                memset(ipServer, 0, 15);

                printf("\nPor favor, ingrese la ip del servidor\n");
                scanf("%s", &ipServer);

                int portServer=0;
                printf("\nPor favor, ingrese el puerto\n");
                scanf("%d", &portServer);
                servaddr.sin_port = htons(portServer);

                servaddr.sin_family = AF_INET;
                servaddr.sin_addr.s_addr = inet_addr( ipServer );
                servaddr.sin_port = htons(portServer);

                if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
                    printf("la conexion con el servidor fallo...\n");

                }else{

                printf("conectado al servidor\n");

                write(sockfd, buf_tx, sizeof(buf_tx));
                read(sockfd, buf_rx, sizeof(buf_rx));

                fin = 0;
                pthread_t proceso;
                if(pthread_create(&proceso, NULL, &cierreInactividad, sockfd) != 0)
                    return -1;
                menu(sockfd);

                if(fin == 1){
                    printf("\nSesion cerrada por inactividad.\n");
                    memset(buf_tx,0,BUF_SIZE);
                    strcat(buf_tx, "i");
                    write(sockfd, buf_tx, sizeof(buf_tx));
                }
                close(sockfd);
                }
      }else if(salir == '2'){
        return 2;
      }else{
          printf("\nLa opcion ingresada no es correcta.\n");
       }
       sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        printf("CLIENT: la creacion del socket ha fallado \n");
        return -1;
      }
    }
}

void *cierreInactividad(void *sockfd){
    inicio = 0;
    while(fin != 1){
        sleep(1);
        inicio++;
        if(inicio >= 120){
            write(sockfd, "i", sizeof("i"));
             fin = 1;
        }
    }
}

void menu(int sockfd){
    int opcion = 0;
    char opc[10];
    char numeros[10] = "1234";
    do{
        char servidorRta[BUF_SIZE] = "";
        printf("\n1-Calcular \n");
        printf("\n2-Ver historial \n");
        printf("\n3-Limpiar pantalla \n");
        printf("\n4-Cerrar sesion \n");
        scanf("%s", &opc);
        inicio = 0;
        if(strstr(opc,"volver")){
            opcion = 4;
        }else{
            opcion = strcspn(numeros,opc) + 1;
        }
        switch(opcion){
            case 1: printf("\nCalculo\n");
                    calculo(sockfd);
                    break;
            case 2: printf("\nVer server log \n");
                    log(sockfd);
                    break;
            case 3: printf("\nLimpiar pantalla \n");
                    system("clear");
                    break;
            case 4: printf("\nSesion cerrada \n");
                            opcion = 4;
                            break;
            case 5: printf("\nSesion cerrada por inactividad\n");
                    opcion = 5;
                    break;
            default: printf("\nLa opcion ingresada no es correcta.\nVuelva a intentar.\n");
            }

    }while(opcion != 4 && fin != 1);
  }


 void calculo(int sockfd){

    char calculo[BUF_SIZE] = "";
    char servidorRta[BUF_SIZE];
    while(!strstr(calculo, "=") && !strstr(calculo, "volver") && fin != 1){

    printf("\nPor favor, ingrese la operacion a calcular\n");
    scanf("%s", &calculo);
    inicio = 0;
    if(strstr(calculo,"volver")){
            break;
    }
    strcat(calculo, "c");
    write(sockfd, calculo, sizeof(calculo));

    if(fin==1){
        return;
     }
     printf("******************************respuesta******************************\n");
    read(sockfd, servidorRta, sizeof(servidorRta));
    printf("Servidor: %s\n", servidorRta);
    inicio = 0;
    memset(servidorRta,0,BUF_SIZE);

    }
 return;
}

void log(int sockfd){
    char servidorRta[BUF_SIZE];
    write(sockfd, "h", sizeof("h"));
    int envio = 0;
    do{
        memset(servidorRta,0,BUF_SIZE);
        read(sockfd, servidorRta, sizeof(servidorRta));
        printf("%s", servidorRta);
        inicio = 0;

    }while(!strstr(servidorRta,"f"));
 }
