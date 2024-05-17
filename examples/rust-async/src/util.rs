pub fn to_raw<T>(x: &mut T) -> *mut () { x as *mut _ as _ }
pub fn static_from_raw<T>(p: *mut ()) -> &'static mut T { unsafe { core::mem::transmute(p) } }
pub fn get_static<T>(x: &mut T) -> &'static mut T { static_from_raw(to_raw(x)) }