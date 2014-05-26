This application is a showcase of the Content Centric Networking (CCN) capabilities in RIOT.

The example consists in two parts: clients and relays.

This file documents the relay functionalities.

A relay implements CCN capabilities for third party content caching, forwarding and subscription.

Upon booting up, the CCN stack is automatically started, which enables the processing of incoming CCN messages (and relaying them if necessary).
