# Used Devices
- nRF52840-DK
- olertia

# Errors found

## nRF52840-DK 

### Failed assertion when in reply mode

How to reproduce: 
  - Set nRF52840-DK to reply mode with "reply"
  - Use spam function from another radio e.g. "spam <long_addr>  10 100 5"
  
### Crashes if packets are not sent with a delay

How to reproduce:
  - Use spam function on radio nRF52840-DK e.g. "spam <long_addr> <len> <number of packets> 0"
