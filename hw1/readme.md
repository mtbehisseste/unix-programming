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
Note that you may run the program with superuser to get the permissions to read `/proc` files owned by other users.
```bash
$ ./hw1
List of TCP connections:
Proto Local Address            Foreign Address          PID/Program name and arguments
tcp   127.0.0.1:41453          0.0.0.0:*                8028/python3
tcp   127.0.0.1:5939           0.0.0.0:*                -/-
tcp   127.0.0.53:53            0.0.0.0:*                -/-
tcp   0.0.0.0:22               0.0.0.0:*                -/-
tcp   127.0.0.1:631            0.0.0.0:*                -/-
tcp   192.168.1.8:22           140.113.0.229:65112      -/-
tcp   192.168.1.8:40586        104.26.5.98:443          1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:43296        192.168.1.3:139          3677/gvfsd-smb-browse
tcp   192.168.1.8:58946        192.168.1.3:139          3677/gvfsd-smb-browse
tcp   192.168.1.8:48808        172.217.160.78:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:52046        172.217.27.142:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:58922        64.233.189.188:5228      1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:35236        34.237.73.95:443         1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   127.0.0.1:35032          127.0.0.1:41453          8027/vim
tcp   192.168.1.8:49020        172.217.27.131:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:47804        74.125.203.189:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:45894        140.82.112.25:443        1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:50864        104.26.4.98:443          1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp6  :::80                    :::*                     -/-
tcp6  :::22                    :::*                     -/-
tcp6  ::1:631                  :::*                     -/-

List of UDP connections:
Proto Local Address            Foreign Address          PID/Program name and arguments
udp   0.0.0.0:631              0.0.0.0:*                -/-
udp   224.0.0.251:5353         0.0.0.0:*                1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
udp   224.0.0.251:5353         0.0.0.0:*                1867/chrome
udp   224.0.0.251:5353         0.0.0.0:*                1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
udp   0.0.0.0:5353             0.0.0.0:*                -/-
udp   0.0.0.0:39068            0.0.0.0:*                -/-
udp   127.0.0.53:53            0.0.0.0:*                -/-
udp   0.0.0.0:68               0.0.0.0:*                -/-
udp6  :::51472                 :::*                     -/-
udp6  :::5353                  :::*                     -/-
```
With `-t` or `--tcp` option:
```bash
$ ./hw1 -t
List of TCP connections:
Proto Local Address            Foreign Address          PID/Program name and arguments
tcp   127.0.0.1:41453          0.0.0.0:*                8028/python3
tcp   127.0.0.1:5939           0.0.0.0:*                -/-
tcp   127.0.0.53:53            0.0.0.0:*                -/-
tcp   0.0.0.0:22               0.0.0.0:*                -/-
tcp   127.0.0.1:631            0.0.0.0:*                -/-
tcp   192.168.1.8:22           140.113.0.229:65112      -/-
tcp   192.168.1.8:40586        104.26.5.98:443          1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:43296        192.168.1.3:139          3677/gvfsd-smb-browse
tcp   192.168.1.8:58946        192.168.1.3:139          3677/gvfsd-smb-browse
tcp   192.168.1.8:37684        172.217.160.74:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:48808        172.217.160.78:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:52046        172.217.27.142:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:58922        64.233.189.188:5228      1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:35236        34.237.73.95:443         1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   127.0.0.1:35032          127.0.0.1:41453          8027/vim
tcp   192.168.1.8:49020        172.217.27.131:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:47804        74.125.203.189:443       1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:45894        140.82.112.25:443        1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp   192.168.1.8:50864        104.26.4.98:443          1904/chrome --type=utility --field-trial-handle=15061460032943566177,9870870319659881893,131072 --lang=en-US --service-sandbox-type=network --disable-webrtc-apm-in-audio-service --shared-files
tcp6  8d:8d8d:::80             :::*                     -/-
tcp6  0:8d:::22                :::*                     -/-
tcp6  0:8d::1:631              :::*                     -/-
```
With `filter-string` given:
```bash
$ ./hw1 --tcp vim
List of TCP connections:
Proto Local Address            Foreign Address          PID/Program name and arguments
tcp   127.0.0.1:35032          127.0.0.1:41453          8027/vim
```

