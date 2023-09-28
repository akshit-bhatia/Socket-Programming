/*
    Akshit Bhatia
    110093528
    Section 4 (Thursday -Evening)
*/

#include <stdio.h>      // sstandrd i/0
#include <stdlib.h>     // sstandrd 1ib
#include <string.h>     // string 1ib
#include <unistd.h>     // f0rk
#include <sys/socket.h> // s0cket
#include <arpa/inet.h>  // internet operations (ip, port)
#include <time.h>       // manuplating time
#include <stdbool.h>    // b00l lib

#define AKKI_CONECTION_ADRES "127.0.0.1" // local address to connect to servr / miror
#define AKKI_SRVR_PRT 8080               // servr prt num
#define AKKI_MIROR_PRT 8081              // miror prt num

int untar_flg = 0;                                  // flg weather to extract the tar file or not
char *allValidExtens[] = {"c", "txt", "pdf", "sh"}; // all the valid extns

// remov blank spaces from the cmnd
char *removeBlankSpaces(char *cmnd)
{
    int indxOfLastSpc = -1;                     // indx of last occurance of spaace
    for (int i = strlen(cmnd) - 1; i >= 0; i--) // iterating from laast char
    {
        if (cmnd[i] == ' ') // if spaace is encountered, save that indx
        {
            indxOfLastSpc = i;
            break;
        }
    }

    if (indxOfLastSpc == -1)
    {
        // return original cmnd
        return strdup(cmnd);
    }

    // new string holding truncated part
    char *result = (char *)malloc(indxOfLastSpc + 2); // 1 for char up to last space and 1 for null
    strncpy(result, cmnd, indxOfLastSpc + 1);         // copy till last space
    result[indxOfLastSpc + 1] = '\0';                 // ading null

    return result;
}

// checking for file namee
int fileNameCheckerr(char *akki_tkn)
{
    int in_dx = 0;                              // indx
    char *str_aftr_dot = strchr(akki_tkn, '.'); // st0re string after 1'st 0ccurance 0f . in akki_tkn
    if (str_aftr_dot != NULL)
    {
        in_dx = str_aftr_dot - akki_tkn; // in_dx using pointr arithmeticc
    }
    return in_dx;
}

// check weather extn exist in allValidExtens arrayy
int extnCheckerr(char *extnsn)
{
    int i, fndFlg = 0; // flg which tells weather extn is valid or not

    for (i = 0; i < sizeof(allValidExtens) / sizeof(allValidExtens[0]); i++)
    {
        // if extn exist in array
        if (strcmp(allValidExtens[i], extnsn) == 0)
        {
            // flg is set too 1
            fndFlg = 1;
            break;
        }
    }
    return fndFlg;
}

// funcn which va1idates the datee
bool dateeValid_atorr(int a_year, int a_mnth, int a_day)
{
    // checkker for wrong inputt
    if (a_day < 1 || a_day > 31 || a_mnth < 1 || a_mnth > 12 || a_year < 1)
        return false;

    // no.of dates as per monthss
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // leap a_year consideratorr
    if (a_year % 4 == 0)
        daysInMonth[2] = 29;

    // returnn turu of falsee as per days in montth
    return a_day <= daysInMonth[a_mnth];
}

