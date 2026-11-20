use crate::error::NegativeErrorExt;
use crate::libc;
use core::convert::TryInto;
use core::marker::PhantomData;
use core::mem::MaybeUninit;
use riot_sys::{coap_optpos_t, coap_pkt_t, gcoap_listener_t};

use riot_sys::coap_resource_t;

/// Give the caller a way of registering Gcoap handlers into the global Gcoap registry inside a
/// callback. When the callback terminates, the registered handlers are deregistered again,
/// theoretically allowing the registration of non-'static handlers.
///
/// As there is currently no way to unregister handlers, this function panics when the callback
/// terminates. (Otherwise, it'd return the callback's return value).
pub fn scope<'env, F, R>(callback: F) -> R
where
    F: for<'id> FnOnce(&mut RegistrationScope<'env, 'id>) -> R,
{
    let mut r = RegistrationScope {
        _phantom: PhantomData,
    };

    let ret = callback(&mut r);

    r.deregister_all();

    ret
}

/// Append a Gcoap listener in the global list of listeners, so that incoming requests are compared
/// to the listener's match functions and, if matching, are run through its handlers.
///
/// Obtaining a static listener is relatively hard (in particular because storing it somewhere
/// static often requires naming its type, and that's both tedious until `type_alias_impl_trait` is
/// stabilized and hard with how handler generators like to return an impl trait). It is often
/// easier to construct them in a scoped fashion with [RegistrationScope::register].
pub fn register<P>(listener: &'static mut P)
where
    P: 'static + ListenerProvider,
{
    // Creating a scope out of thin air. This is OK because we're using it only on static data.
    //
    // RegistrationScope could conceivably have a public constructor for 'static 'static, but we're
    // not exposing it that way because it'd be weird to create something as a scope that's not
    // really scoping just to call that one function that we're providing here as a standalone
    // function anyway.
    let mut scope: RegistrationScope<'static, 'static> = RegistrationScope {
        _phantom: PhantomData,
    };
    scope.register(listener);
}

/// Lifetimed helper through which registrations can happen
///
/// For explanations of the `'env`' and `'id` lifetimes, see
/// [CountingThreadScope](crate::thread::CountingThreadScope) which has the same.
pub struct RegistrationScope<'env, 'id> {
    _phantom: PhantomData<(&'env (), &'id ())>,
}

impl<'env, 'id> RegistrationScope<'env, 'id> {
    /// Append a Gcoap listener in the global list of listeners, so that incoming requests are
    /// compared to the listener's match functions and, if matching, are run through its handlers.
    ///
    /// Note that the only provided way to get a suitable ListenerProvider is through
    /// [SingleHandlerListener].
    pub fn register<P>(&mut self, listener: &'env mut P)
    where
        // AsMut? hm, probably should re-consider the whole concept of the server ownign a mutable
        // reference to the resource. that makes simple server-mutable resources, but if they are
        // to do *anything* fro somewhere else, don't they need interior mutability anyway?
        P: 'env + ListenerProvider,
    {
        // Unsafe: Moving in a pointer to an internal structure to which we were given an exclusive
        // reference that outlives self -- and whoever can create a Self guarantees that
        // deregister_all() will be called before the end of this self's lifetime.
        unsafe { gcoap_register_listener(listener.get_listener() as *mut _) };
    }

    fn deregister_all(&mut self) {
        panic!("Registration callback returned, but Gcoap does not allow deregistration.");
    }
}

pub trait ListenerProvider {
    /// Provide an exclusive reference to the underlying gcoap listener. The function is marked
    /// unsafe as the returned value contains raw pointers that will later be dereferenced, and
    /// returning arbitrary pointers would make RegistratinScope::register() pass bad data on to C.
    unsafe fn get_listener<'a>(&'a mut self) -> &'a mut gcoap_listener_t;
}

/// A combination of the coap_resource_t and gcoap_listener_t structs with only a single resource
/// (Compared to many resources, this allows easier creation in Rust at the expense of larger
/// memory consumption and slower lookups in Gcoap).
///
/// A listener `l` can be hooked into the global Gcoap registry using [`scope`]`(|x| {
/// x.`[`register`](RegistrationScope::register)`(l) })`.
pub struct SingleHandlerListener<'a, H> {
    _phantom: PhantomData<&'a H>,
    resource: coap_resource_t,
    listener: gcoap_listener_t,
}

