                          ZZZZZZ                                                
                        ZZZZZZZZZZZZ                                             
                      ZZZZZZZZZZZZZZZZ                                           
                     ZZZZZZZ     ZZZZZZ                                          
                    ZZZZZZ        ZZZZZ                                          
                    ZZZZZ          ZZZZ                                          
                    ZZZZ           ZZZZZ                                         
                    ZZZZ           ZZZZ                                          
                    ZZZZ          ZZZZZ                                          
                    ZZZZ        ZZZZZZ                                           
                    ZZZZ     ZZZZZZZZ       777        7777       7777777777     
              ZZ    ZZZZ   ZZZZZZZZ         777      77777777    77777777777     
          ZZZZZZZ   ZZZZ  ZZZZZZZ           777     7777  7777       777         
        ZZZZZZZZZ   ZZZZ    Z               777     777    777       777         
       ZZZZZZ       ZZZZ                    777     777    777       777         
      ZZZZZ         ZZZZ                    777     777    777       777         
     ZZZZZ          ZZZZZ    ZZZZ           777     777    777       777         
     ZZZZ           ZZZZZ    ZZZZZ          777     777    777       777         
     ZZZZ           ZZZZZ     ZZZZZ         777     777    777       777         
     ZZZZ           ZZZZ       ZZZZZ        777     777    777       777         
     ZZZZZ         ZZZZZ        ZZZZZ       777     777    777       777         
      ZZZZZZ     ZZZZZZ          ZZZZZ      777     7777777777       777         
       ZZZZZZZZZZZZZZZ            ZZZZ      777      77777777        777         
         ZZZZZZZZZZZ               Z                                             
            ZZZZZ                                                                
                                                                                 
The friendly Operating System for IoT!

RIOT OS is an operating system for Internet of Things (IoT) devices. It is based on a microkernel and designed for
* energy efficiency
* hardware independent development
* a high degree of modularity

Its features comprise
* a preemptive, tickless scheduler with priorities
* flexible memory managment
* high resolution timers
* virtual, long-term timers
* Wiselib support (C++ algorithm library, including routing, clustering, timesync, localization, security and more algorithms)
* IPv6
* TCP and UDP
* 6LoWPAN
* RPL

Development procedures
======================
1. Check if your code follows the Coding Conventions. If the code does not comply these style rules, your code will not be merged.

2. Create a new branch in your own fork for every new feature or fix. Then create the pull request from this branch, once you completed your task (feature or fix). Every commit in this branch will be added to the pull request.

3. The master branch should always be in a working state. The RIOT maintainers will create release tags based on this branch, whenever a milestone is completed.

4. New features and fixes will always be submitted as pull requests. A pull request will be merged if the RIOT maintainers acknowledge the request. Before it will get merged, the pull request has to stay open long enough to discuss possible issues, but no longer than 30 days. The more extensive the changes in the pull request, the more time should be given for discussion.

5. Comments on a pull request should be added to the request itself, and not to the commit.

6. Only RIOT maintainers have commit rights to the main RIOT repository. However, even they have to create pull requests to submit their changes. A RIOT contributor cannot acknowledge his own pull request. All current RIOT maintainers are listed in the wiki.

7. Keep commits to the point, e.g., don't add whitespace/typo fixes to other code changes. If changes are layered, layer the patches.

8. Describe the technical detail of the change(s) as specific as possible.

License
=======
* All sources and binaries that have been developed at Freie Universität Berlin are
  licensed under the GNU General Public License version 3 as published by the
  Free Software Foundation.
* Some external sources, especially files developed by SICS are published under
  a separate license.
  
All code files contain licensing information.

For more information, see the RIOT website:

http://www.riot-os.org