// cmnd validator
int cmnd_valid_atorr(char str[])
{
    char *akki_cmnd = strdup(str);           // create a copy of str
    char *akki_tkn = strtok(akki_cmnd, " "); // extractinhg the 1'st word
    if (strcmp(akki_tkn, "fgets") == 0)
    {
        if (untar_flg == 1) // check for unzip c0mnd
        {
            untar_flg = 0; // set flg to 0
            printf("\nCommand error: 'fgets' cannot be used with '-u'\n");
            free(akki_cmnd);
            return 0;
        }
        // Move to the next token
        akki_tkn = strtok(NULL, " ");
        int fget_files = 0;
        // walk through other tokens
        while (akki_tkn != NULL)
        {
            char *token1 = strdup(akki_tkn); // create a copy of akki_tkn to check weather file name and extnsn are valid
            int aki_indx_of_dot_extn = 0;    // in_dx of . in string
            aki_indx_of_dot_extn = fileNameCheckerr(token1);
            // Check if there are more tokens in the string
            if (token1 == NULL || aki_indx_of_dot_extn == 0)
            {
                printf("Command error: No filename / extnsn provided.\n");
                return 0;
            }

            char *extnsn = strrchr(token1, '.'); // Find the last occurrence of '.'
            if (extnsn != NULL)
            {
                extnsn++;                      // Move past the '.' character
                int fndFlg = 0;                // f1g to st0re weather extnsn is right 0r n0t
                fndFlg = extnCheckerr(extnsn); // check f0r extnsn

                if (fndFlg == 0) // wr0ng extnsn
                {
                    printf("Extension not found: %s of file %s \n", extnsn, token1);
                    return 0;
                }
            }
            else // n0 extnsn pr0vided
            {
                printf("Command error: Invalid format.\n");
                return 0;
            }

            fget_files++;                 // f0und fi1e
            akki_tkn = strtok(NULL, " "); // next t0ken
        }
        if (fget_files > 0 && fget_files <= 4) // f0und fi1e
            return 1;
        else // n0 fi1es f0und
            return 0;
    }

    else if (strcmp(akki_tkn, "tarfgetz") == 0)
    {
        // sze 0f fi1e 1 and 2, countter
        int fle1sze = 0, fle2sze = 0, cnt_r = 0;
        char *newStr = strdup(str);         // cc0py 0f c0mnd
        char *token1 = strtok(newStr, " "); // extract c0mnd
        token1 = strtok(NULL, " ");         // extract size1
        while (token1 != NULL)
        {
            if (cnt_r == 0) // cnt_r == 0, enc0untered size1
            {
                fle1sze = atoi(token1); // c0nvrt to int
            }
            else if (cnt_r == 1) // cnt_r == 0, enc0untered size2
            {
                fle2sze = atoi(token1); // c0nvrt to int
            }
            else
            {
                // m0re than 2 size in c0mnd
                return 0;
            }

            cnt_r++;                    // increase c0uunter
            token1 = strtok(NULL, " "); // extract next size
        }

        if (cnt_r == 2 && fle1sze > 0 && fle2sze > 0) // if cnt_r == 2 and flesze is greater than 0
        {
            // Compare size and ckecking f0r nuLL
            if (fle1sze >= 0 && fle2sze >= 0 && strtok(NULL, " ") == NULL)
            {
                if (fle1sze <= fle2sze) // fle2sze sh0u1d be greater
                {
                    return 1;
                }
            }
        }
        return 0;
    }

    else if (strcmp(akki_tkn, "filesrch") == 0)
    {
        if (untar_flg == 1) // check f0r unzip c0mnd
        {
            untar_flg = 0; // set flg to 0
            printf("\nCommand error: 'filesrch' cannot be used with '-u'\n");
            free(akki_cmnd);
            return 0;
        }

        akki_tkn = strtok(NULL, " "); // Move to the next token
        int aki_indx_of_dot_extn = 0; // in_dx of . in string

        int fndFlg = 0; // flg t0 check extnsn

        aki_indx_of_dot_extn = fileNameCheckerr(akki_tkn); // check fi1e name

        // Check if 0nly 1 name is pr0vided and extnsn is pr0vided
        if (akki_tkn == NULL || aki_indx_of_dot_extn == 0)
        {
            printf("Command error: No filename provided.\n");
            free(akki_cmnd);
            return 0;
        }

        char *extnsn = strrchr(akki_tkn, '.'); // Find 1ast 0ccurrence of '.'
        if (extnsn != NULL)
        {
            extnsn++;                      // Move past the '.' character
            fndFlg = extnCheckerr(extnsn); // check extnsn va1id 0r n0t
        }
        else
        {
            printf("Command error: Invalid filename format.\n");
            free(akki_cmnd);
            return 0;
        }

        // if 0nly 1 fi1e name in c0mnd
        if (strtok(NULL, " ") == NULL)
        {
            free(akki_cmnd);
            return fndFlg;
        }
        // return err0r
        else
        {
            free(akki_cmnd);
            return 0;
        }
    }

    else if (strcmp(akki_tkn, "targzf") == 0)
    {
        // 4 extnsn checks
        char *newStr = strdup(str);         // create a copy of command
        char *token1 = strtok(newStr, " "); // extracting the command again
        token1 = strtok(NULL, " ");         // extracting the first extnsn
        int targzf_files = 0, fndFlg = 0;   // exrnsn names f0und and flag
        // iterrate 0ver 0ther t0ken
        while (token1 != NULL)
        {
            targzf_files++;                         // f0und extnsn name
            fndFlg = fndFlg + extnCheckerr(token1); // check for va1id extnsn

            token1 = strtok(NULL, " "); // extracting the other extensions
        }

        if (targzf_files > 0 && fndFlg > 0 && targzf_files <= 4 && fndFlg <= 4 && targzf_files == fndFlg) // fi1es and extns are va1id
            return 1;
        else
            return 0;
    }

    else if (strcmp(akki_tkn, "getdirf") == 0)
    {
        char *newStr = strdup(str);         // copy of command
        char *token1 = strtok(newStr, " "); // c0mnd
        token1 = strtok(NULL, " ");         // extract 1'st date
        int cnt_r = 0;                      // c0unterr
        char *date_1, *date_2;
        while (token1 != NULL)
        {
            if (cnt_r == 0)
            {
                date_1 = token1; // copy date1
            }
            else if (cnt_r == 1)
            {
                date_2 = token1; // copy date2
            }
            else
            {
                // more than 2 date in command, return 0
                return 0;
            }
            cnt_r++;                    // increase cnt_r
            token1 = strtok(NULL, " "); // extract next date
        }

        if (cnt_r == 2) // t0tal 2 dates
        {
            char date_copy1[11]; // c0py 0f date 1
            char *token_1;

            int year_1, month_1, day_1;

            // C0py date_1 -> date_copy1
            strncpy(date_copy1, date_1, sizeof(date_copy1) - 1);
            date_copy1[sizeof(date_copy1) - 1] = '\0'; // Ensureingg null-termination

            // Breakimg 0n '-'
            token_1 = strtok(date_copy1, "-");
            if (token_1 != NULL)
            {
                year_1 = atoi(token_1);      // c0py year
                token_1 = strtok(NULL, "-"); // extract m0nth
            }
            if (token_1 != NULL)
            {
                month_1 = atoi(token_1);     // c0py m0nth
                token_1 = strtok(NULL, "-"); // extract date
            }
            if (token_1 != NULL)
            {
                day_1 = atoi(token_1); // c0py day
            }

            bool dateFormat1 = dateeValid_atorr(year_1, month_1, day_1); // va1idate date

            char date_copy2[11]; // c0py 0f date 2
            char *token2;

            int year_2, month_2, day_2;

            // C0py date_2 -> date_copy2
            strncpy(date_copy2, date_2, sizeof(date_copy2) - 1);
            date_copy2[sizeof(date_copy2) - 1] = '\0'; // Ensureingg null-termination

            token2 = strtok(date_copy2, "-"); // Breakimg 0n '-'
            if (token2 != NULL)
            {
                year_2 = atoi(token2);      // c0py year
                token2 = strtok(NULL, "-"); // extract m0nth
            }
            if (token2 != NULL)
            {
                month_2 = atoi(token2);     // c0py m0nth
                token2 = strtok(NULL, "-"); // extract date
            }
            if (token2 != NULL)
            {
                day_2 = atoi(token2); // c0py day
            }

            bool dateFormat2 = dateeValid_atorr(year_2, month_2, day_2); // va1idate date

            if (dateFormat1 == 0 || dateFormat2 == 0) // wr0nf f0rmat
            {
                return 0;
            }
            else
            {
                if (year_2 > year_1)
                {
                    return 1;
                }
                else if (year_2 == year_1)
                {
                    if (month_2 > month_1)
                    {
                        return 1;
                    }
                    else if (month_2 == month_1)
                    {
                        if (day_2 >= day_1)
                        {
                            return 1;
                        }
                        else // day 0f date 2 is sma11
                            return 0;
                    }
                    else // m0nth 0f date 2 is sma11
                        return 0;
                }
                else // year 0f date 2 is sma11
                    return 0;
            }
        }
        else // 1ess than 2 0r m0re than 2 dates pr0vided
        {
            return 0;
        }
    }

    else if (strcmp(akki_tkn, "quit") == 0) // if cmnd is quit
    {
        if (untar_flg == 1) // unzip flag is presant in cmnd
        {
            untar_flg = 0; // set f1ag to zeer0
            printf("\nCommand error: 'Quit' cannot be used with '-u'\n");
            free(akki_cmnd); // clear the mem0ry
            return 0;
        }
        free(akki_cmnd);
        return 1;
    }

    else
    {
        free(akki_cmnd);
        return 0;
    }
}

