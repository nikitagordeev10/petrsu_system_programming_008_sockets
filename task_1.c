/*
 * task_1.c — программа для иллюстрации преобразования порядка байт:
 * 1) получает один обязательный аргумент командной строки - беззнаковое целое 32-битное число и сохраняет в переменной
 * 2) выводит содержимое ячеек памяти, в которых хранится эта переменная
 * 3) преобразовает число в сетевой порядок байт и сохраняет в другой переменной
 * 4) выводит содержимое ячеек памяти, в которых хранится новая переменная
 * 5) преобразовает получившееся число в презентационный формат (строка) и выводит
 * 
 * Copyright (c) <2022> <Гордеев Никита>
 * 
 * This code is licensed under a MIT-style license.
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>


int main(int argc, char *argv[]) {
    // Проверка наличия аргументов
    if (argc != 2) {
	    fprintf(stderr, "Использование: ./task_1 0x01020304\n");
        exit(EXIT_FAILURE);
    }

    // конверируем в строку и сохраняем в переменной беззнаковое целое 32-битное число
    uint32_t num = strtol(argv[1], NULL, 16); 

    // содержимое ячеек памяти, в которых хранится эта переменная
    printf("host byte order:\n");
    printf("s_addr=0x0%x, ", num);
    unsigned char *allCells = (unsigned char *) &num;
    for (int i = 0; i < 4; i++) {
        if (i < 3) printf("0x0%x:", allCells[i]);
        else printf("0x0%x\n", allCells[i]);
    }

    // преобразовает число в сетевой порядок байт и сохраняет в другой переменной
    uint32_t new_num = htonl(num);

    // содержимое ячеек памяти, в которых хранится новая переменная
    printf("network byte order:\n");
    printf("s_addr=0x0%x, ", new_num);
    unsigned char *allCells_new = (unsigned char *) &new_num;
    for (int i = 0; i < 4; i++) {
        if (i < 3) printf("0x0%x:", allCells_new[i]);
        else printf("0x0%x\n", allCells_new[i]);
    }

    // преобразование получившегося числа в презентационный формат (строка) и вывод
    printf("s=");
    for (int i = 0; i < 4; i++) {
        if (i < 3) printf("%x.", allCells_new[i]);
        else  printf("%x\n", allCells_new[i]);
    }
    return 0;
}


/*
 * Материалы:
 * Асинхронный веб, или Что такое веб-сокеты // Tproger URL: https://tproger.ru/translations/what-are-web-sockets/ (дата обращения: 04.12.2022).
 * Системное программирование, сокеты // Кафедра Информатики и Математического Обеспечения URL: https://cs.petrsu.ru/~vadim/sp2022/lab8.php.ru (дата обращения: 04.12.2022).
 * Глава 16.3.1. Порядок байтов // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Глава 16.3.2. Форматы адресов // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.1. Вывод сведений о хостах и сетевых службах // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Confused with network byte order and host byte order // stackoverflow URL: https://stackoverflow.com/questions/32205546/confused-with-network-byte-order-and-host-byte-order (дата обращения: 04.12.2022).
 * Сетевое программирование для разработчиков игр. Часть 2: прием и передача пакетов данных // Хабр URL: https://habr.com/ru/post/209524/ (дата обращения: 04.12.2022).
 * Unexpected output from strtol // stackoverflow URL: https://stackoverflow.com/questions/23639990/unexpected-output-from-strtol (дата обращения: 04.12.2022).
 * 
*/