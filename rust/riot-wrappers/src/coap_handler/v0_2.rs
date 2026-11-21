//! This module provides a wrapper around a coap_handler::Handler that can be registered at a RIOT
//! GcoapHandler.

use core::convert::TryInto;

use coap_handler_0_2::{Attribute, Handler, Record, Reporting};

use coap_message_0_3::{
    error::RenderableOnMinimal, MinimalWritableMessage, MutableWritableMessage, ReadableMessage,
};

use crate::coap_message::ResponseMessage;
use crate::gcoap::PacketBuffer;

/// Adapter to get a [crate::gcoap::Handler] from a more generic [coap_handler_0_2::Handler],
/// typically to register it through a [crate::gcoap::SingleHandlerListener].
pub struct GcoapHandler<H>(pub H)
where
    H: Handler;

impl<H> crate::gcoap::Handler for GcoapHandler<H>
where
    H: Handler,
{
    fn handle(&mut self, pkt: PacketBuffer) -> isize {
        let request_data = self.0.extract_request_data(&pkt);
        let mut lengthwrapped = ResponseMessage::new(pkt);
        match request_data {
            Ok(r) => {
                if let Err(e) = self.0.build_response(&mut lengthwrapped, r) {
                    lengthwrapped.rewind();
                    if let Err(_e2) = e.render(&mut lengthwrapped) {
                        // Render double fault
                        lengthwrapped.rewind();
                        lengthwrapped.set_code(coap_numbers::code::INTERNAL_SERVER_ERROR);
                    }
                }
            }
            Err(e) => {
                if let Err(_e2) = e.render(&mut lengthwrapped) {
                    // Render double fault
                    lengthwrapped.rewind();
                    lengthwrapped.set_code(coap_numbers::code::INTERNAL_SERVER_ERROR);
                }
            }
        }
        lengthwrapped.finish()
    }
}

impl<H> crate::gcoap::WithLinkEncoder for GcoapHandler<H>
where
    H: Handler + Reporting,
{
    fn encode(&self, writer: &mut crate::gcoap::LinkEncoder) {
        for record in self.0.report() {
            writer.write_comma_maybe();
            writer.write(b"<");
            for pathelement in record.path() {
                writer.write(b"/");
                writer.write(pathelement.as_ref().as_bytes());
            }
            writer.write(b">");
            if let Some(rel) = record.rel() {
                // Not trying to be smart about whether or not we need the quotes
                writer.write(b";rel=\"");
                writer.write(rel.as_bytes());
                writer.write(b"\"");
            }
            for attr in record.attributes() {
                use Attribute::*;
                match attr {
                    Observable => writer.write(b";obs"),
                    Interface(i) => {
                        writer.write(b";if=\"");
                        writer.write(i.as_bytes());
                        writer.write(b"\"");
                    }
                    ResourceType(r) => {
                        writer.write(b";rt=\"");
                        writer.write(r.as_bytes());
                        writer.write(b"\"");
                    }
                    // FIXME: deduplicate with what's somewhere in coap-handler-implementations;
                    // implement remaining items
                    _ => (),
                }
            }
        }
    }
}

/// Blanket implementation for mutex wrapped resources
///
/// This is useful in combination with the defauilt implementation for Option as well.
impl<'b, H> Handler for &'b crate::mutex::Mutex<H>
where
    H: Handler,
{
    type RequestData = Option<H::RequestData>;

    type ExtractRequestError = H::ExtractRequestError;
    type BuildResponseError<M: MinimalWritableMessage> = H::BuildResponseError<M>;

    fn extract_request_data<M: ReadableMessage>(
        &mut self,
        request: &M,
    ) -> Result<Self::RequestData, Self::ExtractRequestError> {
        self.try_lock()
            .map(|mut h| h.extract_request_data(request))
            .transpose()
    }

    fn estimate_length(&mut self, request: &Self::RequestData) -> usize {
        if let Some(r) = request {
            if let Some(mut s) = self.try_lock() {
                return s.estimate_length(r);
            }
        }

        1
    }

    fn build_response<M: MutableWritableMessage>(
        &mut self,
        response: &mut M,
        request: Self::RequestData,
    ) -> Result<(), Self::BuildResponseError<M>> {
        if let Some(r) = request {
            if let Some(mut s) = self.try_lock() {
                return s.build_response(response, r);
            }
        }

        response.set_code(
            coap_numbers::code::SERVICE_UNAVAILABLE
                .try_into()
                .map_err(|_| "Message type can't even exprss Service Unavailable")
                .unwrap(),
        );
        Ok(())
    }
}
