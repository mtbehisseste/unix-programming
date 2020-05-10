# A 'netstat -nap'-like program
This program implements `nestat -nap`-like command from scratch. It lists TCP/UDP connections and program names/PIDs correspond to each connection.

## Options
- -t or --tcp: list only TCP connections.
- -u or --udp: list only UDP connections.
- An optional string to list only command lines that containing the string.

## Usage
```bash
$ ./hw1 [-t|--tcp] [-u|--udp] [filter-string]
```

## Example
```bash
$ ./hw1
List of TCP connections:
Proto Local Address           Foreign Address         PID/Program name and arguments
tcp   0.0.0.0:993             0.0.0.0:*               2472/dovecot (... you should list full arguments here)
tcp   0.0.0.0:60546           0.0.0.0:*               764/rpc.statd
tcp   0.0.0.0:995             0.0.0.0:*               2472/dovecot
tcp   127.0.0.1:3306          0.0.0.0:*               2684/mysqld
tcp   0.0.0.0:110             0.0.0.0:*               2472/dovecot
tcp   0.0.0.0:143             0.0.0.0:*               2472/dovecot
tcp   0.0.0.0:111             0.0.0.0:*               742/rpcbind
tcp   0.0.0.0:465             0.0.0.0:*               2827/master
tcp   192.168.1.254:53        0.0.0.0:*               30495/named
tcp   127.0.0.1:53            0.0.0.0:*               30495/named
tcp   10.0.3.1:53             0.0.0.0:*               2580/dnsmasq
tcp   127.0.0.1:631           0.0.0.0:*               27868/cupsd
tcp   127.0.0.1:953           0.0.0.0:*               30495/named
tcp   0.0.0.0:25              0.0.0.0:*               2827/master
tcp   0.0.0.0:1723            0.0.0.0:*               2845/pptpd
tcp   140.113.1.1:50275       140.112.172.4:23        16993/telnet ptt.cc
tcp6  :::993                  :::*                    2472/dovecot
tcp6  :::995                  :::*                    2472/dovecot
tcp6  :::2122                 :::*                    2475/sshd
tcp6  :::110                  :::*                    2472/dovecot
tcp6  :::143                  :::*                    2472/dovecot
tcp6  :::111                  :::*                    742/rpcbind
tcp6  :::80                   :::*                    4358/apache2
tcp6  :::465                  :::*                    2827/master
tcp6  :::53                   :::*                    30495/named
tcp6  :::33173                :::*                    764/rpc.statd
tcp6  ::1:631                 :::*                    27868/cupsd
tcp6  ::1:953                 :::*                    30495/named
tcp6  :::25                   :::*                    2827/master
tcp6  :::443                  :::*                    4358/apache2
tcp6  ::1:53859               ::1:631                 2608/cups-browsed

List of UDP connections:
Proto Local Address           Foreign Address         PID/Program name and arguments
udp   0.0.0.0:5353            0.0.0.0:*               1071/avahi-daemon: (... you should list full arguments here)
udp   0.0.0.0:56307           0.0.0.0:*               764/rpc.statd
udp   0.0.0.0:41678           0.0.0.0:*               1071/avahi-daemon:
udp   0.0.0.0:12491           0.0.0.0:*               2559/dhcpd
udp   10.0.3.1:53             0.0.0.0:*               30495/named
udp   192.168.1.254:53        0.0.0.0:*               30495/named
udp   127.0.0.1:53            0.0.0.0:*               30495/named
udp   10.0.3.1:53             0.0.0.0:*               2580/dnsmasq
udp   0.0.0.0:67              0.0.0.0:*               2559/dhcpd
udp   0.0.0.0:67              0.0.0.0:*               2580/dnsmasq
udp   0.0.0.0:111             0.0.0.0:*               742/rpcbind
udp   0.0.0.0:631             0.0.0.0:*               2608/cups-browsed
udp   0.0.0.0:913             0.0.0.0:*               742/rpcbind
udp   127.0.0.1:940           0.0.0.0:*               764/rpc.statd
udp6  :::5353                 :::*                    1071/avahi-daemon:
udp6  :::59492                :::*                    764/rpc.statd
udp6  :::53                   :::*                    30495/named
udp6  :::111                  :::*                    742/rpcbind
udp6  :::913                  :::*                    742/rpcbind
udp6  :::34193                :::*                    1071/avahi-daemon:
udp6  :::18161                :::*                    2559/dhcpd
```
With `--tcp` option given:
```bash
$ ./hw1 --tcp
List of TCP connections:
Proto Local Address           Foreign Address         PID/Program name and arguments
tcp   0.0.0.0:993             0.0.0.0:*               2472/dovecot (... you should list full arguments here)
tcp   0.0.0.0:60546           0.0.0.0:*               764/rpc.statd
tcp   0.0.0.0:995             0.0.0.0:*               2472/dovecot
tcp   127.0.0.1:3306          0.0.0.0:*               2684/mysqld
tcp   0.0.0.0:110             0.0.0.0:*               2472/dovecot
tcp   0.0.0.0:143             0.0.0.0:*               2472/dovecot
tcp   0.0.0.0:111             0.0.0.0:*               742/rpcbind
tcp   0.0.0.0:465             0.0.0.0:*               2827/master
tcp   192.168.1.254:53        0.0.0.0:*               30495/named
tcp   127.0.0.1:53            0.0.0.0:*               30495/named
tcp   10.0.3.1:53             0.0.0.0:*               2580/dnsmasq
tcp   127.0.0.1:631           0.0.0.0:*               27868/cupsd
tcp   127.0.0.1:953           0.0.0.0:*               30495/named
tcp   0.0.0.0:25              0.0.0.0:*               2827/master
tcp   0.0.0.0:1723            0.0.0.0:*               2845/pptpd
tcp   140.113.1.1:50275       140.112.172.4:23        16993/telnet ptt.cc
tcp6  :::993                  :::*                    2472/dovecot
tcp6  :::995                  :::*                    2472/dovecot
tcp6  :::2122                 :::*                    2475/sshd
tcp6  :::110                  :::*                    2472/dovecot
tcp6  :::143                  :::*                    2472/dovecot
tcp6  :::111                  :::*                    742/rpcbind
tcp6  :::80                   :::*                    4358/apache2
tcp6  :::465                  :::*                    2827/master
tcp6  :::53                   :::*                    30495/named
tcp6  :::33173                :::*                    764/rpc.statd
tcp6  ::1:631                 :::*                    27868/cupsd
tcp6  ::1:953                 :::*                    30495/named
tcp6  :::25                   :::*                    2827/master
tcp6  :::443                  :::*                    4358/apache2
tcp6  ::1:53859               ::1:631                 2608/cups-browsed
```
With `--tcp` option and filter string given:
```bash
$ ./hw1 --tcp telnet
List of TCP connections:
Proto Local Address           Foreign Address         PID/Program name and arguments
tcp   140.113.1.1:50275       140.112.172.4:23        16993/telnet ptt.cc
```
