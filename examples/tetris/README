README                                          -*-text-*-

Welcome to Micro Tetris, based on an original implementation by John Tromp,
initiated by Freek Wiedijk.  Currently maintained by Joachim Nilsson.

I decided to call this game Micro Tetris because it is very small and only
utilizes ANSI escape sequences to draw the board. Hence it is very suitable
for todays small embedded devices.

The game is available from as a Git source control archive from GitHub:

	http://github.com/troglobit/tetris

See the file AUTHORS for contact information.

                       -- Joachim Nilsson <troglobit@gmail.com>

Keyboard Control
----------------

    j         - Left
    k         - Rotate
    l         - Right
    <SPACE>   - Drop
    p         - Pause
    q         - Quit


Background
----------
The following game description is from the original IOCCC entry, taken from
John's home page at http://homepages.cwi.nl/~tromp/tetris.html

An ofbuscated tetris, 1989 IOCCC Best Game

This program plays the familiar game of `TETRIS' with the following features:

   * outputs vt100-like escape-sequences for:
      o cursor positioning
      o normal/reverse video 
     in curses like fashion (minimal output for screen updates)
   * continuously increasing speed (except in pause)
   * start speed selectable by giving n as first argument, where n is the
     number of drops per second (default=2).
   * controls also selectable by giving as the second argument a string
     consisting of the following 6 charachters: left, rotate, right,drop, 
     pause, quit (default="jkl pq")
   * the screen is blanked during the pause and the score is shown
   * finally, the program maintains a high-score table. giving a full path
     name for the table will result in a system-wide hiscore allowing a
     competition between users. 

Obfuscation has been achieved by:

   * making effects of signals hard to trace
   * implicit flushing by getchar()
   * tricky cursor-parking
   * minimizing code length
   * hard coding include-file constants
   * faking include-file structures
   * throwing portability out of the window 

For more on the 1989 IOCCC entry, see http://www.ioccc.org/1989/tromp.hint

long h[4];t(){h[3]-=h[3]/3000;setitimer(0,h,0);}c,d,l,v[]={(int)t,0,2},w,s,I,K
=0,i=276,j,k,q[276],Q[276],*n=q,*m,x=17,f[]={7,-13,-12,1,8,-11,-12,-1,9,-1,1,
12,3,-13,-12,-1,12,-1,11,1,15,-1,13,1,18,-1,1,2,0,-12,-1,11,1,-12,1,13,10,-12,
1,12,11,-12,-1,1,2,-12,-1,12,13,-12,12,13,14,-11,-1,1,4,-13,-12,12,16,-11,-12,
12,17,-13,1,-1,5,-12,12,11,6,-12,12,24};u(){for(i=11;++i<264;)if((k=q[i])-Q[i]
){Q[i]=k;if(i-++I||i%12<1)printf("\033[%d;%dH",(I=i)/12,i%12*2+28);printf(
"\033[%dm  "+(K-k?0:5),k);K=k;}Q[263]=c=getchar();}G(b){for(i=4;i--;)if(q[i?b+
n[i]:b])return 0;return 1;}g(b){for(i=4;i--;q[i?x+n[i]:x]=b);}main(C,V,a)char*
*V,*a;{h[3]=1000000/(l=C>1?atoi(V[1]):2);for(a=C>2?V[2]:"jkl pq";i;i--)*n++=i<
25||i%12<2?7:0;srand(getpid());system("stty cbreak -echo stop u");sigvec(14,v,
0);t();puts("\033[H\033[J");for(n=f+rand()%7*4;;g(7),u(),g(0)){if(c<0){if(G(x+
12))x+=12;else{g(7);++w;for(j=0;j<252;j=12*(j/12+1))for(;q[++j];)if(j%12==10){
for(;j%12;q[j--]=0);u();for(;--j;q[j+12]=q[j]);u();}n=f+rand()%7*4;G(x=17)||(c
=a[5]);}}if(c==*a)G(--x)||++x;if(c==a[1])n=f+4**(m=n),G(x)||(n=m);if(c==a[2])G
(++x)||--x;if(c==a[3])for(;G(x+12);++w)x+=12;if(c==a[4]||c==a[5]){s=sigblock(
8192);printf("\033[H\033[J\033[0m%d\n",w);if(c==a[5])break;for(j=264;j--;Q[j]=
0);while(getchar()-a[4]);puts("\033[H\033[J\033[7m");sigsetmask(s);}}d=popen(
"stty -cbreak echo stop \023;sort -mnr -o HI - HI;cat HI","w");fprintf(d,
"%4d from level %1d by %s\n",w,l,getlogin());pclose(d);}

                       -- John Tromp <tromp@cwi.nl>
