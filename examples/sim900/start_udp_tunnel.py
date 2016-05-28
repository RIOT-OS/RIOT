import socket, time, select, struct                                             
from scapy.all import IPv6, IP, ICMPv6EchoReply, ICMPv6EchoRequest, sr1         
sd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)                           
                                                                                
sd.bind(("", 9876))                                                             
                                                                                
while True:                                                                     
    t = select.select([sd.fileno()], [], [], 1)                                 
    if sd.fileno() in t[0]:                                                     
        data = sd.recvfrom(1024)                                                
        recv = IPv6(data[0])                                                    
        #swap src with dest                                                     
        print "Received packet is:"                                             
        recv.show2()                                                            
                                                                                
        #Change ICMPv6 to echo reply                                            
        a = sr1(recv, timeout=3)                                                
                                                                                
        print "Reply packet is:"                                              
        a.show2()                                                               
        sd.sendto(str(a) ,data[1])   
