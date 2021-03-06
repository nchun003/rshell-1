#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include "errno.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <cmath>
#include <string.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <algorithm>

using namespace std;

void list_output(vector<char*> &v, const char* input)
{	
	int width = 0;
	int width2 = 0;
    unsigned width3 = 0;
    unsigned width4 = 0;
	int total = 0;
	
	for(auto x : v) {
		
		string full = input;
		full.append("/");
		full.append(x);
		
		int tempWidth = 0;
		int tempWidth2 = 0;
		struct stat fileStat;
		if(stat(full.c_str(), &fileStat) < 0)
			perror("Failed to run filestat");
		
		total += ceil(fileStat.st_blocks);
        
		struct passwd *pws;
		pws = getpwuid(fileStat.st_uid);
        if(pws < 0)
            perror("Failed to get user name");
		struct group *grp;
		grp = getgrgid(fileStat.st_gid);
        if(grp < 0)
            perror("Failed to get gropu name");
		
		for(; fileStat.st_size != 0; fileStat.st_size /= 10, tempWidth++);
		for(; fileStat.st_nlink != 0; fileStat.st_nlink /= 10, tempWidth2++);
		tempWidth > width ? width = tempWidth : width;
		tempWidth2 > width2 ? width2 = tempWidth2 : width2;
        strlen(pws->pw_name) > width3 ? width3 = strlen(pws->pw_name) : width3;
        strlen(grp->gr_name) > width4 ? width4 = strlen(grp->gr_name) : width4;
	}
    
	cout << "total " << total/2 << endl;
	
    for(auto x : v) {
		
		string full = input;
		full.append("/");
		full.append(x);
		
		struct stat fileStat;
	    if(stat(full.c_str(), &fileStat) < 0)  
			perror("Failed to do filestat");
		
		struct passwd *pws;
		pws = getpwuid(fileStat.st_uid);
        if(pws < 0)
            perror("Failed to get user name");
		struct group *grp;
		grp = getgrgid(fileStat.st_gid);
        if(grp < 0)
            perror("Failed to get gropu name");
        
        bool directory = (S_ISDIR(fileStat.st_mode));
        bool executable = (fileStat.st_mode > 0) && (S_IEXEC & fileStat.st_mode);
		bool hidden = (x[0] == '.');
        
		printf( directory ? "d" : "-");
	    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
	    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
	    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
	    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
	    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
	    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
	    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
	    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
	    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
		printf("  ");
		cout.width(width2);
        cout << fileStat.st_nlink << " ";
        cout.width(width3);
        cout << left << pws->pw_name << " ";
        cout.width(width4);
        cout << left << grp->gr_name << " ";
		cout.width(width);
		cout << std::right << fileStat.st_size << " ";
        
        struct tm *tm;
        char datestring[256];
        tm = localtime(&fileStat.st_mtime);
        strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
        printf("%s ", datestring);
		
        if(directory) {
            if(hidden)
                cout << "\x1b[34;47m" << x << "\x1b[0m" << endl;
            else
                cout << "\x1b[34;40m" << x << "\x1b[0m" << endl;
        }
        else if(executable) {
            if(hidden)
                cout << "\x1b[1;32;47m" << x << "\x1b[0m" << endl;
            else
                cout << "\x1b[1;32;40m" << x << "\x1b[0m" << endl;
        }
        else if(hidden)
            cout << "\x1b[30;47m" << x << "\x1b[0m" << endl;
        else
            cout << x << endl;
	}
}

void standard_output(vector<char*> &v, int length, const char* source)
{
    struct winsize w;
    int test = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if(test < 0)
        perror("Failed to get terminal width");
    int check_width = v.size()*length-1;
    int num_rows = ceil((double)check_width/(double)w.ws_col);
    
    for(int i = 0; i < num_rows; ++i)
    {
        for(unsigned a = i; a < v.size(); a += num_rows) {
			
			string full = source;
            full.append("/");
			full.append(v.at(a));
            
    		struct stat fileStat;
		    if(stat(full.c_str(), &fileStat) < 0)  
				perror("Failed to do filestat");
            
            bool directory = (S_ISDIR(fileStat.st_mode));
            bool executable = (fileStat.st_mode > 0) && (S_IEXEC & fileStat.st_mode);
    		bool hidden = ((v.at(a))[0] == '.');
            
            if(directory) {
                if(hidden)
                    cout << "\x1b[34;47m" << v.at(a) << "\x1b[0m";
                else
                    cout << "\x1b[34;40m" << v.at(a) << "\x1b[0m";
            }
            else if(executable) {
                if(hidden)
                    cout << "\x1b[1;32;47m" << v.at(a) << "\x1b[0m";
                else
                    cout << "\x1b[1;32;40m" << v.at(a) << "\x1b[0m";
            }
            else if(hidden)
                cout << "\x1b[30;47m" << v.at(a) << "\x1b[0m";
            else
                cout << "\x1b[0m" << v.at(a);
            
            for(int i = strlen(v.at(a)); i < length && a < (v.size() - num_rows); ++i)
                cout << " ";
        }
        cout << endl;
    }
}

