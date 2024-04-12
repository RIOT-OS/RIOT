use log::debug;
use rs_matter::error::Error;
use rs_matter::core::Matter;

pub struct Psm<'a> {
    matter: &'a Matter<'a>,
    buf: [u8; 4096],
    dir: &'a str,
}

impl<'a> Psm<'a> {
    #[inline(always)]
    pub fn new(matter: &'a Matter<'a>) -> Result<Self, Error> {
        let mut buf = [0; 4096];
        Ok(Self { matter, buf, dir: "data" })
    }

    pub async fn run(&mut self) -> Result<(), Error> {
        debug!("PersistenceManager running...");
        loop {
            self.matter.wait_changed().await;

            if self.matter.is_changed() {
                if let Some(data) = self.matter.store_acls(&mut self.buf)? {
                    debug!("Store ACL data...");
                    Self::store("acls", data)?;
                }

                if let Some(data) = self.matter.store_fabrics(&mut self.buf)? {
                    debug!("Store fabric data...");
                    Self::store("fabrics", data)?;
                }
            }
        }
    }

    fn load<'b>(key: &str, buf: &'b mut [u8]) -> Result<Option<&'b [u8]>, Error> {
        debug!("Load data");
        Ok(None)
    }

    fn store(key: &str, data: &[u8]) -> Result<(), Error> {
        debug!("Store data");
        Ok(())
    }
}