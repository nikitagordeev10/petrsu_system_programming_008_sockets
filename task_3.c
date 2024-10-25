/*
 * task_3.c — программа для получения имени хоста по IP-адресу (если это возможно)
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
#include <string.h>

int main(int argc, char *argv[]) {
    // Проверка наличия аргументов
    if (argc != 2) {
	    fprintf(stderr, "Использование: ./task_3 8.8.8.8\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sinp;
    char abuf[NI_MAXHOST];

    sinp.sin_family = AF_INET;

    // Преобразуем строку в адрес
    if (inet_pton(AF_INET, argv[1], &sinp.sin_addr) != 1) {
        printf("Ошибка функции inet_pton!\n");
        exit(EXIT_FAILURE);
    }

    // Получаем имя хоста
    if (getnameinfo((struct sockaddr *) &sinp, sizeof(sinp), abuf, sizeof(abuf), NULL, 0, 0) != 0) {
        printf("Ошибка вызова функции getnameinfo!\n");
        exit(EXIT_FAILURE);
    }

    // Печатаем имя хоста
    if (strcmp(argv[1], abuf) == 0) printf("Host %s.in-addr.arpa. not found\n", abuf);
    else printf("%s.in-addr.arpa domain name pointer %s\n", argv[1], abuf);
    return 0;
}

/* 
 * Материалы:
 * Системное программирование, сокеты // Кафедра Информатики и Математического Обеспечения URL: https://cs.petrsu.ru/~vadim/sp2022/lab8.php.ru (дата обращения: 04.12.2022).
 * Глава 16.3.3. Определение адреса // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.1. Вывод сведений о хостах и сетевых службах // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * host (1) // Интерактивная система просмотра системных руководств (man-ов) URL: https://www.opennet.ru/man.shtml?topic=host&category=1&russian=1 (дата обращения: 04.12.2022).
 * getnameinfo (3) // Интерактивная система просмотра системных руководств (man-ов) URL: https://www.opennet.ru/man.shtml?topic=getnameinfo&category=3&russian=0 (дата обращения: 04.12.2022).
 */