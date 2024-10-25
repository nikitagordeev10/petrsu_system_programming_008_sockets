/*
 * task_4.c — TCP эхо-сервер:
 * 1) один обязательный аргумент командной строки - номер порта
 * 2) создать TCP-сокет, назначить ему полученный номер порта, принимать входящие соединения
 * 3) для каждого нового соединения:
 * 3.1) вывести информацию о том, откуда пришло соединение (IP-адрес и порт)
 * 3.2) создать процесс, который все полученные данные будет отправлять обратно
 * 4) после закрытия соединения порожденный процесс должен корректно завершаться (не оставаться в состоянии zombie)
 * 5) Для тестирования можно использовать утилиту netcat
 * 
 * Copyright (c) <2022> <Гордеев Никита>
 * 
 * This code is licensed under srvr MIT-style license.
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>

// Размер буфера для чтения
#define BUFFER_SIZE 1024

// Обработка нового соединения
void handleNewConnect(int);

// Ожидание завершения дочерних процессов
void sigchldHandler(int);

int main(int argc, char *argv[]) {

    // Проверка на наличие аргументов
    if (argc != 2) {
        printf("* \n* Инструкция:\n"
        "* 1) Разделите терминал на 2 командами Ctrl+Shift+5\n"
        "* 2) Выбите порт в диапазоне от 0 до 65535.\n"
        "* 3) Проверьте командой: sudo ss -tulpn | grep :{номер порта}, что выбранный Вами порт не занят системой\n"
        "* 4) Запустите программу в первом терминале командой: ./task_4 {номер порта}\n"
        "* 5) Отправьте запрос во втором терминале командой: echo {сообщение} | netcat -q1 localhost {номер порта}\n*\n");
        printf("Ознакомьтесь с инструкцией выше, пожалуйста\n");
        exit(EXIT_FAILURE);
    }
    
    // Сокращения
    // aip - Адаптивный интернет-протокол

    // Задаем настройки сокета
    struct addrinfo srvr, *aip; 
    srvr.ai_family = AF_INET; 
    srvr.ai_socktype = SOCK_STREAM;
    srvr.ai_flags = AI_PASSIVE;

    // получить адрес по имени хоста и сетевой службы
    if (getaddrinfo(NULL, argv[1], &srvr, &aip) != 0) {
        perror("Ошибка getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Создаем сокет
    int skt;
    if ((skt = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Ошибка функции socket");
        exit(EXIT_FAILURE);
    }

    // Привязываем к сокету адрес
    if (bind(skt, aip->ai_addr, aip->ai_addrlen) != 0) {
        perror("Ошибка bind");
        exit(EXIT_FAILURE);
    }

    // освобождаем память
    freeaddrinfo(aip);
    
    // Слушаем порт
    if (listen(skt, SOMAXCONN) != 0) {
        perror("Ошибка listen");
        exit(EXIT_FAILURE);
    }
    
    // Обработка нового соединения
    handleNewConnect(skt);
    return 0;
}

// Ждем завершения дочерних процессов
void sigchldHandler(int skt) {
    while (waitpid(0, NULL, WNOHANG) > 0);
}

// Обработка нового соединения
void handleNewConnect(int skt) {
    // Создаем структуру для вызова sigaction()
    struct sigaction child;
    sigset_t mask;
    
    // Потому что нельзя предполагать, что инициализация глобальных или статических переменных, выполняемая языком C, соответствует реализации сигналов в заданной системе
    if (sigemptyset(&mask) == -1) {
	    perror("Ошибка sigemptyset");
	    exit(EXIT_FAILURE);
    }

    child.sa_handler = sigchldHandler; 
    child.sa_mask = mask;
    
    // Указываем SA_RESTART, чтобы обработчик не ломал ожидание в вызове accept() 
    child.sa_flags = SA_RESTART;

    // Устанавливаем обработчик SIGCHLD
    if (sigaction(SIGCHLD, &child, NULL) == -1) {
        perror("Ошибка sigaction");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Заранее объявляем переменные
        struct sockaddr_in host;       
        int news;
        char buf[BUFFER_SIZE];
        char *ip;
        int port;

        // Ждем подключения клиента
        socklen_t host_size = sizeof(host);
        if ((news = accept(skt, (struct sockaddr *) &host, &host_size)) == -1) {
            perror("Ошибка accept");
            exit(EXIT_FAILURE);
        }

        // информация о том, откуда пришло соединение (IP-адрес и порт)
        port = ntohs(host.sin_port);
        ip = inet_ntoa(host.sin_addr);
        printf("ipv4 - %skt, port - %d\n", ip, port);
	
        // Создаем дочерний процесс для клиента
        switch (fork()) {
        case 0:
            if (close(skt) < 0) {
                perror("Ошибка close");
                exit(EXIT_FAILURE);
            }
            // Читаем данные от клиента
            int message = recv(news, buf, BUFFER_SIZE, 0);
            switch (message) {
                case 0:
                    // Если прочитано 0 байт - конец соединения
                    exit(0);
                case -1:
                    perror("Ошибка recv");
                    exit(EXIT_FAILURE);
                default:
                    // все полученные данные будет отправлять обратно
                    if (send(news, buf, message, 0) < 0) {
                        perror("Ошибка send");
                        exit(EXIT_FAILURE);
                    }
            }
            exit(0);
        case -1:
            perror("Ошибка fork");
            exit(EXIT_FAILURE);
        default:
            // порожденный процесс должен корректно завершаться
            if (close(news) < 0) {
                perror("Ошибка close");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/*
 * Материалы:
 * Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Глава 16 Межпроцессные взаимодействия в сети: сокеты // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.1. Вывод сведений о хостах и сетевых службах // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.4. Инициализация сокета для сервера // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.5. Клиент, получающий результат выполнения команды uptime // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.6. Сервер, возвращающий результат команды uptime по запросу клиента // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.7. Сервер, демонстрирующий запись вывода команды прямо в сокет // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.8. Клиент, использующий интерфейс дейтаграмм // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * Листинг 16.9. Сервер, который реализует службу uptime // Профессиональное программирование. 3-е изд.» Авторы: У. Ричард Стивенс, Стивен А. Раго Год: 2018 URL: https://www.rulit.me/data/programs/resources/pdf/UNIX-Professionalnoe-programmirovanie_RuLit_Me_609965.pdf (дата обращения: 04.12.2022).
 * v1ack / socket_echo_server // GitHub URL: https://github.com/v1ack/socket_echo_server (дата обращения: 04.12.2022).
 * «Boost.Asio C++ Network Programming». Глава 3: Эхо сервер/клиент // Хабр URL: https://habr.com/ru/post/195386/ (дата обращения: 04.12.2022).
 * Протокол TCP/IP или как работает Интернет (для чайников) // Интернет и компьютеры ПРОСТЫЕ РЕШЕНИЯ СЛОЖНЫХ ПРОБЛЕМ URL: http://www.ofnet.ru/osnovy-interneta/tcpip/ (дата обращения: 04.12.2022).
 * КАК ПОСМОТРЕТЬ ОТКРЫТЫЕ ПОРТЫ В LINUX // Losst URL: https://losst.pro/kak-posmotret-otkrytye-porty-v-linux (дата обращения: 04.12.2022).
 * Максимальное количество tcp-соединений для одного сервера и сводка // Русские Блоги URL: https://russianblogs.com/article/40331454441/ (дата обращения: 04.12.2022).
 * Killing child with SIGTERM // stackoverflow URL: https://stackoverflow.com/questions/51799781/killing-child-with-sigterm (дата обращения: 04.12.2022).
 * ntohl used on sin_port and get negative nubmers // stackoverflow URL: https://stackoverflow.com/questions/9630868/ntohl-used-on-sin-port-and-get-negative-nubmers (дата обращения: 04.12.2022).
 * Why is the client's file descriptor used while calling the recv and send funtions on both server and client sides? // stackoverflow URL: https://stackoverflow.com/questions/64385627/why-is-the-clients-file-descriptor-used-while-calling-the-recv-and-send-funtion (дата обращения: 04.12.2022).
*/

