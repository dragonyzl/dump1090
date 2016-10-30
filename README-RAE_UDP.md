In order to obtain the airplane's Range, Azimuth and Elvation (R, A, E), The fuction elevation(...) is implenmented
based on the algorithm of Matlab Map toolbox (help elecation in Matlab). the GNU Scientific Library (GSL) is used for
Matrix operating in the algorithm (See the file AER.[ch] ).  

Some modifications were made for better interactiveShowData(...) and added the functionality for recording the airplanes' 
RAE data for latter using.

Although the message of ADS-B could be shared using the "--net-*-* "  in the argv, the UDP multicast is implemented for
sharing the airplanes' info in the local network without knowing too much knowlage about ADS-B etc.

Thanks to the antirez, mutability,etc. Thanks to the contributors of dump1090. 

Dragonyzl: njsss100@sohu.com
