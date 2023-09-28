# Socket-Programming
Created a Client Server Mirror Program where client request files from Server/Mirror and load is divided equally on Server and Mirror if a multiple Client tries to connect with Server/Mirror

# Compile all the files.
# Run either Server or Mirror or both
# Run Client

Functions description:
1. fgets file1 file2 file3 file4
The server must search the files (file 1 ..up to file4) in its directory tree rooted at ~ and return temp.tar.gz that contains at least one (or more of the listed files) if they are present
If none of the files are present, the server sends “No file found” to the client 
(which is then printed on the client terminal by the client) 
Ex: C$ fgets new.txt ex1.c ex4.pdf

2. tarfgetz size1 size2 <-u> 
The server must return to the client temp.tar.gz that contains all the files in the directory tree rooted at its ~ whose file-size in bytes is >=size1 and <=size2 and size1 < = size2 (size1>= 0 and size2>=0) 
-u unzip temp.tar.gz in the pwd of the client (optional)
Ex: C$ tarfgetz 1240 12450 -u

3. filesrch filename
If the file filename is found in its file directory tree rooted at ~, the server must return the filename, size(in bytes), and date created to the client and the client prints the received information on its terminal. 
Note: if the file with the same name exists in multiple folders in the directory tree rooted at ~, the server sends information pertaining to the first successful search/match of filename Else the client prints “File not found” 
Ex: C$ filesrch sample.txt

4. targzf <extension list> <-u> //up to 4 different file types 
The server must return temp.tar.gz that contains all the files in its directory tree rooted at ~ belonging to the file type/s listed in the extension list, else the server sends the message “No file found” to the client (which is printed on the client terminal by the client) 
-u unzip temp.tar.gz in the pwd of client (optional)
The extension list must have at least one file type and can have up to six different file types 
Ex: C$ targzf c txt pdf -u

5. getdirf date1 date2 <-u> 
The server must return to the client temp.tar.gz that contains all the files in the directory tree rooted at ~ whose date of creation is <=date2 and >=date1 (date1<=date2) 
-u unzip temp.tar.gz in the pwd of the client (optional)
Ex: C$ getdirf 2023-01-16 2023-03-04 -u

6. quit The command is transferred to the server and the client process is terminated

7. Alternating Between the Server and the Mirror
The first 6 client connections are handled by the server. 
The next 6 client connections are handled by the mirror. 
The remaining client connections are handled by the server and the mirror in an alternating manner- (ex: connection 13 is handled by the server, connection 14 by the mirror, and so on)
