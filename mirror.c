/*
    Akshit Bhatia
    110093528
    Section 4 (Thursday -Evening)
*/

#define _XOPEN_SOURCE 500 // nftw function
#include <stdio.h>        // sstandrd i/0
#include <stdlib.h>       // sstandrd 1ib
#include <unistd.h>       // fork
#include <string.h>       // string 1ib
#include <sys/wait.h>     // waitpid
#include <sys/socket.h>   // s0ckets
#include <netinet/in.h>   // htons, ip adres
#include <time.h>         // mktime
#include <ftw.h>          // nftw
#include <strings.h>      // bzer0
#include <fcntl.h>        // fi1e read/write

#define AKKI_MIROR_PRT 8081 // miror wi11 1isten 0n this p0rt

// fileSrch
static char *fileSrch_fileName; // Dec1are fileSrch_fileName as a gl0bal char
struct stat fileSrch_fileStat;  // Dec1are a gl0bal struct stat

// getDirf
char get_dirf_date_1[11], get_dirf_date_2[11]; // date h0lders
int get_dirf_files_found = 0;                  // fi1es f0und cnt
char get_dirf_file_names[100][256];            // max 100 fi1e names
char get_dirf_file_path[100][256];             // max 100 fi1e paths

// targfz
int targzf_cnt = 0;                                                               // fi1es f0und cnt
char targzf_extn_names[4][128];                                                   // max 4 extn a11owed
int targzf_extn_count = sizeof(targzf_extn_names) / sizeof(targzf_extn_names[0]); // size 0f array
int targzf_unique_file_names_count = 0;                                           // unique fi1es c0unt
char **targzf_unique_File_Names = NULL;                                           // unique fi1es names array
char **targzf_unique_File_Path = NULL;                                            // unique fi1es path array

// tarfgetz
int tarfgetz_size_1 = 0;                  // sze1
int tarfgetz_size_2 = 0;                  // sze 2
int tarfgetz_file_cnter = 0;              // fi1es c0unter
char **tarfgetz_unique_file_names = NULL; // unique fi1es names array
char **tarfgetz_unique_file_path = NULL;  // unique fi1es path array

// nftw functti0n f0r fi1e srch
static int file_Search_nftw_callback(const char *file_path, const struct stat *file_stat, int flag, struct FTW *ftwbuf)
{
    if (flag == FTW_F) // regu1ar fi1e
    {
        if (strcmp(file_path + ftwbuf->base, fileSrch_fileName) == 0) // c0mpare iterated fi1ename and fileSrch_fileName
        {
            fileSrch_fileStat = *file_stat; // C0py current file_stat structure to the global fileStat
            return 1;
        }
    }

    return 0; // C0ntinue
}

// functti0n returningg fi1e f0und (1) 0r n0t (0)
int file_Search(char *search_filename, char *root_direc, struct stat *filestat)
{
    fileSrch_fileName = search_filename;                                 // Set the global fileSrch_fileName variable
    if (nftw(root_direc, file_Search_nftw_callback, 10, FTW_PHYS) == -1) // FTW_PHYS prevent f0110wing symb01ic 1inks i.e. a1ias / sh0rtcuts
    {
        perror("nftw");
        return 0; // File not found
    }

    // S_ISREG is a macro that takes a file mode value and checks whether it corresponds to a regular file or not
    if (S_ISREG(fileSrch_fileStat.st_mode))
    {
        *filestat = fileSrch_fileStat; // Copy the fileSrch_fileStat to the provided filestat parameter
        return 1;                      // File found
    }

    return 0; // File not found
}

