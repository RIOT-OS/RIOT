"""
Image signing and management.
"""

from . import version as versmod
import hashlib
import struct

IMAGE_MAGIC = 0x96f3b83c
IMAGE_HEADER_SIZE = 32

# Image header flags.
IMAGE_F = {
        'PIC':                   0x0000001,
        'SHA256':                0x0000002,
        'PKCS15_RSA2048_SHA256': 0x0000004,
        'ECDSA224_SHA256':       0x0000008,
        'NON_BOOTABLE':          0x0000010,
        'ECDSA256_SHA256':       0x0000020,
        'PKCS1_PSS_RSA2048_SHA256': 0x0000040, }

TLV_VALUES = {
        'SHA256': 1,
        'RSA2048': 2,
        'ECDSA224': 3,
        'ECDSA256': 4, }

TLV_HEADER_SIZE = 4

# Sizes of the image trailer, depending on image alignment.
trailer_sizes = {
        1: 402,
        2: 788,
        4: 1560,
        8: 3104, }

boot_magic = bytes([
    0x77, 0xc2, 0x95, 0xf3,
    0x60, 0xd2, 0xef, 0x7f,
    0x35, 0x52, 0x50, 0x0f,
    0x2c, 0xb6, 0x79, 0x80, ])

class TLV():
    def __init__(self):
        self.buf = bytearray()

    def add(self, kind, payload):
        """Add a TLV record.  Kind should be a string found in TLV_VALUES above."""
        buf = struct.pack('<BBH', TLV_VALUES[kind], 0, len(payload))
        self.buf += buf
        self.buf += payload

    def get(self):
        return bytes(self.buf)

class Image():
    @classmethod
    def load(cls, path, included_header=False, **kwargs):
        """Load an image from a given file"""
        with open(path, 'rb') as f:
            payload = f.read()
        obj = cls(**kwargs)
        obj.payload = payload

        # Add the image header if needed.
        if not included_header and obj.header_size > 0:
            obj.payload = (b'\000' * obj.header_size) + obj.payload

        obj.check()
        return obj

    def __init__(self, version=None, header_size=IMAGE_HEADER_SIZE, pad=0):
        self.version = version or versmod.decode_version("0")
        self.header_size = header_size or IMAGE_HEADER_SIZE
        self.pad = pad

    def __repr__(self):
        return "<Image version={}, header_size={}, pad={}, payloadlen=0x{:x}>".format(
                self.version,
                self.header_size,
                self.pad,
                len(self.payload))

    def save(self, path):
        with open(path, 'wb') as f:
            f.write(self.payload)

    def check(self):
        """Perform some sanity checking of the image."""
        # If there is a header requested, make sure that the image
        # starts with all zeros.
        if self.header_size > 0:
            if any(v != 0 for v in self.payload[0:self.header_size]):
                raise Exception("Padding requested, but image does not start with zeros")

    def sign(self, key):
        self.add_header(key)

        tlv = TLV()

        # Note that ecdsa wants to do the hashing itself, which means
        # we get to hash it twice.
        sha = hashlib.sha256()
        sha.update(self.payload)
        digest = sha.digest()

        tlv.add('SHA256', digest)

        if key is not None:
            sig = key.sign(self.payload)
            tlv.add(key.sig_tlv(), sig)

        self.payload += tlv.get()

    def add_header(self, key):
        """Install the image header.

        The key is needed to know the type of signature, and
        approximate the size of the signature."""

        flags = 0
        tlvsz = 0
        if key is not None:
            flags |= IMAGE_F[key.sig_type()]
            tlvsz += TLV_HEADER_SIZE + key.sig_len()

        flags |= IMAGE_F['SHA256']
        tlvsz += 4 + hashlib.sha256().digest_size

        fmt = ('<' +
            # type ImageHdr struct {
            'I' +   # Magic uint32
            'H' +   # TlvSz uint16
            'B' +   # KeyId uint8
            'B' +   # Pad1  uint8
            'H' +   # HdrSz uint16
            'H' +   # Pad2  uint16
            'I' +   # ImgSz uint32
            'I' +   # Flags uint32
            'BBHI' + # Vers  ImageVersion
            'I'     # Pad3  uint32
            ) # }
        assert struct.calcsize(fmt) == IMAGE_HEADER_SIZE
        header = struct.pack(fmt,
                IMAGE_MAGIC,
                tlvsz, # TlvSz
                0, # KeyId (TODO: allow other ids)
                0,  # Pad1
                self.header_size,
                0, # Pad2
                len(self.payload) - self.header_size, # ImageSz
                flags, # Flags
                self.version.major,
                self.version.minor or 0,
                self.version.revision or 0,
                self.version.build or 0,
                0) # Pad3
        self.payload = bytearray(self.payload)
        self.payload[:len(header)] = header

    def pad_to(self, size, align):
        """Pad the image to the given size, with the given flash alignment."""
        tsize = trailer_sizes[align]
        padding = size - (len(self.payload) + tsize)
        if padding < 0:
            msg = "Image size (0x{:x}) + trailer (0x{:x}) exceeds requested size 0x{:x}".format(
                    len(self.payload), tsize, size)
            raise Exception(msg)
        pbytes = b'\xff' * padding
        pbytes += boot_magic
        pbytes += b'\xff' * (tsize - len(boot_magic))
        self.payload += pbytes
