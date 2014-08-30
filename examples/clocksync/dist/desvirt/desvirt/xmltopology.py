#!/usr/bin/python3.1

import xml.dom.minidom
import logging

class XMLTopology():
    def __init__(self, filename):
        self.xmldoc = xml.dom.minidom.parse(filename)

        self.nodetypes = {}
        self.linktypes = {}
        self.net = None

    def findNodes(self, xml, node_name):
        result = []
        if not xml:
            return result

        for element in xml.childNodes:
            if element.nodeName == node_name:
                result.append(element)
        return result

    def findNode(self, xml, node_name):
        for element in xml.childNodes:
            if element.nodeName == node_name:
                return element

    def parseNodeTypes(self, xmlnode):
        nodeTypes = self.findNode(xmlnode, 'nodeTypes')

        for nodeType in self.findNodes(nodeTypes, 'nodeType'):
           name = nodeType.getAttribute('name')
           interfaces = self.findNode(nodeType, 'interfaces')
           interface_names = []
           for interface in self.findNodes(interfaces, 'interface'):
               if_name = interface.getAttribute('name')
               interface_names.append(if_name)

           self.nodetypes[name] = interface_names


    def parseLinkTypes(self, xmlnode):
        linkTypes = self.findNode(xmlnode, 'linkTypes')
        
        if linkTypes:
            for linkType in self.findNodes(linkTypes, 'linkType'):
               name = linkType.getAttribute('name')
               print("linkType: %s" % name)
               rates = self.parseRates(self.findNode(linkType, 'rates'))
               channels = self.parseChannels(self.findNode(linkType, 'channels'))
               self.linktypes[name] = (rates, channels)

    def parseChannels(self, xmlnode):
        channels = []
        for channel_node in self.findNodes(xmlnode, 'channel'):
            name = channel_node.getAttribute('name')
            channels.append(name)
            print("\tchannel: %s" % name)

        return channels

    def parseRates(self, xmlnode):
        rates = []
        for rate in self.findNodes(xmlnode, 'rate'):
            name = rate.getAttribute('name')
            netto = rate.getAttribute('netto')
            loss = rate.getAttribute('loss')
            print ("\trate: %s netto_rate: %s loss: %s" % (name, netto, loss))
            rates.append((name,netto,loss))
        return rates

    def parseNodes(self, xmlnode):
        nodes = []
        for node in self.findNodes(xmlnode, 'node'):
            n = None
            name = node.getAttribute('name')
            nodeType = node.getAttribute('type')
            binary = node.getAttribute('binary')
            logging.getLogger("").debug("Found node %s of type %s" % (name, nodeType))
            default_ifaces = self.nodetypes[nodeType]
            interfaces = []
            interfaces.extend(default_ifaces)
            logging.getLogger("").debug("Binary for %s is %s" % (name, binary))
            n = self.nodeHandler(name, nodeType, binary)
            nodes.append(n)

            for interface in interfaces:
                n.nics.append(self.nicHandler(interface, net=self.net, node=n))

        return nodes

    def parseLinks(self,xmlnode):
        for link in self.findNodes(xmlnode, 'link'):
            from_node = link.getAttribute('from_node')
            from_if = link.getAttribute('from_if')

            to_node = link.getAttribute('to_node')
            to_if = link.getAttribute('to_if')
            
            loss = link.getAttribute('loss')

            delay = link.getAttribute('delay')
            if not delay:
                delay = 0

            linkType = link.getAttribute('linkType')
            if linkType:
                (rates, channels) = self.linktypes[linkType]
            
            uni_directional = False

            uni_directional_str = link.getAttribute('uni')
            if uni_directional_str:
                if uni_directional_str=="true":
                    uni_directional = True

            self.linkHandler(from_node, from_if, to_node, to_if, None, None, loss, delay, uni_directional)

#            print("link from_node: %s, to_node: %s type: %s" %(from_node, to_node, linkType))
#            print(rates)
#            print(channels)
    
    def parse(self):
        topology = self.findNode(self.xmldoc, 'topology')
        net = self.findNode(topology, 'net')

        net_name = net.getAttribute('name')
        net_desc = net.getAttribute('description')
       
        self.net = self.netHandler(net_name, net_desc)

        self.parseNodeTypes(net)
        self.parseLinkTypes(net)
        self.parseNodes(self.findNode(net, 'nodes'))
        self.parseLinks(self.findNode(net, 'links'))