// read and send tar t0 c1ient
int sendTarToClient(int sockfd)
{
    long int akki_tar_file_size;                     // tar size
    FILE *akki_tar_f_p = fopen("temp.tar.gz", "rb"); // 0pen tar
    if (akki_tar_f_p == NULL)                        // err0r
    {
        fprintf(stderr, "Error opening file\n");
        return -1;
    }
    fseek(akki_tar_f_p, 0, SEEK_END);         // seek t0 end
    akki_tar_file_size = ftell(akki_tar_f_p); // get size
    fseek(akki_tar_f_p, 0, SEEK_SET);         // seek t0 start

    char akki_tar_file_size_char[1024];
    sprintf(akki_tar_file_size_char, "%ld", akki_tar_file_size);               // akki_tar_file_size in char
    send(sockfd, akki_tar_file_size_char, sizeof(akki_tar_file_size_char), 0); // sending tar size
    fclose(akki_tar_f_p);                                                      // close tar

    // again 0peing tar
    int akki_tar_f_d = open("temp.tar.gz", O_RDONLY);
    if (akki_tar_f_d < 0) // err0r
    {
        perror("open failed");
        return -1;
    }

    char akki_bufer[1024]; // buf t0 st0re tar data
    int tar_byte_read;     // n0.0f byte read

    while ((tar_byte_read = read(akki_tar_f_d, akki_bufer, 1024)) > 0)
    {
        send(sockfd, akki_bufer, tar_byte_read, 0); // send t0 c1ient
    }

    memset(akki_bufer, 0, sizeof(akki_bufer)); // set buf to 0
    close(akki_tar_f_d);                       // c10se fi1e
    return 0;
}

// nftw f0r getDirf cmnd
int get_dirf_nftw_call_back(const char *filePath, const struct stat *fileStat, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F || typeflag == FTW_D)
    {
        char *fileName = strrchr(filePath, '/');
        if (fileName != NULL)
        {
            fileName++; // Move past the slash

            struct tm fileTime;
            strptime(get_dirf_date_1, "%Y-%m-%d", &fileTime);
            time_t date1Timestamp = mktime(&fileTime);

            strptime(get_dirf_date_2, "%Y-%m-%d", &fileTime);
            time_t date2Timestamp = mktime(&fileTime);

            if (fileStat->st_mtime >= date1Timestamp && fileStat->st_mtime <= date2Timestamp)
            {
                if (get_dirf_files_found < 100)
                {
                    int isDup = 0;
                    for (int i = 0; i < get_dirf_files_found; i++)
                    {
                        if (strcmp(get_dirf_file_names[i], fileName) == 0)
                        {
                            isDup = 1;
                            break;
                        }
                    }
                    if (isDup == 0)
                    {
                        strcpy(get_dirf_file_names[get_dirf_files_found], fileName);
                        strcpy(get_dirf_file_path[get_dirf_files_found], filePath);
                        get_dirf_files_found++;
                    }
                }
            }
        }
    }
    return 0;
}

// extn exist in targzf_extn_names 0r n0t
int targzf_extn_valid_ator(const char *filename)
{
    const char *fileExt = strrchr(filename, '.'); // extn after .
    if (fileExt != NULL)
    {
        for (int i = 0; i < targzf_extn_count; i++)
        {
            if (strcmp(fileExt, targzf_extn_names[i]) == 0) // checking extn in targzf_extn_names
            {
                return 1;
            }
        }
    }
    return 0;
}

// fi1e name exist in targzf_unique_File_Names or not
int targzf_file_name_check(const char *targzf_file_name)
{
    for (int j = 0; j < targzf_unique_file_names_count; j++)
    {
        if (strcmp(targzf_file_name, targzf_unique_File_Names[j]) == 0)
        {
            return 1; // duplicate
        }
    }
    return 0;
}

// nftw callback f0r targzf functi0n
int targzf_nftw_call_back(const char *targzf_file_path, const struct stat *file_info, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F && targzf_extn_valid_ator(targzf_file_path)) // fi1e and extn 0f fi1e check
    {
        const char *targzf_file_name = strrchr(targzf_file_path, '/'); // extract fi1e name
        if (targzf_file_name == NULL)
        {
            targzf_file_name = targzf_file_path; // use whole path
        }
        else
        {
            targzf_file_name++; // filename
        }

        if (targzf_file_name_check(targzf_file_name))
        {
            return 0; // Not a unique file name
        }

        targzf_unique_File_Path = realloc(targzf_unique_File_Path, (targzf_unique_file_names_count + 1) * sizeof(char *)); // a11ocate mem for filePath
        targzf_unique_File_Path[targzf_unique_file_names_count] = strdup(targzf_file_path);                                // c0py fi1e path t0 targzf_unique_File_Path

        targzf_unique_File_Names = realloc(targzf_unique_File_Names, (targzf_unique_file_names_count + 1) * sizeof(char *)); // a11ocate mem for fileName
        targzf_unique_File_Names[targzf_unique_file_names_count] = strdup(targzf_file_name);                                 // c0py fi1e name t0 targzf_unique_File_Names

        targzf_unique_file_names_count++; // inc. counter
    }
    return 0;
}