impl<'a, H> SingleHandlerListener<'a, H>
where
    H: 'a + Handler,
{
    // keeping methods u32 because the sys constants are too
    pub fn new(path: &'a core::ffi::CStr, methods: u32, handler: &'a mut H) -> Self {
        Self::_new(path, methods, handler, None)
    }

    fn _new(
        path: &'a core::ffi::CStr,
        methods: u32,
        handler: &'a mut H,
        encoder: Option<
            unsafe extern "C" fn(
                *const riot_sys::coap_resource_t,
                *mut libc::c_char,
                riot_sys::size_t,
                *mut riot_sys::coap_link_encoder_ctx_t,
            ) -> riot_sys::ssize_t,
        >,
    ) -> Self {
        let methods = methods.try_into().unwrap();

        SingleHandlerListener {
            _phantom: PhantomData,
            resource: coap_resource_t {
                path: path.as_ptr() as _,
                handler: Some(Self::call_handler),
                methods: methods,
                context: handler as *mut _ as *mut libc::c_void,
            },
            listener: gcoap_listener_t {
                resources: 0 as *const _,
                resources_len: 0,
                next: 0 as *mut _,
                link_encoder: encoder,
                ..Default::default()
            },
        }
    }

    /// Create a listener whose single resource catches all requests and processes them through the
    /// handler.
    ///
    /// This is equivalent to a new single listener at "/" that takes all methods and matches on
    /// subtrees.
    ///
    /// Note that the taken Handler is a Gcoap [Handler] (which is there really only in case anyone
    /// wants extremely fine-grained control of what gcoap does); if you have a
    /// [coap_handler_0_2::Handler], you can wrap it in [crate::coap_handler::v0_2::GcoapHandler]
    /// to for adaptation.
    pub fn new_catch_all(handler: &'a mut H) -> Self {
        Self::new(
            c"/",
            riot_sys::COAP_GET
                | riot_sys::COAP_POST
                | riot_sys::COAP_PUT
                | riot_sys::COAP_DELETE
                | riot_sys::COAP_FETCH
                | riot_sys::COAP_PATCH
                | riot_sys::COAP_IPATCH
                | riot_sys::COAP_MATCH_SUBTREE,
            handler,
        )
    }

    unsafe extern "C" fn call_handler(
        pkt: *mut coap_pkt_t,
        buf: *mut u8,
        len: riot_sys::size_t,
        context: *mut riot_sys::coap_request_ctx_t,
    ) -> riot_sys::ssize_t {
        /* The remaining information in the request_ctx is inaccessible through the CoAP handler
         * API as it is now */
        let h = riot_sys::coap_request_ctx_get_context(context) as *mut H;

        let h = &mut *h;
        let pb = PacketBuffer {
            pkt: &mut *pkt,
            buf,
            len: len.try_into().unwrap(),
        };
        H::handle(h, pb).try_into().unwrap()
    }
}

unsafe extern "C" fn link_encoder<H: WithLinkEncoder>(
    resource: *const riot_sys::coap_resource_t,
    buf: *mut libc::c_char,
    buf_len: riot_sys::size_t,
    ctx: *mut riot_sys::coap_link_encoder_ctx_t,
) -> riot_sys::ssize_t {
    // We're a SingleHandlerListener, therefore we only have a single resource and can
    // back-track to Self
    // (But we don't need this)
    /*
    let self_ = unsafe {
        &*((resource as *const u8)
            .offset(-(memoffset::offset_of!(SingleHandlerListener<H>, resource) as isize))
            as *const SingleHandlerListener<H>)
    };
    */

    let h: &H = unsafe { &*((*resource).context as *const _) };

    let buf = buf as *mut u8; // cast away signedness of char
    let buf = if buf.is_null() {
        None
    } else {
        Some(core::slice::from_raw_parts_mut(buf, buf_len as _))
    };

    link_encoder_safe(h, buf, unsafe { &mut *ctx })
}