bool comparisonFunc(const char *c1, const char *c2) {
    return strcasecmp(c1, c2) < 0;
}

void execute(int argc, char const *argv[])
{
    DIR *dirp;
    string direc = ".";
    bool list = false;
    bool all = false;
    bool recursive = false;
    vector<char*> files;
    unsigned max_length = 9;
	string flags = "";
	vector<const char*> inputs;
	
	for(int i = 1; i < argc; i++) {
		if(argv[i][0] == '-')
			flags += argv[i];
		else
			inputs.push_back(argv[i]);
	}
	
    if(flags.find('a') != string::npos)
        all = true;
    if(flags.find('l') != string::npos)
        list = true;
    if(flags.find('R') != string::npos)
        recursive = true;
	
	sort(inputs.begin(), inputs.end(), comparisonFunc);
	
	if(inputs.size() > 0) {
	
		for(auto in : inputs) {
    
		    if(NULL == (dirp = opendir(const_cast<char*>(in)))) {
        		struct stat fileStat;
    		    if(stat(in, &fileStat) < 0) {
                    perror("There was an error with spefied file. ");
                    exit(1);
                }
                else {
                    cout << in << endl;
                    exit(0);
                }
		    }
			
		    struct dirent *filespecs;
		    errno = 0;
		    while(NULL != (filespecs = readdir(dirp))) {
		        if(filespecs->d_name[0] == '.' && !all)
		            continue;
		        files.push_back(filespecs->d_name);
		        strlen(filespecs->d_name) > max_length ? max_length = strlen(filespecs->d_name) : max_length;
		    }
		    if(errno != 0) {
		        perror("There was an error with readdir(). ");
		        exit(1);
		    }
    
		    sort(files.begin(), files.end(), comparisonFunc);
			
		    if(!list) {
				if(inputs.size() > 1)
					cout << in << ":" << endl;
		        standard_output(files, ++max_length, in);
				if(inputs.size() > 1 && (in != inputs.back()))
					cout << endl;
			}
		    else {
				if(inputs.size() > 1)
					cout << in << ":" << endl;
		        list_output(files, in);
				if(inputs.size() > 1 && (in != inputs.back()))
					cout << endl;
			}
            
            if(recursive)
            {
                for(auto file : files) {
                    string temp = in;
                    temp.append("/");
                    temp.append(file);
            		struct stat fileStat;
        		    if(stat(temp.c_str(), &fileStat) < 0) {
        				perror("Failed to do the filestat");
                    }
                
                    bool directory = (S_ISDIR(fileStat.st_mode));
                    if(directory)
                    {
                        if(strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
                            continue;
                        cout << endl;
                        string out = in;
                        out.append("/");
                        out.append(file);
                        cout << out << ": " << endl;
                        const char * args[256];
                        args[0] = argv[0];
                        args[1] = const_cast<char*>(flags.c_str());
                        args[2] = out.c_str();
                        int arg_c = 3;
                        execute(arg_c, args);
                    }
                }
            
            }
			
			files.clear();
    
		    if(-1 == closedir(dirp)) {
		        perror("There was an error with closedir(). ");
		        exit(1);
		    }
		}
	}
	else {
        
	    if(NULL == (dirp = opendir(const_cast<char*>(direc.c_str())))) {
    		struct stat fileStat;
		    if(stat(direc.c_str(), &fileStat) < 0) {
    	        perror("There was an error with the specified file. ");
    	        exit(1);
            }
            else {
                cout << direc << endl;
                exit(0);
            }
	    }
	
	    struct dirent *filespecs;
	    errno = 0;
	    while(NULL != (filespecs = readdir(dirp))) {
	        if(filespecs->d_name[0] == '.' && !all)
	            continue;
	        files.push_back(filespecs->d_name);
	        strlen(filespecs->d_name) > max_length ? max_length = strlen(filespecs->d_name) : max_length;
	    }
	    if(errno != 0) {
	        perror("There was an error with readdir(). ");
	        exit(1);
	    }

	    sort(files.begin(), files.end(), comparisonFunc);

	    if(!list)
	        standard_output(files, ++max_length, direc.c_str());
	    else
	        list_output(files, direc.c_str());

        if(recursive)
        {
            for(auto file : files) {
        		struct stat fileStat;
    		    if(stat(file, &fileStat) < 0)  
    				perror("Failed to do filestat");
                
                bool directory = (S_ISDIR(fileStat.st_mode));
                if(directory)
                {
                    if(strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
                        continue;
                    cout << file << ": " << endl;
                    const char * args[256];
                    args[0] = argv[0];
                    args[1] = const_cast<char*>(flags.c_str());
                    args[2] = file;
                    int arg_c = 3;
                    execute(arg_c, args);
                    cout << endl;
                }
            }
            
        }
        
        files.clear();
        
	    if(-1 == closedir(dirp)) {
	        perror("There was an error with closedir(). ");
	        exit(1);
	    }
	} 
}

int main (int argc, char const *argv[])
{
    execute(argc, argv);
    return 0;
}