// fi1e name comparer
int get_dirf_file_comparer(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

// is file name unique
int tarfgetz_file_name_checkker(const char *file_name)
{
    for (int j = 0; j < tarfgetz_file_cnter; j++)
    {
        if (strcmp(file_name, tarfgetz_unique_file_names[j]) == 0)
        {
            return 1; // Already added to targzf_unique_File_Names
        }
    }
    return 0;
}

// nftw functi0n for tarfgetz
int tarfgetz_nftw_call_back(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F && sb->st_size > tarfgetz_size_1 && sb->st_size < tarfgetz_size_2)
    {
        const char *file_name = strrchr(file_path, '/');
        if (file_name == NULL)
        {
            file_name = file_path; // No '/' found, use the whole path as the file name
        }
        else
        {
            file_name++; // Skip the '/' and use the remaining part of the path as the file name
        }

        if (tarfgetz_file_name_checkker(file_name))
        {
            return 0; // Not a unique file name
        }

        tarfgetz_unique_file_names = realloc(tarfgetz_unique_file_names, (tarfgetz_file_cnter + 1) * sizeof(char *));
        tarfgetz_unique_file_names[tarfgetz_file_cnter] = strdup(file_name);

        tarfgetz_unique_file_path = realloc(tarfgetz_unique_file_path, (tarfgetz_file_cnter + 1) * sizeof(char *));
        tarfgetz_unique_file_path[tarfgetz_file_cnter] = strdup(file_path);

        tarfgetz_file_cnter++;
    }
    return 0;
}

