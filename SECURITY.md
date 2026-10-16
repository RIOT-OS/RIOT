# RIOT Security Policy

All security bugs reported will be silently fixed in `master` and backported
to the previous release.

When CVE numbers are assigned to RIOT vulnerabilities, they are associated with
[CPE] identifiers in the shape of `cpe:2.3:o:riot-os:riot:<VERSION>`.

[CPE]: https://nvd.nist.gov/products/cpe

## Reporting a Vulnerability

If a security issue is discovered, please report it as
[Security Advisory][GSA-report] (preferred) or report it to the
[security mailing list](mailto:security@riot-os.org) (less preferred).

We try to respond to the report as soon as possible. Due to the current influx
of low quality, AI generated security reports, we may take a bit longer
than we would like.

If you report via e-mail, you can encrypt your report using gpg key id
[44C6AE441172F88D3423E81F5F7964D0F4239033][security-gpg], also included at the
bottom of this file.

[GSA-report]: https://github.com/RIOT-OS/RIOT/security/advisories/new
[security-gpg]: https://riot-os.org/assets/keys/security.asc

**Before reporting, please confirm that:**

- The issue does have real security implications.
    - If there is no real world security implication, please report it as a
      regular issue instead.
- You have read the report, understand it, and are able to answer questions
  about it.
- The report contains all the details needed to understand it, but not more.
- The report is written concisely and to the point.
    - We often get LLM generated walls of text covering multiple screens that
      can be reworded in one or two sentences without losing any information.
- The underlying issue cannot be fixed with a trivial patch changing at most
  three lines of code.
    - For trivial to fix issues, please consider to open a PR with the fix
      instead. We review and merge PRs with trivial and tiny changes very
      quickly.
- The issue is still present in the `master` branch.
- The affected source code is actually part of our git repo.
    - For security flaws in toolchains or packages, please report the issue
      to the upstream project that does maintain the code instead.
    - If the issue is already fixed upstream and we ship an outdated and
      insecure version, you may report that *in at most two sentences*, or
      (much preferred) just open a PR to update the package.

### Classification of a vulnerability

Unless the reporter explicitly requests not to do so,
the RIOT security maintainers may declassify an issue
if the issue is not deemed critical --
for example when it requires an unlikely combination of circumstances and/or configuration options,
or when it can only be exploited by a user who gains no additional privileges.

## Notification of a Vulnerability

After a fix is provided the security issue will be privately disclosed to the
original reporter, RIOT security maintainers, and "Trusted RIOT Users".
A public announcement of the security fix will be made two weeks after the
point release, though this may vary depending on the severity and ability of
trusted RIOT users to provide the fix.

## Trusted RIOT Users

To access the "Trusted RIOT Users" notifications on the
[RIOT forum](https://forum.riot-os.org) please send information
on the RIOT based service or product as well as your
[forum](https://forum.riot-os.org) username to the
[security mailing list](mailto:security@riot-os.org).
Early notification of security bugs will be available and should not be shared
publicly.
If done, it will result in access removal from the "Trusted RIOT Users"
notifications.

## RIOT community GPG key

```
-----BEGIN PGP PUBLIC KEY BLOCK-----

mDMEYY4plhYJKwYBBAHaRw8BAQdALRZ/IJmifuwoSUYTVbKUy9z/m3y0ux6DLMD6
kMs13/+0J1JJT1QtT1Mgc2VjdXJpdHkgPHNlY3VyaXR5QHJpb3Qtb3Mub3JnPoiW
BBMWCAA+AhsDBQsJCAcCBhUKCQgLAgQWAgMBAh4BAheAFiEERMauRBFy+I00I+gf
X3lk0PQjkDMFAmlmGfMFCQuaV10ACgkQX3lk0PQjkDNbmgD9HXKg/SgcnqwJ+lPd
+lMd06vz4gm44drIkmyLTbjBow0A/1VWqLjd+Keh1PkruCQcqjtf7ryHjSMKaJop
q3w5W/4DiQIzBBABCAAdFiEEZi0l9uxO8DMbg2ZS/bt7xc+P5tsFAmGOKbkACgkQ
/bt7xc+P5tvQLA//fCVUg3B5N5J1gCOSGRlplzFO0DELNl8akecxxFuCUU74Hjyc
NSR4r8lQGhGvAVZLanBTprTWeYXtCuLAFfCwvNitbWXnmXRJawQ4k0TQfUXWNsbf
o84QHtKvxEEwLnubVfz+uATw0eahmU2beh2lEl1PKPnpnvc2q9eM019Ff7RV1poe
UD2ctDZ9yn1GDN6A1E9ejAqxowwPxZfafH6uvPcGnDtvBZ0SB2x+EXvFNDpdaFBm
GEqAOY+wBabk6XV9B5qhu0KeVy0ePHni8JaZQJZX+xo2Nzk14IG66nxBF0zz0qTj
2hntxygrS44lQffqkDl+W/Vyc31/k3vsemLQAaC+/ZV1ULxew1VCmBHKE201+8bS
VmQweoiVBa30HtftOhMtlSi+WHyzwG7KGiD148PJIuQx42Dj/iY0MLHCR5c32giF
tW0xJ6fDkVVC0LkLPfBbMJrKxpX5xyWnWVibWHyAXaI/Sh2oK9uIkvdPFh+rWNe7
Wr3Sokn3oUUE2BVkcOiZO9gMYngx6sDWazYwBMTaDxPISIdQofAPZ3LiW/f12wXq
V39RPXrlR3wDf8frhb8Jfxt1q0KHRbU3Drf8cGjpC42H/HazhH4QugbqfUv3BH0C
zJTYg+S79aDgIqUaW5ASxIi5e20jNKoaRnYg7Y1rYk4ttMtH72XylP1vuCK4OARh
jimWEgorBgEEAZdVAQUBAQdAQCSgXzft+sMtSz1vOEaT/s28u/LVmLjUoGtuAcns
in0DAQgHiH4EGBYIACYCGwwWIQRExq5EEXL4jTQj6B9feWTQ9COQMwUCaWYbDAUJ
C5pYdgAKCRBfeWTQ9COQM2mrAP4zmA5fTZtrkHzxghhI4K/I0svTpXZeihxVv20z
DvcBxgEA1FzRNLdF0y3lXzHkEXCTPEkMUUAiApPEe+kFSIJ/hwY=
=1PJl
-----END PGP PUBLIC KEY BLOCK-----
```
