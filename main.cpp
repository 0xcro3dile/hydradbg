#include <bits/stdc++.h>
#include <sys/ptrace.h>
#include "sys/wait.h"
#include "sys/personality.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "ext/linenoise-c894b9e59f02203dbe4e2be657572cf88c4230c3/linenoise.h"
#include "include/debugger.hpp"
using namespace std;

 class debugger
 {

     /*
      * debugger class, listening to user input,
      * launch  from our parent fork of our main function.
      * also print out the child pid.
      */
 public :
    debugger (string prog_name, pid_t pid)
    : m_prog_name{move(prog_name)}, m_pid{pid}{}

    void run();

 private:
    string m_prog_name;
    pid_t m_pid;
};


 void debugger::run()
 {

     // child process has finished launching,  keep on getting input from linenoise get an EOF
     int wait_status;
     auto options = 0 ;
     waitpid(m_pid , &wait_status , options);
     char* line = nullptr;
     while ((line = linenoise("hydradbg >")) != nullptr)
     {
        // handle_command(line);
         linenoiseHistoryAdd(line);
         linenoiseFree(line);

     }
 }
 //void debugger::handle_command(const string& line)
// {

 //}

int main(int argc , char* argv[])
{
    if (argc < 2)
    {
        cerr << "unsepcid Pogramme name";
        return  -1;
    }

    auto prog = argv[1];
    auto pid = fork(); //split the programe into two process child <-> parent

    if (pid == 0) // child process
    {
        //replace anything exceuting with the programe we want to debug
        ptrace(PTRACE_TRACEME , 0 , nullptr);
        /*/
         * ptrace read the registrers
         * control exceu of another processors
         * reading momory
         */
        execl(prog , prog , nullptr);
    }
    else if (pid >= 1)
    {
        //parent
        cout << "Started processing " << pid << '\n';
        debugger dbg{prog , pid};
        dbg.run();

    }

}