void processclient(int sockfd)
{
    char akki_bufer[256]; // bufer t0 st0re resp0nse
    int n;                // 1ength 0f resp0nse

    while (1)
    {
        bzero(akki_bufer, sizeof(akki_bufer));                // c1ear akki_bufer
        n = read(sockfd, akki_bufer, sizeof(akki_bufer) - 1); // read c0mand fr0m c1ient
        if (n <= 0)                                           // 1ength 0f c0mand is 1ess than 0
        {
            perror("Error reading from socket");
            break;
        }

        printf("Command received from client: %s\n", akki_bufer);

        char akki_srvr_response[1024] = "Command received.\n";

        if (strcmp(akki_bufer, "quit") == 0) // quit
        {
            printf("Client requested to quit.\n");
            break;
        }

        else if (strncmp(akki_bufer, "filesrch ", 9) == 0)
        {
            char *cmnd = strtok(akki_bufer, " ");               // cmnd
            char *cmnd_file_srch_file_name = strtok(NULL, " "); // filename and extn
            struct stat filestat;
            memset(&fileSrch_fileStat, 0, sizeof(fileSrch_fileStat)); // settingg all fields to zero

            // search f0r fi1e at ~
            if (file_Search(cmnd_file_srch_file_name, getenv("HOME"), &filestat) != 0)
            {
                // f0rmat date resp0nse
                char file_date_mod[20] = "";
                strftime(file_date_mod, sizeof(file_date_mod), "%Y-%m-%d %H:%M:%S", localtime(&filestat.st_mtime));
                sprintf(akki_srvr_response, "File Name: %s | File Size: %lld bytes | Date Modified: %s\n", cmnd_file_srch_file_name, (long long)filestat.st_size, file_date_mod);

                // sending resp0nse t0 c1ient
                n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                if (n < 0)
                {
                    perror("Error writing to socket");
                    break;
                }
            }

            else
            {
                sprintf(akki_srvr_response, "File Not Found");
                // sending "FNF" resp0nse to c1ient
                n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                if (n < 0)
                {
                    perror("Error writing to socket");
                    break;
                }
            }
        }

        else if (strncmp(akki_bufer, "fgets ", 6) == 0)
        {
            char fgets_files_to_find[4][256];          // Array t0 st0re fi1es name and path
            int fgets_files_found_cnt = 0;             // f0und fi1es count
            char *cmnd = strtok(akki_bufer, " ");      // cmnd
            char *fgets_file_name = strtok(NULL, " "); // 1'st fi1ename
            while (fgets_file_name != NULL)
            {
                strcpy(fgets_files_to_find[fgets_files_found_cnt], fgets_file_name); // c0py fi1e name to fgets_files_to_find array
                fgets_files_found_cnt++;                                             // increase cnter
                fgets_file_name = strtok(NULL, " ");                                 // next fi1ename
            }

            int fgets_file_found = 0; // numb. 0f fi1es f0und

            // Search f0r each fi1e
            for (int i = 0; i < fgets_files_found_cnt; i++)
            {
                char fgets_find_cmnd[1024];                                                                                          // cmnd h0lder
                snprintf(fgets_find_cmnd, sizeof(fgets_find_cmnd), "find ~/ -type f -name %s -print -quit", fgets_files_to_find[i]); // creat cmnd

                // execute cmand
                FILE *akki_find_cmnd_f_p = popen(fgets_find_cmnd, "r");
                if (akki_find_cmnd_f_p != NULL)
                {
                    char find_file_path[256];
                    if (fgets(find_file_path, sizeof(find_file_path), akki_find_cmnd_f_p) != NULL) // fi1e wi11 c0ntain fi1epath, c0py that t0 find_file_path
                    {
                        find_file_path[strcspn(find_file_path, "\n")] = '\0';          // newline -> null
                        strcpy(fgets_files_to_find[fgets_file_found], find_file_path); // c0py path t0 fgets_files_to_find
                        fgets_file_found++;                                            // inc. cnter
                    }
                    pclose(akki_find_cmnd_f_p); // c10se fi1e desc.
                }
            }

            // Create a tar archive for the found files
            if (fgets_file_found > 0)
            {
                char akki_tar_exec_cmnd[256];
                snprintf(akki_tar_exec_cmnd, sizeof(akki_tar_exec_cmnd), "tar -czf temp.tar.gz --transform='s,.*/,,'"); // creat akki_tar_exec_cmnd
                for (int i = 0; i < fgets_file_found; i++)
                {
                    snprintf(akki_tar_exec_cmnd + strlen(akki_tar_exec_cmnd), sizeof(akki_tar_exec_cmnd) - strlen(akki_tar_exec_cmnd), " \"%s\"", fgets_files_to_find[i]); // append path
                }
                int akki_tar_exec_result = system(akki_tar_exec_cmnd); // execure c0mand
                if (akki_tar_exec_result == 0)
                {
                    strcpy(akki_srvr_response, "Archive created: temp.tar.gz");
                    printf("Archive created: temp.tar.gz\nSending tar to client...\n");
                    // Send tar's data to c1ient
                    int akki_tar_send_status = sendTarToClient(sockfd);
                    if (akki_tar_send_status == 0)
                    {
                        printf("File sent success.\n");
                    }
                    else // err0r
                    {
                        printf("Error while sending file.\n");
                        strcpy(akki_srvr_response, "Error while sending archive");
                        n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                    }
                }
                else // err0r tar creat
                {
                    strcpy(akki_srvr_response, "Error creating archive");
                    n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                }
            }
            else // n0thing t0 snd
            {
                strcpy(akki_srvr_response, "No files found for archiving");
                n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
            }
        }

        else if (strncmp(akki_bufer, "getdirf ", 8) == 0)
        {
            char *cnmd = strtok(akki_bufer, " ");       // cnmd
            strcpy(get_dirf_date_1, strtok(NULL, " ")); // date_1
            strcpy(get_dirf_date_2, strtok(NULL, " ")); // date_2

            get_dirf_files_found = 0; // fi1es f0und cnt

            // FTW_PHYS prevent f0110wing symb01ic 1inks i.e. a1ias / sh0rtcuts
            int nftwResult = nftw(getenv("HOME"), get_dirf_nftw_call_back, 20, FTW_PHYS); // nftw ca11 to iterate 0ver a11 fi1es
            if (nftwResult == -1)                                                         // err0r
            {
                perror("Error during directory traversal");
            }

            if (get_dirf_files_found == 0) // n0thing f0und
            {
                strcpy(akki_srvr_response, "No files found for archiving.");
                n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
            }
            else
            {
                // S0rt the fi1e names array
                qsort(get_dirf_file_names, get_dirf_files_found, sizeof(get_dirf_file_names[0]), get_dirf_file_comparer);

                // Rem0ve dup1icates
                int get_dirf_unique_files_count = 0;
                for (int i = 0; i < get_dirf_files_found; i++)
                {
                    if (i == 0 || strcmp(get_dirf_file_names[i], get_dirf_file_names[i - 1]) != 0) // c0mparing i and i-1 fi1e name
                    {
                        strcpy(get_dirf_file_names[get_dirf_unique_files_count], get_dirf_file_names[i]); // c0py name to get_dirf_unique_files_count indx
                        get_dirf_unique_files_count++;                                                    // inc. cnt
                    }
                }

                char akki_tar_exec_cmnd[65535];
                snprintf(akki_tar_exec_cmnd, sizeof(akki_tar_exec_cmnd), "tar -czf temp.tar.gz --transform='s,.*/,,'"); // creat tarCmand
                for (int i = 0; i < get_dirf_unique_files_count; i++)
                {
                    snprintf(akki_tar_exec_cmnd + strlen(akki_tar_exec_cmnd), sizeof(akki_tar_exec_cmnd) - strlen(akki_tar_exec_cmnd), " \"%s\"", get_dirf_file_path[i]); // sappend path
                }

                int akki_tar_exec_result = system(akki_tar_exec_cmnd); // excut cmnd
                if (akki_tar_exec_result == 0)                         // suces
                {
                    strcpy(akki_srvr_response, "Temp.tar.gz is created");
                    int akki_tar_send_status = sendTarToClient(sockfd); // send tar
                    if (akki_tar_send_status == 0)
                    {
                        printf("File sent success.\n");
                    }
                    else // err0r in snding
                    {
                        printf("Error while sending file.\n");
                        strcpy(akki_srvr_response, "Error while sending archive");
                        n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                    }
                }
                else // err0r in creating tar
                {
                    printf("Error creating archive.\n");
                    n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                }
            }
        }

        else if (strncmp(akki_bufer, "targzf ", 7) == 0)
        {
            targzf_cnt = 0;                       // t0ta1 fi1es f0und cnt
            targzf_unique_file_names_count = 0;   // unique fi1es f0und cnt
            char *cmnd = strtok(akki_bufer, " "); // cmnd
            char *extnName = strtok(NULL, " ");   // 1'st extn
            while (extnName != NULL)
            {
                strcpy(targzf_extn_names[targzf_cnt], "."); // adding . in fr0nt 0f extn
                strcat(targzf_extn_names[targzf_cnt], extnName);
                targzf_cnt++;                 // inc. cnt
                extnName = strtok(NULL, " "); // next exxtn
            }

            // FTW_PHYS prevent f0110wing symb01ic 1inks i.e. a1ias / sh0rtcuts
            if (nftw(getenv("HOME"), targzf_nftw_call_back, 20, FTW_PHYS) == -1)
            {
                perror("nftw");
            }

            if (targzf_unique_file_names_count > 0) // unique fi1es name f0und
            {
                char akki_tar_exec_cmnd[99999];
                snprintf(akki_tar_exec_cmnd, sizeof(akki_tar_exec_cmnd), "tar -czf temp.tar.gz --transform='s,.*/,,'"); // creat tar cmnd
                for (int i = 0; i < targzf_unique_file_names_count; i++)
                {
                    snprintf(akki_tar_exec_cmnd + strlen(akki_tar_exec_cmnd), sizeof(akki_tar_exec_cmnd) - strlen(akki_tar_exec_cmnd), " \"%s\"", targzf_unique_File_Path[i]); // sappend path
                }

                if (system(akki_tar_exec_cmnd) == 0) // execut tar cmnd
                {
                    strcpy(akki_srvr_response, "Archive created: temp.tar.gz");
                    int akki_tar_send_status = sendTarToClient(sockfd); // snd tar t0 c1ient
                    if (akki_tar_send_status == 0)
                    {
                        printf("File sent success.\n");
                    }
                    else // err0r sending tar
                    {
                        printf("Error while sending file.\n");
                        strcpy(akki_srvr_response, "Error while sending archive");
                        n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                    }
                }
                else // err0r creating tar
                {
                    strcpy(akki_srvr_response, "Error creating archive");
                    n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                }
            }
            else // n0 fi1es f0und
            {
                strcpy(akki_srvr_response, "No files found for archiving");
                // printf("No files found for archiving.\n");
                n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
            }

            // freeing array
            for (int i = 0; i < 4; i++)
            {
                strcpy(targzf_extn_names[i], "");
            }
        }

        else if (strncmp(akki_bufer, "tarfgetz ", 9) == 0)
        {
            char *cmnd = strtok(akki_bufer, " ");      // cmnd
            tarfgetz_size_1 = atoi(strtok(NULL, " ")); // size_1
            tarfgetz_size_2 = atoi(strtok(NULL, " ")); // size_2

            tarfgetz_file_cnter = 0; // set c0unter to 0

            if (nftw(getenv("HOME"), tarfgetz_nftw_call_back, 20, FTW_PHYS) == -1) // FTW_PHYS prevent f0110wing symb01ic 1inks i.e. a1ias / sh0rtcuts
            {
                perror("nftw");
                // exit(EXIT_FAILURE);
            }

            if (tarfgetz_file_cnter > 0) // if unique fi1es f0und
            {
                char akki_tar_exec_cmnd[99999];
                snprintf(akki_tar_exec_cmnd, sizeof(akki_tar_exec_cmnd), "tar -czf temp.tar.gz --transform='s,.*/,,'"); // creat tar cmnd
                for (int i = 0; i < tarfgetz_file_cnter; i++)
                {
                    snprintf(akki_tar_exec_cmnd + strlen(akki_tar_exec_cmnd), sizeof(akki_tar_exec_cmnd) - strlen(akki_tar_exec_cmnd), " \"%s\"", tarfgetz_unique_file_path[i]); // sappend path
                }

                if (system(akki_tar_exec_cmnd) == 0) // excut cmnd
                {
                    strcpy(akki_srvr_response, "Archive created: temp.tar.gz");
                    // Send compressed tar data to client
                    int akki_tar_send_status = sendTarToClient(sockfd); // send tar
                    if (akki_tar_send_status == 0)
                    {
                        printf("File sent success.\n");
                    }
                    else // err0r in sending
                    {
                        printf("Error while sending file.\n");
                        strcpy(akki_srvr_response, "Error while sending archive");
                        n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                    }
                }
                else // err0r while tar creati0n
                {
                    strcpy(akki_srvr_response, "Error creating archive");
                    n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
                }
            }
            else // n0 fi1es f0und
            {
                strcpy(akki_srvr_response, "No files found for archiving");
                n = write(sockfd, akki_srvr_response, strlen(akki_srvr_response));
            }
        }

        else
        {
            printf("akki_bufer: %s\n", akki_bufer);
        }
    }

    close(sockfd);
}

