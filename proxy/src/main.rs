use tokio::io;
use tokio::io::AsyncWriteExt;
use tokio::net::{TcpListener, TcpStream};

use futures::FutureExt;
use std::env;
use std::error::Error;
use std::sync::{Arc, Mutex};
use std::time::{Duration, SystemTime};

struct Ip {
    ip_addr: String,
    expire: SystemTime,
}

async fn proxy(listen_addr: String, server_addr: String) {
    println!("proxying {} -> {}", listen_addr, server_addr);
    let mut ips: Vec<Arc<Mutex<Ip>>> = Vec::new();
    ips.push(Arc::new(Mutex::new(Ip {
        ip_addr: "192.168.178.131".to_string(),
        expire: SystemTime::now() + Duration::from_secs(30),
    })));
    let listener = TcpListener::bind(listen_addr).await.unwrap();

    while let Ok((inbound, _)) = listener.accept().await {
        let ip_addr = inbound.peer_addr().unwrap().ip().to_string();

        let ip_find = ips.iter().find(|x| x.lock().unwrap().ip_addr == ip_addr);
        if let Some(ip) = ip_find {
            let transfer = transfer(Arc::clone(ip), inbound, server_addr.clone()).map(|r| {
                if let Err(e) = r {
                    println!("{}", e);
                }
            });
            tokio::spawn(transfer);
        }
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let mut handles = vec![];
    handles.push(tokio::spawn(proxy(
        "0.0.0.0:13001".to_string(),
        "192.168.178.130:13001".to_string(),
    )));
    handles.push(tokio::spawn(proxy(
        "0.0.0.0:1800".to_string(),
        "192.168.178.130:1800".to_string(),
    )));
    futures::future::join_all(handles).await;
    Ok(())
}

async fn transfer(
    ip: Arc<std::sync::Mutex<Ip>>,
    mut inbound: TcpStream,
    proxy_addr: String,
) -> Result<(), Box<dyn Error>> {
    if ip.lock().unwrap().expire < SystemTime::now() {
        return Err(std::io::Error::new(
            std::io::ErrorKind::Other,
            format!(
                "{} tried to connect without auth",
                ip.lock().unwrap().ip_addr
            ),
        )
        .into());
    }

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
            if ip.lock().unwrap().expire < SystemTime::now() {
                return Err(std::io::Error::new(
                    std::io::ErrorKind::Other,
                    format!("{} expired", ip.lock().unwrap().ip_addr),
                ));
            }
            tokio::time::sleep(Duration::from_secs(5)).await;
        }
        return Ok(());
    };

    tokio::try_join!(client_to_server, server_to_client, ip_exp)?;

    return Ok(());
}
