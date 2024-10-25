/*
 * task_2.c — программа для получения всех IP-адресов по имени хоста (если это возможно)
 * 
 * Copyright (c) <2022> <Гордеев Никита>
 * 
 * This code is licensed under a MIT-style license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main(int argc, char *argv[]) {
    // Проверка наличия аргументов
    if (argc != 2) {
	    fprintf(stderr, "Использование: ./task_2 ya.ru\n");
        exit(EXIT_FAILURE);
    }

    // Указываем, что хотим получить
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    struct sockaddr_in *sinp_ipv4;
    struct sockaddr_in6 *sinp_ipv6;
    const char *addr;
    char *ip_addr;
    char abuf[INET_ADDRSTRLEN];
    char abuf2[INET6_ADDRSTRLEN];
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    // Получаем адрес (имя хоста)
    if (getaddrinfo(argv[1], NULL, &hint, &ailist) != 0) {
        printf("Ошибка вызова функции getaddrinfo\n");
        exit(EXIT_FAILURE);
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        //домен интернета IPV4
        if (aip->ai_family == AF_INET) {
	        ip_addr="IPV4";
            sinp_ipv4 = (struct sockaddr_in *) aip->ai_addr;
            addr = inet_ntop(AF_INET, &sinp_ipv4->sin_addr, abuf, INET_ADDRSTRLEN);

            // указатель на строку с адресом
            if (addr==NULL) {
                printf("Ошибка функции inet_ntop\n");
                exit(EXIT_FAILURE);
            }
        }
        //домент интернета IPV6
        else {
	        ip_addr="IPV6";
            sinp_ipv6 = (struct sockaddr_in6 *) aip->ai_addr;
            addr = inet_ntop(AF_INET6, &sinp_ipv6->sin6_addr, abuf2, INET6_ADDRSTRLEN);
		    
            // указатель на строку с адресом
            if (addr==NULL) { 
                printf("Ошибка функции inet_ntop\n");
	       	    exit(EXIT_FAILURE);
            }
	    }
        
        // Печатаем IP-адрес
	    printf("%s has address %s %s\n", argv[1], ip_addr, addr ? addr : "не известен");
    }

    freeaddrinfo(aip);

    return 0;
}

/* 
 * Материалы:
 * Системное программирование, сокеты // Кафедра Информатики и Математического Обеспечения URL: https://cs.petrsu.ru/~vadim/sp2022/lab8.php.ru (дата обращения: 04.12.2022).
 * Листинг 16.1. Вывод сведений о хостах и сетевых службах // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * understanding the protocol family argument of socket() and result list of getaddrinfo() // stackoverflow URL: https://stackoverflow.com/questions/37757199/understanding-the-protocol-family-argument-of-socket-and-result-list-of-getadd (дата обращения: 04.12.2022).
 * How to get IP address from sockaddr // stackoverflow URL: https://stackoverflow.com/questions/1824279/how-to-get-ip-address-from-sockaddr (дата обращения: 04.12.2022).
 * Getting IPV4 address from a sockaddr structure // stackoverflow URL: https://stackoverflow.com/questions/1276294/getting-ipv4-address-from-a-sockaddr-structure (дата обращения: 04.12.2022).
 * Как решить ошибку «Temporary failure in name resolution» // ITISGOOD URL: https://itisgood.ru/2020/10/16/kak-reshit-oshibku-temporary-failure-in-name-resolution/ (дата обращения: 04.12.2022).
 * winsock compiling error, it cant find the addrinfo structures and some relating functions // stackoverflow URL: https://stackoverflow.com/questions/4243027/winsock-compiling-error-it-cant-find-the-addrinfo-structures-and-some-relating (дата обращения: 04.12.2022).
 * Почему утилита host делает UDP запрос на 127.0.0.1 на произвольные порты? // Хабр URL: https://qna.habr.com/q/1113270 (дата обращения: 04.12.2022).
 * Не могу присвоить ip адрес сокету // stackoverflow URL: https://ru.stackoverflow.com/questions/1010898/Не-могу-присвоить-ip-адрес-сокету (дата обращения: 04.12.2022).
 */