Application Tester for a feedback scheduling on RIOT

Development Project on RIOT OS  

The goal of the project is twofold:  
--> to modify the current scheduling policy to support a Feedback scheduling policy;  
--> to test the scheduling policy by developing a simple application that spawns different threads;  
To do that: modify the RIOT OS scheduler (for the “native” board) by introducing the Feedback scheduling policy, and implement a tester application (RRTester) capable of testing the new scheduling policy.
More in detail, modify the OS fixed-priority scheduling policy and introduce a Feedback scheduling policy with 3 queues and quantum equals to 0.5 seconds. In addition, the scheduling policy needs to print on the terminal all the context switches of the different threads.  
The application RRTester must be deployed as a RIOT application containing a main() function that that starts 5 new threads, according to the following:  

Thread name    ->     Priority      ->         Service Time (s)  
Main    ->       THREAD_PRIORITY_MAIN         ->      -  
A      ->       THREAD_PRIORITY_MAIN + 1      ->      3  
B      ->       THREAD_PRIORITY_MAIN + 2      ->      6  
C      ->       THREAD_PRIORITY_MAIN + 3      ->      4  
D      ->       THREAD_PRIORITY_MAIN + 4      ->      5  
E      ->       THREAD_PRIORITY_MAIN + 5      ->      2  

Please note that the priority field is a parameter needed for threads’ creation; the priority won’t influence the Feedback algorithm. Each thread must include a printf() function that prints on the console the thread name currently running and the number of seconds remaining for its execution.
