use tokio::io;
use tokio::io::AsyncWriteExt;
use tokio::net::{TcpListener, TcpStream};
use tokio_stream::StreamExt;

use futures::FutureExt;
use std::error::Error;
use tokio::sync::RwLock;
use std::sync::Arc;
use std::time::{Duration, SystemTime};

struct Ip {
    ip_addr: String,
    expire: SystemTime,
}

async fn proxy(ips: Arc<RwLock<Vec<Arc<RwLock<Ip>>>>>,listen_addr: String, server_addr: String) {
    println!("proxying {} -> {}", listen_addr, server_addr);
    
    let listener = TcpListener::bind(listen_addr).await.unwrap();

    while let Ok((inbound, _)) = listener.accept().await {
        let ip_addr = inbound.peer_addr().unwrap().ip().to_string();
        let buf = ips.read().await;
        let mut ip_find = tokio_stream::iter(buf.iter()).map(|x| futures::executor::block_on(async {if x.read().await.ip_addr == ip_addr { Some(x) }else { None } }));
        if let Some(ip) = ip_find.next().await {
            let transfer = transfer(Arc::clone(ip.unwrap()), inbound, server_addr.clone()).map(|r| {
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
    let ips: Arc<RwLock<Vec<Arc<RwLock<Ip>>>>> = Arc::new(RwLock::new(Vec::new()));
    ips.write().await.push(Arc::new(RwLock::new(Ip {
        ip_addr: "192.168.178.131".to_string(),
        expire: SystemTime::now() + Duration::from_secs(30),
    })));
    handles.push(tokio::spawn(proxy(ips.clone(),
        "0.0.0.0:13001".to_string(),
        "192.168.178.130:13001".to_string(),
    )));
    /*handles.push(tokio::spawn(proxy(ips.clone(),
        "0.0.0.0:8001".to_string(),
        "192.168.178.130:8001".to_string(),
    )));
    */
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
            format!(
                "{} tried to connect without auth",
                ip.read().await.ip_addr
            ),
        )
        .into());
    }
    println!("{} connected",ip.read().await.ip_addr);
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
            println!("{:?}","top");
            let lock =ip.read().await; 
            if lock.expire < SystemTime::now() {
                return Err(std::io::Error::new(
                    std::io::ErrorKind::Other,
                    format!("{} expired", ip.read().await.ip_addr),
                ));
            }
            println!("{:?}","lock");
            
            tokio::time::sleep(Duration::from_secs(5)).await;
        }
        return Ok(());
    };

    tokio::try_join!(client_to_server, server_to_client, ip_exp)?;
    
    return Ok(());
}