fn link_encoder_safe<H: WithLinkEncoder>(
    h: &H,
    mut buf: Option<&mut [u8]>,
    ctx: &mut riot_sys::coap_link_encoder_ctx_t,
) -> riot_sys::ssize_t {
    let mut writer = LinkEncoder::new(buf.as_deref_mut(), ctx);
    h.encode(&mut writer);
    let written = writer.written();
    drop(writer);

    if let Some(buf) = buf.as_ref() {
        if written > buf.len() {
            // An odd way to say .rfind(), but there's no such function on slices
            if let Some((i, _)) = buf.windows(2).enumerate().rfind(|(_, x)| x == b",<") {
                // Knowing the syntax of the produced data, we find a point at which we can
                // salvage some records. (The rest is lost for lack of a blockwise-enabled
                // interface).
                i as _
            } else {
                -1
            }
        } else {
            written as _
        }
    } else {
        written as _
    }
}

impl<'a, H> SingleHandlerListener<'a, H>
where
    H: 'a + Handler + WithLinkEncoder,
{
    /// Like [`Self::new()`], but utilizing that the handler is also [WithLinkEncoder] and can thus influence
    /// what is reported when the default .well-known/core handler is queried.
    pub fn new_with_link_encoder(
        path: &'a core::ffi::CStr,
        methods: u32,
        handler: &'a mut H,
    ) -> Self {
        Self::_new(path, methods, handler, Some(link_encoder::<H>))
    }
}

impl<'a, H> ListenerProvider for SingleHandlerListener<'a, H>
where
    H: 'a + Handler,
{
    unsafe fn get_listener(&mut self) -> &mut gcoap_listener_t {
        self.listener.resources = &self.resource;
        self.listener.resources_len = 1;
        self.listener.next = 0 as *mut _;

        &mut self.listener
    }
}

// Can be implemented by application code that'd then need to call some gcoap response functions,
// but preferably using the coap_handler module (behind the with-coap-handler feature).
pub trait Handler {
    fn handle(&mut self, pkt: PacketBuffer) -> isize;
}

/// The message buffer of a .well-known/core file in appication/link-format, as it is passed to a
/// [WithLinkEncoder] handler.
pub struct LinkEncoder<'a> {
    cursor: usize,
    buffer: Option<&'a mut [u8]>,
    context: &'a mut riot_sys::coap_link_encoder_ctx_t,
}

impl<'a> LinkEncoder<'a> {
    fn new(
        buffer: Option<&'a mut [u8]>,
        context: &'a mut riot_sys::coap_link_encoder_ctx_t,
    ) -> Self {
        Self {
            cursor: 0,
            buffer,
            context,
        }
    }

    fn written(&self) -> usize {
        self.cursor
    }

    /// Emit a comma, except the first time this is called
    ///
    /// (This is the separator of the records of application/link-format; RIOT's )
    pub fn write_comma_maybe(&mut self) {
        const COAP_LINK_FLAG_INIT_RESLIST: u16 = riot_sys::COAP_LINK_FLAG_INIT_RESLIST as _;
        if self.context.flags & COAP_LINK_FLAG_INIT_RESLIST != 0 {
            self.context.flags = self.context.flags & !COAP_LINK_FLAG_INIT_RESLIST;
            return;
        }
        self.write(b",");
    }

    /// Emit arbitrary bytes
    pub fn write(&mut self, data: &[u8]) {
        if let Some(buffer) = self.buffer.as_mut() {
            if self.cursor <= buffer.len() {
                let usable = data.len().min(buffer.len() - self.cursor);
                buffer[self.cursor..self.cursor + usable].copy_from_slice(&data[..usable]);
            }
            self.cursor += data.len();
        } else {
            self.cursor += data.len();
        }
    }
}

