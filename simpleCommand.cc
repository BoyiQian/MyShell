#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "simpleCommand.hh"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

SimpleCommand::SimpleCommand() {
  _arguments = std::vector<std::string *>();
}

SimpleCommand::~SimpleCommand() {
  // iterate over all the arguments and delete them
  for (auto & arg : _arguments) {
    delete arg;
  }
}

void SimpleCommand::insertArgument( std::string * argument ) {
  //read environment variablei
  char * arg = (char *) argument->c_str();
  char D = '$';
  char L = '{';
  char * CheckD = strchr(arg,D);
  char * CheckL = strchr(arg,L);
  //printf("argument: %s",arg);
  //char * CheckR = strchr(arg, "}");
  

  //enviromantal expansion
  if (CheckD && CheckL) {
  	char * arg = (char *)argument->c_str();
  	//printf("arg: %s\n", arg);
  	char * str = new char [1000];
  	int i = 0;
  	int len = 0;
  
  	while(arg[i]!='$') {
	  	printf("len: %d\n i: %d\n",len, i);
	  	str[len] = arg[i];
	  	len++;
	  	i++;
  	}
  	str[len] = '\0';
  	while (arg[i] == '$') {
	  	if (arg[i+1] == '{') {
	  		i += 2;
	  		char * env = new char[100];
	  		int j = 0;
	  		while(arg[i] != '}') {
				env[j] = arg[i];
		  		i++;
		  		j++;
	  		}
	  		env[j] = '\0';
	  		char * value =getenv(env);
			//printf("value: %s", value);
			strcat(str, value);
			//printf("String: %s", str);
	  		len = strlen(str);
	  		i+=1;
	  		while(arg[i] != '$' && arg[i] != '\0') {
		  		//printf("HI\n");
				str[len] = arg[i];
				len++;
				i++;
			}
	  	}
  	}
	if (str[len] != '\0') {

  		str[len] = '\0';
	}
  	argument = new std::string(str);
  }

  //tilde Expansion
  if(arg[0] == '~') {
	//struct passwd * info = getpwnam("HOME");
	char * result = new char [1000];
	//strcat(result, info->pw_dir);
	//result = getenv("HOME");
	//int len = strlen(result);
	//int i = 1;
	if (arg[1] =='\0') {
		result = getenv("HOME");
		argument = new std::string(result);
	} else {
		char* temp = new char[100];
		char* name = new char[100];

		int i = 0;
		int j = 1;
		while(arg[j] != '/' && arg[j]!='\0') {
			name[i] = arg[j];
			j++;
			i++;
		}
	
		//printf("arg: %s", arg[7]);

		if(arg[j] == '/') {
			//j++;
			//printf("Hello");
			int k =0;
			while(arg[j]!= '\0') {
				//printf("temp[k]:");
				//printf("arg[j]: %s", j);
				temp[k] = arg[j];
				k++;
				j++;
			}
			temp[k] = '\0';
			result = strcat(getpwnam(name)->pw_dir, temp);
			//result = "abc";
			argument = new std::string(result);
		
		} else {
			result = getpwnam(name)->pw_dir;
			argument = new std::string(result);
		}
	}
  }
				
	
        /*while (	arg[i] != '\0') {
		result[len] = arg[i];
		i++;
		len++;
	}
 	result[len] = '\0';
	argument = new std::string(result);*/	
				  	
  

  
  // simply add the argument to the vector
  _arguments.push_back(argument);
}

// Print out the simple command
void SimpleCommand::print() {
  for (auto & arg : _arguments) {
    std::cout << "\"" << *arg << "\" \t";
  }
  // effectively the same as printf("\n\n");
  std::cout << std::endl;
}
