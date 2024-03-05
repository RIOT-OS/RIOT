use rs_matter::error::Error;
use rs_matter::core::Matter;
use riot_wrappers::println;

pub struct Psm<'a> {
    matter: &'a Matter<'a>,
    buf: [u8; 4096],
    dir: &'a str,
}

impl<'a> Psm<'a> {
    #[inline(always)]
    pub fn new(matter: &'a Matter<'a>) -> Result<Self, Error> {
        let mut buf = [0; 4096];
        Ok(Self { matter, buf, dir: "test" })
    }

    pub async fn run(&mut self) -> Result<(), Error> {
        loop {
            self.matter.wait_changed().await;

            if self.matter.is_changed() {
                if let Some(data) = self.matter.store_acls(&mut self.buf)? {
                    println!("Store ACL data...");
                    Self::store("acls", data)?;
                }

                if let Some(data) = self.matter.store_fabrics(&mut self.buf)? {
                    println!("Store fabric data...");
                    Self::store("fabrics", data)?;
                }
            }
        }
    }

    fn load<'b>(key: &str, buf: &'b mut [u8]) -> Result<Option<&'b [u8]>, Error> {
        Ok(None)
    }

    fn store(key: &str, data: &[u8]) -> Result<(), Error> {
        println!("Store data");
        Ok(())
    }
}