pub trait WithLinkEncoder {
    fn encode(&self, buf: &mut LinkEncoder);
}

use riot_sys::{
    coap_opt_add_opaque, coap_opt_add_uint, coap_opt_get_next, gcoap_register_listener,
    gcoap_resp_init,
};

/// A representation of the incoming or outgoing data on the server side of a request. This
/// includes the coap_pkt_t pre-parsed header and option pointers as well as the memory area
/// dedicated to returning the packet.
///
/// This struct wraps the unsafety of the C API, but does not structurally ensure that valid CoAP
/// messages are created. (For example, it does not keep the user from adding options after the
/// payload marker). Use CoAP generalization for that.
#[derive(Debug)]
pub struct PacketBuffer<'b> {
    pkt: &'b mut coap_pkt_t,
    buf: *mut u8,
    len: usize,
}

impl<'b> PacketBuffer<'b> {
    /// Wrapper for coap_get_code_raw
    pub fn get_code_raw(&self) -> u8 {
        (unsafe { riot_sys::coap_get_code_raw(crate::inline_cast_ref(self.pkt)) }) as u8
        // odd return type in C
    }

    /// Wrapper for gcoap_resp_init
    ///
    /// As it is used and wrapped here, this makes GCOAP_RESP_OPTIONS_BUF bytes unusable, but
    /// working around that would mean duplicating code. Just set GCOAP_RESP_OPTIONS_BUF to zero to
    /// keep the overhead low.
    pub fn resp_init(&mut self, code: u8) -> Result<(), crate::error::NumericError> {
        unsafe {
            gcoap_resp_init(
                self.pkt,
                self.buf,
                self.len.try_into().unwrap(),
                code.into(),
            )
        }
        .negative_to_error()
        .map(|_| ())
    }

    pub fn set_code_raw(&mut self, code: u8) {
        #[cfg(accessible_riot_sys_inline_coap_pkt_set_code)]
        {
            unsafe {
                riot_sys::inline::coap_pkt_set_code(crate::inline_cast_ref_mut(self.pkt), code)
            };
        }
        #[cfg(not(accessible_riot_sys_inline_coap_pkt_set_code))]
        {
            unsafe { (*(*self.pkt).hdr).code = code };
        }
    }

    /// Return the total number of bytes in the message, given that `payload_used` bytes were
    /// written at the payload pointer. Note that those bytes have to include the payload marker.
    ///
    /// This measures the distance between the payload pointer in the pkt and the start of the
    /// buffer. It is the header length after `prepare_response`, and grows as options are added.
    pub fn get_length(&self, payload_used: usize) -> usize {
        let own_length = unsafe { (*self.pkt).payload.offset_from(self.buf) };
        assert!(own_length >= 0);
        let total_length = own_length as usize + payload_used;
        assert!(total_length <= self.len.try_into().unwrap());
        total_length
    }

    /// A view of the current message payload
    ///
    /// This is only the CoAP payload after opt_finish has been called; before, it is a view on the
    /// remaining buffer space after any options that have already been added.
    pub fn payload(&self) -> &[u8] {
        unsafe {
            core::slice::from_raw_parts((*self.pkt).payload, (*self.pkt).payload_len as usize)
        }
    }

    /// A mutable view of the current message payload
    ///
    /// See `payload`.
    pub fn payload_mut(&mut self) -> &mut [u8] {
        unsafe {
            core::slice::from_raw_parts_mut((*self.pkt).payload, (*self.pkt).payload_len as usize)
        }
    }

    /// Add an integer value as an option
    pub fn opt_add_uint(
        &mut self,
        optnum: u16,
        value: u32,
    ) -> Result<(), crate::error::NumericError> {
        unsafe { coap_opt_add_uint(self.pkt, optnum, value) }
            .negative_to_error()
            .map(|_| ())
    }