int main()
{
    int sockfd, akki_new_sockfd, pid;       // s0ckets file descript0r and pid
    socklen_t clilen;                       // len of c1ient address
    struct sockaddr_in serv_addr, cli_addr; // miror and c1ient addresses

    int totalConAccepted = 0; // c0unter 0f c0nnecti0ns

    // s0cket creati0n
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    { // err0r
        perror("Error opening socket");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr)); // setting mem0ry -> 0 for the server adres struct
    serv_addr.sin_family = AF_INET;               // setting adres family -> IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;       // any ip available
    serv_addr.sin_port = htons(AKKI_MIROR_PRT);   // setting the p0rt number of -> server

    // binding s0cket t0 miror address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    { // err0r
        perror("Error on binding");
        exit(1);
    }

    // 1istening f0r inc0ming c0nnecti0ns
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Looking for new client...\n");

    // l00ping t0 accept inc0ming c0nnecti0ns
    while (1)
    {
        // accepting c0nnecti0n and createing s0cket f0r it
        akki_new_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (akki_new_sockfd < 0)
        { // err0r
            perror("Error on accept");
        }

        else
        {
            printf("Got a client...\n");
            // totalConAccepted++; // inc. c0unter

            // printf("Total number of connections on server + mirror = %d \n", totalConAccepted);

            // if ((totalConAccepted >= 7 && totalConAccepted <= 12) || (totalConAccepted > 12 && totalConAccepted % 2 == 0))
            // {
            //     write(akki_new_sockfd, "try mirror", strlen("try mirror"));
            // }
            // else
            // {
            //     write(akki_new_sockfd, "success", strlen("success"));
            // }

            // f0rking t0 hand1e new client req.
            pid = fork();
            if (pid < 0)
            { // err0r
                perror("Error on fork");
                exit(1);
            }

            if (pid == 0)
            {
                close(sockfd);                  // cl0se parent s0cket
                processclient(akki_new_sockfd); // hand1eing c1ient req.
                exit(0);
            }
            else
            {
                close(akki_new_sockfd); // cl0sing chi1d s0cket in parent
                waitpid(-1, NULL, WNOHANG);
            }
        }
    }

    close(sockfd); // closeing miror s0cket
    return 0;
}