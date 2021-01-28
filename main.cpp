#include <bits/stdc++.h>
#include <sys/ptrace.h>
#include "sys/wait.h"
#include "sys/personality.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "ext/linenoise-c894b9e59f02203dbe4e2be657572cf88c4230c3/linenoise.h"
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

              void run(); //wait until the child process has finished launching, then keep on getting input from linenoise until  get an EOF//

         private:
             void handle_command(const string& line);
             void continue_execution();

             string m_prog_name;
             pid_t m_pid;
         };


 vector<string>split(const string &s , char delimeter)
 {
     vector<string> out{};
     stringstream ss {s};;
     string item;

     while (getline(ss , item , delimeter))
     {
         out.push_back(item);
     }
     return out;
 }

 bool is_prefix(const string& s , const string& of)
 {
     if (s.size() > of.size()) return false;
    return equal(s.begin() , s.end(), of.begin());
 }

 void debugger::run()
  {
     //wait until SIGTRAP signal is sent using the waitpid function and its tracdd

     int wait_status ; auto options = 0 ;
     waitpid(m_pid , &wait_status , options);
     char* line = nullptr;

     //linenoise function takes a prompt to display and handles user input by itself. This means we get a nice command line with history and navigation commands

     while ((line = linenoise("cont> ")) != nullptr)
     {
         handle_command(line); //write command in shortway
         linenoiseHistoryAdd(line);
         linenoiseFree(line);
     }
 }


 void debugger::handle_command(const string& line)
 {
     auto args = split(line,' ');
     auto command = args[0];
     if(is_prefix(command , "continue"))
     {
         continue_execution();
     }
     else
     {
         cerr << "Unkown Command \n";
     }
 }

 void debugger::continue_execution()
 {
     ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);

     int wait_status;
     auto options = 0;
     waitpid(m_pid, &wait_status, options);
 }



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
        //execute the given program, passing the name of it as a command-line argument and a nullptr to terminate the list.
    }
    else if (pid >= 1)
    {
        //parent
        cout << "Started processing " << pid << '\n';
        debugger dbg{prog , pid};
        dbg.run();

    }

 }