    /// Add a binary value as an option
    pub fn opt_add_opaque(
        &mut self,
        optnum: u16,
        data: &[u8],
    ) -> Result<(), crate::error::NumericError> {
        unsafe {
            coap_opt_add_opaque(
                self.pkt,
                optnum,
                data.as_ptr() as _,
                data.len().try_into().unwrap(),
            )
        }
        .negative_to_error()
        .map(|_| ())
    }

    pub fn opt_iter<'a>(&'a self) -> PacketBufferOptIter<'a, 'b> {
        PacketBufferOptIter {
            buffer: self,
            state: None,
        }
    }

    pub fn opt_iter_mut<'a>(&'a mut self) -> PacketBufferOptIterMut<'a, 'b> {
        PacketBufferOptIterMut {
            buffer: self,
            state: None,
        }
    }
}

pub struct PacketBufferOptIter<'a, 'b> {
    buffer: &'a PacketBuffer<'b>,
    state: Option<coap_optpos_t>,
}

impl<'a, 'b> Iterator for PacketBufferOptIter<'a, 'b> {
    type Item = (u16, &'a [u8]);

    fn next(&mut self) -> Option<Self::Item> {
        let size;
        let mut start = MaybeUninit::uninit();
        match &mut self.state {
            None => {
                let mut state = MaybeUninit::uninit();
                size = unsafe {
                    coap_opt_get_next(
                        &*self.buffer.pkt,
                        state.as_mut_ptr(),
                        start.as_mut_ptr(),
                        true,
                    )
                };
                if size < 0 {
                    return None;
                }
                // unsafe: as promised by coap_opt_get_next documentation
                self.state = Some(unsafe { state.assume_init() });
            }
            Some(ref mut state) => {
                size = unsafe {
                    coap_opt_get_next(&*self.buffer.pkt, state, start.as_mut_ptr(), false)
                };
                if size < 0 {
                    return None;
                }
            }
        }
        // unsafe: as promised by coap_opt_get_next documentation
        let start = unsafe { start.assume_init() };
        if start == 0 as *mut _ {
            None
        } else {
            // unsafe: that's the parts the coap_opt_get_next documentation promises, and we can
            // build an 'a-lived slice of it because we hold a &'a reference to the whole
            // PacketBuffer
            let slice = unsafe { core::slice::from_raw_parts(start, size as usize) };
            Some((self.state.unwrap().opt_num, slice))
        }
    }
}

pub struct PacketBufferOptIterMut<'a, 'b> {
    buffer: &'a mut PacketBuffer<'b>,
    state: Option<coap_optpos_t>,
}

impl<'a, 'b> Iterator for PacketBufferOptIterMut<'a, 'b> {
    type Item = (u16, &'a mut [u8]);

    fn next(&mut self) -> Option<Self::Item> {
        let size;
        let mut start = MaybeUninit::uninit();
        match &mut self.state {
            None => {
                let mut state = MaybeUninit::uninit();
                size = unsafe {
                    coap_opt_get_next(
                        &*self.buffer.pkt,
                        state.as_mut_ptr(),
                        start.as_mut_ptr(),
                        true,
                    )
                };
                if size < 0 {
                    return None;
                }
                // unsafe: as promised by coap_opt_get_next documentation
                self.state = Some(unsafe { state.assume_init() });
            }
            Some(ref mut state) => {
                size = unsafe {
                    coap_opt_get_next(&*self.buffer.pkt, state, start.as_mut_ptr(), false)
                };
                if size < 0 {
                    return None;
                }
            }
        }

        // unsafe: as promised by coap_opt_get_next documentation
        let start = unsafe { start.assume_init() };
        if start == 0 as *mut _ {
            None
        } else {
            // unsafe: that's the parts the coap_opt_get_next documentation promises, and we can
            // build an 'a-lived mutable slice of it because we hold a &'a mut reference to the
            // whole PacketBuffer, and the options do not overlap
            let slice = unsafe { core::slice::from_raw_parts_mut(start, size as usize) };
            Some((self.state.unwrap().opt_num, slice))
        }
    }
}
