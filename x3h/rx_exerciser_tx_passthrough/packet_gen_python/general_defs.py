from scapy.all import *


def create_udp_packet(dstMacAddr, dstIpAddr, dstUdpPort, payload,
                      srcMacAddr="05:05:05:05:05:05", srcIpAddr="192.168.0.1", srcUdpPort=10000):
    ether = Ether(src=srcMacAddr, dst=dstMacAddr)
    ip = IP(dst=dstIpAddr, src=srcIpAddr)
    udp = UDP(dport=dstUdpPort, sport=srcUdpPort)
    pld = Raw(load=payload)
    pkt = ether / ip / udp / pld
    return pkt
