<h1 align="center">Ghost Key</h1>
<p align="center">A Simple Linux Kernel Module Keylogger</p><br />
<p align="center">Project made in collaboration with Kyle Welsh and Thomas Neumann</p>

## Features
:computer: Print User Input to Dmesg <br />
:computer: Recognize When a User will Type in Their Password <br />
:computer: Simple Code for a Kernel Space Interrupt Handler <br />

## Purpose
The purpose of this project is to simply better understand Linux Modules, as well as Linux module code. Through
this project, which was our term long project in Operating Systems at Norht Central College, one can gain a better understanding
of Linux Kernel Modules and interrupt handlers in Kernel Space.

## Important Information
It is important to note that normally in an interrupt handler pr_info() should not be used. This project
could be improved upon by threading our kernel module, as well as printing output to a seperate server. If one would
actually want to get a users passcode, they must add functionality to this module, as it does not yet print output
to a seperate server, rather, it prints to the same device in which the module is located on. The password is still
successfully printed, however, there is no way for an unethical individual to access said password with this module.<br />

## Using the Module
  Step 1: Clone the repo from GitHub.<br />
  Step 2: CD to the destination at which the cloned files are located.<br />
  Step 3: If you do not have the 'ghostKey.ko' file, run the 'make' command.<br />
  Step 4: Type in 'sudo insmod ghostKey.ko'<br />
  Step 5: Upon 'sudo <command> <enter>' recording will start.<br />
  Step 6: The next text typed in will be considered a password and will be stored after an 'enter' press.<br />
  Step 7: Type in 'sudo rmmod ghostKey.ko'<br />
  Step 8: To view the information 'sudo dmesg'<br />
  
## Important Note
Occasionally the printed out password will involve a certain key multiplied by two. This will be notfied to the user
through a message printed to the same output as the password. When such an error occurs, just remove the most recent
key that is above a series of "signal error" messages. This will allow you to obtain the password<br />

## Known Issue
Rarely, the error catcher will not realize an error has occured during the IRQ, and two of the same key will be printed
without a warning. In this situation, one should note that the key printed twice is always at a consistent unrealistic time
based on how a user types.<br />
  
    - This does not hinder the ability to obtain a passcode.
    - This happens less than 10% of the time in most machine states.
    - Can be solved through output comparisons. (Program to be made at a later date).
  
## Further Information
It is believed that the signalling issue will be solved once server output is implemented. If not, another program will be made to compare
time stamps and characters to instantly remove these duplicate presses. With this program, a password should be able to be obtained one password
input with 100% accurary, as it simply records the keys. If "sudo" is typed again before another password input, one should be able to logically
tell that the input was a command, not the password.
 
## Future Updates
Since this project was done as a project for a undergraduate college-level course, we were limtied on time. However, there are more features we would
like to add over time.
  - Functionality to send the password to a personal server.
  - Removing 'pr_info' calls.
  - A seperate program to read through all of the data and generate a guess on what the password is.
  - Ways to hide the Linux Module on the system.
  - Ways of socially engineering the module onto someones system.
  - Threading the Linux Kernel Module for increased speed.
  
  
  

