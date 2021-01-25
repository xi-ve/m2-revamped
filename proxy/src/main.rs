use tokio::io;
use tokio::io::AsyncWriteExt;
use tokio::net::{TcpListener, TcpStream};

use futures::FutureExt;
use std::env;
use std::error::Error;
use std::sync::{Arc, Mutex};
use std::time::{Duration, SystemTime};
use tokio::task;
use tokio::time;

struct Ip {
    ip_addr: String,
    expire: SystemTime,
}
struct Client {
    future: task::JoinHandle<()>,
}

impl Client {
    fn new(future: task::JoinHandle<()>) -> Self {
        {
            Client { future: future }
        }
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let listen_addr = env::args()
        .nth(1)
        .unwrap_or_else(|| "127.0.0.1:8081".to_string());
    let server_addr = env::args()
        .nth(2)
        .unwrap_or_else(|| "127.0.0.1:8080".to_string());

    println!("Listening on: {}", listen_addr);
    println!("Proxying to: {}", server_addr);

    let mut Ips: Vec<Arc<Mutex<Ip>>> = Vec::new();
    Ips.push(Arc::new(Mutex::new(Ip {
        ip_addr: "192.168.178.131".to_string(),
        expire: SystemTime::now() + Duration::from_secs(60),
    })));
    let mut clients: Vec<Client> = Vec::new();

    let listener = TcpListener::bind(listen_addr).await?;

    while let Ok((inbound, _)) = listener.accept().await {
        let ip_addr = inbound.peer_addr().unwrap().ip().to_string();

        let ip_find = Ips.iter().find(|x| x.lock().unwrap().ip_addr == ip_addr);
        if let Some(ip) = ip_find {
            let transfer = transfer(
                Arc::clone(ip),
                inbound,
                server_addr.clone(),
            )
            .map(|r| {
                if let Err(e) = r {
                    println!("Failed to transfer; error={}", e);
                }
            });
            let handle: task::JoinHandle<_> = tokio::spawn(transfer);
            clients.push(Client::new(handle));
        }
    }
    Ok(())
}

async fn transfer(
    ip: Arc<std::sync::Mutex<Ip>>,
    mut inbound: TcpStream,
    proxy_addr: String,
) -> Result<(), Box<dyn Error>> {
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

    //tokio::spawn(async {

    //})

    let ip_exp = async {
        loop {
            if ip.lock().unwrap().expire < SystemTime::now() {
                panic!("expired ip")
            }
            tokio::time::sleep(Duration::from_secs(5)).await;
        }
        Ok(())
    };

    tokio::try_join!(client_to_server, server_to_client,ip_exp)?;


    Ok(())
}
