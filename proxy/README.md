# tcprs

Just a playground to mess around with rust and tokio. This repo is largely the
example code from the tokio-rs github with minor modifications.

The eventual goal is to have a nice tcp proxy that logs meaningful info about
it's connections.

### Example:

server:
```
cargo run 127.0.0.1:8080 10.0.1.159:22
    Finished dev [unoptimized + debuginfo] target(s) in 0.11s
     Running `target/debug/tcprs '127.0.0.1:8080' '10.0.1.159:22'`
Listening on: 127.0.0.1:8080
Proxying to: 10.0.1.159:22
client wrote 4.4 kB and received 5.9 kB
```

client:
```
$ ssh -p 8080 root@localhost
Welcome to Ubuntu 16.04.4 LTS (GNU/Linux 4.3.0 x86_64)

 * Documentation:  https://help.ubuntu.com
 * Management:     https://landscape.canonical.com
 * Support:        https://ubuntu.com/advantage
   __        .                   .
 _|  |_      | .-. .  . .-. :--. |-
|_    _|     ;|   ||  |(.-' |  | |
  |__|   `--'  `-' `;-| `-' '  ' `-'
                   /  ;  Instance (Ubuntu 16.04 20170403)
                   `-'   https://docs.joyent.com/images/container-native-linux

Last login: Sat Jun 30 04:35:50 2018 from 10.0.1.222
root@plex:~# ls
```
