//use serde_json;
use tokio::io;
use tokio::net::{TcpListener, TcpStream};

use futures::stream::{self, StreamExt};
use futures::FutureExt;
use std::sync::Arc;
use std::time::{Duration, SystemTime};
use std::{error::Error, thread::JoinHandle};
use tokio::io::AsyncBufReadExt;
use tokio::io::AsyncWriteExt;
use tokio::io::BufReader;
use tokio::sync::RwLock;

struct Ip {
    ip_addr: String,
    expire: SystemTime,
}

struct Forwarding {
    listen: String,
    foward: String,
    thread: tokio::task::JoinHandle<()>,
}

struct Server {
    name: String,
    fowardings: Arc<RwLock<Vec<Arc<RwLock<Forwarding>>>>>,
    ips: Arc<RwLock<Vec<Arc<RwLock<Ip>>>>>,
}

async fn proxy(ips: Arc<RwLock<Vec<Arc<RwLock<Ip>>>>>, listen_addr: String, server_addr: String) {
    println!("proxying {} -> {}", listen_addr, server_addr);
    let listener = TcpListener::bind(listen_addr).await.unwrap();

    while let Ok((inbound, _)) = listener.accept().await {
        let ip_addr = inbound.peer_addr().unwrap().ip().to_string();
        let buf = ips.read().await;
        let ip_find = stream::iter(buf.iter()).filter_map(|x| {
            let ip_addr = &ip_addr;
            async move {
                if x.read().await.ip_addr == ip_addr.to_string() {
                    Some(x)
                } else {
                    None
                }
            }
        });
        tokio::pin!(ip_find);
        if let Some(ip) = ip_find.next().await {
            let transfer = transfer(Arc::clone(ip), inbound, server_addr.clone()).map(|r| {
                if let Err(e) = r {
                    println!("{}", e);
                }
            });
            tokio::spawn(transfer);
        }
    }
}
async fn add_ip(server: Arc<RwLock<Server>>, ip: String) {
    let mut found: bool = false;
    let mut index: usize = 0;
    let ip_addr = ip.to_string();
    println!("adding ip {}", ip_addr);
    for n in 0..server.read().await.ips.read().await.len() {
        if server.read().await.ips.read().await[n].read().await.ip_addr == ip_addr.to_string() {
            index = n;
            found = true;
            break;
        }
    }
    if found {
        server.read().await.ips.write().await[index]
            .write()
            .await
            .expire = SystemTime::now() + Duration::from_secs(30);
        return;
    }
    server
        .read()
        .await
        .ips
        .write()
        .await
        .push(Arc::new(RwLock::new(Ip {
            ip_addr: ip,
            expire: SystemTime::now() + Duration::from_secs(30),
        })));
}
async fn process(servers: Arc<RwLock<Vec<Arc<RwLock<Server>>>>>, inbound: TcpStream) {
    let (rd, mut wr) = io::split(inbound);
    let mut reader = BufReader::new(rd);
    let mut line = String::new();
    reader.read_line(&mut line).await.unwrap();
    let data: serde_json::Value = serde_json::from_str(&line).unwrap();
    if data["op"] == 1 {
        match find_server_by_name(servers, data["server_name"].to_string()).await {
            Ok(v) => {
                add_ip(v, data["ip"].to_string()).await;
                wr.write_all(b"done\n").await.unwrap();
            }
            Err(e) => return,
        }
        wr.write_all(b"done\n").await.unwrap();
    }
}
async fn admin(server: Arc<RwLock<Vec<Arc<RwLock<Server>>>>>) {
    let listener = TcpListener::bind("0.0.0.0:1337").await.unwrap();
    while let Ok((inbound, _)) = listener.accept().await {
        tokio::spawn(process(server.clone(), inbound));
    }
}

async fn find_server_by_name(
    servers: Arc<RwLock<Vec<Arc<RwLock<Server>>>>>,
    name: String,
) -> Result<Arc<RwLock<Server>>, &'static str> {
    let mut found: bool = false;
    let mut index: usize = 0;
    for n in 0..servers.read().await.len() {
        if servers.read().await[n].read().await.name == name {
            index = n;
            found = true;
            break;
        }
    }
    if found == true {
        return Ok(Arc::clone(&servers.read().await[index]));
    } else {
        return Err("no server found");
    }
}

async fn add_forwarding(
    servers: Arc<RwLock<Vec<Arc<RwLock<Server>>>>>,
    name: String,
    listen: String,
    forward: String,
) -> Result<bool, &'static str> {
    match find_server_by_name(servers, name).await {
        Ok(v) => v
            .read()
            .await
            .fowardings
            .write()
            .await
            .push(Arc::new(RwLock::new(Forwarding {
                listen: listen.clone(),
                foward: forward.clone(),
                thread: tokio::spawn(proxy(Arc::clone(&v.read().await.ips), listen, forward)),
            }))),
        Err(e) => return Err(e),
    }
    return Ok(true);
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let mut handles = vec![];

    let servers: Arc<RwLock<Vec<Arc<RwLock<Server>>>>> = Arc::new(RwLock::new(Vec::new()));
    servers.write().await.push(Arc::new(RwLock::new(Server {
        name: "test".to_string(),
        ips: Arc::new(RwLock::new(Vec::new())),
        fowardings: Arc::new(RwLock::new(Vec::new())),
    })));
    add_forwarding(
        servers.clone(),
        "test".to_string(),
        "0.0.0.0:13001".to_string(),
        "192.168.178.130:13001".to_string(),
    )
    .await?;
    println!("added!");
    handles.push(tokio::spawn(admin(servers.clone())));
    futures::future::join_all(handles).await;
    Ok(())
}

async fn transfer(
    ip: Arc<RwLock<Ip>>,
    mut inbound: TcpStream,
    proxy_addr: String,
) -> Result<(), Box<dyn Error>> {
    if ip.read().await.expire < SystemTime::now() {
        return Err(std::io::Error::new(
            std::io::ErrorKind::Other,
            format!("{} tried to connect without auth", ip.read().await.ip_addr),
        )
        .into());
    }
    println!("{} connected", ip.read().await.ip_addr);
    let mut outbound = TcpStream::connect(proxy_addr).await?;

    let (mut ri, mut wi) = inbound.split();
    let (mut ro, mut wo) = outbound.split();
    let client_to_server = async {
        io::copy(&mut ri, &mut wo).await?;
        wo.shutdown().await
    };

    let server_to_client = async {
        io::copy(&mut ro, &mut wi).await?;
        wi.shutdown().await
    };
    let ip_exp = async {
        loop {
            let lock = ip.read().await;
            if lock.expire < SystemTime::now() {
                return Err(std::io::Error::new(
                    std::io::ErrorKind::Other,
                    format!("{} expired", ip.read().await.ip_addr),
                ));
            }
            tokio::time::sleep(Duration::from_secs(5)).await;
        }
        return Ok(());
    };

    tokio::try_join!(client_to_server, server_to_client, ip_exp)?;
    return Ok(());
}
