use riot_wrappers::mutex::{Mutex, MutexGuard};
use ringbuf::Rb;

pub(crate) struct Stdin<const N: usize> {
    pub(crate) lock_for_waiter: Option<MutexGuard<'static, ()>>,
    pub(crate) buffer: ringbuf::LocalRb<u8, [core::mem::MaybeUninit<u8>; N]>,
}

pub struct StdinHandler<const N: usize>(pub(crate) &'static Mutex<Option<&'static mut Stdin<N>>>);

impl<const N: usize> coap_handler::Handler for StdinHandler<N> {
    type RequestData = u8;
    fn extract_request_data(&mut self, req: &impl coap_message::ReadableMessage) -> u8 {
        use coap_handler_implementations::option_processing::OptionsExt;

        let opts = req.options().ignore_elective_others();

        if opts.is_err() {
            // FIXME: We could also use its Render implementation if that were public
            return coap_numbers::code::BAD_OPTION;
        }

        if req.code().into() != coap_numbers::code::POST {
            return coap_numbers::code::METHOD_NOT_ALLOWED;
        }

        let Some(mut stdin) = self.0.try_lock() else {
            // This only happens when someone is just in the process of locking stdin -- we'll need
            // to return in ordrr to let them place their mutex there. FIXME: Max-Age:0
            return coap_numbers::code::SERVICE_UNAVAILABLE;
        };
        let Some(stdin) = stdin.as_mut() else {
            // Stdin not initialized
            //
            // Returning as before here not only because it's a sensible error, but also because it
            // should allow the compiler to collapse the branches.
            return coap_numbers::code::SERVICE_UNAVAILABLE;
        };

        let maybe_the_waiter = stdin.lock_for_waiter.take();

        if stdin.buffer.free_len() < req.payload().len() {
            if req.payload().len() > N {
                // Buffer full. FIXME: Max-Age? Too-Many-Requests? Request-Entity-Too-Large?
                return coap_numbers::code::SERVICE_UNAVAILABLE;
            } else {
                // Not a chance
                return coap_numbers::code::REQUEST_ENTITY_TOO_LARGE;
            }
        }
        stdin.buffer.push_slice(req.payload());

        drop(maybe_the_waiter);

        coap_numbers::code::CHANGED
    }
    fn estimate_length(&mut self, _req: &u8) -> usize {
        1
    }
    fn build_response(&mut self, res: &mut impl coap_message::MutableWritableMessage, req: u8) {
        res.set_code(req.try_into().ok().unwrap());
        if req == coap_numbers::code::REQUEST_ENTITY_TOO_LARGE {
            if let (Ok(opt), Ok(size)) = (
                coap_numbers::option::SIZE1.try_into(),
                u16::try_from(N),
            ) {
                res.add_option_uint(opt, size);
            }
        }
        res.set_payload(b"");
    }
}
