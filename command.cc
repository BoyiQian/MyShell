/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "command.hh"
#include "shell.hh"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern char **environ;
extern int last_return;
extern int last_pid;
extern std::string last_command;

Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
    _appends = false;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

    if ( _outFile ) {
	    if (_outFile == _errFile) {
		    _errFile = NULL;
	    }
        delete _outFile;
    }
    _outFile = NULL;

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    if ( _errFile ) {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        if ( isatty(0) ) {
	    Shell::prompt();
	}
        return;
    }
    
    last_command = "";
    for(int i = 0; i < (int)_simpleCommands.size(); i++) {
	    for (int j =0; j<(int)_simpleCommands[i]->_arguments.size(); j++) {
		    last_command = _simpleCommands[i]->_arguments[j]->c_str();
		    //last_command = last_command + " ";
	    }
	    /*if (i != (int)_simpleCommands.size()-1) {
		    last_command = last_command + "| ";
	    }*/
    }

    // Print contents of Command data structure
    //print();
    
    //exit
    if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "exit")) {
	    printf("Good bye!!\n");
	    exit(1);
    }

    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec

    int tmpin = dup(0);
    int tmpout = dup(1);
    int tmperr = dup(2);
    int fdin;
    int fdout;
    int fderr;
    if (_inFile != NULL) {
	    fdin = open(_inFile->c_str(), O_RDONLY, 0777);
    } else {
	    fdin = dup(tmpin);
    }
	
    if (_errFile != NULL) {
	    if(_appends) {
		    fderr = open(_errFile->c_str(), O_WRONLY|O_CREAT|O_APPEND, 0777);
	    } else {
		    fderr = open(_errFile->c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0777);
	    }
    } else {
	    fderr = dup(tmperr);
    }
	
    //write(1,"11111", 5);
    int ret;
    //int i = 0;
    for ( int i = 0; i < (int)_simpleCommands.size(); i++ ) {
	    //Build-in
	   
            if(strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "setenv") == 0) {
		if(setenv(_simpleCommands[i]->_arguments[1]->c_str(), _simpleCommands[i]->_arguments[2]->c_str(),1) !=0) {
			perror("setenv");
		}
		clear();
		Shell::prompt();
		return;
		
            }
	    if(strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "unsetenv") == 0) { 
        	if(unsetenv(_simpleCommands[i]->_arguments[1]->c_str()) !=0) {
		        perror("unsetenv");
		}
		clear();
		Shell::prompt();
		return;
		
	    }
	    if(strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "cd") == 0) { 
        	if(_simpleCommands[i]->_arguments.size() == 1) {
			if(chdir(getenv("HOME")) != 0) {
				perror("cd");
			}
		} else {
			if(chdir(_simpleCommands[i]->_arguments[1]->c_str()) !=0) {
				//perror("cd");
				fprintf(stderr, "cd: can't cd to %s\n", _simpleCommands[i]->_arguments[1]->c_str());
			}
		}
		clear();
		Shell::prompt();
		return;
		
	    }
	    if(strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "source") == 0) {
		FILE * fp = NULL;
		char buff [1000];
		fp = fopen(_simpleCommands[i]->_arguments[1]->c_str(), "r");
		fgets(buff, 1000, (FILE*) fp);
		printf("You Win!\n");
		fclose(fp);

		/*int tmpin  = dup(0);
		int tmpout =dup(1); 
		int fdpipein[2];
 		int fdpipeout[2];
		
		pipe(fdpipein);
		pipe(fdpipeout);
		write(fdpipein[1], buff, strlen(buff));
		write(fdpipein[1], "\n", 1);

		close(fdpipein[1]);
		dup2(fdpipein[0],0);
		close(fdpipein[0]);
		dup2(fdpipeout[1], 1);
		close(fdpipeout[1]);

		int ret_source = fork();
		//printf("%d\n", ret_source);
		if (ret_source == 0) {
			int index = 0;
			for(int i =0; i< strlen(buff); i++) {
				if (buff[i] == ' ') {
					index = 1;
				}
			}
		 	const char *argv[3];
			argv[0] = "echo";
			argv[1] = "You Win!\n";
			argv[2] = NULL;*/
			/*execvp("/proc/self/ex/", NULL);
			
			//printf("%s\n",argv[1]);
			//fclose(fp);
			_exit(1);
		} else if (ret_source < 0) {
			perror("fork");
			//fclose(fp);
			exit(1);
		}
		dup2(tmpout,1);
		dup2(tmpin,0);
		close(tmpout);
		close(tmpin);
		char * buffer = new char[1000];
		char ch;
	    	int i = 0;
		while (read(fdpipeout[0], &ch, 1)) {
			if (ch == '\n') {
				buffer[i] = ' ';
				i++;
			} else {
				buffer[i] = ch;
				i++;
			}
		}

		buffer[i] = '\0';
		printf("%s\n", buffer);*/
		exit(1);
	
	    }

			


	    //ret = fork();
	    dup2(fdin, 0);
	    close(fdin);
	    dup2(fderr,2);
	    close(fderr);

	    if (i == (int)_simpleCommands.size() -1) {
	    	if (_outFile != NULL) {
			if (_appends) {
				fdout = open(_outFile->c_str(),O_WRONLY|O_APPEND|O_CREAT, 0777);
			} else {
				fdout = open(_outFile->c_str(),O_WRONLY|O_CREAT|O_TRUNC, 0777);
			}	
	    	} else {
		 	fdout = dup(tmpout);
	    	}
	    } else {
		    int fdpipe[2];
		    pipe(fdpipe);
		    fdout = fdpipe[1];
		    fdin = fdpipe[0];
	    }
	    dup2(fdout,1);
	    close(fdout);

            ret = fork();
	    if (ret == 0) {
		//child
		//

		if(strcmp(_simpleCommands[i]->_arguments[0]->c_str(),"printenv") == 0){
			//extern ** environ;
	                char ** env = environ;
			while(*env) {
				 fprintf(stdout, "%s\n", *env);
				 env++;
			}
			exit(1);

		}
		/*if(strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "source") == 0) {
			FILE * fp = NULL;
			char buff [1000];
			fp = fopen(_simpleCommands[i]->_arguments[1]->c_str(), "r");
			fgets(buff, 1000, (FILE*) fp);
			//printf("check :%s\n",buff);
			execvp(buff[0], buff);

			fclose(fp);
			exit(1);
		}*/


		int size = _simpleCommands[i]->_arguments.size() + 1;
		
		char** argu;
		argu = (char **)malloc(size*sizeof(char **));

		//char *argu[] = new char*[size];
		//int j = 0;
		for (int j = 0;j <(int)_simpleCommands[i]->_arguments.size(); j++) {
			argu[j] = (char *)_simpleCommands[i]->_arguments[j]->c_str();
		}
		argu[size - 1] = NULL;
		execvp(argu[0],(char* const*)argu);
	    	perror("execvp");
		free(argu);
		argu = NULL;
		_exit(1);	
	    } else if (ret < 0) {
	    	perror("fork");
	        return;
	    }
    }
	
    dup2(tmpin,0);
    dup2(tmpout,1);
    dup2(tmperr,2);
    close(tmpin);
    close(tmpout);
    close(tmperr);

    
    if (!_background) {
	    // wait for last proceissi
	    int pid = 0;
	    waitpid(ret, &pid, 0);
	    last_return = WEXITSTATUS(pid);

    } else {

    	last_pid = ret;
    }

    // Clear to prepare for next command
    clear();

    // Print new prompt
    if ( isatty(0)) {
    	Shell::prompt();
    }
}

SimpleCommand * Command::_currentSimpleCommand;
