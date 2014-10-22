#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <math.h>

#include "../../sr_module.h"

#define BUFFER_SIZE 8192
typedef int Bool;
#define True 1
#define False 0

typedef struct StatsConnection{
    char *ip;
    char *port;
} StatsConnection;


typedef struct StatsdSocket {
    char *name; // name
    int sock; // socket
    int timeout; // how many miliseconds to wait for an answer
    time_t last_failure; // time of the last failure
    char data[BUFFER_SIZE]; // buffer for the answer data
} StatsdSocket;

static StatsdSocket statsd_socket = {
    "/var/run/statsd/statsd.sock",
    -1,
    500, // timeout 500ms if no answer
    0,
    ""
};

static StatsConnection statsd_connection = {
    "127.0.0.1",
    "8125"
};

static Bool statsd_connect(void){

    struct addrinfo *serverAddr;

    if (statsd_socket.sock > 0){
        return True;
    }

    int error = getaddrinfo(
        statsd_connection.ip, statsd_connection.port,
        NULL, &serverAddr);
    if (error != 0)
    {
        LM_ERR("could not initiate server information (%s)\n",gai_strerror(error));
        return False;
    }

    statsd_socket.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (statsd_socket.sock == 0 ){
        LM_ERR("could not initiate a connect to statsd\n");
        return False;
    }

    int rc = connect(
        statsd_socket.sock, serverAddr->ai_addr, serverAddr->ai_addrlen);
    if (rc <0){
        LM_ERR("could not initiate a connect to statsd\n");
        return False;
    }
    return True;
}

static int send_command(char *command){
    if (!statsd_connect()){
        LM_ERR("Connection lost to statsd");
        return False;
    }
    if (send(statsd_socket.sock, command, strlen(command), 0) < 0){
        LM_ERR("could not send the correct info to statsd");
        return False;
    }
    LM_DBG("Sending to statsd (%s)", command);
    return True;
}

static int statsd_set(char *key, char *value){
   char* end = 0;
   char command[254];
   int val = strtol(value, &end, 0);
   if (*end){
       LM_ERR("statsd_gauge can't use the provide value(%s)", value);
       return False;
   }
   sprintf(command, "%s:%i|s\n", key, val);
   return send_command(command);
}


static int statsd_gauge(char *key, char *value){
   /* char* end = 0; */
   char command[254];
   /* int val = strtol(value, &end, 0); */
   /* if (*end){ */
   /*     LM_ERR("statsd_gauge can't use the provide value(%s)",value); */
   /*     return False; */
   /* } */
   sprintf(command, "%s:%s|g\n", key, value);
   return send_command(command);
}

static int statsd_count(char *key, char *value){
   char* end = 0;
   char command[254];
   int val = strtol(value, &end, 0);
   if (*end){
       LM_ERR("statsd_count could not  use the provide value(%s)",value);
       return False;
   }
   sprintf(command, "%s:%i|c\n", key, val);
   return send_command(command);
}

static int statsd_timing(char *key, int value){
   char command[254];
   sprintf(command,"%s:%i|ms\n",key,value);
   return send_command(command);
}

static int statsd_init(char *ip, char *port){

    if (ip != NULL){
        statsd_connection.ip = ip;
    }
    if (port != NULL ){
       statsd_connection.port = port;
    }
    LM_ERR("Statsd_init ip %s",statsd_connection.ip);
    LM_ERR("Statsd_init port %s",statsd_connection.port);
    return statsd_connect();
}

static int statsd_destroy(void){
    statsd_socket.sock = 0;
    return True;
}