int main()
{
    int akki_clnt_scket;                   // s0ckets file descript0r
    struct sockaddr_in connection_address; // server adres inf0rmation -> struct
    char akki_buff[1024];                  // buffer to h0ld the resp0nse

    // create socket
    akki_clnt_scket = socket(AF_INET, SOCK_STREAM, 0); // this wi11 create a TCP s0cket
    if (akki_clnt_scket < 0)                           // check f0r any err0r
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE); // exit c1ient
    }

    // set server address
    memset(&connection_address, 0, sizeof(connection_address));           // setting mem0ry -> 0 for the server adres struct
    connection_address.sin_family = AF_INET;                              // setting adres family -> IPv4
    connection_address.sin_addr.s_addr = inet_addr(AKKI_CONECTION_ADRES); // setting i.p. adres of -> server
    connection_address.sin_port = htons(AKKI_SRVR_PRT);                   // setting the p0rt number of -> server

    // try t0 c0nect t0 server
    if (connect(akki_clnt_scket, (struct sockaddr *)&connection_address, sizeof(connection_address)) < 0)
    {
        perror("connection to server failed");

        // n0w try t0 c0nect to mirr0r

        //  creati0n 0f s0cket
        akki_clnt_scket = socket(AF_INET, SOCK_STREAM, 0); // this wi11 create a TCP s0cket
        if (akki_clnt_scket < 0)                           // check f0r any err0r
        {
            perror("socket creation failed");
            exit(EXIT_FAILURE); // exit c1ient
        }

        // set mirr0r address
        memset(&connection_address, 0, sizeof(connection_address));           // setting mem0ry -> 0 for the mirr0r adres struct
        connection_address.sin_family = AF_INET;                              // setting adres family -> IPv4
        connection_address.sin_addr.s_addr = inet_addr(AKKI_CONECTION_ADRES); // setting i.p. adres of -> mirr0r
        connection_address.sin_port = htons(AKKI_MIROR_PRT);                  // setting the p0rt number of -> mirr0r

        // connect to mirr0r
        if (connect(akki_clnt_scket, (struct sockaddr *)&connection_address, sizeof(connection_address)) < 0)
        {
            perror("connection to mirror failed");
            exit(EXIT_FAILURE); // exit c1ient
        }
        else
        {
            printf("Connection Established with Mirror...\n");
        }
    }
    else
    {
        read(akki_clnt_scket, akki_buff, 1024); // read resp0nse fr0m server and st0re resp0nse in akki_buff
        if (strcmp(akki_buff, "success") != 0)  // if resp0nse is 0ther than success, try t0 c0nnect to imirr0r
        {
            printf("Connecting to Mirror...\n\n");
            close(akki_clnt_scket); // cl0se ear1ier TCP s0cket

            //  creati0n 0f s0cket
            akki_clnt_scket = socket(AF_INET, SOCK_STREAM, 0); // this wi11 create a TCP s0cket
            if (akki_clnt_scket < 0)                           // check f0r any err0r
            {
                perror("socket creation failed");
                exit(EXIT_FAILURE); // exit c1ient
            }

            // set mirr0r address
            memset(&connection_address, 0, sizeof(connection_address));           // setting mem0ry -> 0 for the mirr0r adres struct
            connection_address.sin_family = AF_INET;                              // setting adres family -> IPv4
            connection_address.sin_addr.s_addr = inet_addr(AKKI_CONECTION_ADRES); // setting i.p. adres of -> mirr0r
            connection_address.sin_port = htons(AKKI_MIROR_PRT);                  // setting the p0rt number of -> mirr0r

            // connect to mirr0r
            if (connect(akki_clnt_scket, (struct sockaddr *)&connection_address, sizeof(connection_address)) < 0)
            {
                perror("connection to mirror failed");
                exit(EXIT_FAILURE); // exit c1ient
            }
            else
            {
                printf("Connection Established with Mirror...\n");
            }
        }
        else
        {
            printf("Connection Established with Server...\n");
        }
    }

    while (1) // l0op indefinate1y
    {
        printf("\nEnter command: ");
        char akki_c0mandd[1024];
        fgets(akki_c0mandd, sizeof(akki_c0mandd), stdin); // read c0mand from userr

        if (strstr(akki_c0mandd, "-u") != NULL) // check if -u exist in c0mand
        {
            untar_flg = 1;                                                                  // change f1ag t0 1
            if (strlen(akki_c0mandd) > 0 && akki_c0mandd[strlen(akki_c0mandd) - 1] == '\n') // check 1ast char is \n 0r n0t
                akki_c0mandd[strlen(akki_c0mandd) - 1] = '\0';                              // change 1ast char to nu11

            strcpy(akki_c0mandd, removeBlankSpaces(akki_c0mandd)); // c0py the channges
        }

        if (strcmp(akki_c0mandd, "\n") == 0) // if n0 cmnd and user press enter
            strcpy(akki_c0mandd, "filesrch\n");

        if (strcmp(akki_c0mandd, "filesrch\n") != 0)
        {
            if (akki_c0mandd[strlen(akki_c0mandd) - 1] == '\n') // if 1ast char is a newline i.e. \n, make it nu11
                akki_c0mandd[strlen(akki_c0mandd) - 1] = '\0';
        }

        int cmnd_chkr_flg = -1;                         // cmnd checkker f1ag
        cmnd_chkr_flg = cmnd_valid_atorr(akki_c0mandd); // check if c0mand is va1id

        if (cmnd_chkr_flg == 0) // err0r in cmnd
            printf("\nPlease enter the correct command. \n");
        else
        {
            // snd c0mand to server / mirr0r
            if (send(akki_clnt_scket, akki_c0mandd, strlen(akki_c0mandd), 0) < 0)
            {
                perror("send failed");
                exit(EXIT_FAILURE); // exit c1ient
            }

            // receive response from server
            char akki_clnt_srvr_response[1024];
            memset(&akki_clnt_srvr_response, 0, sizeof(akki_clnt_srvr_response)); // set memory to 0 for the response

            if (strstr(akki_c0mandd, "quit") || strstr(akki_c0mandd, "filesrch "))
            {
                if (recv(akki_clnt_scket, akki_clnt_srvr_response, sizeof(akki_clnt_srvr_response), 0) < 0) // receive the response from the server
                {
                    perror("receive failed");
                    exit(EXIT_FAILURE);
                }

                // check if response is error message
                if (strcmp(akki_clnt_srvr_response, "ERROR") == 0)
                {
                    printf("Please enter correct command.\n");
                    continue; // g0 t0 start 0f the l0op
                }

                if (strcmp(akki_c0mandd, "quit") == 0) // exit
                {
                    printf("Exiting client...\n");
                    close(akki_clnt_scket); // cl0sing s0cket
                    exit(0);
                }

                // print response
                printf("%s", akki_clnt_srvr_response);
            }

            else
            {
                // h0w many bytes of data t0 receive
                long file_size;
                recv(akki_clnt_scket, &akki_clnt_srvr_response, sizeof(akki_clnt_srvr_response), 0);

                // c0nvertiing string resp0nse to l0ng
                if (sscanf(akki_clnt_srvr_response, "%ld", &file_size) == 1)
                {
                    // creatting tar at c1ient side
                    FILE *akki_tar_file_desc_riptor = fopen("download.tar.gz", "wb");
                    if (akki_tar_file_desc_riptor == NULL)
                    {
                        printf("Error creating tar file.\n");
                        return -1;
                    }

                    long total_bytes_received = 0; // t0tal bytes received
                    // get fi1e from server / mirr0r unti1 total_bytes_received is grreater than file_size
                    while (total_bytes_received < file_size)
                    {
                        // check f0r resp0nse
                        int bytes_received = recv(akki_clnt_scket, akki_clnt_srvr_response, 1024, 0);
                        if (bytes_received < 0) // if resp0nse is less than zeeroo
                        {
                            printf("Error receiving file data");
                            return -1;
                        }

                        // write data t0 tar fi1e
                        fwrite(akki_clnt_srvr_response, sizeof(char), bytes_received, akki_tar_file_desc_riptor);
                        total_bytes_received += bytes_received; // update total_bytes_received
                        if (total_bytes_received >= file_size)  // if total_bytes_received is greatterr than file_size, break
                        {
                            break;
                        }
                    }

                    // update resp0nse
                    strcpy(akki_clnt_srvr_response, "File saved in download.tar.gz");
                    printf("File saved in download.tar.gz \n");
                    fclose(akki_tar_file_desc_riptor); // c1ose fi1e descript0r
                }
                else // n0 fi1e found / any err0r
                {
                    printf("%s", akki_clnt_srvr_response);
                }
            }

            // extract tar file if option is specified
            if (untar_flg == 1)
            {
                untar_flg = 0;                                          // update f1g to zerro
                if (strstr(akki_clnt_srvr_response, "download.tar.gz")) // check resp0nse
                {
                    char unzipCommand[256];
                    sprintf(unzipCommand, "tar -xzf download.tar.gz"); // unzip
                    system(unzipCommand);
                    printf("Tar file is unzipped.\n");
                }
            }
        }
    }
    // close socket
    close(akki_clnt_scket);
    return 0; // exit
}