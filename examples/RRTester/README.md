Development Project on RIOT OS  
The goal of the project is twofold:  
--> to modify the current scheduling policy to support a Feedback scheduling policy;  
--> to test the scheduling policy by developing a simple application that spawns different threads;  

To do that, modify the RIOT OS scheduler (for the “native” board) by introducing the Feedback scheduling policy, and implement a tester application (RRTester) 
capable of testing the new scheduling policy.  

More in detail, modify the OS fixed-priority scheduling policy and introduce a Feedback scheduling policy with 3 queues and quantum equals to 0.5 seconds.   
In addition, the scheduling policy needs to print on the terminal all the context switches of the different threads.
The application RRTester must be deployed as a RIOT application containing a main() function that that starts 5 new threads.  
